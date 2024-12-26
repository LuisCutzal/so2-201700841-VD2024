#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/oom.h>
#include <linux/stat.h>
#include <linux/uaccess.h>  // Para la manipulación de datos de usuarios
#include <linux/mmzone.h>   // Para obtener la memoria total del sistema
#include <linux/sysinfo.h> // Para si_meminfo

#define KB (1024)
#define MB (1024 * 1024)

#define __NR_luis_recoleccion_general 555

// Estructura para almacenar la información de memoria del proceso
struct process_memory_info {
    unsigned long reserved_memory_kb;   // Memoria reservada en KB
    unsigned long committed_memory_kb;  // Memoria comprometida en KB
    unsigned long used_memory_kb;       // Memoria usada en KB
    int oom_score;                      // OOM score calculado manualmente
    int percentage_used_memory;         // Porcentaje de memoria usada
};

// Función de la syscall que obtiene la información de un proceso
SYSCALL_DEFINE2(luis_recoleccion_general, pid_t, pid, struct process_memory_info __user *, mem_info) {
    struct task_struct *task;
    struct mm_struct *mm;
    unsigned long reserved_memory, committed_memory, used_memory_kb;
    unsigned long total_memory_kb;
    int oom_score;
    unsigned long reserved_memory_kb;
    int percentage_used_memory;
    struct sysinfo si;  // Estructura para obtener información del sistema

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

    // Obtener la memoria utilizada (RSS, en KB)
    committed_memory = get_mm_rss(mm) * PAGE_SIZE / KB;  // memoria residente
    used_memory_kb = committed_memory;

    // Calcular el porcentaje de memoria utilizada
    if (reserved_memory > 0) {
        percentage_used_memory = (used_memory_kb * 100) / reserved_memory_kb;
    } else {
        percentage_used_memory = 0;  // Si no hay memoria reservada, el porcentaje es 0
    }

    // Obtener la memoria total del sistema (en KB) usando si_meminfo
    si_meminfo(&si);
    total_memory_kb = si.totalram * si.mem_unit / KB;

    // Calcular el OOM score manualmente
    if (total_memory_kb > 0) {
        oom_score = (used_memory_kb * 1000) / total_memory_kb;  // Escala por 1000
    } else {
        oom_score = 0;  // Si no hay memoria total, no se puede calcular
    }

    // Copiar los resultados a la estructura proporcionada por el espacio de usuario
    if (copy_to_user(mem_info, &(struct process_memory_info) {
        reserved_memory_kb, committed_memory, used_memory_kb, oom_score, percentage_used_memory
    }, sizeof(struct process_memory_info))) {
        return -EFAULT;  // Error al copiar los datos al espacio de usuario
    }

    // Retornar 0 si todo fue exitoso
    return 0;
}
