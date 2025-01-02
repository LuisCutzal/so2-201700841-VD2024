#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

// Estructura para la memoria limitada (debe coincidir con la del kernel)
struct memory_limitation {
    pid_t pid;
    size_t memory_limit;
};

#define SYS_so2_get_memory_limits 558 // Número de syscall

int main() {
    struct memory_limitation processes[10]; // Buffer de procesos limitados
    int processes_returned = 0;             // Variable para saber cuántos procesos se devolvieron
    size_t max_entries = 10;                // Tamaño máximo de procesos que podemos obtener

    // Llamar a la syscall para obtener la lista de procesos limitados
    long result = syscall(SYS_so2_get_memory_limits, processes, max_entries, &processes_returned);

    if (result == 0) {
        printf("Syscall 2 (so2_get_memory_limits) exitó!\n");
        printf("Número de procesos devueltos: %d\n", processes_returned);

        // Imprimir la lista de procesos limitados
        for (int i = 0; i < processes_returned; i++) {
            printf("Proceso %d -> PID: %d, Límite de memoria: %zu KB\n",
                i + 1, processes[i].pid, processes[i].memory_limit);
        }
    } else {
        // Si hubo un error, mostrar el código de error
        printf("Error en syscall 2 (so2_get_memory_limits): %s\n", strerror(errno));
    }

    return 0;
}


//  gcc -o test_syscall8 test_syscall8.c
//  ./test_syscall8
