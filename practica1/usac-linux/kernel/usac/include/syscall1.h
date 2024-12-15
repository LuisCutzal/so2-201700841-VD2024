#ifndef _LINUX_SYSCALL1_H
#define _LINUX_SYSCALL1_H

#include <linux/types.h> // Para tipos estándar como `unsigned long`

/*
 * Estructura utilizada para capturar el estado de memoria del sistema.
 * Los campos representan diferentes métricas de memoria en bytes o páginas.
 */
struct memory_snapshot {
    unsigned long total_memory;   // Memoria total del sistema
    unsigned long free_memory;    // Memoria libre disponible
    unsigned long used_memory;    // Memoria utilizada
    unsigned long active_pages;   // Número de páginas activas
    unsigned long cache_pages;    // Páginas usadas como caché
    unsigned long swap_pages;     // Páginas en swap
};

/*
 * Declaración de la syscall para capturar el estado de memoria.
 * Esta función será implementada en el kernel y se llama desde espacio de usuario.
 */
asmlinkage long sys_luis_capture_memory_snapshot(struct memory_snapshot __user *snapshot);

#endif // _LINUX_SYSCALL1_H
