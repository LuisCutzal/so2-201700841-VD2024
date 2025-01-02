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

void print_memory_summary(struct memory_summary summary) {
    const char* green = "\033[32m";   // Color verde
    const char* yellow = "\033[33m";  // Color amarillo
    const char* red = "\033[31m";     // Color rojo
    const char* reset = "\033[0m";    // Reset de color

    // Variables para el color de las columnas de memoria
    const char* reserved_color;
    const char* committed_color;

    // Lógica para asignar color a la memoria reservada
    if (summary.reserved_memory_mb < 1024) {
        reserved_color = green;  // Verde si es menos de 1GB
    } else if (summary.reserved_memory_mb < 2048) {
        reserved_color = yellow; // Amarillo si está entre 1GB y 2GB
    } else {
        reserved_color = red;    // Rojo si es más de 2GB
    }

    // Lógica para asignar color a la memoria comprometida
    if (summary.committed_memory_mb < 1024) {
        committed_color = green;  // Verde si es menos de 1GB
    } else if (summary.committed_memory_mb < 2048) {
        committed_color = yellow; // Amarillo si está entre 1GB y 2GB
    } else {
        committed_color = red;    // Rojo si es más de 2GB
    }

    // Imprimir la información con colores
    printf("\n%sResumen de Memoria Total:%s\n", green, reset);
    printf("+----------------------------------+------------------------------------+\n");
    printf("| %sMemoria Reservada (VmSize MB)%s    | %sMemoria Comprometida (VmRSS MB)%s    |\n", yellow, reset, yellow, reset);
    printf("+----------------------------------+------------------------------------+\n");
    printf("| %-26s%-4lu%s        | %-26s%-4lu%s       |\n", reserved_color, summary.reserved_memory_mb, reset, committed_color, summary.committed_memory_mb, reset);
    printf("+----------------------------------+------------------------------------+\n");
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



// gcc -o test_syscall6 test_syscall6.c
//  ./test_syscall6
