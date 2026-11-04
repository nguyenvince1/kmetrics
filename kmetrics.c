// K-METRICS: A Linux kernel module that provides system metrics such as uptime, CPU counts, and memory usage through a character device interface.
// v0.5 by Vince Nguyen (nguyenvince1@gmail.com)
// Work in progress. This module is intended for educational purposes and may not be suitable for production use.
// v0.6 (next): Introduce wait queues, blocking I/O interference, polling, wakeups, and multi-device interfaces.

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/timekeeping.h>
#include <linux/cpumask.h>
#include <linux/mm.h>
#include <linux/sysinfo.h>
#include <linux/cpufreq.h>
#include <linux/slab.h> //persistent module mem
#include <linux/workqueue.h>
#include <linux/jiffies.h>
#include <linux/kernel_stat.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>
#include <linux/kfifo.h>

#include "kmetrics_uapi.h"

#define KMETRICS_BASE_SIZE       256
#define KMETRICS_PER_CPU_SIZE     96
#define KMETRICS_HISTORY_SIZE 256

// Question: What is the difference between local and persistent state in kernel development? When to use local versus persistent state?
// kmalloc versus kcalloc: One for temporary, the other persistent?

// AI: Design an actual UAPI with ioctl() or sysfs interface for this module.
// The current implementation uses a simple character device interface, which is not ideal for exposing structured data to user space.

// Data structures
struct kmetrics_cpu_sample {
    u64 prev_idle_us;
    u64 prev_wall_us;
    unsigned int usage_pct;
    bool initialized;
};

struct kmetrics_cpu_snapshot {
    time64_t uptime_sec;
	unsigned int online_cpus; // cpus
	unsigned int possible_cpus;
	unsigned int present_cpus;

    u64 total_mib;
    u64 free_mib;
};

//v0.5
struct kmetrics_history_entry {
    u64 timestamp_ns;
    unsigned int cpu_id;
    unsigned int usage_pct;
};

static struct kmetrics_cpu_sample *cpu_samples;

// We would want to protect each lock and treat them as separate entity.
static DEFINE_MUTEX(metrics_lock);
static DEFINE_MUTEX(history_lock);

static struct delayed_work sample_work;

// DEFINE_KFIFO satisfies 2^n requirement.
static DEFINE_KFIFO(history_fifo,
                    struct kmetrics_history_entry,
                    KMETRICS_HISTORY_SIZE);

// Metric collection
static void kmetrics_collect_snapshot(struct kmetrics_cpu_snapshot *snapshot)
{
    struct sysinfo mem;

    snapshot->uptime_sec = ktime_get_boottime_seconds();
    snapshot->online_cpus = num_online_cpus();
	snapshot->possible_cpus = num_possible_cpus();
    snapshot->present_cpus = num_present_cpus();

    si_meminfo(&mem);

    snapshot->total_mib = ((u64)mem.totalram * mem.mem_unit) / (1024 * 1024);
    snapshot->free_mib = ((u64)mem.freeram * mem.mem_unit) / (1024 * 1024);
}

// Output formatting
static int kmetrics_append_snapshot(char *output,
                                    size_t size,
                                    int len,
                                    const struct kmetrics_cpu_snapshot *snapshot)
{
    len += scnprintf(output + len,
                     size - len,
                     "uptime_sec: %lld\n"
                     "online_cpus: %u\n"
					"possible_cpus: %u\n"
					"present_cpus: %u\n"
                     "memory_total_mib: %llu\n"
                     "memory_free_mib: %llu\n",
                     (long long)snapshot->uptime_sec,
                     snapshot->online_cpus,
                     snapshot->possible_cpus,
                     snapshot->present_cpus,
                     (unsigned long long)snapshot->total_mib,
                     (unsigned long long)snapshot->free_mib);

    return len;
}

static int kmetrics_append_cpu_freqs(char *output,
                                     size_t size,
                                     int len)
{
    int cpu;
    unsigned int freq_khz;

    for_each_online_cpu(cpu) {
        freq_khz = cpufreq_quick_get(cpu);
        if (freq_khz != 0) {
            len += scnprintf(output + len,
                             size - len,
                             "cpu%d_freq_mhz: %u\n",
                             cpu,
                             freq_khz / 1000);
        } else {
            len += scnprintf(output + len,
                             size - len,
                             "cpu%d_freq_mhz: unavailable\n",
                             cpu);
        }
    }
    return len;
}

