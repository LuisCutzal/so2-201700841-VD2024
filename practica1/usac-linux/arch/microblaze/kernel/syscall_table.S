/* SPDX-License-Identifier: GPL-2.0 */

#define __SYSCALL(nr, entry) .long entry
ENTRY(sys_call_table)
#include <asm/syscall_table.h>

/* Agrega la nueva syscall a la tabla */
.long sys_luis_capture_memory_snapshot

/*Esto es para el proyecto 2 la segunda syscall*/
.long sys_luis_recoleccion_general
.long sys_luis_resumen_total

/*Esto es para el proyecto 3 todas las syscalls*/
.long sys_so2_add_memory_limit
.long sys_so2_get_memory_limits
.long sys_so2_update_memory_limit
.long sys_so2_remove_memory_limit