#include <stdio.h>
#include <stm32f446xx.h>

#ifndef RTOS_H
#define RTOS_H

volatile extern int ThreadCount;


#define MAX_THREADS 5

//thread stack
extern uint32_t stack[MAX_THREADS][256];


//thread control block
typedef struct TCB{
	uint32_t *sp;
	struct TCB *next;
}TCB;

//thread list
extern TCB ThreadList[MAX_THREADS];
extern int ThreadIndex;
//thread list pointer
extern TCB *cHead;

//thread maximum
extern TCB *OSCur;
extern TCB *OSNext;


//function pointer
typedef void (*OSThreadFunction)(void);

//void allocate thread
void AllocateThread(TCB);

//OSThreadInit
void OSThreadInit(
									OSThreadFunction,
									uint32_t*,
									uint32_t 
);

//os init
void OSInit(void);

//os scheduler
void OS_Scheduler(void);


#endif //RTOS_H





