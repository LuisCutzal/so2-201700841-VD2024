#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>

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

// Función para imprimir las estadísticas de memoria en formato de tabla con colores
void print_memory_info(struct process_memory_info mem_info, pid_t pid) {
    // Colores ANSI
    const char* green = "\033[32m";
    const char* yellow = "\033[33m";
    const char* red = "\033[31m";
    const char* reset = "\033[0m";

    // Calcular el color de porcentaje de memoria usada
    const char* used_color;
    if (mem_info.percentage_used_memory < 50) {
        used_color = green;
    } else if (mem_info.percentage_used_memory < 80) {
        used_color = yellow;
    } else {
        used_color = red;  // Rojo para alta utilización
    }

    // Imprimir la tabla con colores
    printf("\n%sInformación de memoria para el PID %d:%s\n", green, pid, reset);
    printf("+----------------------+------------------------+------------------------+------------------+-------------------------+\n");

    // Encabezado sin colores
    printf("|   %sReservada%s          |   %sComprometida%s         |   %sUsada%s                | %sOOM Score%s        | %sPorcentaje Usada%s        |\n", 
           green, reset, green, reset, green, reset, green, reset,green, reset);  // Primera fila con colores
    printf("+----------------------+------------------------+------------------------+------------------+-------------------------+\n");

    // Imprimir los valores con los colores aplicados solo a los valores dinámicos
    printf("| %-20lu | %-22lu | %-22lu | %-16d | %s%-3d%%%s                    |\n",
           mem_info.reserved_memory_kb,
           mem_info.committed_memory_kb,
           mem_info.used_memory_kb,
           mem_info.oom_score,
           used_color, mem_info.percentage_used_memory, reset); // Datos de la fila
    printf("+----------------------+------------------------+------------------------+------------------+-------------------------+\n");
}

// Función para obtener todos los PIDs del sistema
void get_all_pids_and_show_memory() {
    DIR *dir = opendir("/proc");
    struct dirent *entry;
    struct process_memory_info mem_info;
    pid_t pid;
    long result;

    if (dir == NULL) {
        perror("No se pudo abrir el directorio /proc");
        return;
    }

    // Leer todos los directorios en /proc
    while ((entry = readdir(dir)) != NULL) {
        // Verificar si el nombre del directorio es un número (PID)
        if (isdigit(entry->d_name[0])) {
            pid = atoi(entry->d_name);  // Convertir el nombre a un PID

            // Llamar a la syscall para obtener la recolección de memoria
            result = luis_recoleccion_general(pid, &mem_info);
            if (result < 0) {
                printf("No se pudo obtener la información de memoria para el PID %d\n", pid);
                continue;
            }

            // Mostrar la información de memoria para ese proceso
            print_memory_info(mem_info, pid);
        }
    }

    closedir(dir);
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
            // Si el PID es 0, mostrar todos los procesos
            get_all_pids_and_show_memory();
        } else if (pid < 0) {
            // Verificar si se ingresa un PID inválido
            printf("PID no válido.\n");
            printf("Saliendo...\n");
            return 0;
        }else {
            // Si se ingresa un PID específico
            result = luis_recoleccion_general(pid, &mem_info);
            if (result < 0) {
                printf("Proceso con PID %d terminado o no encontrado.\n", pid);
            } else {
                // Mostrar la información de memoria para el PID ingresado
                print_memory_info(mem_info, pid);
            }
        }

        // Preguntar nuevamente
        printf("\n");
    }

    return 0;
}



// gcc -o test_syscall5 test_syscall5.c
//  ./test_syscall5
