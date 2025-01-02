#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sys/uid.h>
#include <sys/types.h>

#define _GNU_SOURCE
#define SYS_so2_remove_memory_limit 560

long so2_remove_memory_limit(pid_t pid) {
    return syscall(SYS_so2_remove_memory_limit, pid);
}

// Función para verificar si el proceso tiene privilegios de sudo (superusuario)
int is_superuser() {
    return (getuid() == 0); // Verifica si el UID es 0 (root)
}

void test_so2_remove_memory_limit() {
    pid_t pid = getpid();

    long result;

    // Simulamos que este proceso tiene un límite de memoria
    // Este es un paso necesario antes de probar la remoción del límite
    // La implementación real de esta simulación depende de cómo gestionas la lista de limitaciones
    // Por ejemplo, podrías usar otra syscall para agregar el límite

    // Intentar remover el límite de memoria para el proceso con el PID especificado
    result = so2_remove_memory_limit(pid);
    if (result == 0) {
        printf("Límite de memoria removido exitosamente para el proceso %d\n", pid);
    } else {
        printf("Error al remover límite de memoria para el proceso %d: %s\n", pid, strerror(-result));
    }

    // Caso con un PID inválido (proceso inexistente)
    pid_t invalid_pid = pid + 1000; // PID muy improbable que exista
    result = so2_remove_memory_limit(invalid_pid);
    if (result == -ESRCH) {
        printf("Error esperado -ESRCH para PID inexistente: %s\n", strerror(-ESRCH));
    }

    // Caso con un PID no limitado
    pid_t non_limited_pid = getpid() + 1000; // PID que no debería estar limitado
    result = so2_remove_memory_limit(non_limited_pid);
    if (result == -102) {
        printf("Error esperado -102 para PID no limitado: %s\n", strerror(-102));
    }

    // Caso sin privilegios de sudo
    if (!is_superuser()) {
        result = so2_remove_memory_limit(pid); // Este test debe fallar si no tienes privilegios
        if (result == -EPERM) {
            printf("Error esperado -EPERM para falta de permisos de sudo: %s\n", strerror(-EPERM));
        }
    } else {
        printf("Usuario tiene privilegios de sudo. Se espera que la syscall funcione correctamente.\n");
    }
}

int main() {
    test_so2_remove_memory_limit();
    return 0;
}

// gcc -o test_syscall10 test_syscall10.c
// ./test_syscall10