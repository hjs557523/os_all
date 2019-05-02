#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>   //task_struct
#include <linux/init_task.h>
#include <linux/moduleparam.h> //含参模块
MODULE_LICENSE("GPL");

static int pid;//获取参数值的变量声明为全局变量
module_param(pid,int,0644);//加载模块时传递参数

static int family_task_init(void){
       struct task_struct *own,*parent,*ptr = NULL;
       struct list_head *head,*list_ptr = NULL;

       //获得sibling在task_struct中的相对偏移量
       unsigned long sibling_offset = (unsigned long)&((struct task_struct *)0)->sibling;

       for_each_process(own){//依次访问链表的每个进程
	   if(own->pid == pid){
	       //当前进程:
               printk("本进程:程序名(comm)=%s\t 进程号(pid)=%d\t 进程状态(state)=%ld\n",own->comm,pid,own->state);

	       //父进程:
	       parent = own->parent;
	       printk("父进程:程序名(comm)=%s\t 进程号(pid)=%d\t 进程状态(state)=%ld\n",parent->comm,parent->pid,parent->state);

	       //子进程:
	       head = &own->children;//头指针
	       list_ptr = head->next;
	       while(list_ptr != head){
		    ptr = (struct task_struct *)((char *)list_ptr - sibling_offset);
		    printk("子进程:程序名(comm)=%s\t 进程号(pid)=%d\t 进程状态(state)=%ld\n",ptr->comm,ptr->pid,ptr->state);
		    list_ptr = list_ptr->next;
	       }

	       //兄弟进程
	       head = &own->parent->children;//头指针
	       list_ptr = head->next;
	       while(list_ptr != head){
		    ptr = (struct task_struct *)((char *)list_ptr - sibling_offset);
		    if(ptr->pid != pid){
		        printk("兄弟进程:程序名(comm)=%s\t 进程号(pid)=%d\t 进程状态(state)=%ld",ptr->comm,ptr->pid,ptr->state);
		    }
		    list_ptr = list_ptr->next;
	       }
	       break;
	   }
       }
       return 0;
}

static void family_task_exit(void){
     printk("family_task_mod exit\n");
}

module_init(family_task_init);
module_exit(family_task_exit);

