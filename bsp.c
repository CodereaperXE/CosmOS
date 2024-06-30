#include "bsp.h"


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



//GPIO Init
void GPIO_Init(void) {
    // Enable the clocks for GPIOA and GPIOB
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN);
    
    // Set PA10 (D2) as output
    GPIOA->MODER &= ~(3U << (10 * 2));  // Clear the MODER bits for PA10
    GPIOA->MODER |= (1U << (10 * 2));   // Set PA10 as output
    
    // Set PB3 (D3) as output
    GPIOB->MODER &= ~(3U << (3 * 2));   // Clear the MODER bits for PB3
    GPIOB->MODER |= (1U << (3 * 2));    // Set PB3 as output
	
		// Set PB4 (D5) as output
    GPIOB->MODER &= ~(3U << (4 * 2));  // Clear the MODER bits for PB4
    GPIOB->MODER |= (1U << (4 * 2));   // Set PB4 as output
}

//gpio write pin
void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t Pin, uint8_t State) {
    if (State) {
        GPIOx->BSRR = Pin;  // Set the pin
    } else {
        GPIOx->BSRR = (Pin << 16);  // Reset the pin
    }
}