// Jason Bowman
// jbowman@hmc.edu
// 10-13-24
// This code uses quadrature encoding on a DC motor along with interrutps in order to print our the speed and direction of the DC motor.

// include global headers
#include "stdio.h"

// include internal headers
#include "STM32L432KC_GPIO.h"
#include "STM32L432KC_RCC.h"
#include "STM32L432KC_TIM.h"
#include "STM32L432KC_FLASH.h"
#include "STM32L432KC_TIM.h"
#include <math.h>

// Necessary includes for printf to work///////////////////
#include "stm32l432xx.h"

// Function used by printf to send characters to the laptop
int _write(int file, char *ptr, int len) {
  int i = 0;
  for (i = 0; i < len; i++) {
    ITM_SendChar((*ptr++));
  }
  return len;
}
////////////////////////////////////////////////////////////

// define pins
#define A_IN_PIN PA8 //"FT_a" 5V capatible
#define B_IN_PIN PA6 //"FT_a" 5V capatible

//Global variables
#define interupt_flag   //the internal software flag that says there was an interupt that happened
#define COUNT_TIM TIM6  //make TIM2 to be the counter timer
#define DELAY_TIM TIM2  //make TIM6 to be the delay timer
signed int delta;       //the number of clock cycles per revolution
int rps = 0;            //number of ms it took for 1 revolution
int PPR = 120;          //based on the data sheet
int still = 1;          //1=not moving motor, 0=moving motor
double C = 2*3.141592*(0.000157/2); //circumference of the motor shaft

//********************************
void GPIOinit() { //GPIO PA8 & PA6 enable
  gpioEnable(GPIO_PORT_A); //enable GPIOA

  // GPIO PA8 (A interupt)
  pinMode(A_IN_PIN, GPIO_INPUT);                    //set PA8 input mode as 
  GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD8, 0b01); //set PA8 as pull up input
  
  // GPIO PA6 (B interupt)
  pinMode(B_IN_PIN, GPIO_INPUT);                    //set PA6 as input
  GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD6, 0b01); //set PA6 as pull up input
}

void EXTIcfgr() { //configure external interrupts specific for this project
    EXTI->IMR1 |= _VAL2FLD(EXTI_IMR1_IM6, 1); //enable externam interrupt mask for pin A6
    EXTI->IMR1 |= _VAL2FLD(EXTI_IMR1_IM8, 1); //enable externam interrupt mask for pin A8
    
    // 2. Enable rising edge trigger
    EXTI->RTSR1 |= _VAL2FLD(EXTI_RTSR1_RT6, 1); 
    EXTI->RTSR1 |= _VAL2FLD(EXTI_RTSR1_RT8, 1);

    // 3. Enable falling edge trigger
    EXTI->FTSR1 |= _VAL2FLD(EXTI_FTSR1_FT6, 1); 
    EXTI->FTSR1 |= _VAL2FLD(EXTI_FTSR1_FT8, 1); 
}

int main(void) {
  __enable_irq(); // enable global interupts
 
  configureFlash(); //configure flash memory
  configureClock(); //configure clock to be SYSCLK
  GPIOinit();       //initialize GPIOs

  RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN; //configure TIM6 and TIM2 to be on and connected to the SYSCLK
  initTIM(DELAY_TIM);                   //initialize delay timer TIM6
  initTIM(COUNT_TIM);                   //initialize counter timer TIM2

   //__enable_irq(); // enable global interupts
  SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI8_PA; // Select PA8
  SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI6_PA; // Select PA6

  
  EXTIcfgr(); //configure external interrupts specific for this project

  NVIC->ISER[0] |= (1 << EXTI9_5_IRQn); //turn on bitmask region relating to pins PA6 & PA8
  
    //initialize RPSs
    double rps1 = 0;
    double rps2 = 0;
    double rps3 = 0;
    double rps4 = 0;
    double rps = 0;
    while(1){ //calculate the RPS and Direction then print
  
        delay_millis(DELAY_TIM, 250); //time between prints

        // if clock is not reset for a long time, then the motor is not turning, toggle off
        if(COUNT_TIM->CNT > 50){
          still = 1;
        }
        
        // if off is 1, then motor is not turning and rpm is 0
        if(still){
          rps = 0;
        
        // else calculations for motor speed
        }else {
          if(rps == 0){
            rps1 = 1/(double)(PPR*abs(delta)*4/1000.0); //calculate the new RPS
            rps2 = rps1;
            rps3 = rps1;
            rps4 = rps1;
          } else {
            rps1 = rps2;
            rps2 = rps3;
            rps3 = rps4;
            rps4 = 1/(double)(PPR*abs(delta)*4/1000.0); //calculate the new RPS
          }
          rps = (rps1+rps2+rps3+rps4)/4;
        }

    //int rps_ms_calc = rps;  //intermediary to calculate the m/s val using rps
    double speed_linear = rps*C; //calculate the m/s speed of the motor from rps

        //Printing values
        if (delta > 0) {
    //printf("Direction: Counter Clockwise, Speed: %f m/s\n", speed_linear); //CCW m/s
    printf("Direction: Counter Clockwise, RPS: %.3f\n", rps);            //CCW RPS
        } else if (delta < 0) {
    //printf("Direction: Clockwise, Speed: %f m/s\n", speed_linear);         //CW m/s
    printf("Direction: Clockwise, RPS: %.3f\n", rps);                    //CW RPS
        } else {
    printf("Delta is zero, no rotation. m/s: %f\n", rps);
        }

    }
}



void EXTI9_5_IRQHandler(void) { //outputs delta (the time between A=1 and B=1 interupts
  int Ainterupt = digitalRead(A_IN_PIN); //reading the value of PA8 through the on board 5V ADC
  int Binterupt = digitalRead(B_IN_PIN); //reading the value of PA6 through the on board 5V ADC

  //if A interupt happens
  if (EXTI->PR1 & (1 << 8)){
    still = 0; //the motor is not still
    if((Binterupt==1) && (Ainterupt==1)){ //if a pulse occurs
      delta = COUNT_TIM->CNT; //clock cycles going CW
       //printf("Delta1: %d\n", delta);
    }
    EXTI->PR1 |= (1 << 8); //clear the interupt flag
    COUNT_TIM->CNT = 0; //reset counter
  }

  //if B interupt happens
  if (EXTI->PR1 & (1 << 6)){
    still = 0; //the motor is not still
   if((Binterupt==1) && (Ainterupt==1)){ //if a pulse occurs
      delta = -COUNT_TIM->CNT; //clock cycles going CCW
    }
    EXTI->PR1 |= (1 << 6); //clear the interupt flag
    COUNT_TIM->CNT = 0;    //reset counter
  }
}