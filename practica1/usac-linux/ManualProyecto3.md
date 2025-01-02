#### Universidad de San Carlos de Guatemala
#### Facultad de Ingenieria
#### Sistemas Operativos 2
#### Auxiliar: Brian Matus
<br><br><br><br><br><br><br>
<p style="text-align: center;"><strong> Proyecto 3 <br>
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

## Objetivo General

+ Desarrollar un limitador de memoria que cuente con las operaciones CRUD de procesos en una
 lista enlazada simple dentro de memoria en espacio de kernel.

## Objetivos específicos

+ Comprenderlosprincipios de asignación de memoria, memoria alojada y límite de recursos.
+ Implementartécnicas de control de asignación de memoria a funciones como malloc, que
 internamente hace uso de mmap
+ Entenderelfuncionamiento de malloc y como un proceso en espacio de usuario solicita
 dinámicamente más memoria.
+ Adquirir experiencia práctica en la manipulación y modificación del kernel de Linux para la
 gestión de memoria.



## Cronograma

| **Día** | **Actividad**                                      |
|---------|----------------------------------------------------|
| Día 1   | Investigación de estructura de listas en kernel y maneras de limitar recursos a procesos.     |
| Día 2-4   | Diseñoeimplementación de la estructura de datos para la lista.      |
| Día 5-6   | Desarrollo de la limitación de memoria      |
| Día 6-7 |  Validaciones de errores de argumentos y runtime de las syscalls         |
| Día 7   |  Pruebas, resolución de errores y elaboración de documentación   |


<h1>Pasos previos a modificar caracteristicas</h1>

+ Descargar la version 6.8.0 desde la pagina : www.kernel.org (descargar el tarball)

+ Descomprimir el kernel en una maquina virtual (Recomendable para evitar dañar nuestro sistema)

## Instalar las dependencias

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
## Implementación de nuevas llamadas al sistema

###  Limitar un proceso</h2>

- Sedebepoderagregar entradas de procesos nuevos a ser limitados
- Debeestar limitada al uso de usuarios sudoers
- Cadaentrada a la lista debe hacer uso del siguiente struct

```cpp
struct memory_limitation{pid_t pid; size_t memory_limit;};
```
- Debetener el ID de syscall 557

## Debecontar con la siguiente definición de función
- Devolver en el long, y TAMBIEN setear el errno con el codigo de resultado
- Si el proceso no existe con ese PID: devolver error -ESRCH
- Si el proceso ya excede este limite:devolver error -100
- Si el proceso ya esta en la lista: devolver error -101
- La cantidad de memoria especificada es negativa: -EINVAL
- El PID es negativo : -EINVAL
- Si no es sudoer, -EPERM
- Si ya no hay espacio en memoria para agregar un nodo mas a la lista: -ENOMEM


<h3> Implementación </h3>

- Primero sera crear la ruta para el archivo: "syscall_64.tbl" el cual contendra nuestra primera syscall, teniendo en cuenta que el archivo se ubica en
- arch/x86/entry/syscalls/syscall_64.tbl

se modifica hasta el final del archivo.

``` cpp
557 common so2_add_memory_limit sys_so2_add_memory_limit
```

<h4>Nota: tomar en cuenta que el número 554 puede variar dependiendo de la version de kernel, se recomienda utilizar ese número en adelante, tambien debe de agregar: "common" ya que es un abi "común", "64" o "x32" para este archivo. Por motivos de proyecto se agregó el nombre en este caso luis. Tambien es necesario solocar "sys" al nombre porque los talones __x64_sys_*() se crean sobre la marcha para las llamadas al sistema sys_*(). </h4>

- Crear la ruta para el archivo: "syscalls.h" el cual es la declaración de una nueva syscall personalizada para el kernel de Linux, la ruta completa seria:

- include/linux/syscalls.h

- Dentro de la ruta antes mencionada colocar el siguente comando

``` cpp
asmlinkage long sys_so2_add_memory_limit(pid_t process_pid, size_t memory_limit);
```
- Crear una ruta para el archivo "Makefile" y agregar: obj-y += usac/ el cual se debe de encontrar antes de este comando: obj-$(CONFIG_MODULES) += module/, se modifica ese archivo porque se debe incluir el subdirectorio usac/ como parte del proceso de compilación del kernel.

