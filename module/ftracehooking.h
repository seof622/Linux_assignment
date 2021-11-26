#include <linux/module.h>
#include <asm/atomic.h>
#include <linux/sched/mm.h>
#include <linux/highmem.h>
#include <linux/kallsyms.h>
#include <linux/syscalls.h>
#include <asm/syscall_wrapper.h>
#include <linux/init_task.h>
#include <linux/sched.h>
#include <linux/fdtable.h>
#include <linux/init.h>

#define __NR_ftrace 336

void **syscall_table;
void *real_ftrace;

