#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/io_uring.h>  // Para operaciones I/O
#include <linux/blkdev.h>    // Para estadísticas de disco
#include "syscall3.h"  // Estructura io_throttle_stats

/*
 * Implementación de la syscall para obtener las estadísticas de I/O de un proceso.
 */
SYSCALL_DEFINE1(luis_get_io_throttle, pid_t, pid, struct io_throttle_stats __user *, stats)
{
    struct task_struct *task;
    struct io_throttle_stats kernel_stats = {0};
    struct task_io_accounting ioac;

    // Obtener el proceso con el PID dado
    task = get_pid_task(find_vpid(pid), PIDTYPE_PID);
    if (!task) {
        pr_err("Proceso no encontrado\n");
        return -ESRCH;
    }

    // Obtener las estadísticas de I/O del proceso
    ioac = task->ioac;

    // Asignar los valores de las estadísticas a la estructura
    kernel_stats.read_bytes = ioac.read_bytes;
    kernel_stats.write_bytes = ioac.write_bytes;
    kernel_stats.read_disk_bytes = ioac.read_disk_bytes;
    kernel_stats.write_disk_bytes = ioac.write_disk_bytes;
    kernel_stats.io_wait_time = ioac.io_wait_time;

    // Copiar los datos al espacio de usuario
    if (copy_to_user(stats, &kernel_stats, sizeof(struct io_throttle_stats))) {
        pr_err("Error al copiar los datos al espacio de usuario.\n");
        return -EFAULT;
    }

    pr_info("Syscall get_io_throttle ejecutada exitosamente para el proceso %d.\n", pid);
    return 0;
}
