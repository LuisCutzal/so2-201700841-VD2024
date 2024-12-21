#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

#define __NR_luis_recoleccion_general 555

// Declaración de la syscall
long luis_recoleccion_general(pid_t pid) {
    return syscall(__NR_luis_recoleccion_general, pid);
}

int main() {
    pid_t pid;

    // Solicitar al usuario el PID del proceso que desea verificar
    printf("Ingrese el PID del proceso: ");
    scanf("%d", &pid);

    // Llamar a la syscall para obtener la recolección de memoria
    long result = luis_recoleccion_general(pid);
    if (result < 0) {
        perror("Error al obtener información del proceso");
        return 1;
    }

    // Obtener la información desempaquetada
    unsigned long reserved_memory_kb = (result >> 24) & 0xFF;
    unsigned long percentage_used_memory = (result >> 16) & 0xFF;
    int oom_score = result & 0xFFFF;

    // Imprimir la información obtenida
    printf("Información de memoria para el proceso %d:\n", pid);
    printf("Memoria reservada: %lu KB\n", reserved_memory_kb);
    printf("Porcentaje de memoria utilizada: %lu%%\n", percentage_used_memory);
    printf("OOM Score: %d\n", oom_score);

    return 0;
}
