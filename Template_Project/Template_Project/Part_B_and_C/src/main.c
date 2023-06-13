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
#include "LED.h"
#include "DMA.h"
#include "UART.h"
#include "motor.h"
#include "SPI.h"
#include "I2C.h"
#include "accelerometer.h"
#include <string.h>
#include <stdio.h>

static char buffer[IO_SIZE];
volatile uint8_t complete = 0;
volatile uint8_t pos = 0;
volatile uint8_t cAfterR = 0;
void UART_onInput(char* inputs, uint32_t size) {
	if (size == 2) {
		if (inputs[0] == 'O' || inputs[0] == 'o') {
			setDire(0);
			setRot(1);
			pos = 0;
			cAfterR = 3;
			complete = 1;
		}
		if (inputs[0] == 'C' || inputs[0] == 'c') {
			setDire(1);
			setRot(1);
			pos = 1;
			cAfterR = 3;
			complete = 1;
		}
	}
}

int main(void) {
	// Switch System Clock = 80 MHz
	System_Clock_Init(); 
	Motor_Init();
	SysTick_Init();
	UART2_Init();
	LED_Init();	
	SPI1_GPIO_Init();
	SPI1_Init();
	initAcc();
	I2C_GPIO_Init();
 	I2C_Initialization();
	double x,y,z;
	sprintf(buffer, "Program Starts.\r\n");
	UART_print(buffer);
	uint8_t SlaveAddress;
	uint8_t Data_Receive[1];
	uint8_t Data_Send[1];
	Data_Send[0] = 0x00;
	SlaveAddress = 0b1001000 << 1;

	while(1) {
 		I2C_SendData(I2C1, SlaveAddress, Data_Send, 1);
		I2C_ReceiveData(I2C1, SlaveAddress, Data_Receive, 1);
		//memset( buffer, '\0', sizeof(char)*IO_SIZE );
		//sprintf(buffer, "Program Starts.\r\n");
		sprintf(buffer,"%d\r\n", Data_Receive[0]);
		UART_print(buffer);
		readValues(&x, &y, &z);
		sprintf(buffer, "Acceleration: %.2f, %.2f, %.2f\r\n", x, y, z);
		UART_print(buffer);
		
		if (cAfterR > 0 ) {
			if (complete == 0)cAfterR--;
		} else {
			if (Data_Receive[0] >= 24) {
				setDire(0);
				setRot(1);
				pos = 0;
			} else if (Data_Receive[0] < 23) {
				setDire(1);
				setRot(1);
				pos = 1;
			}
		}
		
		if(pos == 1 && y > 8.20) {
			setRot(0);
			complete = 0;
		} else if (pos == 1 && y < 8.15) {
			setRot(1);
		}
		
		if(pos == 0 && z > 7.38) {
			setRot(0);
			complete = 0;
		} else if (pos == 0 && z < 7.35) {
			setRot(1);
		}
		LED_Toggle();
		delay(1000);
	}
}


