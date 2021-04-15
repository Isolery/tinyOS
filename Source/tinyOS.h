#ifndef TINYOS_H
#define TINYOS_H 

#include <stdint.h>

typedef uint32_t tTaskStack;

typedef struct  {
    tTaskStack * stack;

    // 任务延时计数器
    uint32_t delayTicks;
}tTask;

typedef struct {
	unsigned long * stackPtr;
}BlockType_t;

extern tTask* currentTask;
extern tTask* nextTask;

void tTaskRunFirst(void);
void tTaskSwitch(void);

#endif
