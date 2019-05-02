#include <linux/init.h>  //包含模块初始化和清理函数的定义
#include <linux/module.h>//包含加载模块所需要的函数和符号的定义
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init_task.h>

static int all_task_init(void){
     struct task_struct *p;
     for_each_process(p){//宏for_each_process()依次访问链表的每个进程
	if(p->mm==NULL)//如果是内核线程,p->mm等于NULL
	    printk(KERN_ALERT"内核线程程序名:%s\tPID:%d\t进程状态:%ld\t进程优先级:%d\t父进程的PID:%d\n",p->comm,p->pid,p->state,p->prio,p->parent->pid);
	    //printk可以指定消息的打印级别,内核根据这个指定的级别来决定是否将消息打印到终端上
	    //KERN_ALERT:必须立即响应
     }
     return 0;
}

static void all_task_exit(void){
	printk(KERN_ALERT"all_task module exit!\n");
}

module_init(all_task_init);
module_exit(all_task_exit);
MODULE_LICENSE("GPL");//模块声明许可


