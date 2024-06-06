#include "rtos.h"

//initialization
volatile int ThreadCount=0;
TCB *cHead=NULL;
TCB ThreadList[MAX_THREADS];
int ThreadIndex=-1;
uint32_t stack[MAX_THREADS][256];
TCB *OSCur;
TCB *OSNext;

//allocated process tcb in scheduling chain (round robin)


void AllocateThread(TCB thread){
	/*TCB *temp;
	temp=cHead;
	while(temp!=NULL){
		temp=temp->next;
	}
	temp->next=thread;
	thread->next=cHead;
	temp=NULL;
	*/
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
	*(sp-1)=(1U<<24); //xPSR
	*(sp-2)=(uint32_t)ThreadFunction; //PC
	//16 banked registers in total
	sp=sp-8;
	
	
	//constructing tcb
	TCB curThread;
	curThread.sp=sp;
	
	//adding tcb to scheduler list
	AllocateThread(curThread);
	
}

//os init
TCB* temp=&ThreadList[0];
void OSInit(){
	OSCur=(TCB *)0;
	OSNext=temp;
}

//os scheduler
void OS_Scheduler(){
	if(OSNext==(TCB*)&ThreadList[ThreadIndex]+1){
		OSNext=&ThreadList[0];
	}
	//OSNext++ will be done by Pendsv_handler
	//calling pendsv_handler for context switch
	*((volatile uint32_t *)0xE000ED04)= (1U<<28);
	
}

//TCB *OSCur;
//TCB *OSNext=&ThreadList[0];
//int tc=0;
uint32_t *sp;

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
	
	/*
	__asm("CPSID         I");
	__asm("LDR           r0,=OSCur");
	__asm("LDR           r2,[r0,#0x00]");

	__asm("CMP           r2,#0x00");
	__asm("MOV           r1,sp");
	
  //OSCur->sp=sp;	
  // } 
	//context switch 

	__asm("ITT           NE");
	__asm("LDR           r3,[r1,#0x00]");
	__asm("STR           r3,[r2,#0x00]");

  //OSCur=OSNext; 


	__asm("LDR           r2,=OSNext");
	__asm("LDR           r2,[r2,#0x00]");
	__asm("STR           r2,[r0,#0x00]");
	
	//OSNext++
	
	__asm("LDR           r3,=OSNext");
	__asm("LDR           r1,[r3,#0x00]");
	__asm("ADDS          r1,r1,#0x08");
	__asm("STR           r1,[r3,#0x00]");
	
	//sp=OSCur->sp; 
        
  //pop registers 
    
        
  //__enable_irq(); 
        

	__asm("LDR           r0,[r2,#0x00]");
	__asm("MOV           sp,r0");
	__asm("CPSIE         I");
 //}
	__asm("BX            lr");
	*/
	
	
	//--------------------
	    //if(OSCur!=NULL){ 
    //push all registers 


__asm("CPSID         I");

__asm("LDR           r1,=OSCur");
__asm("LDR           r2,[r1,#0x00]");

__asm("CMP           r2,#0x00");

//__asm("MOV           r0,sp");

            //OSCur->sp=sp; 
         //} 
        //context switch 

__asm("ITT           NE");
__asm("MOV           r3,sp");
__asm("STR           r3,[r2,#0x00]");

     //OSCur=OSNext; 
           
__asm("LDR           r2,=OSNext");
__asm("LDR           r3,[r2,#0x00]");
__asm("STR           r3,[r1,#0x00]");

    // OSNext++; 
            
__asm("ADD           r1,r3,#0x08");
__asm("STR           r1,[r2,#0x00]");

     //sp=OSCur->sp; 
__asm("LDR           r1,[r3,#0x00]");
__asm("MOV           sp,r1");
//} 

__asm("CPSIE         I");
}