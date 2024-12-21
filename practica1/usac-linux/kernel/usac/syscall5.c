#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/oom.h>
#include <linux/stat.h>
#include <linux/uaccess.h>  // Para la manipulación de datos de usuarios

#define KB (1024)
#define MB (1024 * 1024)

// Definir el número de la nueva syscall
#define __NR_luis_recoleccion_general 555

// Función de la syscall que obtiene la información de un proceso
SYSCALL_DEFINE1(luis_recoleccion_general, pid_t, pid) {
    struct task_struct *task;
    struct mm_struct *mm;
    unsigned long reserved_memory, committed_memory, used_memory_kb;
    int oom_score;
    unsigned long reserved_memory_kb;
    int percentage_used_memory;

    // Buscar el task_struct del proceso usando el PID
    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    if (!task) {
        return -ESRCH;  // Error si no encontramos el proceso
    }

    // Obtener la estructura mm del proceso, que contiene la memoria
    mm = task->mm;
    if (!mm) {
        return -EFAULT;  // Si el proceso no tiene espacio de direcciones
    }

    // Obtener la memoria reservada (en KB)
    reserved_memory = mm->total_vm * PAGE_SIZE / KB;  // total_vm es en páginas, convertimos a KB
    reserved_memory_kb = reserved_memory;

    // Obtener la memoria comprometida (en KB)
    committed_memory = mm->shared_vm * PAGE_SIZE / KB;  // memoria compartida, esto depende de tu caso específico
    used_memory_kb = committed_memory;

    // Obtener el OOM score
    oom_score = task->signal->oom_score_adj;

    // Calcular el porcentaje de memoria utilizada
    if (reserved_memory > 0) {
        percentage_used_memory = (used_memory_kb * 100) / reserved_memory_kb;
    } else {
        percentage_used_memory = 0;  // Si no hay memoria reservada, el porcentaje es 0
    }

    // Imprimir la información de memoria en el kernel log
    printk(KERN_INFO "PID: %d\n", pid);
    printk(KERN_INFO "Reserved Memory: %lu KB (%lu MB)\n", reserved_memory_kb, reserved_memory_kb / MB);
    printk(KERN_INFO "Committed Memory: %lu KB (%lu MB)\n", used_memory_kb, used_memory_kb / MB);
    printk(KERN_INFO "Used Memory: %d%% of reserved memory\n", percentage_used_memory);
    printk(KERN_INFO "OOM Score: %d\n", oom_score);

    // Retornar 0 si todo fue exitoso
    return 0;
}
