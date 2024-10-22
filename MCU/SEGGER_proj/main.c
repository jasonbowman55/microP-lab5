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
#include "STM32L432KC_USART.h"
#include "STM32L432KC_SPI.h"
#include "STM32L432KC_TIM.h"

// define pins
#define A_IN_PIN PA8 //"FT_a" 5V capatible
#define B_IN_PIN PA6 //"FT_a" 5V capatible

//Global variables
#define interupt_flag     //the internal software flag that says there was an interupt that happened
unsigned char direction   //1 = clockwise and 0 = counter clockwise
int state                 //this tells you the state which the interupts are at (if Ainterupt and Binterupt are on or off) [0,1,2,3] encoding
int PulseCount            //pulses per revolution (1 pulse is when A and B interupts both equal 1
int rps                   //number of S it took for 1 revolution
int msPR                  //number of ms it took for 1 revolution
int PPR = 360;            //NOTE: need to change likely through experimentation
int A_on                  //internal flag saying that interupt A is triggered
int B_on                  //internal flag saying that interupt B is triggered

//********************************
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

void countTIMinit() { //TIM6
  RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;
  TIM6->PSC &= ~(65535); //clear TIM6_PSC
  uint32_t PSCval = uint32_t (SystemCoreClock/1000); //make counter 1ms
  TIM6->PSC |= (PSCval); //effectively do not use PSC

  TIM6->EGR |= (TIM_EGR_UG); //generate update event to update PSC
  TIM6->CR1 |= (TIM_CR1_CEN); //enable TIM6 counter
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

void delay(int ms) { // Function to create a delay using TIM6
    for (int i = 0; i < ms; i++) {
        TIM6->CNT = 0; // Reset the timer count
        while (TIM6->CNT < 1); // Wait until the count reaches 1 (1 ms)
    }
}

void rps_calc(int state) { //calculates rps based on the state
    
  //+1 to PulseCount every time A and B interupts are hi
  if (state == 3) {
    PulseCount = PulseCount + 1;
  }
  /////

  //outputs how many seconds it took for one revolution
  while (PulseCount < PPR) {
    delay(1); //delay 1ms
    msPR = msPR + 1;
    rps = msPR / 1000;
  }
  /////
}
//********************************

int main(void) {
  GPIOinit(); //initialize GPIOs
  SYSCLKinit(); //initialize 4MHz MSI SYSCLK
  countTIMinit(); //TIM6 init

  __enable_irq(); // enable global interupts
  
  EXTIcfgr(); //configure external interrupts specific for this project

  NVIC->ISER[0] |= (1 << EXTI9_5_IRQn); //turn on bitmask region relating to pins PA6 & PA8
  
  rps_calc(int state); //calculated the rps and loop in here

  void updateDirection(int state) {
    if (A_on) {
      switch(state) {
          case 0:
              delay(700);
              printf("Direction: Clockwise\n");         // CW
              printf("rps: %d\n", rps);
              break;
          case 1:
              delay(700);
              printf("Direction: Counter-Clockwise\n"); // CC
              printf("rps: %d\n", rps);
              break;
          case 2:
              delay(700);
              printf("Direction: Counter-Clockwise\n"); // CC
              printf("rps: %d\n", rps);
              break;
          case 3:
              delay(700);
              printf("Direction: Clockwise\n");         // CW
              printf("rps: %d\n", rps);
              break;
          default:
              delay(700);
              printf("Unknown state\n");
              break;
      }
      A_on = 0;
    }

    if (B_on) {
      switch(state) {
          case 0:
              delay(700);
              printf("Direction: Counter-Clockwise\n"); // CC
              printf("rps: %d\n", rps);
              break;
          case 1:
              delay(700);
              printf("Direction: Clockwise\n");         // CW
              printf("rps: %d\n", rps);
              break;
          case 2:
              delay(700);
              printf("Direction: Counter-Clockwise\n"); // CC
              printf("rps: %d\n", rps);
              break;
          case 3:
              delay(700);
              printf("Direction: Clockwise\n");         // CW
              printf("rps: %d\n", rps);
              break;
          default:
              delay(700);
              printf("Unknown state\n");
              break;
      }
      B_on = 0;
    }
  }
}


void EXTI9_5_IRQHandler(void) {
  int Ainterupt = digitalRead(A_IN_PIN); //reading the value of PA8 through the on board 5V ADC
  int Binterupt = digitalRead(B_IN_PIN); //reading the value of PA6 through the on board 5V ADC
  state = Ainterupt + (2 * Binterupt);   //creates a 0,1,2,3 encoding for the state of the interupts
  //NOTE: 0=A_low B_low / 1=A_hi B_low / 2=A_low B_hi / 3=A_hi B_hi
    
  //NOTE: is the indedxing correct below? I remember someone saying that this part was strange

    //interupt A (PA8) triggered the interupt
    if (EXTI->PR1 & (1 << 8)){
      A_on = 1; //internal flag signaling A interupt is triggered
       switch(state){
          case 0:             //0=A_low B_low
            direction = 1;    //CW
          case 1:             //1=A_hi B_low
            direction = 0;    //CC
          case 2:             //2=A_low B_hi
            direction = 0;    //CC
          case 3:             //3=A_hi B_hi
            direction = 1;    //CW
       }
      EXTI->PR1 |= (1 << 8); //clear the interupt flag
    }

    //interupt B (PA6) triggered the interupt
    if (EXTI->PR1 & (1 << 6)){ //if interupt A (PA8) triggered the interupt
      B_on = 1; //internal flag signaling B interupt is triggered
       switch(state){
          case 0:             //0=A_low B_low
            direction = 0;    //CC
          case 1:             //1=A_hi B_low
            direction = 1;    //CW
          case 2:             //2=A_low B_hi
            direction = 1;    //CC
          case 3:             //3=A_hi B_hi
            direction = 0;    //CW
       }
      EXTI->PR1 |= (1 << 6); //clear the interupt flag
    }
}