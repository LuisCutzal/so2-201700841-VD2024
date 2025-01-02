#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <stdlib.h>

#define __NR_so2_add_memory_limit 557

int main() {
    pid_t test_pid;
    size_t test_memory_limit;
    long result;

    // Caso 1: Proceso válido con límite válido
    test_pid = getpid(); // Obtener el PID del proceso actual
    test_memory_limit = 1024; // 1 MB
    printf("Test 1: Agregar proceso válido con límite válido.\n");
    result = syscall(__NR_so2_add_memory_limit, test_pid, test_memory_limit);
    if (result == 0) {
        printf("Test 1 passed: Proceso limitado correctamente.\n");
    } else {
        printf("Test 1 failed: Error %ld (errno: %d).\n", result, errno);
    }

    // Caso 2: Proceso inexistente
    test_pid = -99999; // PID inválido
    test_memory_limit = 1024;
    printf("\nTest 2: Proceso inexistente.\n");
    result = syscall(__NR_so2_add_memory_limit, test_pid, test_memory_limit);
    if (result == -1 && errno == ESRCH) {
        printf("Test 2 passed: Error ESRCH detectado correctamente.\n");
    } else {
        printf("Test 2 failed: Error %ld (errno: %d).\n", result, errno);
    }

    // Caso 3: Proceso duplicado
    printf("\nTest 3: Agregar proceso duplicado.\n");
    result = syscall(__NR_so2_add_memory_limit, getpid(), test_memory_limit);
    if (result == -1 && errno == 101) {
        printf("Test 3 passed: Error 101 detectado correctamente.\n");
    } else {
        printf("Test 3 failed: Error %ld (errno: %d).\n", result, errno);
    }

    // Caso 4: Límite de memoria negativo
    test_pid = getpid();
    test_memory_limit = -1024;
    printf("\nTest 4: Límite de memoria negativo.\n");
    result = syscall(__NR_so2_add_memory_limit, test_pid, test_memory_limit);
    if (result == -1 && errno == EINVAL) {
        printf("Test 4 passed: Error EINVAL detectado correctamente.\n");
    } else {
        printf("Test 4 failed: Error %ld (errno: %d).\n", result, errno);
    }

    // Caso 5: Usuario sin permisos
    test_pid = getpid();
    test_memory_limit = 2048; // 2 MB
    printf("\nTest 5: Usuario sin permisos.\n");
    // Ejecuta este caso como un usuario no root para probarlo
    result = syscall(__NR_so2_add_memory_limit, test_pid, test_memory_limit);
    if (result == -1 && errno == EPERM) {
        printf("Test 5 passed: Error EPERM detectado correctamente.\n");
    } else {
        printf("Test 5 failed: Error %ld (errno: %d).\n", result, errno);
    }

    return 0;
}


// gcc -o test_syscall7 test_syscall7.c
// sudo ./test_syscall7
// para el caso 5 sin sudo ->  ./test_syscall7