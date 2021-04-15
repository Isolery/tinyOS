#include "tinyOS.h"
#include "ARMCM3.h"

__asm void PendSV_Handler(void)
{
    IMPORT currentTask
    IMPORT nextTask

    MRS R0, PSP
    CBZ R0, PendSVHandler_nosave    // PSP = 0说明当前要执行的是初始任务, 也就不需要保存上下文信息了

	// 下面是用于保存要被切换的任务上下文信息
    STMDB R0!, {R4-R11}

    LDR R1, =currentTask
    LDR R1, [R1]
    STR R0, [R1]

PendSVHandler_nosave          // 执行初始任务, 初始任务执行完后就再也不会执行到该处  

	// 下面是进行任务的切换
    LDR R0, =currentTask
    LDR R1, =nextTask
    LDR R2, [R1]
    STR R2, [R0]          // currentTask = nextTask

    LDR R0, [R2]          // 从currentTask中取出堆栈的地址, 也就是要切换的任务的堆栈地址
    LDMIA R0!, {R4-R11}   // 加载R4-R11寄存器的值

    MSR PSP, R0
    ORR LR, LR, #0x04    // 返回后使用PSP指针
    BX  LR
}

void tTaskRunFirst()
{
    __set_PSP(0);

    *(volatile unsigned long *)0xE000ED22 = 0x000000FF;    // 设置PendSV异常优先级为最低
	*(volatile unsigned long *)0xE000ED04 = 0x10000000;    // 触发PendSV异常
}

void tTaskSwitch()
{
    *(volatile unsigned long *)0xE000ED04 = 0x10000000;    // 触发PendSV异常
}
