#### Universidad de San Carlos de Guatemala
#### Facultad de Ingenieria
#### Sistemas Operativos 2
#### Auxiliar: Brian Matus
<br><br><br><br><br><br><br>
<p style="text-align: center;"><strong> Practica 1 <br>
</strong></p>
<br><br><br><br><br><br><br>

| Nombre                              | Carnet    |
| :---:                               |  :----:  |
| Luis Antonio Cutzal Chalí   | 201700841 |

<br><br>

---



<h1>Introducción</h1>

El kernel es la base sobre la cual todo el sistema se construye. En el caso de Linux, uno de los aspectos más interesantes es la posibilidad de personalizar y modificar el kernel para adaptarlo a necesidades específicas

Este proyecto busca brindar a los estudiantes la oportunidad de adentrarse en este mundo, ofreciéndoles una experiencia práctica en la modificación del kernel de Linux. A través de una máquina virtual, podrán hacer cambios controlados, como modificar el nombre del sistema, personalizar los mensajes de arranque o crear módulos que proporcionen estadísticas del sistema

En definitiva, este proyecto les brindará una comprensión más profunda del funcionamiento de Linux y les ofrecerá valiosas habilidades para su desarrollo profesional

<h1>Objetivo General</h1>

+ Modificar y personalizar el kernel de Linux para añadir nuevas llamadas al sistema y
funcionalidades específicas

<h1>Objetivos específicos</h1>

+ Configurar un entorno de desarrollo para la compilación y modificación del kernel.
+ Descargar y compilar el kernel de Linux desde el código fuente.
+ Modificar el kernel para personalizar el nombre del sistema y añadir mensajes personalizados en
el arranque.
+ Desarrollar módulos del kernel que permitan obtener estadísticas del sistema


<h1>Pasos previos a modificar caracteristicas</h1>

+ Descargar la version 6.8.0 desde la pagina : www.kernel.org (descargar el tarball)

+ Descomprimir el kernel en una maquina virtual (Recomendable para evitar dañar nuestro sistema)

<h1> Instalar las dependencias </h1>

- sudo apt-get install build-essential libncurses5-dev fakeroot wget bzip2 openssl
- sudo apt-get install build-essential libncurses-dev bison flex libssl-dev libelf-dev

si les sale un error de que kernel-package no existe. De ser asi, solo quitenlo de la lista
- sudo apt-get install build-essential kernel-package libncurses5-dev fakeroot wget bzip2 openssl
- sudo apt-get install build-essential libncurses-dev bison flex libssl-dev libelf-dev

Si se topan con el error:
make[3]: *** No rule to make target 'debian/canonical-certs.pem', needed by 'certs/x509_certificate_list'.  Stop.

Ejecutar esto en la raiz del proyecto:

- scripts/config --disable SYSTEM_TRUSTED_KEYS
- scripts/config --disable SYSTEM_REVOCATION_KEYS

Comando para crear un acceso directo a los archivos
ln -s < archivo real > < nombre sylim >  


Copiar el archivo de config por primera vez
+ cp -v /boot/config-$(uname -r) .config

Limpiar el ambiente de compilacion
+ make clean

Modificar la version del kernel en el archivo Makefile que esta en la raiz

Hasta arriba, estara asi:

+ EXTRAVERSION = 
+ EXTRAVERSION = -49-usac1

Ahora te podes guiar del script "compile_and_install.sh" para compilar e instalar

Lo marcas con "chmod +x compile_and_install.sh" para permitir que se ejecute
+ ./compile_and_install.sh

+ Le damos que No a los primeros 3 comandos, los siguintes le damos Si excepto el ultimo comando.


