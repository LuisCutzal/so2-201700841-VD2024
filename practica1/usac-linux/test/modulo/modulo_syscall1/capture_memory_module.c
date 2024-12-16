#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/seq_file.h>
#include <linux/init.h>

#include "syscall1.h"  // Estructura memory_snapshot

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Luis Antonio Cutzal Chalí");
MODULE_DESCRIPTION("Módulo para capturar un snapshot de la memoria usando una syscall");
MODULE_VERSION("1.0");

static int mostrar_snapshot(struct seq_file *m, void *v)
{
    struct memory_snapshot snapshot;
    long ret;

    // Llamar a la syscall para capturar el estado de la memoria
    ret = syscall(551, &snapshot);  // 551 es el número de la syscall
    if (ret != 0) {
        seq_printf(m, "Error al capturar el snapshot de memoria\n");
        return -EFAULT;
    }

    // Mostrar el estado de la memoria
    seq_printf(m, "Total Memory: %llu KB\n", snapshot.total_memory / 1024);
    seq_printf(m, "Free Memory: %llu KB\n", snapshot.free_memory / 1024);
    seq_printf(m, "Used Memory: %llu KB\n", snapshot.used_memory / 1024);
    seq_printf(m, "Active Pages: %llu\n", snapshot.active_pages);
    seq_printf(m, "Cache Pages: %llu\n", snapshot.cache_pages);
    seq_printf(m, "Swap Pages: %llu\n", snapshot.swap_pages);

    return 0;
}

static int open_snapshot(struct inode *inode, struct file *file)
{
    return single_open(file, mostrar_snapshot, NULL);
}

static const struct file_operations proc_fops = {
    .owner = THIS_MODULE,
    .open = open_snapshot,
    .read = seq_read,
    .release = single_release,
};

static int __init capture_memory_module_init(void)
{
    proc_create("capture_memory_snapshot", 0, NULL, &proc_fops);
    pr_info("Módulo para capturar snapshot de memoria cargado\n");
    return 0;
}

static void __exit capture_memory_module_exit(void)
{
    remove_proc_entry("capture_memory_snapshot", NULL);
    pr_info("Módulo para capturar snapshot de memoria descargado\n");
}

module_init(capture_memory_module_init);
module_exit(capture_memory_module_exit);

//make -C /lib/modules/$(uname -r)/build M=$(PWD) modules
//sudo insmod capture_memory_module.ko
//cat /proc/capture_memory_snapshot
//sudo rmmod capture_memory_module 