- En el archvio "Makefile" ubicado en la ruta: kernel/usac/Makefile colocar:

```cpp
obj-y += project3/
```
- Crear una carpeta llamada project3 y una carpeta llamda include esto dentro de la carpeta usac.

- Crear un archivo "Makefile" ubicado en la ruta: kernel/usac/project3/Makefile colocar:
```cpp
obj-y += syscall7.o
```

- Se crea una carpeta llamada project3 y dentro de ella se coloca el archivo: "syscall7.c" con lo siguiente.

```cpp




```

- Como paso final debe de recompilar el kernel, utilizando el archivo: ./compile_and_install.sh


## Obtener una lista de procesos limitados

- Sedebepoderobtener la lista de procesos que han sido limitados
- Sedebepasar el puntero a un struct donde el kernel escribirá los procesos
- Sedebeindicar el tamaño maximo de nuestro buffer
- Sedebepasar unpuntero para que el kernel nos indique cuántos procesos realmente
 escribió en nuestro buffer
- Dicholo anterior, la definición de función debe tener la siguiente forma
```cpp
long SYSCALL_DEFINE3(so2_get_memory_limits, struct memory_limitation*, u_processes_buffer, size_t, max_entries, int*, processes_returned)
```
- Debetener el ID de syscall 558

## Sedebemanejarlos errores de la siguiente manera

- Devolver en el long, y TAMBIEN setear el errno con el codigo de resultado
- Tienen que llenar el struct con las entradas correspondientes en espacio de usuario
- Tiene que settear processes_returned a la cantidad de procesos que se escribieron
- Validar el puntero de user-space: -EINVAL
- Si el max es <= 0: -EINVAL
- Si el buffer es mas pequeño, solo usar el limite del buffer (no se devuelve error)

<h3> Implementación </h3>

- Modificar el archivo "syscall_64.tbl", agregarle la siguiente linea de codigo:

``` cpp
558 common so2_get_memory_limits sys_so2_get_memory_limits
```

- Agregar en el archivo "syscalls.h" lo siguiente y tomando en cuenta que debe de ser seguido de la anterior modificacion.
``` cpp
asmlinkage long sys_so2_get_memory_limits(struct memory_limitation __user *u_processes_buffer, size_t max_entries, int __user *processes_returned);
```

- Deontro del archivo "syscall7.c" se agrega lo siguiente:

```cpp

```
- En el archvio "Makefile" no se modifica
- Crear la ruta microblaze/kernel y dentro el archivo syscall_table.S y colocar en las ultimas lineas el siguiente comando:

```cpp
.long sys_so2_get_memory_limits
```

- Como paso final debe de recompilar el kernel, utilizando el archivo: ./compile_and_install.sh

<h2>Actualizar el límite de un proceso</h2>

- Sedebepoderactualizar entradas de procesos previamente limitados
- Debeestar limitada al uso de usuarios sudoers
- Debetener el ID de syscall 559
- Debecontar con la siguiente definición de función
```cpp
long SYSCALL_DEFINE2(so2_update_memory_limit, pid_t, process_pid, size_t, memory_limit)
```
## Debemanejarerrores de la siguiente forma

- Devolver en el long, y TAMBIEN setear el errno con el codigo de resultado
- Si el proceso no existe con ese PID: devolver error -ESRCH
- Si el proceso ya excede este limite:devolver error -100
- Si el proceso NO esta en la lista:devolver error -102
- La cantidad de memoria especificada es negativa: -EINVAL
- El PID es negativo : -EINVAL
- Si no es sudoer, -EPERM

<h3> Implementación </h3>

- Primero sera crear la ruta para el archivo: "syscall_64.tbl" el cual contendra nuestra primera syscall, teniendo en cuenta que el archivo se ubica en
- arch/x86/entry/syscalls/syscall_64.tbl

Se modifica hasta el final del archivo.

``` cpp
559 common so2_update_memory_limit sys_so2_update_memory_limit
```
- Agregar en el archivo "syscalls.h" lo siguiente y tomando en cuenta que debe de ser seguido de la anterior modificacion.
``` cpp
asmlinkage long sys_so2_update_memory_limit(pid_t process_pid, size_t memory_limit);
```

- Dentro del archivo "syscall7.c" agregar lo siguiente:

```cpp
 




```
En el archvio "Makefile" no se debe de modificar:


- Crear la ruta microblaze/kernel y dentro el archivo syscall_table.S y colocar en las ultimas lineas el siguiente comando:

```cpp
.long sys_so2_update_memory_limit
```


## Remover el límite de un proceso

- Sedebepoderremover entradas de procesos previamente limitados
- Debeestar limitada al uso de usuarios sudoers
- Debetener el ID de syscall 560
- Debecontar con la siguiente definición de función
```cpp
long SYSCALL_DEFINE1(so2_remove_memory_limit, pid_t, process_pid)
```
## Debemanejarerrores de la siguiente forma

- Devolver en el long, y TAMBIEN setear el errno con el codigo de resultado
- Si el proceso no existe con ese PID: devolver error -ESRCH
- Si el proceso NO esta en la lista:devolver error -102
- El PID es negativo : -EINVAL
- Si no es sudoer, -EPERM

<h3> Implementación </h3>

- Primero sera crear la ruta para el archivo: "syscall_64.tbl" el cual contendra nuestra primera syscall, teniendo en cuenta que el archivo se ubica en
- arch/x86/entry/syscalls/syscall_64.tbl

Se modifica hasta el final del archivo.

``` cpp
560 common so2_remove_memory_limit sys_so2_remove_memory_limit
```
- Agregar en el archivo "syscalls.h" lo siguiente y tomando en cuenta que debe de ser seguido de la anterior modificacion.
``` cpp
asmlinkage long sys_so2_remove_memory_limit(pid_t process_pid);
```

- Dentro del archivo "syscall7.c" agregar lo siguiente:

```cpp
 




```
En el archvio "Makefile" no se debe de modificar:


- Crear la ruta microblaze/kernel y dentro el archivo syscall_table.S y colocar en las ultimas lineas el siguiente comando:

```cpp
.long sys_so2_remove_memory_limit
```





<h1>Hacer pruebas</h1>


Para la primera prueba y poder verificar que las implementaciones estan correctas en el kernel, es necesario el siguiente archvio de prueba el cual se llama: test_syscall4.c

```cpp




```
- Para compilar el test se debe de ir desde la terminal a la ruta donde se guardo el archivo test_syscall1.c y como administrador ejecutar los siguientes comandos:

```cpp
gcc -o test_syscall4 test_syscall4.c
./test_syscall4
```

- Mostrara un en la terminal lo suguiente

![primera imagen](./imagenes_manual/pro2_1.png)

- sera necesario escribir el tamaño de la memoria que queremos y precionar "Enter"

![primera imagen](./imagenes_manual/pro2_2.png)

- como ultimo paso precionar la tecla "Enter" nuevamente 

![primera imagen](./imagenes_manual/pro2_3.png)

Para la segunda prueba y poder verificar que las implementaciones estan correctas en el kernel, es necesario el siguiente archvio de prueba el cual se llama: test_syscall5.c