static int kmetrics_append_cpu_usage(char *output,
                                     size_t size,
                                     int len)
{
    int cpu;

    mutex_lock(&metrics_lock);
    for_each_online_cpu(cpu) {
        if (cpu_samples[cpu].initialized) {
            len += scnprintf(output + len,
                             size - len,
                             "cpu%d_usage_pct: %u\n",
                             cpu,
                             cpu_samples[cpu].usage_pct);
        } else {
            len += scnprintf(output + len,
                             size - len,
                             "cpu%d_usage_pct: unavailable\n",
                             cpu);
        }
    }
    mutex_unlock(&metrics_lock);
    return len;
}

static void kmetrics_history_push(u64 timestamp_ns,
                                  unsigned int cpu_id,
                                  unsigned int usage_pct)
{
    struct kmetrics_history_entry entry;

    entry.timestamp_ns = timestamp_ns;
    entry.cpu_id = cpu_id;
    entry.usage_pct = usage_pct;

    mutex_lock(&history_lock);

    // kfifo_put() normally fails if the FIFO is full. For a metrics history buffer, we prefer to discard
    // the oldest *sample* and preserve the newest one. (FIFO)

    if (kfifo_is_full(&history_fifo))
        kfifo_skip(&history_fifo);

    kfifo_put(&history_fifo, entry);
    mutex_unlock(&history_lock);
}

// Background sampling - mutex
static void kmetrics_sample_work(struct work_struct *work)
{
    int cpu;

    u64 idle_us;
    u64 wall_us;

    u64 delta_idle;
    u64 delta_wall;
    u64 busy_us;

    u64 timestamp_ns;
    unsigned int usage_pct;
    bool usage_valid;
    struct kmetrics_cpu_sample *sample;

    (void)work;

    // Give all samples from this sampling cycle approximately the same timestamp.

    timestamp_ns = ktime_get_boottime_ns();
    for_each_online_cpu(cpu) {
        usage_valid = false;
        usage_pct = 0;

        // Read current accounting before taking our own lock.
        idle_us = get_cpu_idle_time(cpu,
                                    &wall_us,
                                    1);
        mutex_lock(&metrics_lock);
        sample = &cpu_samples[cpu];

        if (sample->initialized) {

            if (wall_us >= sample->prev_wall_us &&
                idle_us >= sample->prev_idle_us) {

                delta_wall = wall_us - sample->prev_wall_us;
                delta_idle = idle_us - sample->prev_idle_us;

                if (delta_wall > 0) {
                    if (delta_idle >= delta_wall)
                        busy_us = 0;
                    else
                        busy_us =
                            delta_wall - delta_idle;
                    usage_pct = (unsigned int)((busy_us * 100) / delta_wall);
                    sample->usage_pct = usage_pct;
                    usage_valid = true;
                }
            }
        } else {
            sample->initialized = true;
        }

        sample->prev_idle_us = idle_us;
        sample->prev_wall_us = wall_us;

        mutex_unlock(&metrics_lock);

        // Kmetrics History is protected separately.
        if (usage_valid) {
            kmetrics_history_push(timestamp_ns,
                                  cpu,
                                  usage_pct);
        }
    }
    schedule_delayed_work(&sample_work,
                          msecs_to_jiffies(1000));
}

static ssize_t kmetrics_read(struct file *file,
                             char __user *buf,
                             size_t count,
                             loff_t *ppos)
{
	// Implemented kmalloc as suggested
    char *output;
    size_t output_size;
    struct kmetrics_cpu_snapshot snapshot;
    int len = 0;
    ssize_t ret;

    //Use nr_cpu_ids instead of num_online_cpus() to ensure we allocate enough space for all possible CPU IDs driver might encounter, even if some are offline.
    output_size = KMETRICS_BASE_SIZE + ((size_t)nr_cpu_ids * KMETRICS_PER_CPU_SIZE);
    output = kmalloc(output_size, GFP_KERNEL);
    if (!output)
        return -ENOMEM;

    kmetrics_collect_snapshot(&snapshot);
    len = kmetrics_append_snapshot(output,
                                   output_size,
                                   len,
                                   &snapshot);
    len = kmetrics_append_cpu_freqs(output,
                                    output_size,
                                    len);
    len = kmetrics_append_cpu_usage(output,
                                    output_size,
                                    len);
    ret = simple_read_from_buffer(buf,
                                   count,
                                   ppos,
                                   output,
                                   len);
    kfree(output);
    
    return ret;
}

