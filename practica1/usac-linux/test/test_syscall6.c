#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

#define __NR_luis_resumen_total 556

// Estructura para almacenar la memoria total
struct memory_summary {
    unsigned long reserved_memory_mb; // Memoria reservada en MB
    unsigned long committed_memory_mb; // Memoria comprometida en MB
};

// Declaración de la syscall
long luis_resumen_total(struct memory_summary *summary) {
    return syscall(__NR_luis_resumen_total, summary);
}

// Función para imprimir el resumen de la memoria
void print_memory_summary(struct memory_summary summary) {
    printf("\nResumen de Memoria Total:\n");
    printf("+----------------------------+----------------------------+\n");
    printf("| Memoria Reservada (MB)     | Memoria Comprometida (MB)  |\n");
    printf("+----------------------------+----------------------------+\n");
    printf("| %-26lu | %-26lu |\n", summary.reserved_memory_mb, summary.committed_memory_mb);
    printf("+----------------------------+----------------------------+\n");
}

int main() {
    struct memory_summary summary;
    long result;

    // Llamar a la syscall para obtener el resumen de la memoria
    result = luis_resumen_total(&summary);
    if (result < 0) {
        perror("Error al obtener el resumen de memoria");
        return -1; // Salir en caso de error
    }

    // Mostrar el resumen de la memoria
    print_memory_summary(summary);

    return 0;
}
