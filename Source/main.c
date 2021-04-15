#include "tinyOS.h"
#include "ARMCM3.h"

void tSetSysTickPeriod(uint32_t ms);
void tTaskDelay(uint32_t delay);

uint32_t tTaskEnterCritical(void)
{
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	return primask;
}

void tTaskExitCritical(uint32_t status)
{
	__set_PRIMASK(status);
}

void delay(unsigned int i)
{
	while(i--);
}

// 定义任务句柄
tTask tTask1;   
tTask tTask2;
tTask tTaskIDLE;    // 空闲任务

// 定义任务的栈空间
tTaskStack task1Env[20];
tTaskStack task2Env[20];
tTaskStack taskIDLE[20];

tTask* currentTask;
tTask* nextTask;
tTask* IDLETask;
tTask* taskTable[2];

uint32_t tickCount;

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
	uint32_t status = tTaskEnterCritical();
	
	if(currentTask == IDLETask)
	{
		if(taskTable[0]->delayTicks == 0)
		{
			nextTask = taskTable[0];
		}
		else if(taskTable[1]->delayTicks == 0)
		{
			nextTask = taskTable[1];
		}
		else
		{
			tTaskExitCritical(status);
			return;
		}
	}
	else
	{
		if(currentTask == taskTable[0])
		{
			if(taskTable[1]->delayTicks == 0)
			{
				nextTask = taskTable[1];
			}
			else if(currentTask->delayTicks != 0)
			{
				nextTask = IDLETask;
			}
			else
			{
				tTaskExitCritical(status);
				return;
			}
		}
		else if(currentTask == taskTable[1])
		{
			if(taskTable[0]->delayTicks == 0)
			{
				nextTask = taskTable[0];
			}
			else if(currentTask->delayTicks != 0)
			{
				nextTask = IDLETask;
			}
			else
			{
				tTaskExitCritical(status);
				return;
			}
		}
	}
	
	tTaskExitCritical(status);
	
	tTaskSwitch();
}

int task1Flag;
void task1Entry(void* param)
{
	tSetSysTickPeriod(10);
	
	for(;;)
	{
		task1Flag = 0;
		tTaskDelay(100);
		task1Flag = 1;
		tTaskDelay(100);
	}
}

int task2Flag;
void task2Entry(void* param)
{
	for(;;)
	{
		uint32_t status = tTaskEnterCritical();
		
		uint32_t counter = tickCount;

		tickCount = counter + 1;
			
		tTaskExitCritical(status);

		task2Flag = 0;
		tTaskDelay(100);
		task2Flag = 1;
		tTaskDelay(100);
	}
}

void taskIDLEEntry(void* param)
{
	for(;;)
	{

	}
}

void tTaskSysTickHandler()
{
	int i;
	
	uint32_t status = tTaskEnterCritical();
	for(i = 0; i < 2; i++)
	{
		if(taskTable[i]->delayTicks > 0)
		{
			taskTable[i]->delayTicks--;
		}
	}

	tickCount++;
	tTaskExitCritical(status);
	
	tTaskSched();
}

void tTaskDelay(uint32_t delay)
{
	uint32_t status = tTaskEnterCritical();
	
	currentTask->delayTicks = delay / 10;
	
	tTaskExitCritical(status);
	
	tTaskSched();
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
	tTaskSysTickHandler();    // 当触发SysTick异常时进行任务切换
}

int main(void)
{
	tTaskInit(&tTask1, task1Entry, (void*)0x11111111, &task1Env[20]);
	tTaskInit(&tTask2, task2Entry, (void*)0x22222222, &task2Env[20]);
	tTaskInit(&tTaskIDLE, taskIDLEEntry, 0, &taskIDLE[20]);

	taskTable[0] = &tTask1;
	taskTable[1] = &tTask2;

	nextTask = taskTable[0];
	IDLETask = &tTaskIDLE;

	tTaskRunFirst();    // 运行第一个任务

	return 0;
}
