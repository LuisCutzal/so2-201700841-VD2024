#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/statfs.h>
#include <linux/uaccess.h>
#include <linux/namei.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Luis Antonio Cutzal Chalí");
MODULE_DESCRIPTION("Modulo para estadisticas de CPU, memoria y almacenamiento");
MODULE_VERSION("1.0");

#define PARTITION_PATH "/"

static int mostrar_estadisticas(struct seq_file *m, void *v) {
    // Estadísticas de CPU
    struct file *file;
    char buf[256];
    ssize_t read_bytes;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    unsigned long long total, busy;

    file = filp_open("/proc/stat", O_RDONLY, 0);
    if (IS_ERR(file)) {
        seq_printf(m, "Error leyendo /proc/stat\n");
        return -1;
    }

    read_bytes = kernel_read(file, buf, sizeof(buf) - 1, &file->f_pos);
    filp_close(file, NULL);

    if (read_bytes > 0) {
        buf[read_bytes] = '\0';
        sscanf(buf, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
               &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
        total = user + nice + system + idle + iowait + irq + softirq + steal;
        busy = total - idle;
        seq_printf(m, "CPU Usage: %llu%%\n", busy * 100 / total);
    } else {
        seq_printf(m, "Error leyendo datos de CPU\n");
    }

    // Estadísticas de Memoria desde /proc/meminfo
    struct file *mem_file;
    char mem_buf[256];
    unsigned long total_mem = 0, free_mem = 0;

    mem_file = filp_open("/proc/meminfo", O_RDONLY, 0);
    if (!IS_ERR(mem_file)) {
        read_bytes = kernel_read(mem_file, mem_buf, sizeof(mem_buf) - 1, &mem_file->f_pos);
        filp_close(mem_file, NULL);
        if (read_bytes > 0) {
            mem_buf[read_bytes] = '\0';
            sscanf(mem_buf, "MemTotal: %lu kB\nMemFree: %lu kB\n", &total_mem, &free_mem);
        }
    }
    seq_printf(m, "Memory Total: %lu KB\n", total_mem);
    seq_printf(m, "Memory Free: %lu KB\n", free_mem);

    // Estadísticas de Almacenamiento
    struct kstatfs stat;
    struct path path;

    if (kern_path(PARTITION_PATH, LOOKUP_FOLLOW, &path) == 0) {
        if (vfs_statfs(&path, &stat) == 0) {
            unsigned long long total_space = (stat.f_blocks * stat.f_bsize) >> 10; // KB
            unsigned long long free_space = (stat.f_bfree * stat.f_bsize) >> 10;   // KB
            seq_printf(m, "Storage Total: %llu KB\n", total_space);
            seq_printf(m, "Storage Free: %llu KB\n", free_space);
        } else {
            seq_printf(m, "Error obteniendo estadísticas de almacenamiento\n");
        }
        path_put(&path);
    } else {
        seq_printf(m, "Error obteniendo el path para %s\n", PARTITION_PATH);
    }

    return 0;
}

static int abrir_proc(struct inode *inode, struct file *file) {
    return single_open(file, mostrar_estadisticas, NULL);
}

static const struct proc_ops proc_ops = {
    .proc_open = abrir_proc,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init stats_module_init(void) {
    proc_create("stats_module", 0, NULL, &proc_ops);
    pr_info("Modulo de estadisticas cargado correctamente\n");
    return 0;
}

static void __exit stats_module_exit(void) {
    remove_proc_entry("stats_module", NULL);
    pr_info("Modulo de estadisticas eliminado\n");
}

module_init(stats_module_init);
module_exit(stats_module_exit);
