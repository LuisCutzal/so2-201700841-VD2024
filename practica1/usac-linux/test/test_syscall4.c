#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <time.h>
#include <sys/mman.h>
#include <string.h>

#define __NR_luis_tamalloc 554

// Función para verificar los "page faults" durante la asignación de memoria
void check_page_faults(void *start, size_t size) {
    unsigned char *addr = start;
    size_t page_size = sysconf(_SC_PAGE_SIZE);
    unsigned char *vec = (unsigned char *)malloc(size / page_size);
    
    if (!vec) {
        perror("Failed to allocate vector for mincore");
        exit(1);
    }

    // Verificamos las páginas para ver si están cargadas en memoria
    if (mincore(addr, size, vec) == -1) {
        perror("mincore failed");
        free(vec);
        exit(1);
    }

    // Recorremos las páginas y contamos los "page faults"
    int page_fault_count = 0;
    for (size_t i = 0; i < size / page_size; i++) {
        if (vec[i] == 0) {  // Si el bit es 0, significa que la página no está en memoria
            page_fault_count++;
            //printf("Page fault detected at address: %p\n", addr + i * page_size);
        }
    }
    free(vec);
    printf("Total page faults detected: %d\n", page_fault_count);
}

int main() {
    size_t total_size;
    printf("Program for tamalloc PID: %d\n", getpid());

    // Solicitar al usuario el tamaño de la memoria en MB
    printf("Ingrese el tamaño de memoria a asignar (en MB): ");
    if (scanf("%zu", &total_size) != 1) {
        perror("Invalid input");
        return 1;
    }

    // Convertir de MB a bytes (1 MB = 1024 * 1024 bytes)
    total_size *= 1024 * 1024;

    printf("Program to Allocate Memory using tamalloc. Press ENTER to continue...\n");
    getchar(); // Para consumir el '\n' que queda en el buffer

    // Usamos la syscall tamalloc
    char *buffer = (char *)syscall(__NR_luis_tamalloc, total_size);
    if ((long)buffer < 0) {
        perror("tamalloc failed");
        return 1;
    }
    printf("Allocated %zu MB of memory using tamalloc at address: %p\n", total_size / (1024 * 1024), buffer);

    // Verificamos si se causaron "page faults" durante la asignación
    printf("Checking for page faults during memory allocation...\n");
    check_page_faults(buffer, total_size);

    printf("Press ENTER to start reading memory byte by byte...\n");
    getchar();

    srand(time(NULL));

    // Verificamos la inicialización de la memoria
    for (size_t i = 0; i < total_size; i++) {
        char t = buffer[i]; // Esto activa la asignación perezosa (lazy allocation) 
                             // y debería estar inicializada a 0.
        if (t != 0) {
            printf("ERROR FATAL: Memory at byte %zu was not initialized to 0\n", i);
            return 10;
        }

        // Escribir un carácter aleatorio para activar Copy-on-Write (CoW)
        char random_letter = 'A' + (rand() % 26);
        buffer[i] = random_letter;

        if (i % (1024 * 1024) == 0 && i > 0) { // Cada 1MB
            printf("Checked %zu MB...\n", i / (1024 * 1024));
            sleep(1);
        }

        // Imprimir la dirección de la memoria tocada para observar el proceso
        if (i % (1024 * 1024) == 0) { // Cada 1MB para no inundar la salida
            printf("Touched memory at address: %p\n", (void *)(buffer + i));
        }
    }

    // Verificar las páginas cargadas en la memoria usando mincore
    printf("Verifying memory pages loaded into physical memory...\n");
    check_page_faults(buffer, total_size);

    printf("All memory verified to be zero-initialized. Press ENTER to exit.\n");
    getchar();
    return 0;
}


// gcc -o test_syscall4 test_syscall4.c
//  strace ./test_syscall4
// watch -n 0.5 "cat /proc/PID/status | grep -E 'VmRSS|VmSize'"
//  cat /proc/PID/status
