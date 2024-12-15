/* SPDX-License-Identifier: GPL-2.0 */

#define __SYSCALL(nr, entry) .long entry
ENTRY(sys_call_table)
#include <asm/syscall_table.h>

/* Agrega la nueva syscall a la tabla */
.long sys_luis_capture_memory_snapshot

