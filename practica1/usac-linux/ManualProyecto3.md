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
SYSCALL_DEFINE2(so2_add_memory_limit, pid_t, process_pid, size_t, memory_limit) {
    struct memory_limitation *entry;
    struct task_struct *task;
    struct mm_struct *mm;

    // Validar entrada: PID negativo o memoria negativa
    if (process_pid <= 0 || memory_limit <= 0) {
        return -EINVAL;  // Devolver error EINVAL
    }

    // Verificar permisos de usuario
    if (!capable(CAP_SYS_ADMIN)) {
        return -EPERM;  // Devolver error EPERM
    }

    // Buscar el proceso
    task = pid_task(find_vpid(process_pid), PIDTYPE_PID);
    if (!task) {
        return -ESRCH;  // Devolver error ESRCH si el proceso no existe
    }

    // Verificar memoria usada
    mm = task->mm;
    if (!mm) {
        return -ESRCH;  // Devolver error ESRCH si no se encuentra el mm del proceso
    }
    if (get_mm_rss(mm) * PAGE_SIZE / 1024 > memory_limit) { // Convertimos a KB
        return -100;  // Devolver error -100 si el proceso excede el límite
    }

    // Bloquear la lista para acceso seguro
    mutex_lock(&memory_limit_lock);

    // Verificar si ya existe el proceso en la lista
    list_for_each_entry(entry, &memory_limit_list, list) {
        if (entry->pid == process_pid) {
            mutex_unlock(&memory_limit_lock);
            return -101;  // Devolver error -101 si el proceso ya está en la lista
        }
    }

    // Crear nueva entrada en la lista
    entry = kmalloc(sizeof(*entry), GFP_KERNEL);
    if (!entry) {
        mutex_unlock(&memory_limit_lock);
        return -ENOMEM;  // Devolver error ENOMEM si no hay memoria para el nodo
    }
    entry->pid = process_pid;
    entry->memory_limit = memory_limit;
    INIT_LIST_HEAD(&entry->list);

    // Agregar la entrada a la lista
    list_add(&entry->list, &memory_limit_list);

    // Desbloquear la lista
    mutex_unlock(&memory_limit_lock);

    return 0; // Éxito
}
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
SYSCALL_DEFINE3(so2_get_memory_limits, 
                     struct memory_limitation*, u_processes_buffer, 
                     size_t, max_entries, 
                     int*, processes_returned) {
    struct memory_limitation *kernel_buffer;
    struct memory_limitation *entry;
    size_t count = 0;
    int result;

    // Validar entradas
    if (max_entries <= 0 || !u_processes_buffer || !processes_returned) {
        return -EINVAL;
    }

    // Bloquear lista para acceso seguro
    mutex_lock(&memory_limit_lock);

    // Contar los procesos limitados
    list_for_each_entry(entry, &memory_limit_list, list) {
        count++;
    }

    // Ajustar el número máximo al límite del buffer proporcionado
    if (count > max_entries) {
        count = max_entries;
    }

    // Reservar espacio en el kernel para el buffer
    kernel_buffer = kmalloc_array(count, sizeof(struct memory_limitation), GFP_KERNEL);
    if (!kernel_buffer) {
        mutex_unlock(&memory_limit_lock);
        return -ENOMEM;
    }

    // Copiar los procesos limitados al buffer del kernel
    count = 0;
    list_for_each_entry(entry, &memory_limit_list, list) {
        if (count >= max_entries) {
            break;
        }
        kernel_buffer[count].pid = entry->pid;
        kernel_buffer[count].memory_limit = entry->memory_limit;
        count++;
    }

    // Desbloquear la lista
    mutex_unlock(&memory_limit_lock);

    // Copiar datos del kernel al espacio de usuario
    result = copy_to_user(u_processes_buffer, kernel_buffer, count * sizeof(struct memory_limitation));
    if (result) {
        kfree(kernel_buffer);
        return -EFAULT;
    }

    // Copiar la cantidad de procesos retornados al espacio de usuario
    result = put_user(count, processes_returned);
    if (result) {
        kfree(kernel_buffer);
        return -EFAULT;
    }

    // Liberar el buffer del kernel
    kfree(kernel_buffer);

    return 0; // Éxito
}
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
long SYSCALL_DEFINE2(so2_update_memory_limit, pid_t, process_pid, size_t, memory_limit) {
    struct memory_limitation *entry;
    struct task_struct *task;
    struct mm_struct *mm;

    // Validaciones de entrada
    if (process_pid <= 0 || memory_limit <= 0) {
        set_errno(EINVAL);
        return -EINVAL;
    }

    // Verificar permisos de usuario
    if (!capable(CAP_SYS_ADMIN)) {
        set_errno(EPERM);
        return -EPERM;
    }

    // Buscar el proceso en el sistema
    task = pid_task(find_vpid(process_pid), PIDTYPE_PID);
    if (!task) {
        set_errno(ESRCH);
        return -ESRCH;
    }

    // Obtener memoria usada por el proceso
    mm = task->mm;
    if (!mm) {
        set_errno(ESRCH);
        return -ESRCH;
    }
    if (get_mm_rss(mm) * PAGE_SIZE / 1024 > memory_limit) { // Convertimos a KB
        set_errno(100); // Código de error personalizado
        return -100;
    }

    // Bloquear la lista para acceso seguro
    mutex_lock(&memory_limit_lock);

    // Buscar en la lista global
    list_for_each_entry(entry, &memory_limit_list, list) {
        if (entry->pid == process_pid) {
            // Actualizar el límite de memoria
            entry->memory_limit = memory_limit;
            mutex_unlock(&memory_limit_lock);
            return 0; // Éxito
        }
    }

    // Desbloquear la lista si no se encontró el proceso
    mutex_unlock(&memory_limit_lock);

    // Proceso no encontrado en la lista
    set_errno(102); // Código de error personalizado
    return -102;
}
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
 SYSCALL_DEFINE1(so2_remove_memory_limit, pid_t, process_pid) {
    struct memory_limitation *entry, *tmp;
    int found = 0;

    // Validar que el PID sea positivo
    if (process_pid <= 0) {
        set_errno(EINVAL);
        return -EINVAL;
    }

    // Verificar si el usuario es un sudoer
    if (!capable(CAP_SYS_ADMIN)) {
        set_errno(EPERM);
        return -EPERM;
    }

    // Bloquear la lista para acceso seguro
    mutex_lock(&memory_limit_lock);

    // Buscar el proceso en la lista
    list_for_each_entry_safe(entry, tmp, &memory_limit_list, list) {
        if (entry->pid == process_pid) {
            // Encontrado el proceso, eliminarlo
            list_del(&entry->list);
            kfree(entry);  // Liberar la memoria asignada
            found = 1;
            break;
        }
    }

    // Desbloquear la lista
    mutex_unlock(&memory_limit_lock);

    // Si no se encontró el proceso en la lista, devolver error -102
    if (!found) {
        set_errno(ESRCH);
        return -102;
    }

    return 0;  // Éxito
}
```
En el archvio "Makefile" no se debe de modificar:


- Crear la ruta microblaze/kernel y dentro el archivo syscall_table.S y colocar en las ultimas lineas el siguiente comando:

```cpp
.long sys_so2_remove_memory_limit
```

<h1>Hacer pruebas</h1>


Para la primera prueba y poder verificar que las implementaciones estan correctas en el kernel, es necesario el siguiente archvio de prueba el cual se llama: test_syscall7.c, este test es un ejemplo compartido por el auxiliar del curso.

```cpp
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <stdlib.h>

