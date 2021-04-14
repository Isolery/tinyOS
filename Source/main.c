#include "tinyOS.h"

typedef struct
{
	unsigned long * stackPtr;
}BlockType_t;

BlockType_t * blockPtr;
unsigned long stackBuffer[20];
BlockType_t block;

#define NVIC_INT_CTRL    0xE000ED04
#define NVIC_PENDSVSET   0x10000000
#define NVIC_SYSPRI2     0xE000ED22
#define NVIC_PENDSV_PRI  0x000000FF

void triggerPendSVC(void)
{
	*(volatile unsigned long *)0xE000ED22 = 0x000000FF;    // 设置PendSV异常优先级为最低
	*(volatile unsigned long *)0xE000ED04 = 0x10000000;    // 触发PendSV异常
}

void delay(unsigned int i)
{
	while(i--);
}

int flag;

int main(void)
{
	block.stackPtr = &stackBuffer[20];
	blockPtr = &block;

	for(;;)
	{
		flag = 0;
		delay(100);
		flag = 1;
		delay(100);

		triggerPendSVC();
	}
	
	return 0;
}
