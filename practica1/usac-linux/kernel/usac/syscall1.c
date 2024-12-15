#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <linux/vmstat.h>    // Para global_node_page_state
#include <linux/node.h>      // Para obtener información de nodos
#include "syscall1.h"  // Estructura memory_snapshot

/*
 * Implementación de la syscall para capturar el estado de memoria.
 */
SYSCALL_DEFINE1(luis_capture_memory_snapshot, struct memory_snapshot __user *, snapshot)
{
    struct memory_snapshot kernel_snapshot;

    // Calcular memoria total en bytes
    kernel_snapshot.total_memory = totalram_pages() << PAGE_SHIFT;

    // Calcular memoria libre en bytes usando global_node_page_state
    kernel_snapshot.free_memory = (long)global_node_page_state((enum node_stat_item)NR_FREE_PAGES) << PAGE_SHIFT;

    // Calcular memoria utilizada
    kernel_snapshot.used_memory = kernel_snapshot.total_memory - kernel_snapshot.free_memory;

    // Obtener páginas activas (usa NR_INACTIVE_FILE si NR_INACTIVE_PAGES no está disponible)
    kernel_snapshot.active_pages = global_node_page_state(NR_INACTIVE_FILE);

    // Obtener páginas de caché
    kernel_snapshot.cache_pages = global_node_page_state(NR_FILE_PAGES);

    // Obtener las páginas de swap (verifica si ZSWPIN y ZSWPOUT son correctas)
    kernel_snapshot.swap_pages = global_node_page_state(ZSWPIN) + global_node_page_state(ZSWPOUT);

    // Copiar los datos al espacio de usuario
    if (copy_to_user(snapshot, &kernel_snapshot, sizeof(struct memory_snapshot))) {
        pr_err("Error al copiar datos al espacio de usuario.\n");
        return -EFAULT;
    }

    pr_info("Syscall luis_capture_memory_snapshot ejecutada exitosamente.\n");
    return 0;
}
