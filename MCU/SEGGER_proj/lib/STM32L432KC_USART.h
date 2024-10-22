// STM32F401RE_USART.h
// Header for USART functions

#ifndef STM32L4_USART_H //include guard (if not defined)
#define STM32L4_USART_H //redefine (WHY DO WE USE THIS?)

#include <stdint.h> //includ standar integer types
#include <stm32l432xx.h> //include CMSIS refs for STM32L432KC

// Defines for USART case statements
#define USART1_ID   1 //I do not understand why you need these identifiers, but use them because I saw them in the other .h files
#define USART2_ID   2 //since identifiers are integers, all calls for this variable will be type "int"

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

USART_TypeDef * id2Port(int USART_ID);
USART_TypeDef * initUSART(int USART_ID, int baud_rate);
void sendChar(USART_TypeDef * USART, char data);
char readChar(USART_TypeDef * USART);
void sendString(USART_TypeDef * USART, char * charArray);
void readString(USART_TypeDef * USART, char * charArray);

#endif