#define SYS_MATUS_ADD_MEMORY_LIMIT 557
#define SYS_MATUS_GET_MEMORY_LIMITS 558
#define SYS_MATUS_UPDATE_MEMORY_LIMIT 559
#define SYS_MATUS_REMOVE_MEMORY_LIMIT 560

struct memory_limitation {
	pid_t pid;
	size_t memory_limit;
};

void add_memory_limit(pid_t pid, size_t memory_limit) {
	//TODO Chequeo de errores como en el enunciado

	if (syscall(SYS_MATUS_ADD_MEMORY_LIMIT, pid, memory_limit) < 0) {  //FIXME != 0
		perror("SYS_MATUS_ADD_MEMORY_LIMIT");
		return;
	}
	printf("Memory limit added for PID %d %zu bytes \n", pid, memory_limit);
}

void get_memory_limits(size_t max_entries) {
	struct memory_limitation *buffer = malloc(max_entries * sizeof(struct memory_limitation));
	int processes_returned;

	if (!buffer) {
		perror("Allocation for memory_limitation buffer failed");
		return;
	}

	if (syscall(SYS_MATUS_GET_MEMORY_LIMITS, buffer, max_entries, &processes_returned) < 0) {  //FIXME != 0
		perror("SYS_MATUS_GET_MEMORY_LIMITS");
		free(buffer);
		return;
	}

	printf("Restricted proccesses memory succesfully:\n");
	for (int i = 0; i < processes_returned; ++i) {
		printf("PID: %d, Memory Limit: %zu bytes\n", buffer[i].pid, buffer[i].memory_limit);
	}
	free(buffer);
}

