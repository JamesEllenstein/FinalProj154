/*
 * ECE 153B - Spring 2023
 *
 * Name(s):
 * Section:
 * Lab: 6C
 */

#include "stm32l476xx.h"
#include "SysClock.h"
#include "SysTimer.h"
#include "UART.h"
#include "motor.h"
#include <stdio.h>

int main(void) {
	char rxByte;
	// Switch System Clock = 80 MHz
	System_Clock_Init(); 
	Motor_Init();
	SysTick_Init();
	UART2_GPIO_Init();
	UART2_Init();
	USART_Init(USART2);//TODO
	
	printf("Program Starts.\r\n");
	while(1) {
		printf("Please give an input(0->counterclockwise/1->clockwise):");
		scanf("%c",&rxByte);
		
		if (rxByte == '0') {
			setDire(0);
			printf("Now rotating counterclockwise \n");
		} else if (rxByte == '1') {
			setDire(1);
			printf("Now rotating clockwise \n");
		} else {
			printf("Please give a valid input \n");
		}
	}
}


