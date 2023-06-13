/*
 * ECE 153B - Winter 2023
 *
 * Lab: 6A
 */

#include "stm32l476xx.h"
#include "motor.h"
/*
static const uint32_t MASK = 0;//TODO
static const uint32_t HalfStep[8] = {0,0,0,0,0,0,0,0};//TODO
*/
static volatile int8_t dire = 1;
static volatile uint8_t step = 0;
//5-A Blue  8-B Pink  6-A`Yellow  9-B`Orange
/*
#define DELAY 3500	// delay between steps of the sequences


void Half_Stepping_Clockwise(void){
	GPIOC->ODR &= 0<<12;
	GPIOC->ODR |= (GPIO_ODR_OD5 | GPIO_ODR_OD9);

	for (int j=0; j<512; j++) {
		
		GPIOC->ODR ^= GPIO_ODR_OD9;
		for (int i =0;i < DELAY; i++);
		GPIOC->ODR ^= GPIO_ODR_OD8;
		for (int i =0;i < DELAY; i++);
		GPIOC->ODR ^= GPIO_ODR_OD5;
		for (int i =0;i < DELAY; i++);
		GPIOC->ODR ^= GPIO_ODR_OD6;
		for (int i =0;i < DELAY; i++);
		GPIOC->ODR ^= GPIO_ODR_OD8;
		for (int i =0;i < DELAY; i++);
		GPIOC->ODR ^= GPIO_ODR_OD9;
		for (int i =0;i < DELAY; i++);
		GPIOC->ODR ^= GPIO_ODR_OD6;
		for (int i =0;i < DELAY; i++);
		GPIOC->ODR ^= GPIO_ODR_OD5;
		for (int i =0;i < DELAY; i++);

	}
}

void Half_Stepping_CounterClockwise(void){
	GPIOC->ODR &= 0<<12;
	GPIOC->ODR |= (GPIO_ODR_OD5 | GPIO_ODR_OD9);
	for (int i =0;i < DELAY; i++);
	for (int j=0; j<512; j++) {
		
		GPIOC->ODR ^= GPIO_ODR_OD5;
		for (int i =0;i < DELAY; i++);
		GPIOC->ODR ^= GPIO_ODR_OD6;
		for (int i =0;i < DELAY; i++);
		GPIOC->ODR ^= GPIO_ODR_OD9;
		for (int i =0;i < DELAY; i++);
		GPIOC->ODR ^= GPIO_ODR_OD8;
		for (int i =0;i < DELAY; i++);
		GPIOC->ODR ^= GPIO_ODR_OD6;
		for (int i =0;i < DELAY; i++);
		GPIOC->ODR ^= GPIO_ODR_OD5;
		for (int i =0;i < DELAY; i++);
		GPIOC->ODR ^= GPIO_ODR_OD8;
		for (int i =0;i < DELAY; i++);
		GPIOC->ODR ^= GPIO_ODR_OD9;
		for (int i =0;i < DELAY; i++);

	}
}
*/
void Motor_Init(void) {	
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN; //equivalent to (1<<0)
	GPIOC->MODER &= (GPIO_MODER_MODE5_0 | GPIO_MODER_MODE6_0 | GPIO_MODER_MODE8_0 | GPIO_MODER_MODE9_0);
	GPIOC->OSPEEDR |= (GPIO_OSPEEDR_OSPEED5 | GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED8 | GPIO_OSPEEDR_OSPEED9);
	GPIOC->PUPDR &= 0 << 31;
	GPIOC->OTYPER &= 0 << 31;
	GPIOC->ODR &= 0<<12;
}

void rotate(void) {
	if (dire == 0) {
		if (step == 7) {
			step = 0;
		} else {
			step++;
		}
	} else {
		if (step == 0) {
			step = 7;
		} else {
			step--;
		}
	}
	
	switch (step){
		case 0:
			GPIOC->ODR &= 0<<12;
			GPIOC->ODR |= (GPIO_ODR_OD5 | GPIO_ODR_OD9);
			break;
		case 1:
			GPIOC->ODR &= 0<<12;
			GPIOC->ODR |= (GPIO_ODR_OD9);
			break;
		case 2:
			GPIOC->ODR &= 0<<12;
			GPIOC->ODR |= (GPIO_ODR_OD9 | GPIO_ODR_OD6);
			break;
		case 3:
			GPIOC->ODR &= 0<<12;
			GPIOC->ODR |= (GPIO_ODR_OD6);
			break;
		case 4:
			GPIOC->ODR &= 0<<12;
			GPIOC->ODR |= (GPIO_ODR_OD6 | GPIO_ODR_OD8);
			break;
		case 5:
			GPIOC->ODR &= 0<<12;
			GPIOC->ODR |= (GPIO_ODR_OD8);
			break;
		case 6:
			GPIOC->ODR &= 0<<12;
			GPIOC->ODR |= (GPIO_ODR_OD8 | GPIO_ODR_OD5);
			break;
		case 7:
			GPIOC->ODR &= 0<<12;
			GPIOC->ODR |= (GPIO_ODR_OD5);
			break;
	}
}

void setDire(int8_t direction) {
	dire = direction;
}
	