```cpp
#!/bin/bash

compilation_command="make -j$(nproc --ignore=1)"
# If having problems with compilation, activate verbose mode for more info in console
#compilation_command="make -j$(nproc --ignore=1) V=1"
#compilation_command="sleep 3" # for testing

declare -a pre_compile_commands=(
    "make clean:N"
    "make oldconfig:N"
    "make menuconfig:N"
    "make localmodconfig:N"
)

declare -a post_compile_commands=(
    "make modules_install:Y"
    "make install:Y"
    "make headers_install:Y"
    "update-grub2:N"
)
########################################################################################################################
############################################################Helper Functions############################################
########################################################################################################################

ask_question() {
    local command=$1
    local default=$2
    local default_display="[Y/n]"

    if [[ "$default" == "N" ]]; then
        default_display="[y/N]"
    fi

    printf "Do you want to run '%s'? %s: " "$command" "$default_display" >&2
    read -r answer
    answer=$(echo "$answer" | xargs) # Trim whitespaces

    # If no input
    if [[ -z "$answer" ]]; then
        answer=$default
    fi

    # Normalize answer
    answer=$(echo "$answer" | tr '[:lower:]' '[:upper:]')

    echo "$answer"
}


process_commands() { #Split command and default behavior
    local commands=("$@")
    for entry in "${commands[@]}"; do
        local command=${entry%:*}
        local default=${entry#*:}

        answer=$(ask_question "$command" "$default")
        if [[ "$answer" == "Y" ]]; then
            echo "Running '$command'..."
            eval "$command"
        else
            echo "Skipping '$command'."
        fi
    done
}


echo "Compilation helper script started"
process_commands "${pre_compile_commands[@]}"
############################################################Compilation#################################################
# 
# Capture start time
start_time=$(date +%s)
start_time_full=$(date)

echo "Compiling with command: $compilation_command"
eval $compilation_command

# Check if it was successful
if [[ $? -ne 0 ]]; then
    echo "Compilation failed. Skipping further steps."
    exit 1
fi
echo "Compilation successful."

echo "Starting time: $start_time_full"
end_time=$(date +%s)
echo "Ending time: $(date)"
elapsed_time=$((end_time - start_time)) # in seconds
elapsed_minutes=$(echo "scale=2; $elapsed_time / 60" | bc) # in minutes

echo "Compilation took $elapsed_time seconds ($elapsed_minutes minutes)"
########################################################################################################################
process_commands "${post_compile_commands[@]}"

```

<h1>Personalización del nombre del sistema</h1>

- Dirigirnos al kernel que descomprimimos.
- Entrar a la ruta: include/linux/uts.h
- Modificar el archivo de esta forma:

```cpp
#define UTS_SYSNAME "Linux sopesOS"
```

<h1>Mensajes de inicio personalizados</h1>

- Dirigirnos al kernel que descomprimimos.
- Entrar a la ruta: init/main.c
- Modificar el archivo de esta forma:

```cpp
	printk(KERN_INFO "Luis Antonio cutzal Chalí - 201700841\n");
	printk(KERN_INFO ".__        .__        \n");
	printk(KERN_INFO "|  |  __ __|__| ______\n");
	printk(KERN_INFO "|  | |  |  \  |/  ___/\n");
	printk(KERN_INFO "|  |_|  |  /  |\___ \ \n");
	printk(KERN_INFO "|____/____/|__/____  >\n");
	printk(KERN_INFO "				     \/ \n");
```

- Tener en cuenta que se debe de modificar a partir del comentario: trace_printk ubicado en la linea 936

<h2>Nota: cada vez que se modifica algo del kernel es necesario volver a ejecutar el archivo:

+ ./compile_and_install.sh

</h2>

- para saber si se modifico la Personalización del nombre del sistema y los Mensajes de inicio personalizados se debe de utilzar los siguientes comandos respectivamente:

```cpp
uname -s
```
![primera imagen](./imagenes_manual/Imagen%20de%20WhatsApp%202024-12-14%20a%20las%2020.28.10_531b7f68.jpg)

```cpp
dmesg
```
![primera imagen](./imagenes_manual/Imagen%20de%20WhatsApp%202024-12-14%20a%20las%2020.27.42_1e8be9ff.jpg)




