#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/sched.h>  // Para for_each_process
#include <linux/fs.h>

#define __NR_luis_resumen_total 556 

// Definir la estructura para el resumen de memoria
struct memory_summary {
    unsigned long reserved_memory_mb;  // Memoria reservada total (en MB)
    unsigned long committed_memory_mb; // Memoria comprometida total (en MB)
};

// Función para obtener la memoria total de todos los procesos
SYSCALL_DEFINE1(luis_resumen_total, struct memory_summary __user *, summary) {
    struct memory_summary sys_summary = {0};
    struct task_struct *task;
    unsigned long reserved, committed;

    // Iterar sobre todos los procesos del sistema
    for_each_process(task) {
        // Asegurarse de que el proceso tiene un espacio de direcciones válido
        if (!task->mm) {
            continue;
        }

        // Obtener los valores de memoria del proceso
        reserved = get_mm_rss(task->mm);  // Memoria residente (RSS)
        committed = task->mm->total_vm;   // Memoria comprometida (Total VM)

        // Convertir de páginas a MB
        sys_summary.reserved_memory_mb += reserved * PAGE_SIZE / (1024 * 1024);
        sys_summary.committed_memory_mb += committed * PAGE_SIZE / (1024 * 1024);
    }

    // Copiar el resultado a la memoria del usuario
    if (copy_to_user(summary, &sys_summary, sizeof(sys_summary))) {
        return -EFAULT;
    }

    return 0; // Retorno exitoso
}
