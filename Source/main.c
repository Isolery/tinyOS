#include "tinyOS.h"
#include "ARMCM3.h"

void tSetSysTickPeriod(uint32_t ms);

void delay(unsigned int i)
{
	while(i--);
}

// 定义任务句柄
tTask tTask1;   
tTask tTask2;

// 定义任务的栈空间
tTaskStack task1Env[20];
tTaskStack task2Env[20];

tTask* currentTask;
tTask* nextTask;
tTask* taskTable[2];

// task: 任务句柄
// entry: 任务函数
// param: 任务函数参数
// stack: 任务运行的栈空间
void tTaskInit(tTask* task, void(*entry)(void*), void* param, tTaskStack* stack)    
{	
	*(--stack) = (unsigned long)(1 << 24);
	*(--stack) = (unsigned long)entry;
	*(--stack) = (unsigned long)0x14;
	*(--stack) = (unsigned long)0x12;
	*(--stack) = (unsigned long)0x3;
	*(--stack) = (unsigned long)0x2;
	*(--stack) = (unsigned long)0x1;
	*(--stack) = (unsigned long)param;
	*(--stack) = (unsigned long)0x11;
	*(--stack) = (unsigned long)0x10;
	*(--stack) = (unsigned long)0x9;
	*(--stack) = (unsigned long)0x8;
	*(--stack) = (unsigned long)0x7;
	*(--stack) = (unsigned long)0x6;
	*(--stack) = (unsigned long)0x5;
	*(--stack) = (unsigned long)0x4;

	task->stack = stack;
}

void tTaskSched()
{
	if(currentTask == taskTable[0])
	{
		nextTask = taskTable[1];
	}
	else
	{
		nextTask = taskTable[0];
	}

	tTaskSwitch();
}

int task1Flag;
void task1Entry(void* param)
{
	tSetSysTickPeriod(10);
	
	for(;;)
	{
		task1Flag = 0;
		delay(100);
		task1Flag = 1;
		delay(100);
	}
}

int task2Flag;
void task2Entry(void* param)
{
	for(;;)
	{
		task2Flag = 0;
		delay(100);
		task2Flag = 1;
		delay(100);
	}
}

void tSetSysTickPeriod(uint32_t ms)
{
	SysTick->LOAD = ms * SystemCoreClock / 1000 - 1;
	NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
					SysTick_CTRL_TICKINT_Msk |
					SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Handler()
{
	tTaskSched();
}

int main(void)
{
	tTaskInit(&tTask1, task1Entry, (void*)0x11111111, &task1Env[20]);
	tTaskInit(&tTask2, task2Entry, (void*)0x22222222, &task2Env[20]);

	taskTable[0] = &tTask1;
	taskTable[1] = &tTask2;

	nextTask = taskTable[0];

	tTaskRunFirst();    // 运行第一个任务

	return 0;
}
