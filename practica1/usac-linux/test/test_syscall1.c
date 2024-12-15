#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

// Definir el número de la syscall que hemos asignado
#define __NR_luis_capture_memory_snapshot 551

// Estructura para almacenar el estado de la memoria
struct memory_snapshot {
    unsigned long total_memory;
    unsigned long free_memory;
    unsigned long used_memory;
    unsigned long active_pages;
    unsigned long cache_pages;
    unsigned long swap_pages;
};

int main() {
    struct memory_snapshot snapshot;

    // Llamada a la syscall
    int result = syscall(__NR_luis_capture_memory_snapshot, &snapshot);

    // Verificación de la ejecución de la syscall
    if (result == 0) {
        printf("Total Memory: %lu\n", snapshot.total_memory);
        printf("Free Memory: %lu\n", snapshot.free_memory);
        printf("Used Memory: %lu\n", snapshot.used_memory);
        printf("Active Pages: %lu\n", snapshot.active_pages);
        printf("Cache Pages: %lu\n", snapshot.cache_pages);
        printf("Swap Pages: %lu\n", snapshot.swap_pages);
    } else {
        // Si ocurre un error
        perror("syscall");
    }

    return 0;
}


//gcc -o test_syscall1 test_syscall1.c
//  ./test_syscall1