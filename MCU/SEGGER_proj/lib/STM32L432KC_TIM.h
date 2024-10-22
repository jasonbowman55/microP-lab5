// This .c handles the TIMx initializations and other common functionalities

#ifndef STM32L4_TIM_H
#define STM32L4_TIM_H

#include <stdint.h> //include standart integers
#include <stm32l412xx.h> //CMSIS
#include "STM32L432KC_GPIO.h"
#include "STM32L432KC_RCC.h"

//////////////////////
//Function Prototypes
//////////////////////

void initTIMcnt(TIM_TypeDef * TIMx); //initialize counter timer based on its name (counts in ms)
void initTIMfreq(TIM_TypeDef * TIMx); //initialize counter to output freq (i.e. TIM16)
void delay_millis(TIM_TypeDef * TIMx, uint32_t ms); //delays by a certain # of ms, based on SR reg and ARR (PSC handled in initTIMcnt)

#endif