<h3> Tambien es necesario que cada archvio que se necesite modificar se utilice el comando para crear el acceso directo, tener en cuenta que se debe de crear una ruta igual a la ruta que tiene el archivo original</h3>

![primera imagen](./imagenes_manual/Captura%20de%20pantalla%20(108).png)

<h1>Implementación de nuevas llamadas al sistema</h1>

<h2>capture_memory_snapshot</h2>

- Descripción: Esta llamada realiza una captura del estado de la memoria en un instante
de tiempo, guardando un "snapshot" o instantánea de las áreas de memoria ocupadas y
libres, así como información sobre las páginas de memoria activas. Esta información
puede ayudar a entender el uso exacto de la memoria y el estado de fragmentación en
tiempo real

<h3> Implementación </h3>

- Primero sera crear la ruta para el archivo: "syscall_64.tbl" el cual contendra nuestra primera syscall, teniendo en cuenta que el archivo se ubica en
- arch/x86/entry/syscalls/syscall_64.tbl

se modifica hasta el final del archivo.

``` cpp
551 common luis_capture_memory_snapshot sys_luis_capture_memory_snapshot
```

<h4>Nota: tomar en cuenta que el número 551 puede variar dependiendo de la version de kernel, se recomienda utilizar ese número en adelante, tambien debe de agregar: "common" ya que es un abi "común", "64" o "x32" para este archivo. Por motivos de proyecto se agregó el nombre en este caso luis. Tambien es necesario solocar "sys" al nombre porque los talones __x64_sys_*() se crean sobre la marcha para las llamadas al sistema sys_*(). </h4>

- Crear la ruta para el archivo: "syscalls.h" el cual es la declaración de una nueva syscall personalizada para el kernel de Linux, la ruta completa seria:

- include/linux/syscalls.h

- Crear una ruta para el archivo "Makefile" y agregar: obj-y += usac/ el cual se debe de encontrar antes de este comando: obj-$(CONFIG_MODULES) += module/, se modifica ese archivo porque se debe incluir el subdirectorio usac/ como parte del proceso de compilación del kernel.

- Se crea una carpeta llamada usac y dentro de ella se coloca el archivo: "syscall1.c" con lo siguiente.

```cpp
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <linux/vmstat.h>    // Para global_node_page_state
#include <linux/node.h>      // Para obtener información de nodos
#include "syscall1.h"  // Estructura memory_snapshot

/*
 * Implementación de la syscall para capturar el estado de memoria.
 */
SYSCALL_DEFINE1(luis_capture_memory_snapshot, struct memory_snapshot __user *, snapshot)
{
    struct memory_snapshot kernel_snapshot;

    // Calcular memoria total en bytes
    kernel_snapshot.total_memory = totalram_pages() << PAGE_SHIFT;

    // Calcular memoria libre en bytes usando global_node_page_state
    kernel_snapshot.free_memory = (long)global_node_page_state((enum node_stat_item)NR_FREE_PAGES) << PAGE_SHIFT;

    // Calcular memoria utilizada
    kernel_snapshot.used_memory = kernel_snapshot.total_memory - kernel_snapshot.free_memory;

    // Obtener páginas activas (usa NR_INACTIVE_FILE si NR_INACTIVE_PAGES no está disponible)
    kernel_snapshot.active_pages = global_node_page_state(NR_INACTIVE_FILE);

    // Obtener páginas de caché
    kernel_snapshot.cache_pages = global_node_page_state(NR_FILE_PAGES);

    // Obtener las páginas de swap (verifica si ZSWPIN y ZSWPOUT son correctas)
    kernel_snapshot.swap_pages = global_node_page_state(ZSWPIN) + global_node_page_state(ZSWPOUT);

    // Copiar los datos al espacio de usuario
    if (copy_to_user(snapshot, &kernel_snapshot, sizeof(struct memory_snapshot))) {
        pr_err("Error al copiar datos al espacio de usuario.\n");
        return -EFAULT;
    }

    pr_info("Syscall luis_capture_memory_snapshot ejecutada exitosamente.\n");
    return 0;
}
```
- Es necesario crear un archivo Makefile dentro de la ruta: kernel/usac/Makefile el cual contendra lo siguiente:

```cpp
obj-y += syscall1.o
```

- Crear la ruta: arch/microbiaze/kernel para el archivo syscall_table.S y colocar en la ultima linea:

```cpp
.long sys_luis_capture_memory_snapshot
```

- Como paso final debe de recompilar el kernel, utilizando el archivo: ./compile_and_install.sh


<h2>track_syscall_usage</h2>

- Descripción: Esta llamada permite al usuario monitorizar cuántas veces y cuándo se
ejecuta una lista de llamadas al sistema específicas, como open, write, read, fork, entre
otras. Esto es útil para auditorías de seguridad o para aplicaciones de rendimiento en
donde es necesario entender los patrones de uso de las llamadas al sistema

<h3> Implementación </h3>

- Modificar el archivo "syscall_64.tbl", agregarle la siguiente linea de codigo:

``` cpp
552 common luis_track_syscall_usage sys_luis_track_syscall_usage
```

- Agregar en el archivo "syscalls.h" lo siguiente y tomando en cuenta que debe de ser seguido de la anterior modificacion.
``` cpp
asmlinkage long sys_luis_track_syscall_usage(void);
```

- Dentro de la carpeta donde se creo el archivo "syscall1.c" se crea un nuevo archivo llamado "syscall2.c" con lo siguiente:

```cpp
#include <linux/export.h>
#include <linux/mm.h>
#include <linux/mm_inline.h>
#include <linux/utsname.h>
#include <linux/mman.h>
#include <linux/reboot.h>
#include <linux/prctl.h>
#include <linux/highuid.h>
#include <linux/fs.h>
#include <linux/kmod.h>
#include <linux/ksm.h>
#include <linux/perf_event.h>
#include <linux/resource.h>
#include <linux/kernel.h>
#include <linux/workqueue.h>
#include <linux/capability.h>
#include <linux/device.h>
#include <linux/key.h>
#include <linux/times.h>
#include <linux/posix-timers.h>
#include <linux/security.h>
#include <linux/random.h>
#include <linux/suspend.h>
#include <linux/tty.h>
#include <linux/signal.h>
#include <linux/cn_proc.h>
#include <linux/getcpu.h>
#include <linux/task_io_accounting_ops.h>
#include <linux/seccomp.h>
#include <linux/cpu.h>
#include <linux/personality.h>
#include <linux/ptrace.h>
#include <linux/fs_struct.h>
#include <linux/file.h>
#include <linux/mount.h>
#include <linux/gfp.h>
#include <linux/syscore_ops.h>
#include <linux/version.h>
#include <linux/ctype.h>
#include <linux/syscall_user_dispatch.h>

#include <linux/compat.h>
#include <linux/syscalls.h>
#include <linux/kprobes.h>
#include <linux/user_namespace.h>
#include <linux/time_namespace.h>
#include <linux/binfmts.h>

#include <linux/sched.h>
#include <linux/sched/autogroup.h>
#include <linux/sched/loadavg.h>
#include <linux/sched/stat.h>
#include <linux/sched/mm.h>
#include <linux/sched/coredump.h>
#include <linux/sched/task.h>
#include <linux/sched/cputime.h>
#include <linux/rcupdate.h>
#include <linux/uidgid.h>
#include <linux/cred.h>

#include <linux/nospec.h>

#include <linux/kmsg_dump.h>
// Incluir la información de la versión del kernel (para evitar recompilaciones innecesarias)
#include <generated/utsrelease.h>

#include <linux/uaccess.h>
#include <asm/io.h>
#include <asm/unistd.h>

#include <linux/time.h>
#include <linux/ktime.h>
#include "syscall2.h"

// Definimos una estructura para llevar el registro de cada syscall (llamada al sistema)
// Esta estructura tiene un contador para cuántas veces se ha llamado a un syscall
// y la última vez que se usó (fecha y hora).
struct syscall_usage {
    unsigned long count;  // Contador de cuántas veces se llamó a este syscall
    struct timespec64 time_last_used;  // Hora de la última vez que se usó este syscall
};

#define MAX_SYS_CALLS 1024  // Establecemos un máximo de 1024 syscalls
static struct syscall_usage *syscall_counters;  // Apuntador al array que almacena el contador de cada syscall

// Declaración de la función para inicializar el array de contadores
static int init_syscall_counters(void);

// Definición de la syscall personalizada para rastrear el uso de las syscalls
SYSCALL_DEFINE1(luis_track_syscall_usage, struct syscall_usage __user *, estatic) {
    // Verificamos si el array de contadores de syscalls ha sido inicializado
    if (!syscall_counters) {
        // Si no está inicializado, lo inicializamos
        if (init_syscall_counters() != 0) {
            return -ENOMEM;  // Devolvemos un error si no se puede inicializar
        }
    }

    // Copiamos la información del contador de syscalls al espacio de usuario
    int resultadoCopi = copy_to_user(estatic, syscall_counters, sizeof(struct syscall_usage) * MAX_SYS_CALLS);
    if (resultadoCopi) {
        return -EFAULT;  // Devolvemos un error si no podemos copiar los datos
    }

    return 0;  // Todo salió bien
}

// Función que inicializa el array de contadores de syscalls
static int init_syscall_counters(void) {
    // Asignamos memoria para el array de contadores de syscalls
    syscall_counters = kzalloc(
        sizeof(struct syscall_usage) * MAX_SYS_CALLS,  // Necesitamos espacio para MAX_SYS_CALLS estructuras
        GFP_KERNEL);  // Usamos GFP_KERNEL para la asignación de memoria en el kernel
    if (syscall_counters == NULL) {
        return -ENOMEM;  // Si no se puede asignar memoria, devolvemos un error
    }

    return 0;  // La inicialización fue exitosa
}

// Esta función se llama cada vez que se rastrea una llamada al sistema
void track_syscall(int syscall_id) {
    // Verificamos que los contadores estén inicializados y que el id del syscall sea válido
    if (!syscall_counters || syscall_id >= MAX_SYS_CALLS) {
        return;  // Si no es válido, salimos de la función
    }

    // Incrementamos el contador de la syscall correspondiente
    syscall_counters[syscall_id].count++;

    // Obtenemos la hora actual
    struct timespec64 now;  // Variable para almacenar la hora actual
    ktime_get_real_ts64(&now);  // Función que obtiene la hora real del sistema en formato de 64 bits
    syscall_counters[syscall_id].time_last_used = now;  // Actualizamos la hora de la última vez que se usó
}
```