```cpp
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

#define __NR_luis_recoleccion_general 555

// Estructura para almacenar la información de memoria
struct process_memory_info {
    unsigned long reserved_memory_kb;
    unsigned long committed_memory_kb;
    unsigned long used_memory_kb;
    int oom_score;
    int percentage_used_memory;
};

// Declaración de la syscall
long luis_recoleccion_general(pid_t pid, struct process_memory_info *mem_info) {
    return syscall(__NR_luis_recoleccion_general, pid, mem_info);
}

// Función para imprimir todos los procesos en una tabla
void print_all_memory_info() {
    // Colores ANSI
    const char* green = "\033[32m";
    const char* yellow = "\033[33m";
    const char* red = "\033[31m";
    const char* reset = "\033[0m";

    // Encabezado de la tabla
    printf("\n%s+-------+----------------------+------------------------+------------------------+------------------+-------------------------+%s\n", green, reset);
    printf("| %-5s | %-20s | %-22s | %-22s | %-16s | %-23s |\n", "PID", "Reservada (KB)", "Comprometida (KB)", "Usada (KB)", "OOM Score", "Porcentaje Usada (%)");
    printf("+-------+----------------------+------------------------+------------------------+------------------+-------------------------+\n");

    struct process_memory_info mem_info;
    long result;

    // Iterar sobre un rango razonable de PIDs
    for (pid_t pid = 1; pid <= 32768; pid++) {
        // Llamar a la syscall para obtener la recolección de memoria
        result = luis_recoleccion_general(pid, &mem_info);
        if (result < 0) {
            continue; // Ignorar procesos donde falle la syscall
        }

        // Calcular el color del porcentaje de memoria usada
        const char* used_color;
        if (mem_info.percentage_used_memory < 50) {
            used_color = green;
        } else if (mem_info.percentage_used_memory < 80) {
            used_color = yellow;
        } else {
            used_color = red;
        }

        // Imprimir la información del proceso en una fila
        printf("| %-5d | %-20lu | %-22lu | %-22lu | %-16d | %s%-3d%%%s                |\n",
               pid,
               mem_info.reserved_memory_kb,
               mem_info.committed_memory_kb,
               mem_info.used_memory_kb,
               mem_info.oom_score,
               used_color, mem_info.percentage_used_memory, reset);
    }

    // Cierre de la tabla
    printf("+-------+----------------------+------------------------+------------------------+------------------+-------------------------+\n");
}

int main() {
    pid_t pid;
    struct process_memory_info mem_info;
    long result;

    while (1) {
        // Solicitar el PID
        printf("Ingrese el PID del proceso (0 para mostrar todos los procesos): ");
        scanf("%d", &pid);

        if (pid == 0) {
            // Si el PID es 0, mostrar todos los procesos en una tabla
            print_all_memory_info();
        } else if (pid < 0) {
            // Verificar si se ingresa un PID inválido
            printf("PID no válido.\n");
            printf("Saliendo...\n");
            return 0;
        } else {
            // Si se ingresa un PID específico
            result = luis_recoleccion_general(pid, &mem_info);
            if (result < 0) {
                printf("Proceso con PID %d terminado o no encontrado.\n", pid);
            } else {
                // Mostrar la información de memoria para el PID ingresado
                printf("\n");
                printf("Información de memoria para el PID %d:\n", pid);
                printf("Reservada: %lu KB\n", mem_info.reserved_memory_kb);
                printf("Comprometida: %lu KB\n", mem_info.committed_memory_kb);
                printf("Usada: %lu KB\n", mem_info.used_memory_kb);
                printf("OOM Score: %d\n", mem_info.oom_score);
                printf("Porcentaje de memoria usada: %d%%\n", mem_info.percentage_used_memory);
            }
        }
        printf("\n");
    }

    return 0;
}
```

- Para compilar el test se debe de ir desde la terminal a la ruta donde se guardo el archivo test_syscall5.c y como administrador ejecutar los siguientes comandos:

```cpp
gcc -o test_syscall5 test_syscall5.c
./test_syscall5
```
- Mostrara un en la terminal lo suguiente

![primera imagen](./imagenes_manual/pro2_4.png)

- Mostrara un pequeño menu, si se preciona el 0 mostrara una lista de todos los procesos, si se escribe el PID de algun proceso mostrala solo los datos de ese proceso.

![primera imagen](./imagenes_manual/pro2_5.png)
![primera imagen](./imagenes_manual/pro2_6.png)


Para la tecera prueba y poder verificar que las implementaciones estan correctas en el kernel, es necesario el siguiente archvio de prueba el cual se llama: test_syscall6.c

