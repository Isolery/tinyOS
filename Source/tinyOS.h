#ifndef TINYOS_H
#define TINYOS_H 

#include <stdint.h>

typedef uint32_t tTaskStack;

typedef struct  {
    tTaskStack * stack;
}tTask;

#endif
