// CLI for kmetrics module
// This program is a command-line interface (CLI) for the kmetrics kernel module.
// It opens the /dev/kmetrics device, retrieves a snapshot of system metrics using an ioctl call,
// and prints the metrics to the console.

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "kmetrics_uapi.h"

int main(void)
{
    int fd;
    struct kmetrics_snapshot_uapi metrics = { 0 };

    fd = open("/dev/kmetrics", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    if (ioctl(fd, KMETRICS_GET_SNAPSHOT, &metrics) < 0) {
        perror("ioctl");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("KMetrics Snapshot\n");
    printf("-----------------\n");
    printf("Uptime:        %llu seconds\n", (unsigned long long)metrics.uptime_sec);
    printf("Online CPUs:   %u\n", metrics.online_cpus);
    printf("Possible CPUs: %u\n", metrics.possible_cpus);
    printf("Present CPUs:  %u\n", metrics.present_cpus);
    printf("Total memory:  %llu MiB\n", (unsigned long long)metrics.total_mib);
    printf("Free memory:   %llu MiB\n", (unsigned long long)metrics.free_mib);

    close(fd);
    return EXIT_SUCCESS;
}