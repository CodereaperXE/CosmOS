#include <stdio.h>

#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_Buttons.h"              // ::Board Support:Buttons

#include "stm32f4xx.h"                  // Device header

#include "rtos.h"


#ifndef BSP_H
#define BSP_H
//setting systick config
void SystemCoreClockConfigure(void);

//gpio init
void GPIO_Init(void);

//gpio write pin
void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t Pin, uint8_t State);

#endif