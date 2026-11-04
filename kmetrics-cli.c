// CLI for kmetrics module
// This program is a command-line interface (CLI) for the kmetrics kernel module.
// It opens the /dev/kmetrics device, retrieves a snapshot of system metrics using an ioctl call,
// and prints the metrics to the console.

// Normal userspace executable

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "kmetrics_uapi.h"


static int show_snapshot(int fd)
{
    struct kmetrics_snapshot_uapi metrics = { 0 };

    if (ioctl(fd,
              KMETRICS_GET_SNAPSHOT,
              &metrics) < 0) {
        perror("KMETRICS_GET_SNAPSHOT");
        return EXIT_FAILURE;
    }

    printf("KMetrics Snapshot\n");
    printf("-----------------\n");

    printf("Uptime:        %llu seconds\n",
           (unsigned long long)metrics.uptime_sec);

    printf("Online CPUs:   %u\n",
           metrics.online_cpus);

    printf("Possible CPUs: %u\n",
           metrics.possible_cpus);

    printf("Present CPUs:  %u\n",
           metrics.present_cpus);

    printf("Total memory:  %llu MiB\n",
           (unsigned long long)metrics.total_mib);

    printf("Free memory:   %llu MiB\n",
           (unsigned long long)metrics.free_mib);

    return EXIT_SUCCESS;
}

static int show_cpu(int fd, unsigned int cpu_id)
{
    struct kmetrics_cpu_uapi cpu = { 0 };

    cpu.cpu_id = cpu_id;

    if (ioctl(fd, KMETRICS_GET_CPU, &cpu) < 0) {
        perror("KMETRICS_GET_CPU");
        return EXIT_FAILURE;
    }

    printf("CPU %u\n", cpu.cpu_id);
    printf("-----\n");

    if (cpu.flags & KMETRICS_CPU_USAGE_VALID)
        printf("Usage:     %u%%\n",
               cpu.usage_pct);
    else
        printf("Usage: unavailable\n");

    if (cpu.flags & KMETRICS_CPU_FREQ_VALID)
        printf("Frequency: %u MHz\n",
               cpu.freq_mhz);
    else
        printf("Frequency: unavailable\n");
    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    int fd;
    int ret;

    fd = open("/dev/kmetrics", O_RDONLY);

    if (fd < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    if (argc == 2 && strcmp(argv[1], "snapshot") == 0) {
        ret = show_snapshot(fd);
    } else if (argc == 3 && strcmp(argv[1], "cpu") == 0) {
        char *end;
        unsigned long cpu_id;

        cpu_id = strtoul(argv[2],
                         &end,
                         10);

        if (*end != '\0') {
            fprintf(stderr,
                    "Invalid CPU ID: %s\n",
                    argv[2]);

            close(fd);
            return EXIT_FAILURE;
        }
        ret = show_cpu(fd,
                       (unsigned int)cpu_id);
    } else {
        fprintf(stderr,
                "Usage:\n"
                "  %s snapshot\n"
                "  %s cpu <id>\n",
                argv[0],
                argv[0]);
        ret = EXIT_FAILURE;
    }

    close(fd);
    return ret;
}