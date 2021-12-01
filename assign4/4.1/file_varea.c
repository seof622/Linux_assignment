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

__SYSCALL_DEFINEx(1, ftrace, pid_t, pid){

	struct task_struct *t;
	struct mm_struct *mm;
	struct vm_area_struct *vm;

	//parameter for get file path
	char* file_path = NULL;
	struct file* exe_file;
	char path_buf[100];

	t = pid_task(find_vpid(pid), PIDTYPE_PID);
	mm = get_task_mm(t);	
	vm = mm->mmap;

	printk(KERN_INFO "####### Loaded files of a process \'%s(%d) \' in VM #######", t->comm, t->pid);

	//get File Path
	down_read(&mm->mmap_sem);
	exe_file = mm->exe_file;
	if(exe_file)get_file(exe_file);
	up_read(&mm->mmap_sem);
	file_path = d_path(&exe_file->f_path, path_buf, 100*sizeof(char));

	//Print Kernel Message
	printk(KERN_INFO "mem(%d~%d) code(%d~%d) data(%d~%d) heap(%d~%d) %s \n" ,vm->vm_start, vm->vm_end ,mm->start_code, mm->end_code, mm->start_data, mm->end_data, mm->start_brk, mm->brk, file_path); 	
	printk(KERN_INFO "##############################################################\n");
	mmput(mm);

	return pid;
}

void make_rw(void *addr){
	unsigned int level;
	pte_t *pte = lookup_address((u64)addr, &level);
	if(pte -> pte &~ _PAGE_RW){
		pte -> pte |= _PAGE_RW;
	}
}

void make_ro(void *addr){
	unsigned int level;
	pte_t *pte = lookup_address((u64)addr, &level);

	pte -> pte = pte -> pte &~ _PAGE_RW;
}
static int __init ftracehooking_init(void){
	syscall_table = (void**) kallsyms_lookup_name("sys_call_table");
	make_rw(syscall_table);//Change syscall
	real_ftrace = syscall_table[__NR_ftrace];
	syscall_table[__NR_ftrace] = __x64_sysftrace;
	return 0;
}

static void __exit ftracehooking_exit(void){
	syscall_table[__NR_ftrace] = real_ftrace;
	make_ro(syscall_table);//Recover syscall
}

module_init(ftracehooking_init);
module_exit(ftracehooking_exit);
MODULE_LICENSE("GPL");

