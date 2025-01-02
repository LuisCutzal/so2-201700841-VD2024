#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>  // Para acceder a las estructuras de proceso
#include <linux/uidgid.h> // Para comprobar si es un sudoer (root o con privilegios)
#include <linux/errno.h>
#include <linux/list.h>
#include <linux/slab.h>

// Estructura de memoria de los procesos
struct memory_limitation {
    pid_t pid;
    size_t memory_limit;
    struct list_head list;
};

// Lista global de procesos con límite de memoria
static LIST_HEAD(memory_limit_list);

// Mutex para sincronización
static DEFINE_MUTEX(memory_limit_lock);

// Función para verificar si el usuario es sudoer
static bool is_sudoer(void) {
    struct cred *cred = current_cred();
    return (cred->uid.val == 0 || cred->euid.val == 0);  // Verificar si el UID o EUID es root
}

// La syscall para eliminar el límite de memoria de un proceso
long SYSCALL_DEFINE1(so2_remove_memory_limit, pid_t, process_pid) {
    struct memory_limitation *entry, *tmp;
    bool found = false;

    // Verificar si el PID es válido
    if (process_pid <= 0) {
        return -EINVAL;
    }

    // Verificar si el usuario tiene privilegios de sudoer
    if (!is_sudoer()) {
        return -EPERM;
    }

    // Buscar el proceso en la lista de límites de memoria
    mutex_lock(&memory_limit_lock);

    list_for_each_entry_safe(entry, tmp, &memory_limit_list, list) {
        if (entry->pid == process_pid) {
            // Eliminar la entrada de la lista
            list_del(&entry->list);
            kfree(entry);  // Liberar la memoria de la estructura del proceso
            found = true;
            break;
        }
    }

    mutex_unlock(&memory_limit_lock);

    // Si no se encuentra el proceso en la lista, retornar error
    if (!found) {
        return -102;  // Error customizado para proceso no encontrado
    }

    // Si el PID no existe, retornar error ESRCH
    if (found && !process_pid) {
        return -ESRCH;
    }

    return 0;  // Éxito
}
