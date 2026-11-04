#ifndef KMETRICS_UAPI_H
#define KMETRICS_UAPI_H

#include <linux/types.h>
#include <linux/ioctl.h>


#define KMETRICS_IOC_MAGIC 'K'
#define KMETRICS_HISTORY_BATCH 32


/*
 * General system metrics.
 */
struct kmetrics_snapshot_uapi {
    __u64 uptime_sec;

    __u64 total_mib;
    __u64 free_mib;

    __u32 online_cpus;
    __u32 possible_cpus;
    __u32 present_cpus;

    __u32 reserved;
};

// Per-CPU metrics.
struct kmetrics_cpu_uapi {
    __u32 cpu_id;

    __u32 usage_pct;
    __u32 freq_mhz;

    __u32 flags;

    __u32 reserved[4];
};

struct kmetrics_history_sample_uapi {
    __u64 timestamp_ns;

    __u32 cpu_id;
    __u32 usage_pct;
};

struct kmetrics_history_uapi {
    __u32 count;
    __u32 reserved;

    struct kmetrics_history_sample_uapi
        samples[KMETRICS_HISTORY_BATCH];
};

// Validity flags for per-CPU metrics.
#define KMETRICS_CPU_USAGE_VALID   (1U << 0)
#define KMETRICS_CPU_FREQ_VALID    (1U << 1)


// ioctl commands
#define KMETRICS_GET_SNAPSHOT \
    _IOR(KMETRICS_IOC_MAGIC, 0x01, struct kmetrics_snapshot_uapi)

#define KMETRICS_GET_CPU \
    _IOWR(KMETRICS_IOC_MAGIC, 0x02, struct kmetrics_cpu_uapi)

#define KMETRICS_GET_HISTORY \
    _IOR(KMETRICS_IOC_MAGIC, 0x03, struct kmetrics_history_uapi)

#endif