#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/list.h>
#include <linux/capability.h>

#define __NR_so2_add_memory_limit 557
#define __NR_so2_get_memory_limits 558
#define __NR_so2_update_memory_limit 559
#define __NR_so2_remove_memory_limit 560

// Estructura para la lista de limitaciones
struct memory_limitation {
    pid_t pid;
    size_t memory_limit; // Límite en KB
    struct list_head list;
};

// Lista global de limitaciones y mutex
static LIST_HEAD(memory_limit_list);
static DEFINE_MUTEX(memory_limit_lock);

// Syscall 1: Agregar límite de memoria
long SYSCALL_DEFINE2(so2_add_memory_limit, pid_t, process_pid, size_t, memory_limit) {
    struct memory_limitation *entry;
    struct task_struct *task;
    struct mm_struct *mm;

    // Validar entrada
    if (process_pid <= 0 || memory_limit <= 0) {
        return -EINVAL;
    }

    // Verificar permisos de usuario
    if (!capable(CAP_SYS_ADMIN)) {
        return -EPERM;
    }

    // Buscar el proceso
    task = pid_task(find_vpid(process_pid), PIDTYPE_PID);
    if (!task) {
        return -ESRCH;
    }

    // Verificar memoria usada
    mm = task->mm;
    if (!mm) {
        return -ESRCH;
    }
    if (get_mm_rss(mm) * PAGE_SIZE / 1024 > memory_limit) { // Convertimos a KB
        return -100; // Código de error personalizado
    }

    // Bloquear la lista para acceso seguro
    mutex_lock(&memory_limit_lock);

    // Verificar si ya existe
    list_for_each_entry(entry, &memory_limit_list, list) {
        if (entry->pid == process_pid) {
            mutex_unlock(&memory_limit_lock);
            return -101; // Código de error personalizado
        }
    }

    // Crear nueva entrada
    entry = kmalloc(sizeof(*entry), GFP_KERNEL);
    if (!entry) {
        mutex_unlock(&memory_limit_lock);
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

// Syscall 2: Obtener lista de procesos limitados
long SYSCALL_DEFINE3(so2_get_memory_limits, 
                     struct memory_limitation*, u_processes_buffer, 
                     size_t, max_entries, 
                     int*, processes_returned) {
    struct memory_limitation *kernel_buffer;
    struct memory_limitation *entry;
    size_t count = 0;
    int result;

    // Validar entradas
    if (max_entries <= 0 || !u_processes_buffer || !processes_returned) {
        return -EINVAL;
    }

    // Bloquear lista para acceso seguro
    mutex_lock(&memory_limit_lock);

    // Contar los procesos limitados
    list_for_each_entry(entry, &memory_limit_list, list) {
        count++;
    }

    // Ajustar el número máximo al límite del buffer proporcionado
    if (count > max_entries) {
        count = max_entries;
    }

    // Reservar espacio en el kernel para el buffer
    kernel_buffer = kmalloc_array(count, sizeof(struct memory_limitation), GFP_KERNEL);
    if (!kernel_buffer) {
        mutex_unlock(&memory_limit_lock);
        return -ENOMEM;
    }

    // Copiar los procesos limitados al buffer del kernel
    count = 0;
    list_for_each_entry(entry, &memory_limit_list, list) {
        if (count >= max_entries) {
            break;
        }
        kernel_buffer[count].pid = entry->pid;
        kernel_buffer[count].memory_limit = entry->memory_limit;
        count++;
    }

    // Desbloquear la lista
    mutex_unlock(&memory_limit_lock);

    // Copiar datos del kernel al espacio de usuario
    result = copy_to_user(u_processes_buffer, kernel_buffer, count * sizeof(struct memory_limitation));
    if (result) {
        kfree(kernel_buffer);
        return -EFAULT;
    }

    // Copiar la cantidad de procesos retornados al espacio de usuario
    result = put_user(count, processes_returned);
    if (result) {
        kfree(kernel_buffer);
        return -EFAULT;
    }

    // Liberar el buffer del kernel
    kfree(kernel_buffer);

    return 0; // Éxito
}

// syscall 3 Actualizar el límite de un proceso
long SYSCALL_DEFINE2(so2_update_memory_limit, pid_t, process_pid, size_t, memory_limit) {
    struct memory_limitation *entry;
    struct task_struct *task;
    struct mm_struct *mm;

    // Validaciones de entrada
    if (process_pid <= 0 || memory_limit <= 0) {
        set_errno(EINVAL);
        return -EINVAL;
    }

    // Verificar permisos de usuario
    if (!capable(CAP_SYS_ADMIN)) {
        set_errno(EPERM);
        return -EPERM;
    }

    // Buscar el proceso en el sistema
    task = pid_task(find_vpid(process_pid), PIDTYPE_PID);
    if (!task) {
        set_errno(ESRCH);
        return -ESRCH;
    }

    // Obtener memoria usada por el proceso
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

    // Buscar en la lista global
    list_for_each_entry(entry, &memory_limit_list, list) {
        if (entry->pid == process_pid) {
            // Actualizar el límite de memoria
            entry->memory_limit = memory_limit;
            mutex_unlock(&memory_limit_lock);
            return 0; // Éxito
        }
    }

    // Desbloquear la lista si no se encontró el proceso
    mutex_unlock(&memory_limit_lock);

    // Proceso no encontrado en la lista
    set_errno(102); // Código de error personalizado
    return -102;
}

// Syscall 4: Remover el límite de memoria de un proceso
long SYSCALL_DEFINE1(so2_remove_memory_limit, pid_t, process_pid) {
    struct memory_limitation *entry, *tmp;
    int found = 0;

    // Validar que el PID sea positivo
    if (process_pid <= 0) {
        set_errno(EINVAL);
        return -EINVAL;
    }

    // Verificar si el usuario es un sudoer
    if (!capable(CAP_SYS_ADMIN)) {
        set_errno(EPERM);
        return -EPERM;
    }

    // Bloquear la lista para acceso seguro
    mutex_lock(&memory_limit_lock);

    // Buscar el proceso en la lista
    list_for_each_entry_safe(entry, tmp, &memory_limit_list, list) {
        if (entry->pid == process_pid) {
            // Encontrado el proceso, eliminarlo
            list_del(&entry->list);
            kfree(entry);  // Liberar la memoria asignada
            found = 1;
            break;
        }
    }

    // Desbloquear la lista
    mutex_unlock(&memory_limit_lock);

    // Si no se encontró el proceso en la lista, devolver error -102
    if (!found) {
        set_errno(ESRCH);
        return -102;
    }

    return 0;  // Éxito
}