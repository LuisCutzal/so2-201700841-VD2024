#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include "syscall3.h"  // Estructura io_throttle_stats

#define __NR_luis_get_io_throttle 553  // Número de syscall (deberías comprobarlo en tu kernel)

int main() {
    pid_t pid = getpid();  // Obtener el PID del proceso actual
    struct io_throttle_stats stats;

    // Realizar la syscall
    long ret = syscall(__NR_luis_get_io_throttle, pid, &stats);
    if (ret == 0) {
        printf("Bytes leídos: %lu\n", stats.bytes_read);
        printf("Bytes escritos: %lu\n", stats.bytes_written);
        printf("Tiempo de espera de I/O: %lu\n", stats.io_wait_time);
        printf("Tiempo de lectura: %lu\n", stats.read_time);
        printf("Tiempo de escritura: %lu\n", stats.write_time);
    } else {
        perror("Error al obtener estadísticas de I/O");
    }

    return 0;
}
