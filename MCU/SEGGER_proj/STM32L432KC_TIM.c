// This .c handles the TIMx initializations and other common functionalities

#include "STM32L432KC_TIM.h"

void initTIMcnt(TIM_TypeDef * Timx){

  uint32_t psc_val = (uint32_t) ((SystemCoreClock/1000)); //ouput a uint32 psc value, that results in # clk cycles to acheive 1ms based on the SYSCLK
  TIMx->PSC = (psc_val-1) //still not sure why "-1" but I accept it lol
  TIMx->EGR |= 1; //generate update event to load new PSC value
  TIMx->CR1 |= 1; //enable the counter within its respective control register
}

void initTIMfreq(TIM_TypeDef * TIMx){
  //GOAL: have sample the motor 300,000 time per period of encoder
  //80MHz SYSCLK/267=~300,000. if PSC=267, SYSCLK/300,000 =PSC
  uint32_t psc_val = (uint32_t) ((SystemCoreClock/300000));
  TIMx->PSC = (psc_val-1) //still not sure why "-1" but I accept it lol
  TIMx->EGR |= 1; //generate update event to load new PSC value
  TIMx->CR1 |= 1; //enable the counter within its respective control register
  TIMx->ARR = 65535; //ARR max allows for sull count
}

void (delay_millis(TIM_TypeDef * TIMx, uint32_t ms) {

  TIMx->ARR = ms;// Set timer max count
  TIMx->EGR |= 1;     // Force update
  TIMx->SR &= ~(0x1); // Clear UIF
  TIMx->CNT = 0;      // Reset count

  while(!(TIMx->SR & 1)); // Wait for UIF to go high

}