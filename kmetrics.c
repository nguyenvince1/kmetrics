// K-METRICS: A Linux kernel module that provides system metrics such as uptime, CPU counts, and memory usage through a character device interface.
// v0.2 by Vince Nguyen (nguyenvince1@gmail.com)
// Work in progress. This module is intended for educational purposes and may not be suitable for production use.

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

static ssize_t kmetrics_read(struct file *file,
                             char __user *buf,
                             size_t count,
                             loff_t *ppos)
{
    char output[1024];
	struct sysinfo mem;
    time64_t uptime; // kernel uptime in seconds
	unsigned int online_cpus; // cpus
	unsigned int possible_cpus;
	unsigned int present_cpus;
	unsigned int cpu0_freq_khz; // CPU0 frequency in kHz
	unsigned int cpu1_freq_khz; // CPU1 frequency in kHz
	unsigned int cpu2_freq_khz; // CPU2 frequency in kHz
	unsigned int cpu3_freq_khz; // CPU3 frequency in kHz
	u64 total_mib; // total memory in MiB
	u64 free_mib; // free memory in MiB
	int len;

    uptime = ktime_get_boottime_seconds();
    online_cpus = num_online_cpus();
	possible_cpus = num_possible_cpus();
	present_cpus = num_present_cpus();
	cpu0_freq_khz = cpufreq_quick_get(0);
	cpu1_freq_khz = cpufreq_quick_get(1);
	cpu2_freq_khz = cpufreq_quick_get(2);
	cpu3_freq_khz = cpufreq_quick_get(3);
	si_meminfo(&mem);

	total_mib = ((u64)mem.totalram * mem.mem_unit) /(1024 * 1024);
	free_mib =((u64)mem.freeram * mem.mem_unit) /(1024 * 1024);

	// AI: Find a way to make this scalable as driver grows...
    len = scnprintf(output,
                    sizeof(output),
                    "uptime_sec: %lld\n"
					"online_cpus: %u\n"
					"possible_cpus: %u\n"
					"present_cpus: %u\n"
					"total_memory_mib: %llu\n"
					"free_memory_mib: %llu\n"
					"cpu0_freq_khz: %u\n"
					"cpu1_freq_khz: %u\n"
					"cpu2_freq_khz: %u\n"
					"cpu3_freq_khz: %u\n",
                    (long long)uptime,
                    online_cpus,
                    possible_cpus,
                    present_cpus,
                    (unsigned long long)total_mib,
                    (unsigned long long)free_mib,
					(unsigned int)cpu0_freq_khz/1000,
					(unsigned int)cpu1_freq_khz/1000,
					(unsigned int)cpu2_freq_khz/1000,
					(unsigned int)cpu3_freq_khz/1000);

    return simple_read_from_buffer(buf,
                                   count,
                                   ppos,
                                   output,
                                   len);
}

static const struct file_operations kmetrics_fops = {
        .owner = THIS_MODULE,
	.read = kmetrics_read,
};

static struct miscdevice kmetrics_device = {
        .minor = MISC_DYNAMIC_MINOR,
        .name = "kmetrics",
        .fops = &kmetrics_fops,
        .mode = 0444,
};

static int __init kmetrics_init(void){
	int ret;

	ret = misc_register(&kmetrics_device);

	//  Misc. regsitration logic
	if (ret){
		pr_err("kmetrics: FAILED REGISTERING MISC DEVICE: %d\n", ret);
		return ret;
	};

	pr_info("kmetrics: module loaded\n");
	return 0;
};

static void __exit kmetrics_exit(void){
	misc_deregister(&kmetrics_device);
	pr_info("kmetrics: module unloaded\n");
};

module_init(kmetrics_init);
module_exit(kmetrics_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vince Nguyen");
MODULE_DESCRIPTION("Computer system metrics kernel driver module. Keep building!");