void update_memory_limit(pid_t pid, size_t memory_limit) {
	if (syscall(SYS_MATUS_UPDATE_MEMORY_LIMIT, pid,  memory_limit) < 0) {  //FIXME != 0
		perror("SYS_MATUS_UPDATE_MEMORY_LIMIT");
		return;
	}
	printf("Memory limit for PID %d updated to %zu \n", pid, memory_limit);
}


void remove_memory_limit(pid_t pid) {
	if (syscall(SYS_MATUS_REMOVE_MEMORY_LIMIT, pid) < 0) {  //FIXME != 0
		perror("SYS_MATUS_REMOVE_MEMORY_LIMIT");
		return;
	}
	printf("Memory limit for PID %d removed\n", pid);
}

int main() {
	int choice;
	pid_t pid;
	size_t memory_limit;
	size_t max_entries;

	while (1) {
		printf("-----------------------------------------------------------------\n");
		printf("\nMemory Limitation for Project 3 SO2 VD2024\n");
		printf("1. Add Memory Limit\n");
		printf("2. Get Memory Limit\n");
		printf("3. Update Memory Limit\n");
		printf("4. Remove Memory Limit\n");
		printf("5. Exit\n");
		printf("Enter a number option to proceed\n");
		scanf("%d", &choice);
		printf("-----------------------------------------------------------------\n");

		switch(choice) {
			case 1:
				printf("Enter PID:");
				scanf("%d", &pid);
				printf("Enter Memory limit in KB:");
				scanf("%zu", &memory_limit);
				memory_limit *= 1024;
				add_memory_limit(pid, memory_limit);
				break;
			case 2:
				printf("Enter Max entries to receive:");
				scanf("%zu", &max_entries);
				get_memory_limits(max_entries);
				break;
			case 3:
				printf("Enter PID:");
				scanf("%d", &pid);
				printf("Enter Memory limit in KB:");
				scanf("%zu", &memory_limit);
				memory_limit *= 1024;
				update_memory_limit(pid, memory_limit);
				break;
			case 4:
				printf("Enter PID to remove limit:");
				scanf("%d", &pid);
				remove_memory_limit(pid);
				break;
			case 5:
				printf("Exiting...\n");
				return 0;
				break;
			default:
				printf("Invalid option. Try again!");
		}
	}
	return 0;
}
```
- Para compilar el test se debe de ir desde la terminal a la ruta donde se guardo el archivo test_syscall1.c y como administrador ejecutar los siguientes comandos:

```cpp
gcc -o test_syscall7 test_syscall7.c
./test_syscall7
```

- Mostrara un en la terminal con un menú

- Sera necesario escribir el tamaño de la memoria que queremos y precionar "Enter"

![primera imagen](./imagenes_manual/Imagen%20de%20WhatsApp%202024-12-31%20a%20las%2018.46.13_54bbbae7.jpg)

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

- El error se soluciono eliminando el long luego de una definicio de la syscall: SYSCALL_DEFINE y si lo usamos estamos generando un conflicto en la declaración de la función y el compilador no podrá procesarla correctamente

<h1> Reflexión personal y autoevaluación</h1>

+ Tome en cuenta la reflexion del proyecto pasado, leí varias veces el enunciado y pregunte al auxiliar en los laboratorios sobre todas mis dudas y problemas.

+ Gracias a la ayuda y guia del auxiliar logre completar de manera efectiva el proyecto2 y sobre todo en base a una investigación detallada sobre lo que debo hacer y luego comenzar con la implementación y las pruebas correspondientes.

+ También debo evitar pensar en todo lo que se debe de hacer en el enunciado, ya que esto me genera ansiedad, pereza y estrés. Es mejor abordarlo poco a poco. Dividir el proyecto en tareas más pequeñas me permitirá avanzar de manera más ordenada y menos abrumada.

+ Tengo que aprender a no distribuir bien mi tiempo entre los 2 cursos que llevé estas vacaciones de diciembre, porque estar haciendo el proyecto a ultima hora es bastante pesado.
