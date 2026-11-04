Kmetrics version history tracking:
v0.1
────
Character device,
read(),
uptime,
CPU topology,
memory

v0.2
────
CPUFreq,
per-CPU iteration

v0.3
────
CPU utilization,
delayed_work,
persistent state,
kmalloc/kcalloc,
mutexes


v0.4
────
UAPI,
ioctl(),
_IOR,
_IOWR,
copy_to_user(),
copy_from_user(),
input validation,
userspace CLI

v0.5
────
kfifo circular history buffer,
timestamped CPU utilization samples,
overwrite-oldest behavior when full,
separate history mutex,
shorter metrics_lock critical sections,
KMETRICS_GET_HISTORY ioctl,
kmetrics-cli history

v0.6 (Next version)
────
Wait queues,
Blocking I/O,
O_NONBLOCK,
poll(),
Wakeups,
Producer/consumer synchronization,
Multiple device interfaces
--------------------------------------------------
How to use CLI:
gcc -Wall -Wextra -O2 -o kmetrics-cli kmetrics-cli.c
./kmetrics-cli history
./kmetrics-cli snapshot

To stress CPU usage:
Open one terminal and type: yes > /dev/null
On another terminal: enter
./kmetrics-cli cpu 0
./kmetrics-cli cpu 1
./kmetrics-cli cpu 2
./kmetrics-cli cpu 3
