// Jason Bowman
// jbowman@hmc.edu
// 10-13-24

// include global headers
#include "studio.h"
#include <stm32l432xx.h>

// include internal headers
#include "STM32L432KC_GPIO.h"
#include "STM32L432KC_RCC.h"
#include "STM32L432KC_TIM.h"
#include "STM32L432KC_FLASH.h"
#include "STM32L432KC_USART.h"
#include "STM32L432KC_SPI.h"
#include "STM32L432KC_TIM.h"

// define pins
#define A_IN_PIN PA8 //"FT_a" 5V capatible
#define B_IN_PIN PA6 //"FT_a" 5V capatible


void GPIOinit() { //GPIO PA8 & PA6 enable
  gpioEnable(GPIO_PORT_A); //enable GPIOA

  // GPIO PA8 (A interupt)
  pinMode(A_IN_PIN, GPIO_INPUT); //set PA8 input mode as 
  GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD8, 0b01) //set PA8 as pull up input
  
  // GPIO PA6 (B interupt)
  pinMode(B_IN_PIN, GPIO_INPUT); //set PA6 as input
  GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD6, 0b01) //set PA6 as pull up input
}

void SYSCLKinit() { //SYSCLK = MSI (4MHz)
  RCC->CR |= (RCC_CR_MSION); //make MSI the SYSCLK
  RCC->CR &= ~(RCC_CR_MSIRANGE); //clear MSI range
  RCC->CR |= _FLD2VAL(RCC_CR_MSIRANGE, 0b0110); //set MSI as defalut 4MHz
}

void delayTIMinit() { //TIM2 = 1000Hz -> 1ms delay
  uint32_t PSCval = uint32_t (SystemCoreClock/4000); //divide 4MHz/4000 = 1000Hz
  TIM2->PSC &= ~(65535); //clear TIM2_PSC
  TIM2->PSC |= (PSCval); //enter calsulated PSC val to result in 1000Hz count

  TIM2->EGR |= (TIM_EGR_UG); //generate update event to update PSC
  TIM2->CR1 |= (TIM_CR1_CEN); //enable TIM2 counter
}

void countTIMinit() { //TIM6 = SYSCLK = 4MHz
  RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;

  TIM6->PSC &= ~(65535); //clear TIM6_PSC
  TIM6->PSC |= (1); //effectively do not use PSC

  TIM6->EGR |= (TIM_EGR_UG); //generate update event to update PSC
  TIM6->CR1 |= (TIM_CR1_CEN); //enable TIM6 counter
}

__enable_irq(); // enable global interupts



int main(void) {
 GPIOinit(); //initialize GPIOs
 countTIMinit(); //TIM6 init


  __enable_irq(); // enable global interupts

}
