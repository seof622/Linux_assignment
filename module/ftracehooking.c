#include "ftracehooking.h"

__SYSCALL_DEFINEx(1, ftrace, pid_t, pid){

	struct task_struct *t;
	struct mm_struct *mm;
	struct vm_area_struct *vm;
	char* file_path = NULL;
	struct file* exe_file;
	char path_buf[100];
	t = pid_task(find_vpid(pid), PIDTYPE_PID);
	mm = get_task_mm(t);	
	vm = mm->mmap;

	printk(KERN_INFO "####### Loaded files of a process \'%s(%d) \' in VM #######", t->comm, t->pid);
	down_read(&mm->mmap_sem);
	exe_file = mm->exe_file;
	if(exe_file)get_file(exe_file);
	up_read(&mm->mmap_sem);
	file_path = d_path(&exe_file->f_path, path_buf, 100*sizeof(char));
//	//printk(KERN_INFO "name[%s]\ncode[%d~%d]\n",t->comm, mm->start_code, mm->end_code );

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

