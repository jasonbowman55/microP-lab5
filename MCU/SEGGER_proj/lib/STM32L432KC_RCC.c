// STM32L432KC_RCC.c
// Source code for RCC functions

#include "STM32L432KC_RCC.h"

void configurePLL() {
    // Set clock to 80 MHz
    // Output freq = (src_clk) * (N/M) / R
    // (4 MHz) * (N/M) / R = 80 MHz
    // M: XX, N: XX, R: XX
    // Use MSI as PLLSRC

    // TODO: Turn off PLL
        RCC->CR &= ~(1 << 24);
    // TODO: Wait till PLL is unlocked (e.g., off)
        //uint32_t bit_filter = (1 << 25); //Jason way

        while((RCC->CR >> 25 & 1) == 0); //Brake way

    // Load configuration
    // TODO: Set PLL SRC to MSI
        RCC->CFGR |= (1 << 0);
        RCC->CFGR &= ~(1 << 1);
    // TODO: Set PLLN
        //RCC->CFGR &= ~(0b11111111 << 8); //clear PLLN --- WHY IS THIS 0b?
        RCC->CFGR |= (0b10100000 << 8); //PLLN = 80 --- PLLN IS ONLY 7 BIT?

    // TODO: Set PLLM
        //RCC->CFGR &= ~(0b111 << 4); //clear PLLM
        RCC->CFGR |= (0b000 << 4); //PLLM = 1

    // TODO: Set PLLR
        //RCC->CFGR &= ~(0b11 << 25); //clear PLLR
        RCC->CFGR |= (0b01 << 25); //PLLR = 4
    
    // TODO: Enable PLLR output
        RCC->CFGR |= (1 << 24); //connect PLLREN

    // TODO: Enable PLL
        RCC->CR |= (1 << 24); //PLL on
    
    // TODO: Wait until PLL is locked
        while((RCC->CR >> 25 & 1) != 0); //wait for PLL lock --- DOES THIS STOP THE PROGRAM?

     // enable AHB1
     //   RCC->AHB1ENR &= ~(1 << 12);
     //   RCC->AHB1ENR |= (1 << 12);
    
    // enable APB1
     //   RCC->APB2ENR |= (1 << 17);
}

void configureClock(){
    // Configure and turn on PLL
    configurePLL();

    // Select PLL as clock source
    RCC->CFGR |= (0b11 << 0);
    while(!((RCC->CFGR >> 2) & 0b11));
}