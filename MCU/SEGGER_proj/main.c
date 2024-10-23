// Jason Bowman
// jbowman@hmc.edu
// 10-13-24

// include global headers
#include "stdio.h"

// include internal headers
#include "STM32L432KC_GPIO.h"
#include "STM32L432KC_RCC.h"
#include "STM32L432KC_TIM.h"
#include "STM32L432KC_FLASH.h"
#include "STM32L432KC_TIM.h"
#include <math.h>


// Necessary includes for printf to work
#include "stm32l432xx.h"

// Function used by printf to send characters to the laptop
int _write(int file, char *ptr, int len) {
  int i = 0;
  for (i = 0; i < len; i++) {
    ITM_SendChar((*ptr++));
  }
  return len;
}
/////

// define pins
#define A_IN_PIN PA8 //"FT_a" 5V capatible
#define B_IN_PIN PA6 //"FT_a" 5V capatible

//Global variables
#define interupt_flag      //the internal software flag that says there was an interupt that happened
#define COUNT_TIM TIM6     //make TIM2 to be the counter timer
#define DELAY_TIM TIM2    //make TIM6 to be the delay timer
int direction;             //1 = clockwise and 0 = counter clockwise
unsigned int delta;                 //the number of 
int rpm = 0;                   //number of S it took for 1 revolution
//int PPR = 100;             //NOTE: need to find it in the data sheet
int still = 1;                 //1=not moving motor, 0=moving motor
int A;
int B;


//********************************
void GPIOinit() {                   //GPIO PA8 & PA6 enable
  gpioEnable(GPIO_PORT_A); //enable GPIOA

  // GPIO PA8 (A interupt)
  pinMode(A_IN_PIN, GPIO_INPUT); //set PA8 input mode as 
  GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD8, 0b01); //set PA8 as pull up input
  
  // GPIO PA6 (B interupt)
  pinMode(B_IN_PIN, GPIO_INPUT); //set PA6 as input
  GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD6, 0b01); //set PA6 as pull up input
}

void EXTIcfgr() {                   //configure external interrupts specific for this project
    EXTI->IMR1 |= _VAL2FLD(EXTI_IMR1_IM6, 1); //enable externam interrupt mask for pin A6
    EXTI->IMR1 |= _VAL2FLD(EXTI_IMR1_IM8, 1); //enable externam interrupt mask for pin A8
    
    // 2. Enable rising edge trigger
    EXTI->RTSR1 |= _VAL2FLD(EXTI_RTSR1_RT6, 1); 
    EXTI->RTSR1 |= _VAL2FLD(EXTI_RTSR1_RT8, 1);

    // 3. Enable falling edge trigger
    EXTI->FTSR1 |= _VAL2FLD(EXTI_FTSR1_FT6, 1); 
    EXTI->FTSR1 |= _VAL2FLD(EXTI_FTSR1_FT8, 1); 
}

//void delay(int ms) {                //Function to create a delay using TIM6
//    for (int i = 0; i < ms; i++) {
//        DELAY_TIM->CNT = 0;              //Reset the timer count
//        while (DELAY_TIM->CNT < 1);      //Wait until the count reaches 1 (1 ms)
//    }
//}

//void rpm_calc (delta) {         //calculate the RPS and Direction of the motor
//  int ms_per_rev = fabs(delta) * PPR;   //time per revolution in ms
//  int min_per_rev = ms_per_rev / 60000; //min per revolution
//  rpm = 1 / min_per_rev;                //calculate rpm
//}
//********************************

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
  
  //rpm_calc(delta); //calculated the rps and loop in here




    double rpm1 = 0;
    double rpm2 = 0;
    double rpm3 = 0;
    double rpm4 = 0;
    double rpm = 0;
    while(1){  
  
        delay_millis(DELAY_TIM, 250);

        // if clock is not reset for a long time, then the motor is not turning, toggle off
        if(COUNT_TIM->CNT > 45000){
          still = 1;
        }
        
        // if off is 1, then motor is not turning and rpm is 0
        if(still){
          rpm = 0;
        
        // else calculations for motor speed
        }else {
          if(rpm == 0){
            rpm1 = 1/(double)(120*abs(delta)*4/1000000.0);
            rpm2 = rpm1;
            rpm3 = rpm1;
            rpm4 = rpm1;
          } else {
            rpm1 = rpm2;
            rpm2 = rpm3;
            rpm3 = rpm4;
            rpm4 = 1/(double)(120*abs(delta)*4/1000000.0);
          }
          rpm = (rpm1+rpm2+rpm3+rpm4)/4;
        }
        
        printf("Revolutions per Second: %f\n", rpm);
        printf("Delta: %d\n", delta);

    }
}



void EXTI9_5_IRQHandler(void) { //outputs delta (the time between A=1 and B=1 interupts
  int Ainterupt = digitalRead(A_IN_PIN); //reading the value of PA8 through the on board 5V ADC
  int Binterupt = digitalRead(B_IN_PIN); //reading the value of PA6 through the on board 5V ADC
  //unsigned int initial_tim = COUNT_TIM->CNT;
  A = Ainterupt;
  B = COUNT_TIM->CNT;

  //if A interupt happens
  if (EXTI->PR1 & (1 << 8)){
    //EXTI->PR1 |= (1 << 8); //clear the interupt flag
    still = 0;             //the motor is not still
    //printf("Delta1: %d\n", A);
    //printf("Delta1: %d\n", B);

    if((Binterupt==1) && (Ainterupt==1)){ //if a pulse occurs
      delta = COUNT_TIM->CNT; //clock cycles going CW
       printf("Delta1: %d\n", delta);
    }
    EXTI->PR1 |= (1 << 8); //clear the interupt flag
    COUNT_TIM->CNT = 0; //reset counter
  }

  //if B interupt happens
  if (EXTI->PR1 & (1 << 6)){
    //EXTI->PR1 |= (1 << 6); //clear the interupt flag
    still = 0;             //the motor is not still

   if((Binterupt==1) && (Ainterupt==1)){ //if a pulse occurs
      delta = -COUNT_TIM->CNT; //clock cycles going CCW
      printf("Delta2: %d\n", delta);
    }
    EXTI->PR1 |= (1 << 6); //clear the interupt flag
    COUNT_TIM->CNT = 0; //reset counter
  }
}

