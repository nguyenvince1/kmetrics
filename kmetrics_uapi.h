#ifndef KMETRICS_UAPI_H
#define KMETRICS_UAPI_H

#include <linux/types.h>
#include <linux/ioctl.h>

#define KMETRICS_IOC_MAGIC 'K'

// __u32 and __u64 mean fixed-width integer types. Safer for an ABI shared between kernel and userspace.
// Why __u32 and __u64? For UAPIs, should we avoid using long and unsigned long?
struct kmetrics_snapshot_uapi {
    __u64 uptime_sec;

    __u64 total_mib;
    __u64 free_mib;

    __u32 online_cpus;
    __u32 possible_cpus;
    __u32 present_cpus;

    __u32 reserved;
};

// IOR creates ioctol command number. Magic number is 0x01, 'K' is ioctl family.
// IOR = "read" from kernel to userspace. The last argument is the type of data being transferred.
#define KMETRICS_GET_SNAPSHOT \
    _IOR(KMETRICS_IOC_MAGIC, 0x01, struct kmetrics_snapshot_uapi)

#endif