- En el archvio "Makefile" ubicado en la ruta: kernel/usac/Makefile colocar:

```cpp
obj-y += syscall2.o
```

- Ahora se debe de modificar los archivos del kernel recordando que se debe de crear el acceso directo con la misma ruta de los archivos, el primer archivo se encuentra en la ruta: kernel/fork.c y colocar en la linea 3038 lo siguiente:

```cpp
track_syscall(__NR_clone);
```

- Crear la carpeta: "fs" y dentro colocar 2 archivos, primero el archivo "open.c" y agregarle en su interiro la siguiente linea de comando:

```cpp
track_syscall(__NR_open);
```

- Colocar en la misma carpeta: "fs" el segundo archivo llamado "read_write.c" y agregar en su interiro lo siguiente:

```cpp
//despues de la siguiente linea de codigo: SYSCALL_DEFINE3(read, unsigned int, fd, char __user *, buf, size_t, count) 

track_syscall(__NR_read); 


//despues de la siguiente linea de codigo: SYSCALL_DEFINE3(write, unsigned int, fd, const char __user *, buf, size_t, count)

track_syscall(__NR_write);
```

- Como paso final debe de recompilar el kernel, utilizando el archivo: ./compile_and_install.sh

<h2>Hacer pruebas</h2>


Para la primera prueba y poder verificar que las implementaciones estan correctas en el kernel, es necesario el siguiente archvio de prueba el cual se llama: test_syscall1.c

