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
        // Depuración: imprimir los valores de count para cada syscall
        for (int i = 0; i < 1024; i++) {
            if (stats[i].count > 0) {
                // Convertir la parte de segundos del tiempo a una estructura tm
                char time_str[100];
                struct tm *tm_info = localtime(&stats[i].time_last_used.tv_sec);

                // Formatear la fecha y hora de manera legible
                strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

                // Imprimir el ID de la syscall junto con la cuenta y el tiempo formateado
                printf("Syscall ID %d: Count = %lu, Last used = %s.%09ld\n",
                       i, stats[i].count,
                       time_str, stats[i].time_last_used.tv_nsec);
            }
        }
    } else {
        perror("syscall");
    }

    // Liberar la memoria asignada
    free(stats);
    return 0;
}


//gcc -o test_syscall2 test_syscall2.c
// ./test_syscall2 