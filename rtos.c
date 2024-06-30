#include "rtos.h"

//initialization
volatile int ThreadCount=0;
TCB *cHead=NULL;
TCB ThreadList[MAX_THREADS];
int ThreadIndex=-1;
int CurThreadIndex=-1;
uint32_t stack[MAX_THREADS][256];
//current 
TCB *OSCur;
//next
TCB *OSNext;

// bitsmask of threads ready to run
uint32_t OS_readySet;


//allocated process tcb in scheduling chain (round robin)


void AllocateThread(TCB thread){
	ThreadList[++ThreadIndex]=(TCB)thread;
}


void OSThreadInit(
									OSThreadFunction ThreadFunction,
									uint32_t *stkTop,
									uint32_t stkSize
){
	//construction tcb for current thread
	//stack alignment
	uint32_t *sp=(uint32_t*)((((uint32_t)stkTop + stkSize) /8)*8);
	
	//setting up stack
	*(sp-1)=(1U<<24); //xPSR enabling 24 bit to one to set to thumb state
	*(sp-2)=(uint32_t)ThreadFunction; //PC loaded with function pointer
	//16 banked registers in total
	sp=sp-8; // padding
	
	
	//constructing tcb
	TCB curThread;
	curThread.sp=sp;
	
	//adding tcb to scheduler list
	AllocateThread(curThread);
	
	//setting the first thread to ready state
	if(ThreadIndex>0U){
		OS_readySet |= (1U << (ThreadIndex));
	}
	
}



//defining idle thread
OSThreadFunction IdleThreadFptr;
uint32_t *IdleStackPtr=&stack[0][0];
void IdleThread(void){
	while(1){
	}
}


//os init
void OSInit(){
	//setting up idle thread
	IdleThreadFptr=&IdleThread;
	//index 0 is the idle thread and idle stack always
	OSThreadInit(IdleThreadFptr,IdleStackPtr,sizeof(stack[0]));
	
	TCB* temp=&ThreadList[1];
	OSCur=(TCB *)0;
	OSNext=temp;
}

//os tick

void OS_Tick(void){ //removes thread from blocked state
	uint8_t n;
	for(n=1u;n<=ThreadIndex;n++){ //iterating through all threads
		if(ThreadList[n].timeout != 0U){ //checking if thread has timeout
			--ThreadList[n].timeout; 
			if(ThreadList[n].timeout==0U){ //checking if thread has timeout to remove from blocked state
				OS_readySet |= (1U << n); //setting thread to ready state
			}
		}
	}
}
//os delay
void OS_Delay(uint32_t ticks){
	__disable_irq();
	OSCur->timeout=ticks;//setting current thread timeout in TCB
	OS_readySet &= ~(1U <<(CurThreadIndex)); //setting thread as blocked
	OS_Scheduler(); //scheduling next thread
	__enable_irq();
}
//os scheduler
void OS_Scheduler(){
	if(OS_readySet==0U){ //if all threads are blocked
		//OSCur=(TCB*)&ThreadList[0]; //setting to idle thread (depricated)
		CurThreadIndex=0U; //setting to idle thread index
	}else{
		//if(OSNext==(TCB*)&ThreadList[ThreadIndex]+1){
		//	OSNext=&ThreadList[1];
		//}
		
		do {
			CurThreadIndex++;
			if(CurThreadIndex>ThreadIndex){
				CurThreadIndex=1U;
			}
			
		}while((OS_readySet & (1U << (CurThreadIndex)))==0); //checking if thread is ready
	}
	OSNext=&ThreadList[CurThreadIndex]; //setting OSNext to next thread
	//OSNext++ will be done by Pendsv_handler(depricated)
	//calling pendsv_handler for context switch
	*((volatile uint32_t *)0xE000ED04)= (1U<<28);
		
	
}

//TCB *OSCur;
//TCB *OSNext=&ThreadList[0];
//int tc=0;
//uint32_t *sp;

void PendSV_Handler(void){
	
	/*
	__disable_irq();
	if(OSCur!=NULL){
	//push all registers
		OSCur->sp=sp;
	}
	//context switch
	OSCur=OSNext;
	
	//added custom 
	OSNext++;
	
	sp=OSCur->sp;
	
	//pop registers
	
	
	__enable_irq();
	*/
	
	
	
//--------------------
	     //if(OSCur!=NULL){ 
       //push all registers 


__asm("CPSID         I");

__asm("LDR           r1,=OSCur");
__asm("LDR           r2,[r1,#0x00]");

__asm("CMP           r2,#0x00");


       //OSCur->sp=sp; 
 
       //context switch 

__asm("ITT           NE");
__asm("MOV           r3,sp");
__asm("STR           r3,[r2,#0x00]");

       //OSCur=OSNext; 
           
__asm("LDR           r2,=OSNext");
__asm("LDR           r3,[r2,#0x00]");
__asm("STR           r3,[r1,#0x00]");

       // OSNext++; (depricated)
			 
//__asm("ADD           r1,r3,#0x08");
//__asm("STR           r1,[r2,#0x00]");

       //sp=OSCur->sp; 
__asm("LDR           r1,[r3,#0x00]");
__asm("MOV           sp,r1");


__asm("CPSIE         I");
}