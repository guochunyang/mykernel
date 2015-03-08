/*
 *  linux/mykernel/mymain.c
 *
 *  Kernel internal my_start_kernel
 *
 *  Copyright (C) 2013  Mengning
 *
 */
#include <linux/types.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/tty.h>
#include <linux/vmalloc.h>


#include "mypcb.h"

tPCB task[MAX_TASK_NUM]; // 所有的进程
tPCB * my_current_task = NULL; // 当前进程的指针
volatile int my_need_sched = 0;

void my_process(void);


void __init my_start_kernel(void)
{
    int pid = 0;
    int i;
    /* Initialize process 0*/
    // 初始化0号进程
    task[pid].pid = pid;
    task[pid].state = 0;/* -1 unrunnable, 0 runnable, >0 stopped */
    // eip指向my_process代码段
    task[pid].task_entry = task[pid].thread.ip = (unsigned long)my_process;
    // esp指向栈底，此时为空栈，栈的地址增长空间为从高到低
    task[pid].thread.sp = (unsigned long)&task[pid].stack[KERNEL_STACK_SIZE-1];
    // 此时链表中的进程只有一个，这是一个循环链表
    task[pid].next = &task[pid];
    /*fork more process */
    // 依次启动NUM个进程
    for(i=1;i<MAX_TASK_NUM;i++)
    {
        // 初始化PCB控制块
        memcpy(&task[i],&task[0],sizeof(tPCB));
        task[i].pid = i;
        task[i].state = -1; // 从未运行过的
        task[i].thread.sp = (unsigned long)&task[i].stack[KERNEL_STACK_SIZE-1];
        task[i].next = task[i-1].next; // 将上个进程的next（其实就是0号进程的地址）赋给进程的next
        task[i-1].next = &task[i];  // 将当前进程，链接到上个进程的后面
    }
    /* start process 0 by task[0] */
    pid = 0;
    my_current_task = &task[pid]; // 当前运行的进程为0
	asm volatile(
    	"movl %1,%%esp\n\t" 	/* set task[pid].thread.sp to esp */
    	"pushl %1\n\t" 	        /* push ebp */
    	"pushl %0\n\t" 	        /* push task[pid].thread.ip */
    	"ret\n\t" 	            /* pop task[pid].thread.ip to eip */
    	"popl %%ebp\n\t"
    	: 
    	: "c" (task[pid].thread.ip),"d" (task[pid].thread.sp)	/* input c or d mean %ecx/%edx*/
	);
}   

// 进程的运行逻辑
void my_process(void)
{
    int i = 0;
    while(1)
    {
        i++;
        // 每一千万次循环
        if(i%10000000 == 0)
        {
            // 该进程停止运行
            printk(KERN_NOTICE "this is process %d -\n",my_current_task->pid);
            if(my_need_sched == 1)
            {
                my_need_sched = 0;
        	    my_schedule(); // 执行调度
        	}
            // 该进程开始运行
        	printk(KERN_NOTICE "this is process %d +\n",my_current_task->pid);
        }     
    }
}