/*
 * ECE 153B - Spring 2023
 *
 * Name(s):
 * Section:
 * Lab: 6C
 */
 
#include "DMA.h"
#include "SysTimer.h"

void DMA_Init_UARTx(DMA_Channel_TypeDef * tx, USART_TypeDef * uart) {
	
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	delay(1);
	////////////////////////////(DMA1_BASE + 0x08 + 0xd20 * 5) &= ~DMA_CCR_EN;
	tx->CCR &= ~DMA_CCR_EN;
	tx->CCR &= ~DMA_CCR_PL;
	tx->CCR &= ~DMA_CCR_PSIZE; 
	tx->CCR &= ~DMA_CCR_MSIZE; 
	tx->CCR &= ~DMA_CCR_PINC;
	tx->CCR |= DMA_CCR_MINC;
	tx->CCR &= ~DMA_CCR_CIRC;
	tx->CCR |= DMA_CCR_DIR;
	tx->CCR &= ~DMA_CCR_HTIE;
	tx->CCR &= ~DMA_CCR_TEIE;
	tx->CCR |= DMA_CCR_TCIE;
	//DMA1_Channel6->CCR |= DMA_CCR_MEM2MEM;

	tx->CPAR = (uint32_t)&(uart->TDR);  
}
