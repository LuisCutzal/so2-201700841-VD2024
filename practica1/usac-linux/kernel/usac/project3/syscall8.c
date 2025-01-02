// #include <linux/kernel.h>
// #include <linux/syscalls.h>
// #include <linux/mm.h>
// #include <linux/slab.h>
// #include <linux/uaccess.h>
// #include <linux/list.h>

// #define __NR_so2_get_memory_limits 558

// // Estructura de la lista de limitaciones (ya definida en la syscall anterior)
// struct memory_limitation {
//     pid_t pid;
//     size_t memory_limit; // Límite en KB
// };

// // Lista global de limitaciones (ya definida en la syscall anterior)
// extern LIST_HEAD(memory_limit_list);
// extern DEFINE_MUTEX(memory_limit_lock);

// SYSCALL_DEFINE3(so2_get_memory_limits, 
//                 struct memory_limitation __user *, u_processes_buffer, 
//                 size_t, max_entries, 
//                 int __user *, processes_returned) {
//     struct memory_limitation *kernel_buffer;
//     struct memory_limitation *entry;
//     size_t count = 0;
//     int result;

//     // Validar entradas
//     if (max_entries <= 0 || !u_processes_buffer || !processes_returned) {
//         return -EINVAL;
//     }

//     // Bloquear lista para acceso seguro
//     mutex_lock(&memory_limit_lock);

//     // Contar los procesos limitados
//     list_for_each_entry(entry, &memory_limit_list, list) {
//         count++;
//     }

//     // Ajustar el número máximo al límite del buffer proporcionado
//     if (count > max_entries) {
//         count = max_entries;
//     }

//     // Reservar espacio en el kernel para el buffer
//     kernel_buffer = kmalloc_array(count, sizeof(struct memory_limitation), GFP_KERNEL);
//     if (!kernel_buffer) {
//         mutex_unlock(&memory_limit_lock);
//         return -ENOMEM;
//     }

//     // Copiar los procesos limitados al buffer del kernel
//     count = 0;
//     list_for_each_entry(entry, &memory_limit_list, list) {
//         if (count >= max_entries) {
//             break;
//         }
//         kernel_buffer[count].pid = entry->pid;
//         kernel_buffer[count].memory_limit = entry->memory_limit;
//         count++;
//     }

//     // Desbloquear la lista
//     mutex_unlock(&memory_limit_lock);

//     // Copiar datos del kernel al espacio de usuario
//     result = copy_to_user(u_processes_buffer, kernel_buffer, count * sizeof(struct memory_limitation));
//     if (result) {
//         kfree(kernel_buffer);
//         return -EFAULT;
//     }

//     // Copiar la cantidad de procesos retornados al espacio de usuario
//     result = put_user(count, processes_returned);
//     if (result) {
//         kfree(kernel_buffer);
//         return -EFAULT;
//     }

//     // Liberar el buffer del kernel
//     kfree(kernel_buffer);

//     return 0; // Éxito
// }