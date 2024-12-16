# Este Makefile compila un módulo de kernel llamado stats_module.c

# Nombre del módulo
obj-m += stats_module.o

# Instrucciones para compilar
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

# Instrucciones para limpiar los archivos generados
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
