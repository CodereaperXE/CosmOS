
#include <stdio.h>

#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_Buttons.h"              // ::Board Support:Buttons

#include "stm32f4xx.h"                  // Device header

#include "rtos.h"

extern int stdout_init (void);




volatile uint32_t msTicks;                            /* counts 1ms timeTicks */
/*----------------------------------------------------------------------------
 * SysTick_Handler:
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
  msTicks++;
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
void SystemCoreClockConfigure(void) {

  RCC->CR |= ((uint32_t)RCC_CR_HSION);                     /* Enable HSI */
  while ((RCC->CR & RCC_CR_HSIRDY) == 0);                  /* Wait for HSI Ready */

  RCC->CFGR = RCC_CFGR_SW_HSI;                             /* HSI is system clock */
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);  /* Wait for HSI used as system clock */

  FLASH->ACR  = (FLASH_ACR_PRFTEN     |                    /* Enable Prefetch Buffer */
                 FLASH_ACR_ICEN       |                    /* Instruction cache enable */
                 FLASH_ACR_DCEN       |                    /* Data cache enable */
                 FLASH_ACR_LATENCY_5WS );                  /* Flash 5 wait state */

  RCC->CFGR |= (RCC_CFGR_HPRE_DIV1  |                      /* HCLK = SYSCLK */
                RCC_CFGR_PPRE1_DIV2 |                      /* APB1 = HCLK/2 */
                RCC_CFGR_PPRE2_DIV1  );                    /* APB2 = HCLK/1 */

  RCC->CR &= ~RCC_CR_PLLON;                                /* Disable PLL */

  /* PLL configuration:  VCO = HSI/M * N,  Sysclk = VCO/P */
  RCC->PLLCFGR = ( 16ul                   |                /* PLL_M =  16 */
                 (200ul <<  6)            |                /* PLL_N = 200 */
                 (  0ul << 16)            |                /* PLL_P =   2 */
                 (RCC_PLLCFGR_PLLSRC_HSI) |                /* PLL_SRC = HSI */
                 (  7ul << 24)            |                /* PLL_Q =   7 */
                 (  2ul << 28)             );              /* PLL_R =   2 */

  RCC->CR |= RCC_CR_PLLON;                                 /* Enable PLL */
  while((RCC->CR & RCC_CR_PLLRDY) == 0) __NOP();           /* Wait till PLL is ready */

  RCC->CFGR &= ~RCC_CFGR_SW;                               /* Select PLL as system clock source */
  RCC->CFGR |=  RCC_CFGR_SW_PLL;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);  /* Wait till PLL is system clock src */
}


void OS_run(){
	SystemCoreClockConfigure();                              /* configure HSI as System Clock */
  SystemCoreClockUpdate();

  SysTick_Config(SystemCoreClock / 1000);  	/* SysTick 1 msec interrupts */
		
	//RTOS SETUP
	OSInit();
	
	//setting systick to highest priority
	NVIC_SetPriority(SysTick_IRQn,0U);


}

//gpio setup
#define PIN_PA10 (1U << 10)
#define PIN_PB3  (1U << 3)

void GPIO_Init(void);
void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t Pin, uint8_t State);


void GPIO_Init(void) {
    // Enable the clocks for GPIOA and GPIOB
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN);
    
    // Set PA10 (D2) as output
    GPIOA->MODER &= ~(3U << (10 * 2));  // Clear the MODER bits for PA10
    GPIOA->MODER |= (1U << (10 * 2));   // Set PA10 as output
    
    // Set PB3 (D3) as output
    GPIOB->MODER &= ~(3U << (3 * 2));   // Clear the MODER bits for PB3
    GPIOB->MODER |= (1U << (3 * 2));    // Set PB3 as output
}

void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t Pin, uint8_t State) {
    if (State) {
        GPIOx->BSRR = Pin;  // Set the pin
    } else {
        GPIOx->BSRR = (Pin << 16);  // Reset the pin
    }
}


//defining threads



void blink1(void){
	while(1){

		//LED_On(0);
		GPIO_WritePin(GPIOA, PIN_PA10, 1);
		Delay(10);
		GPIO_WritePin(GPIOA, PIN_PA10, 0);
		//LED_Off(0);
		Delay(10);
	
	}
}

void blink2 (void){
	while(1){

		//LED_On(0);
		//Delay(1000);
		//LED_Off(0);
		GPIO_WritePin(GPIOB, PIN_PB3, 1);
		Delay(50);
		GPIO_WritePin(GPIOB, PIN_PB3, 0);
		Delay(50);
	
		
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

	

	
	//setting up stacks
	uint32_t* b1sp=&stack[0][0];
	uint32_t* b2sp=&stack[1][0];
	//setting up osthreads
	OSThreadFunction b1;
	OSThreadFunction b2;
	b1=&blink1;
	b2=&blink2;
	
	OSThreadInit(b1,b1sp,sizeof(stack[0]));
	OSThreadInit(b2,b2sp,sizeof(stack[1]));
	
	
	//temp
	GPIO_Init();
	
	//clock setup to prevent scheduling during setup

	OS_run();
	
	
  for (;;) {
		
		//led code
    //LED_On(num);                                           /* Turn specified LED on */
    //Delay(500);                                            /* Wait 500ms */
    //while (Buttons_GetState() & (1 << 0));                 /* Wait while holding USER button */
    //LED_Off(num);                                          /* Turn specified LED off */
    //Delay(500);                                            /* Wait 500ms */
    //while (Buttons_GetState() & (1 << 0));                 /* Wait while holding USER button */

    //num++;                                                 /* Change LED number */
    //if (num >= max_num) {
    //  num = 0;                                             /* Restart with first LED */
    //}

    //printf ("Hello World\n\r");
		//-----------------------------------------------------
  }

}
