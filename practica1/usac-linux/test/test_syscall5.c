#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

#define __NR_luis_recoleccion_general 555

// Estructura para almacenar la información de memoria
struct process_memory_info {
    unsigned long reserved_memory_kb;
    unsigned long committed_memory_kb;
    unsigned long used_memory_kb;
    int oom_score;
    int percentage_used_memory;
};

// Declaración de la syscall
long luis_recoleccion_general(pid_t pid, struct process_memory_info *mem_info) {
    return syscall(__NR_luis_recoleccion_general, pid, mem_info);
}

// Función para imprimir todos los procesos en una tabla
void print_all_memory_info() {
    // Colores ANSI
    const char* green = "\033[32m";
    const char* yellow = "\033[33m";
    const char* red = "\033[31m";
    const char* reset = "\033[0m";

    // Encabezado de la tabla
    printf("\n%s+-------+----------------------+------------------------+------------------------+------------------+-------------------------+%s\n", green, reset);
    printf("| %-5s | %-20s | %-22s | %-22s | %-16s | %-23s |\n", "PID", "Reservada (KB)", "Comprometida (KB)", "Usada (KB)", "OOM Score", "Porcentaje Usada (%)");
    printf("+-------+----------------------+------------------------+------------------------+------------------+-------------------------+\n");

    struct process_memory_info mem_info;
    long result;

    // Iterar sobre un rango razonable de PIDs
    for (pid_t pid = 1; pid <= 32768; pid++) {
        // Llamar a la syscall para obtener la recolección de memoria
        result = luis_recoleccion_general(pid, &mem_info);
        if (result < 0) {
            continue; // Ignorar procesos donde falle la syscall
        }

        // Calcular el color del porcentaje de memoria usada
        const char* used_color;
        if (mem_info.percentage_used_memory < 50) {
            used_color = green;
        } else if (mem_info.percentage_used_memory < 80) {
            used_color = yellow;
        } else {
            used_color = red;
        }

        // Imprimir la información del proceso en una fila
        printf("| %-5d | %-20lu | %-22lu | %-22lu | %-16d | %s%-3d%%%s                |\n",
               pid,
               mem_info.reserved_memory_kb,
               mem_info.committed_memory_kb,
               mem_info.used_memory_kb,
               mem_info.oom_score,
               used_color, mem_info.percentage_used_memory, reset);
    }

    // Cierre de la tabla
    printf("+-------+----------------------+------------------------+------------------------+------------------+-------------------------+\n");
}

int main() {
    pid_t pid;
    struct process_memory_info mem_info;
    long result;

    while (1) {
        // Solicitar el PID
        printf("Ingrese el PID del proceso (0 para mostrar todos los procesos): ");
        scanf("%d", &pid);

        if (pid == 0) {
            // Si el PID es 0, mostrar todos los procesos en una tabla
            print_all_memory_info();
        } else if (pid < 0) {
            // Verificar si se ingresa un PID inválido
            printf("PID no válido.\n");
            printf("Saliendo...\n");
            return 0;
        } else {
            // Si se ingresa un PID específico
            result = luis_recoleccion_general(pid, &mem_info);
            if (result < 0) {
                printf("Proceso con PID %d terminado o no encontrado.\n", pid);
            } else {
                // Mostrar la información de memoria para el PID ingresado
                printf("\n");
                printf("Información de memoria para el PID %d:\n", pid);
                printf("Reservada: %lu KB\n", mem_info.reserved_memory_kb);
                printf("Comprometida: %lu KB\n", mem_info.committed_memory_kb);
                printf("Usada: %lu KB\n", mem_info.used_memory_kb);
                printf("OOM Score: %d\n", mem_info.oom_score);
                printf("Porcentaje de memoria usada: %d%%\n", mem_info.percentage_used_memory);
            }
        }
        printf("\n");
    }

    return 0;
}

// gcc -o test_syscall5 test_syscall5.c
//  ./test_syscall5
