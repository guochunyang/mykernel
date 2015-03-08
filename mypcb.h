/*
 *  linux/mykernel/mypcb.h
 *
 *  Kernel internal PCB types
 *
 *  Copyright (C) 2013  Mengning
 *
 */

#define MAX_TASK_NUM        4
#define KERNEL_STACK_SIZE   1024*8

/* CPU-specific state of this task */
// CPU特定状态
struct Thread {
    unsigned long		ip;	// eip寄存器
    unsigned long		sp;	// esp  栈顶寄存器
};

// 进程控制块
typedef struct PCB{
    int pid;	// 进程id
    // 进程状态，未运行、可运行、停止
    volatile long state;	/* -1 unrunnable, 0 runnable, >0 stopped */
    // 进程的栈空间
    char stack[KERNEL_STACK_SIZE];
    /* CPU-specific state of this task */
    struct Thread thread;	// CPU相关的状态
    unsigned long	task_entry;
    struct PCB *next; // 下个进程
}tPCB;

void my_schedule(void);

