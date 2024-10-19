// STM32L432KC_USART.c
// Source code for USART functions

#include "STM32L432KC.h"
#include "STM32L432KC_USART.h"
#include "STM32L432KC_GPIO.h"
#include "STM32L432KC_RCC.h"
#include <stm32l432xx.h>


USART_TypeDef * id2USARTbase(int USART_ID) { //return pointer to base register (USART_TypeDef *)
    USART_TypeDef * USART;
    //case statement to switch between USARTs
    switch(USART_ID){
        case(USART1_ID) :
            USART = USART1;
            break;
        case(USART2_ID) :
            USART = USART2;
            break;
        default :
            USART = 0;
    }
    return USART;
}

USART_TypeDef * initUSART(int USART_ID, int baud_rate) {
  gpioEnable(GPIO_PORT_A);  // Enable clock for GPIOA
  RCC->CR |= RCC_CR_HSION;  // Turn on HSI 16 MHz clock
}



USART_TypeDef * initUSART(int USART_ID, int baud_rate) {
    gpioEnable(GPIO_PORT_A);  // Enable clock for GPIOA
    RCC->CR |= RCC_CR_HSION;  // Turn on HSI 16 MHz clock

    USART_TypeDef * USART = id2USARTbase(USART_ID); // Get pointer to USART

    switch(USART_ID){
        case USART1_ID: //USART 1 base register
            RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Set clk tree to USART1EN
            RCC->CCIPR |= (0b10 << 0); // Set HSI16 (16 MHz) as USART clock source

            // Configure pin modes as ALT function
            pinMode(PA9, GPIO_ALT); // TX
            pinMode(PA10, GPIO_ALT); // RX

            break;
        case USART2_ID://USART 2 base register
            RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN; // Set USART2EN
            RCC->CCIPR |= (0b10 << RCC_CCIPR_USART2SEL_Pos); // Set HSI16 (16 MHz) as USART clock source

            // Configure pin modes as ALT function
            pinMode(PA2, GPIO_ALT); // TX
            pinMode(PA15, GPIO_ALT); // RX

            break;
    }

    // Set M = 00
    USART->CR1 &= ~(USART_CR1_M0 | USART_CR1_M1);    // M=00 corresponds to 1 start bit, 8 data bits, n stop bits
    USART->CR1 &= ~USART_CR1_OVER8; // Set to 16 times sampling freq
    USART->CR2 &= ~USART_CR2_STOP;  // 0b00 corresponds to 1 stop bit

    // Set baud rate to 115200 (see RM 38.5.4 for details)
    // Tx/Rx baud = f_CK/USARTDIV (since oversampling by 16)
    // f_CK = 16 MHz (HSI)

    USART->BRR = (uint16_t) (HSI_FREQ / baud_rate);

    USART->CR1 |= USART_CR1_UE;     // Enable USART
    USART->CR1 |= USART_CR1_TE | USART_CR1_RE; // Enable transmission and reception

    return USART;
}

void sendChar(USART_TypeDef * USART, char data){
    while(!(USART->ISR & USART_ISR_TXE));
    USART->TDR = data;
    while(!(USART->ISR & USART_ISR_TC));
}

void sendString(USART_TypeDef * USART, char * charArray){

    uint32_t i = 0;
    do{
        sendChar(USART, charArray[i]);
        i++;
    }
    while(charArray[i] != 0);
}

char readChar(USART_TypeDef * USART) {
        char data = USART->RDR;
        return data;
}

void readString(USART_TypeDef * USART, char* charArray){
    int i = 0;
    do{
        charArray[i] = readChar(USART);
        i++;
    }
    while(USART->ISR & USART_ISR_RXNE);
}