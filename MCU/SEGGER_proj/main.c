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

// define pins
#define A_IN_PIN PA1 //"FT_a" 5V capatible
#define B_IN_PIN PA6 //"FT_a" 5V capatible


void GPIOinit() {
  gpioE
  pinMode(A_IN_PIN, GPIO_INPUT);
  RCC
  pinMode(B_IN_PIN, GPIO_INPUT);
}











int main(void) {

}
