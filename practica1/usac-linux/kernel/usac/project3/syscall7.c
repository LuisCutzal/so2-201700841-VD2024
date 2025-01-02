#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/capability.h>

#define __NR_so2_add_memory_limit 557

// Estructura para representar una limitación de memoria
struct memory_limitation {
    pid_t pid;                  // ID del proceso
    size_t memory_limit;        // Límite de memoria en KB
    struct list_head list;      // Enlace para la lista enlazada
};

// Lista global para almacenar las limitaciones de memoria
static LIST_HEAD(memory_limit_list);

// Mutex para asegurar acceso seguro a la lista
static DEFINE_MUTEX(memory_limit_lock);

// Definición de la syscall
SYSCALL_DEFINE2(so2_add_memory_limit, pid_t, process_pid, size_t, memory_limit) {
    struct memory_limitation *entry;
    struct task_struct *task;
    struct mm_struct *mm;

    // Validación de parámetros de entrada
    if (process_pid <= 0 || memory_limit <= 0) {
        return -EINVAL; // Parámetros inválidos
    }

    // Verificar permisos de usuario (requiere privilegios de administrador)
    if (!capable(CAP_SYS_ADMIN)) {
        return -EPERM; // Permiso denegado
    }

    // Buscar la estructura del proceso por PID
    task = pid_task(find_vpid(process_pid), PIDTYPE_PID);
    if (!task) {
        return -ESRCH; // Proceso no encontrado
    }

    // Obtener la estructura mm (gestión de memoria) del proceso
    mm = task->mm;
    if (!mm) {
        return -ESRCH; // El proceso no tiene mm asociado
    }

    // Verificar si el proceso ya está excediendo el límite de memoria solicitado
    if (get_mm_rss(mm) * PAGE_SIZE / 1024 > memory_limit) { // Convertir páginas a KB
        return -100; // Código de error personalizado
    }

    // Bloquear el mutex para acceso seguro a la lista
    mutex_lock(&memory_limit_lock);

    // Verificar si el proceso ya tiene una limitación configurada
    list_for_each_entry(entry, &memory_limit_list, list) {
        if (entry->pid == process_pid) {
            mutex_unlock(&memory_limit_lock);
            return -101; // Código de error personalizado: Limitación ya existente
        }
    }

    // Crear una nueva entrada para la limitación de memoria
    entry = kmalloc(sizeof(*entry), GFP_KERNEL);
    if (!entry) {
        mutex_unlock(&memory_limit_lock);
        return -ENOMEM; // Memoria insuficiente
    }

    // Configurar los datos de la nueva entrada
    entry->pid = process_pid;
    entry->memory_limit = memory_limit;
    INIT_LIST_HEAD(&entry->list);

    // Agregar la nueva entrada a la lista de limitaciones
    list_add(&entry->list, &memory_limit_list);

    // Desbloquear el mutex
    mutex_unlock(&memory_limit_lock);

    return 0; // Éxito
}
