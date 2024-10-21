// This .c handles the TIMx initializations and other common functionalities

#include "STM32L432KC_TIM.h"

void initTIMcnt(TIM_TypeDef * Timx){

  uint32_t psc_val = (uint32_t) ((SystemCoreClock/1000)); //ouput a uint32 psc value, that results in # clk cycles to acheive 1ms based on the SYSCLK
  TIMx->PSC = (psc_val-1) //still not sure why "-1" but I accept it lol
  TIMx->EGR |= 1; //generate update event to load new PSC value
  TIMx->CR1 |= 1; //enable the counter within its respective control register


}