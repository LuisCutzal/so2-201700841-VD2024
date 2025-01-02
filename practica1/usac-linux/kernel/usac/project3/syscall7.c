#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/capability.h>

#define __NR_so2_add_memory_limit 557

// Estructura para la lista de limitaciones
struct memory_limitation {
    pid_t pid;
    size_t memory_limit; // Límite en KB
    struct list_head list;
};

// Lista global de limitaciones
static LIST_HEAD(memory_limit_list);
static DEFINE_MUTEX(memory_limit_lock); // Para acceso concurrente a la lista

// Definición de la syscall
SYSCALL_DEFINE2(so2_add_memory_limit, pid_t, process_pid, size_t, memory_limit) {
    struct memory_limitation *entry;
    struct task_struct *task;
    struct mm_struct *mm;

    // Validar entrada
    if (process_pid <= 0 || memory_limit <= 0) {
        set_errno(EINVAL);
        return -EINVAL;
    }

    // Verificar permisos de usuario
    if (!capable(CAP_SYS_ADMIN)) {
        set_errno(EPERM);
        return -EPERM;
    }

    // Buscar el proceso
    task = pid_task(find_vpid(process_pid), PIDTYPE_PID);
    if (!task) {
        set_errno(ESRCH);
        return -ESRCH;
    }

    // Verificar memoria usada
    mm = task->mm;
    if (!mm) {
        set_errno(ESRCH);
        return -ESRCH;
    }
    if (get_mm_rss(mm) * PAGE_SIZE / 1024 > memory_limit) { // Convertimos a KB
        set_errno(100); // Código de error personalizado
        return -100;
    }

    // Bloquear la lista para acceso seguro
    mutex_lock(&memory_limit_lock);

    // Verificar si ya existe
    list_for_each_entry(entry, &memory_limit_list, list) {
        if (entry->pid == process_pid) {
            mutex_unlock(&memory_limit_lock);
            set_errno(101); // Código de error personalizado
            return -101;
        }
    }

    // Crear nueva entrada
    entry = kmalloc(sizeof(*entry), GFP_KERNEL);
    if (!entry) {
        mutex_unlock(&memory_limit_lock);
        set_errno(ENOMEM);
        return -ENOMEM;
    }
    entry->pid = process_pid;
    entry->memory_limit = memory_limit;
    INIT_LIST_HEAD(&entry->list);

    // Agregar a la lista
    list_add(&entry->list, &memory_limit_list);

    // Desbloquear la lista
    mutex_unlock(&memory_limit_lock);

    return 0; // Éxito
}
