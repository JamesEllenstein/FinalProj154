#include "LED.h"

void LED_Init(void) {


    // Enable GPIO Clock

    RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN); //equivalent to (1<<0)

    // Initialize Green LED

		GPIOA->MODER &= ~GPIO_MODER_MODE5_1;	

		GPIOA->OTYPER &=  ~GPIO_OTYPER_OT5;

		GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD5;

		GPIOA->ODR &= ~(1<<5);





	

}



void LED_Off(void) {

	GPIOA->ODR &= ~(1<<5);

}



void LED_On(void) {

	GPIOA->ODR |= (1<<5);

}



void LED_Toggle(void) {

	GPIOA->ODR ^= (1<<5);

}
