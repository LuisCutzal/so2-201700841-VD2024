#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>

#define __NR_so2_get_memory_limits 558
#define __NR_so2_add_memory_limit 559
#define __NR_so2_update_memory_limit 560

// Estructura para probar
struct memory_limitation {
    pid_t pid;
    size_t memory_limit; // Límite en KB
};

// Función para invocar la syscall
long so2_get_memory_limits(struct memory_limitation *u_processes_buffer, size_t max_entries, int *processes_returned) {
    return syscall(__NR_so2_get_memory_limits, u_processes_buffer, max_entries, processes_returned);
}

// Función para agregar un límite de memoria
long so2_add_memory_limit(pid_t pid, size_t memory_limit) {
    return syscall(__NR_so2_add_memory_limit, pid, memory_limit);
}

// Función para actualizar un límite de memoria
long so2_update_memory_limit(pid_t pid, size_t new_memory_limit) {
    return syscall(__NR_so2_update_memory_limit, pid, new_memory_limit);
}

void test_so2_get_memory_limits_valid_case() {
    struct memory_limitation buffer[10];
    int processes_returned = 0;

    // Añadir procesos con límites
    assert(so2_add_memory_limit(1234, 500) == 0);  // PID 1234 con 500 KB
    assert(so2_add_memory_limit(5678, 1000) == 0); // PID 5678 con 1000 KB

    // Llamar a la syscall
    long ret = so2_get_memory_limits(buffer, 10, &processes_returned);
    assert(ret == 0);  // La syscall debería ser exitosa

    // Verificar que los procesos retornados son correctos
    assert(processes_returned == 2);
    assert(buffer[0].pid == 1234 && buffer[0].memory_limit == 500);
    assert(buffer[1].pid == 5678 && buffer[1].memory_limit == 1000);
}

void test_so2_get_memory_limits_buffer_too_small() {
    struct memory_limitation buffer[1];  // Buffer más pequeño de lo necesario
    int processes_returned = 0;

    // Añadir algunos procesos
    assert(so2_add_memory_limit(1234, 500) == 0);  // PID 1234 con 500 KB
    assert(so2_add_memory_limit(5678, 1000) == 0); // PID 5678 con 1000 KB

    // Llamar a la syscall con un buffer pequeño
    long ret = so2_get_memory_limits(buffer, 1, &processes_returned);
    assert(ret == 0);  // La syscall debería ser exitosa, pero solo debería devolver 1 proceso

    // Verificar que solo se ha devuelto un proceso
    assert(processes_returned == 1);
    assert(buffer[0].pid == 1234 && buffer[0].memory_limit == 500);
}

void test_so2_get_memory_limits_invalid_max_entries() {
    struct memory_limitation buffer[10];
    int processes_returned = 0;

    // Llamar a la syscall con un valor inválido para max_entries (0)
    long ret = so2_get_memory_limits(buffer, 0, &processes_returned);
    assert(ret == -1);  // Debería devolver -EINVAL

    // Llamar a la syscall con un valor inválido para el buffer
    ret = so2_get_memory_limits(NULL, 10, &processes_returned);
    assert(ret == -1);  // Debería devolver -EINVAL
}

void test_so2_get_memory_limits_no_processes() {
    struct memory_limitation buffer[10];
    int processes_returned = 0;

    // Llamar a la syscall sin haber añadido ningún proceso
    long ret = so2_get_memory_limits(buffer, 10, &processes_returned);
    assert(ret == 0);  // La syscall debería ser exitosa

    // Verificar que no se ha retornado ningún proceso
    assert(processes_returned == 0);
}

void test_so2_update_memory_limit() {
    // Primero añadir un proceso
    assert(so2_add_memory_limit(1234, 500) == 0);  // PID 1234 con 500 KB

    // Luego actualizar su límite de memoria
    assert(so2_update_memory_limit(1234, 1500) == 0); // Actualizar a 1500 KB

    struct memory_limitation buffer[10];
    int processes_returned = 0;

    // Verificar que el límite se actualizó correctamente
    long ret = so2_get_memory_limits(buffer, 10, &processes_returned);
    assert(ret == 0);  // La syscall debería ser exitosa
    assert(processes_returned == 1);
    assert(buffer[0].pid == 1234 && buffer[0].memory_limit == 1500);
}

int main() {
    printf("Running tests for so2_get_memory_limits...\n");

    test_so2_get_memory_limits_valid_case();
    test_so2_get_memory_limits_buffer_too_small();
    test_so2_get_memory_limits_invalid_max_entries();
    test_so2_get_memory_limits_no_processes();
    test_so2_update_memory_limit();

    printf("All tests passed successfully.\n");
    return 0;
}

// gcc -o test_syscall9 test_syscall9.c
// ./test_syscall9