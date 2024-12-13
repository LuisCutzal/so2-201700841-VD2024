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
/* Move somewhere else to avoid recompiling? */
#include <generated/utsrelease.h>

#include <linux/uaccess.h>
#include <asm/io.h>
#include <asm/unistd.h>

#include <linux/time.h>
#include <linux/ktime.h>
#include "syscall2.h"

//estructura para guardar los valores
struct syscall_usage{
    unsigned long count;
    struct timespec64 time_last_used;
};  // <- Punto y coma aquí

#define MAX_SYS_CALLS 1024
static struct syscall_usage *syscall_counters;

static int init_syscall_counters(void);  // Declarar la función

SYSCALL_DEFINE1(luis_track_syscall_usage, struct syscall_usage __user *, estatic)
{
    if(!syscall_counters){ // Si ya inicializamos el array de contadores
        if(init_syscall_counters() != 0){ // En caso de no inicializarlo
            return -ENOMEM; // Equivalente a retornar -ENOMEM
        }
    }

    // Devolvemos estadísticas al usuario
    int resultadoCopi = copy_to_user(estatic, syscall_counters, sizeof(struct syscall_usage) * MAX_SYS_CALLS);
    if (resultadoCopi) {
        return -EFAULT;
    }
    return 0;
}

//funcion para inicializar las variables
static int init_syscall_counters(void){
    syscall_counters = kzalloc(
        sizeof(struct syscall_usage) * MAX_SYS_CALLS, //espacio de una estructura * 
        GFP_KERNEL);
    if(syscall_counters == NULL){
        return -ENOMEM;
    }
    return 0;
}

void track_syscall(int syscall_id){
    if(!syscall_counters || syscall_id >= MAX_SYS_CALLS){
        return;
    }
    //en caso si nos interese 
    syscall_counters[syscall_id].count++;
    struct timespec64 now;  // Declarar la variable now
    ktime_get_real_ts64(&now);
    syscall_counters[syscall_id].time_last_used = now;
}