```cpp
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

// Definir el número de la syscall que hemos asignado
#define __NR_luis_capture_memory_snapshot 551

// Estructura para almacenar el estado de la memoria
struct memory_snapshot {
    unsigned long total_memory;
    unsigned long free_memory;
    unsigned long used_memory;
    unsigned long active_pages;
    unsigned long cache_pages;
    unsigned long swap_pages;
};

int main() {
    struct memory_snapshot snapshot;

    // Llamada a la syscall
    int result = syscall(__NR_luis_capture_memory_snapshot, &snapshot);

    // Verificación de la ejecución de la syscall
    if (result == 0) {
        printf("Total Memory: %lu\n", snapshot.total_memory);
        printf("Free Memory: %lu\n", snapshot.free_memory);
        printf("Used Memory: %lu\n", snapshot.used_memory);
        printf("Active Pages: %lu\n", snapshot.active_pages);
        printf("Cache Pages: %lu\n", snapshot.cache_pages);
        printf("Swap Pages: %lu\n", snapshot.swap_pages);
    } else {
        // Si ocurre un error
        perror("syscall");
    }

    return 0;
}

```

- Para compilar el test se debe de ir desde la terminal a la ruta donde se guardo el archivo test_syscall1.c y como administrador ejecutar los siguientes comandos:

```cpp
gcc -o test_syscall1 test_syscall1.c
./test_syscall1
```

- Mostrara un en la terminal lo suguiente

![primera imagen](./imagenes_manual/Imagen%20de%20WhatsApp%202024-12-14%20a%20las%2019.37.33_080045fc.jpg)


Para la segunda prueba y poder verificar que las implementaciones estan correctas en el kernel, es necesario el siguiente archvio de prueba el cual se llama: test_syscall2.c

```cpp
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
```

- Para compilar el test se debe de ir desde la terminal a la ruta donde se guardo el archivo test_syscall2.c y como administrador ejecutar los siguientes comandos:

```cpp
gcc -o test_syscall2 test_syscall2.c
./test_syscall2
```
- Mostrara un en la terminal lo suguiente

![primera imagen](./imagenes_manual/Imagen%20de%20WhatsApp%202024-12-13%20a%20las%2017.31.30_260a7a79.jpg)

<h1> Problemas </h1>

Tener en cuenta que al estar modificando cosas del kernel es muy facil poder tener errores en la compilacion y esto haga que no podamos entrar a nuestro kernel, se debe de seguir los siguientes pasos:

- Entrar al group del sistema.
- Entrar a las opciones avanzadas
- Seleccionar el kernel por defecto, ya que el kernel por defecto esta separado del kernel que estamos modificando esto con el fin de poder tener un mejor control de los posibles errores y no se tenga que eliminar la maquina virtual.

![primera imagen](./imagenes_manual/Imagen%20de%20WhatsApp%202024-12-13%20a%20las%2014.37.02_a0f7dbd5.jpg)

Otro de los problemas puede ser que aparezca lo siguiente:

![primera imagen](./imagenes_manual/Imagen%20de%20WhatsApp%202024-12-13%20a%20las%2021.19.40_b38e1653.jpg)

Esto sucede porque en el archivo syscall_64.tbl no esta la llamada del syscall

![primera imagen](./imagenes_manual/Imagen%20de%20WhatsApp%202024-12-14%20a%20las%2017.15.04_2219d1c6.jpg)

Como se puede ver en la imagen, se debe de colocar las llamadas de las llamadas al sistema