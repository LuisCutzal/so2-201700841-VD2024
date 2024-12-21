#ifndef _SYSCALL3_H
#define _SYSCALL3_H

#include <linux/types.h>

struct io_throttle_stats {
    __u64 read_bytes;
    __u64 write_bytes;
    __u64 read_disk_bytes;
    __u64 write_disk_bytes;
    __u64 io_wait_time;  // En milisegundos
};

#endif
