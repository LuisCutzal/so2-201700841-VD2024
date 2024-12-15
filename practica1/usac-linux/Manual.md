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



<h3> Tambien es necesario que cada archvio que se necesite modificar se utilice el comando para crear el acceso directo, tener en cuenta que se debe de crear una ruta igual a la ruta que tiene el archivo original</h3>

![primera imagen](./imagenes_manual/Captura%20de%20pantalla%20(108).png)