static long kmetrics_ioctl(struct file *file,
                           unsigned int cmd,
                           unsigned long arg)
{
    void __user *argp = (void __user *)arg;
    (void)file;

    /* Allows driver to support multiple commands. */
    switch (cmd) {
    case KMETRICS_GET_SNAPSHOT:
    {
        struct kmetrics_cpu_snapshot snapshot;
        struct kmetrics_snapshot_uapi user_snapshot = { 0 };

        kmetrics_collect_snapshot(&snapshot);

        user_snapshot.uptime_sec = snapshot.uptime_sec;
        user_snapshot.total_mib = snapshot.total_mib;
        user_snapshot.free_mib = snapshot.free_mib;
        user_snapshot.online_cpus = snapshot.online_cpus;
        user_snapshot.possible_cpus = snapshot.possible_cpus;
        user_snapshot.present_cpus = snapshot.present_cpus;

        if (copy_to_user(argp,
                         &user_snapshot,
                         sizeof(user_snapshot)))
            return -EFAULT;

        return 0;
    }

    case KMETRICS_GET_CPU:
    {
        struct kmetrics_cpu_uapi cpu_data = { 0 };
        unsigned int freq_khz;
        unsigned int cpu_id;

        // Userspace provides the requested CPU ID.
        if (copy_from_user(&cpu_data,
                           argp,
                           sizeof(cpu_data)))
            return -EFAULT;

        // Save the input field before clearing the output structure.
        
        cpu_id = cpu_data.cpu_id;

        // Do not trust output fields supplied by userspace.
        // The kernel owns usage_pct, freq_mhz, flags, and reserved.

        cpu_data = (struct kmetrics_cpu_uapi){ 0 };
        cpu_data.cpu_id = cpu_id;

        // Validate before using cpu_id as an array index.
        if (cpu_id >= nr_cpu_ids)
            return -EINVAL;

        // CPU ID is valid but that CPU is currently offline.
        if (!cpu_online(cpu_id))
            return -ENODEV;

        // cpu_samples[] is shared with the delayed-work sampler, so protect it with metrics_lock.
        mutex_lock(&metrics_lock);

        if (cpu_samples[cpu_id].initialized) {
            cpu_data.usage_pct =
                cpu_samples[cpu_id].usage_pct;

            cpu_data.flags |=
                KMETRICS_CPU_USAGE_VALID;
        }

        mutex_unlock(&metrics_lock);

        // CPU frequency is independent of cpu_samples[], so metrics_lock is not needed here.
        freq_khz = cpufreq_quick_get(cpu_id);

        if (freq_khz != 0) {
            cpu_data.freq_mhz =
                freq_khz / 1000;

            cpu_data.flags |=
                KMETRICS_CPU_FREQ_VALID;
        }

        if (copy_to_user(argp,
                         &cpu_data,
                         sizeof(cpu_data)))
            return -EFAULT;

        return 0;
    }

    case KMETRICS_GET_HISTORY:
    {
        struct kmetrics_history_uapi history = { 0 };
        struct kmetrics_history_entry entry;
        unsigned int i = 0;

        mutex_lock(&history_lock);

        while (i < KMETRICS_HISTORY_BATCH &&
            kfifo_get(&history_fifo, &entry)) {

            history.samples[i].timestamp_ns = entry.timestamp_ns;
            history.samples[i].cpu_id = entry.cpu_id;
            history.samples[i].usage_pct = entry.usage_pct;

            i++;
        }

        mutex_unlock(&history_lock);

        history.count = i;

        if (copy_to_user(argp,
                        &history,
                        sizeof(history)))
            return -EFAULT;

        return 0;
    }
    default:
        return -ENOTTY;
    }
}

static const struct file_operations kmetrics_fops = {
        .owner = THIS_MODULE,
	    .read = kmetrics_read,
        .unlocked_ioctl = kmetrics_ioctl,
};

static struct miscdevice kmetrics_device = {
        .minor = MISC_DYNAMIC_MINOR,
        .name = "kmetrics",
        .fops = &kmetrics_fops,
        .mode = 0444,
};

static int __init kmetrics_init(void)
{
    int ret;

    cpu_samples = kcalloc(nr_cpu_ids,
                          sizeof(*cpu_samples),
                          GFP_KERNEL);
    if (!cpu_samples)
        return -ENOMEM;
    INIT_DELAYED_WORK(&sample_work, kmetrics_sample_work);

    ret = misc_register(&kmetrics_device);
    if (ret) {
        pr_err("kmetrics: failed to register misc device: %d\n", ret);
        kfree(cpu_samples);

        return ret;
    }
    schedule_delayed_work(&sample_work, 0);
    pr_info("kmetrics: module loaded\n");
    return 0;
}

static void __exit kmetrics_exit(void)
{
    cancel_delayed_work_sync(&sample_work);
    misc_deregister(&kmetrics_device);
    kfree(cpu_samples);
    pr_info("kmetrics: module unloaded\n");
}

module_init(kmetrics_init);
module_exit(kmetrics_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vince Nguyen");
MODULE_DESCRIPTION("Computer system metrics kernel driver module. Keep building!");
