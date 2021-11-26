#include "ftracehooking.h"

__SYSCALL_DEFINEx(1, ftrace, pid_t, pid){
	
	//find exe file name
	struct task_struct *findtask = &init_task;
	struct task_struct *sibling_struct, *child_struct, *pgid_struct;
	struct task_struct *my_parent = current -> parent;
	struct list_head *list_sibling,*list_child;
	struct pid *pgid;	
	int count_sibling = 0, count_child = 0;

	do{
		findtask = next_task(findtask);
	}while(findtask -> pid != pid);

	//print pid, exe file name
	printk(KERN_INFO "##### TASK INFORMATION of ''[%d] %s '' #####", findtask -> pid, findtask -> comm);
	//print state of current state
	switch(findtask->state){
		case 0:
			printk(KERN_INFO "- task state : Running or ready");
			break;
		case 1:
			printk(KERN_INFO "- task state : Wait");
                        break;
		case 2:
			printk(KERN_INFO "- task state : Wait with ignoring all signals");
                        break;
		case 4:
			printk(KERN_INFO "- task state : Stopped");
			break;
		case 10:
			printk(KERN_INFO "- task state : Dead");
			break;
		case 20:
                        printk(KERN_INFO "- task state : Zombie process");
                        break;
		default:
			printk(KERN_INFO "- task state : etc.");
			break;
	};
	
	pgid = task_pgrp(findtask->pid);
	if(pgid!=NULL)pgid_struct = pid_task(pgid , PIDTYPE_PID);
	else return -1;
	if(pgid_struct!=NULL)printk( "- Process Group Leader : [%d] %s", pgid_struct -> pid, pgid_struct -> comm);
	else return -1;
	printk(KERN_INFO "- Number of context switches : %d", findtask -> nivcsw);
	printk(KERN_INFO "- it's parent process : [%d] %s", my_parent -> pid, my_parent -> comm);
	printk(KERN_INFO "- it's sibling process(es) :");
	list_for_each(list_sibling, &my_parent -> children){
		sibling_struct = list_entry(list_sibling, struct task_struct, sibling);
		for_each_process(sibling_struct){
			printk(KERN_INFO "> [%d] %s", sibling_struct -> pid, sibling_struct -> comm);
			count_sibling++;
		}
	}
	if(count_sibling == 0)printk(KERN_INFO " > It has no sibling.");	
	else	printk(KERN_INFO "> This process has %d sibling process(es)", count_sibling);

           
        list_for_each(list_child, &current -> children){
        	child_struct = list_entry(list_child, struct task_struct, sibling);
	        for_each_process(child_struct){
		        printk(KERN_INFO "> [%d] %s", child_struct -> pid, child_struct -> comm);
		        count_child++;
	        }
        }
        

	if(count_child == 0)printk(KERN_INFO "> It has no child.");
        else	printk(KERN_INFO "> This process has %d child process(es)", count_child);
 
	printk("##### END OF INFORMATION #####");
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

