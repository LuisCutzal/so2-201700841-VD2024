#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <time.h> // Para struct timespec

#define __NR_luis_track_syscall_usage 552 // Ajusta al número correcto de la syscall

// Definir la estructura que usas en el espacio de usuario
struct syscall_usage {
    unsigned long count;               // Número de veces que se llamó la syscall
    struct timespec time_last_used;    // Tiempo de la última ejecución
};

int main() {
    // Reservar memoria para almacenar los datos de las syscalls
    struct syscall_usage *stats = malloc(sizeof(struct syscall_usage) * 1024);

    if (stats == NULL) {
        perror("malloc");
        return -1;
    }

    // Llamar a la syscall con el número y pasar el puntero a la estructura
    int result = syscall(__NR_luis_track_syscall_usage, stats);

    if (result == 0) {
        printf("Syscall executed successfully\n");
        // Imprimir los resultados de las syscalls
        for (int i = 0; i < 1024; i++) {
            if (stats[i].count > 0) {
                printf("Syscall ID %d: Count = %lu, Last used = %ld.%09ld\n",
                       i, stats[i].count,
                       stats[i].time_last_used.tv_sec, stats[i].time_last_used.tv_nsec);
            }
        }
    } else {
        perror("syscall");
    }

    // Liberar la memoria asignada
    free(stats);
    return 0;
}