```cpp
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

#define __NR_luis_resumen_total 556

// Estructura para almacenar la memoria total
struct memory_summary {
    unsigned long reserved_memory_mb; // Memoria reservada en MB
    unsigned long committed_memory_mb; // Memoria comprometida en MB
};

// Declaración de la syscall
long luis_resumen_total(struct memory_summary *summary) {
    return syscall(__NR_luis_resumen_total, summary);
}

void print_memory_summary(struct memory_summary summary) {
    const char* green = "\033[32m";   // Color verde
    const char* yellow = "\033[33m";  // Color amarillo
    const char* red = "\033[31m";     // Color rojo
    const char* reset = "\033[0m";    // Reset de color

    // Variables para el color de las columnas de memoria
    const char* reserved_color;
    const char* committed_color;

    // Lógica para asignar color a la memoria reservada
    if (summary.reserved_memory_mb < 1024) {
        reserved_color = green;  // Verde si es menos de 1GB
    } else if (summary.reserved_memory_mb < 2048) {
        reserved_color = yellow; // Amarillo si está entre 1GB y 2GB
    } else {
        reserved_color = red;    // Rojo si es más de 2GB
    }

    // Lógica para asignar color a la memoria comprometida
    if (summary.committed_memory_mb < 1024) {
        committed_color = green;  // Verde si es menos de 1GB
    } else if (summary.committed_memory_mb < 2048) {
        committed_color = yellow; // Amarillo si está entre 1GB y 2GB
    } else {
        committed_color = red;    // Rojo si es más de 2GB
    }

    // Imprimir la información con colores
    printf("\n%sResumen de Memoria Total:%s\n", green, reset);
    printf("+----------------------------------+------------------------------------+\n");
    printf("| %sMemoria Reservada (VmSize MB)%s    | %sMemoria Comprometida (VmRSS MB)%s    |\n", yellow, reset, yellow, reset);
    printf("+----------------------------------+------------------------------------+\n");
    printf("| %-26s%-4lu%s        | %-26s%-4lu%s       |\n", reserved_color, summary.reserved_memory_mb, reset, committed_color, summary.committed_memory_mb, reset);
    printf("+----------------------------------+------------------------------------+\n");
}

int main() {
    struct memory_summary summary;
    long result;

    // Llamar a la syscall para obtener el resumen de la memoria
    result = luis_resumen_total(&summary);
    if (result < 0) {
        perror("Error al obtener el resumen de memoria");
        return -1; // Salir en caso de error
    }

    // Mostrar el resumen de la memoria
    print_memory_summary(summary);

    return 0;
}
```

- Para compilar el test se debe de ir desde la terminal a la ruta donde se guardo el archivo test_syscall6.c y como administrador ejecutar los siguientes comandos:

```cpp
gcc -o test_syscall6 test_syscall6.c
./test_syscall6
```

- Mostrara un en la terminal lo suguiente

![primera imagen](./imagenes_manual/pro2_7.png)

<h1> Resolución de problemas y manejo de dificultades </h1>

Tener en cuenta que al estar modificando cosas del kernel es muy facil poder tener errores en la compilacion y esto haga que no podamos entrar a nuestro kernel, se debe de seguir los siguientes pasos:

- Entrar al group del sistema.
- Entrar a las opciones avanzadas
- Seleccionar el kernel por defecto, ya que el kernel por defecto esta separado del kernel que estamos modificando esto con el fin de poder tener un mejor control de los posibles errores y no se tenga que eliminar la maquina virtual.

![primera imagen](./imagenes_manual/Imagen%20de%20WhatsApp%202024-12-13%20a%20las%2014.37.02_a0f7dbd5.jpg)

Otro de los problemas puede ser que aparezca lo siguiente:

![primera imagen](./imagenes_manual/Imagen%20de%20WhatsApp%202024-12-13%20a%20las%2021.19.40_b38e1653.jpg)

Esto sucede porque en el archivo syscall_64.tbl no esta la llamada del syscall,se debe de colocar las llamadas de las llamadas al sistema

- Lo que se debe de hacer es realizar syscall por syscall y no hacer todo y luego compilar el archivo ya que sera más dificil encontrar el error

![primera imagen](./imagenes_manual/Captura%20de%20pantalla%202025-01-02%20004710.png)

![primera imagen](./imagenes_manual/Captura%20de%20pantalla%20(121).png)

- Es mejor luego de terminar una syscall compilar el kernel para que así tengamos de una mejor manera los posibles errores.

<h1> Reflexión personal y autoevaluación</h1>

+ Tome en cuenta la reflexion del proyecto pasado, leí varias veces el enunciado y pregunte al auxiliar en los laboratorios sobre todas mis dudas y problemas.

+ Gracias a la ayuda y guia del auxiliar logre completar de manera efectiva el proyecto2 y sobre todo en base a una investigación detallada sobre lo que debo hacer y luego comenzar con la implementación y las pruebas correspondientes.

+ También debo evitar pensar en todo lo que se debe de hacer en el enunciado, ya que esto me genera ansiedad, pereza y estrés. Es mejor abordarlo poco a poco. Dividir el proyecto en tareas más pequeñas me permitirá avanzar de manera más ordenada y menos abrumada.

