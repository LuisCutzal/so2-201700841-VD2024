#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <linux/mman.h>

SYSCALL_DEFINE1(luis_tamalloc, size_t, size) {
    if (size == 0) {
        return -EINVAL; // Retornar error si el tamaño es 0
    }

    // Redondear el tamaño a múltiplo de PAGE_SIZE
    size_t rounded_size = PAGE_ALIGN(size);

    // Reservar espacio de memoria sin asignar páginas físicas inmediatamente
    void *user_memory = vm_mmap(NULL, 0, rounded_size, PROT_READ | PROT_WRITE,
                                MAP_ANONYMOUS | MAP_PRIVATE | MAP_NORESERVE, 0);
    if (IS_ERR(user_memory)) {
        return PTR_ERR(user_memory); // Manejar errores en la asignación
    }

    // Inicializar la memoria a 0 (esto no tocará las páginas hasta el primer acceso)
    //memset(user_memory, 0, rounded_size);

    // Retornar la dirección del puntero al espacio reservado
    return (long)user_memory;
}
