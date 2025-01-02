#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/capability.h>
#include <linux/list.h>

#define __NR_so2_update_memory_limit 559

// Estructura de la lista de limitaciones (debe coincidir con la definida en las otras syscalls)
struct memory_limitation {
    pid_t pid;
    size_t memory_limit; // Límite en KB
    struct list_head list; // Para enlazar en la lista
};

// Lista global de limitaciones
extern LIST_HEAD(memory_limit_list);
extern DEFINE_MUTEX(memory_limit_lock);

// Definición de la syscall
SYSCALL_DEFINE2(so2_update_memory_limit, pid_t, process_pid, size_t, memory_limit) {
    struct memory_limitation *entry;
    struct task_struct *task;
    struct mm_struct *mm;

    // Validar entradas
    if (process_pid <= 0 || memory_limit <= 0) {
        return -EINVAL; // PID o límite negativo
    }

    // Verificar permisos de usuario
    if (!capable(CAP_SYS_ADMIN)) {
        return -EPERM; // No tiene permisos de sudo
    }

    // Buscar el proceso con el PID proporcionado
    task = pid_task(find_vpid(process_pid), PIDTYPE_PID);
    if (!task) {
        return -ESRCH; // No existe el proceso con ese PID
    }

    // Verificar si el proceso ya excede el límite de memoria
    mm = task->mm;
    if (!mm) {
        return -ESRCH; // No se pudo obtener el espacio de memoria del proceso
    }
    if (get_mm_rss(mm) * PAGE_SIZE / 1024 > memory_limit) { // Convertimos a KB
        return -100; // El proceso ya excede el límite
    }

    // Bloquear la lista para acceso seguro
    mutex_lock(&memory_limit_lock);

    // Buscar el proceso en la lista de limitaciones
    list_for_each_entry(entry, &memory_limit_list, list) {
        if (entry->pid == process_pid) {
            // Proceso encontrado, actualizar el límite
            entry->memory_limit = memory_limit;
            mutex_unlock(&memory_limit_lock);
            return 0; // Éxito: Límite actualizado
        }
    }

    // Si no encontramos el proceso en la lista
    mutex_unlock(&memory_limit_lock);
    return -102; // El proceso no está en la lista de limitados
}
