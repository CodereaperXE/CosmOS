
#include <stdio.h>

#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_Buttons.h"              // ::Board Support:Buttons
#include "stm32f4xx.h"                  // Device header
#include "bsp.h"												// Board Support Package
#include "rtos.h"												// RTOS Support

extern int stdout_init (void);

volatile uint32_t msTicks;                            /* counts 1ms timeTicks */
/*----------------------------------------------------------------------------
 * SysTick_Handler:
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
  msTicks++;
	//calling OS tick
	OS_Tick();
	//calling OS_Scheduler
	__disable_irq();
	OS_Scheduler();
	__enable_irq();

}

/*----------------------------------------------------------------------------
 * Delay: delays a number of Systicks
 *----------------------------------------------------------------------------*/
void Delay (uint32_t dlyTicks) {
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) { __NOP(); }
}

/*----------------------------------------------------------------------------
 * SystemCoreClockConfigure: configure SystemCoreClock using HSI
                             (HSE is not populated on Nucleo board)
 *----------------------------------------------------------------------------*/
void SystemCoreClockConfigure(void);


void OS_run(){
	SystemCoreClockConfigure();               /* configure HSI as System Clock */
  SystemCoreClockUpdate();

  SysTick_Config(SystemCoreClock / 1000);  	/* SysTick 1 msec interrupts */
		
	//RTOS SETUP
	//OSInit();
	
	//setting systick to highest priority
	NVIC_SetPriority(SysTick_IRQn,0U);

}

//gpio setup
#define PIN_PA10 (1U << 10)
#define PIN_PB3  (1U << 3)
#define PIN_PB4	 (1U << 4)

void GPIO_Init(void);
void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t Pin, uint8_t State);
void GPIO_Init(void);
void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t Pin, uint8_t State);


//defining threads


//GPIO_WritePin(GPIOA, PIN_PA10, 1);
//GPIO_WritePin(GPIOB, PIN_PB3, 1);
void blink1(void){
	while(1){

		//LED_On(0);
		//OS_Delay(1000);
		//LED_Off(0);
		//OS_Delay(1000);
		GPIO_WritePin(GPIOB,PIN_PB4,1);
		OS_Delay(500);
		GPIO_WritePin(GPIOB,PIN_PB4,0);
		OS_Delay(500);
		

	}
}

void blink2 (void){
	while(1){

		GPIO_WritePin(GPIOB, PIN_PB3, 1);
		OS_Delay(100);
		GPIO_WritePin(GPIOB, PIN_PB3, 0);
		OS_Delay(100);

		
	}
}

/*----------------------------------------------------------------------------
 * main: blink LED and check button state
 *----------------------------------------------------------------------------*/
int main (void) {
	//setup
	
  int32_t max_num = LED_GetCount();
  int32_t num = 0;

  LED_Initialize();
  Buttons_Initialize();
  stdout_init();                                           /* Initialize Serial interface */
	//running os init
	OSInit();
	//setting up stacks
	uint32_t* b1sp=&stack[1][0];
	uint32_t* b2sp=&stack[2][0];
	//setting up osthreads
	OSThreadFunction b1;
	OSThreadFunction b2;
	b1=&blink1;
	b2=&blink2;
	
	OSThreadInit(b1,b1sp,sizeof(stack[1]));
	OSThreadInit(b2,b2sp,sizeof(stack[2]));
	
	
	//temp
	GPIO_Init();
	
	//clock setup to prevent scheduling during setup

	OS_run();
	
	
  for (;;) {
		

  }

}
