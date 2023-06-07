/*
 * ECE 153B - Spring 2023
 *
 * Name(s):
 * Section:
 * Lab: 6C
 */


#include "UART.h"
#include "DMA.h"

static volatile DMA_Channel_TypeDef * tx;
static volatile char inputs[IO_SIZE];
static volatile uint8_t data_t_0[IO_SIZE];
static volatile uint8_t data_t_1[IO_SIZE];
static volatile uint8_t input_size = 0;
static volatile uint8_t pending_size = 0;
static volatile uint8_t active_size = 0;
static volatile uint8_t * active = data_t_0;
static volatile uint8_t * pending = data_t_1;
#define SEL_0 1
#define BUF_0_EMPTY 2
#define BUF_1_EMPTY 4
#define BUF_0_PENDING 8
#define BUF_1_PENDING 16
#define DMA2CSELR (uint32_t)(*DMA1_Channel7 + 0xA8)

void transfer_data(char ch);
void on_complete_transfer(void);

void UART1_Init(void) {
	//Enable clock for UART1
	RCC->APB2ENR &= ~(RCC_APB2ENR_USART1EN);
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	//Choose System clock as clock source for USART1
	RCC->CCIPR &= ~RCC_CCIPR_USART1SEL;
	RCC->CCIPR |= RCC_CCIPR_USART1SEL_0;
	
	USART1->CR1 |= USART_CR1_TCIE | USART_CR1_RXNEIE;
	
}

void UART2_Init(void) {
	UART2_GPIO_Init();
	//Enable clock for UART2
	RCC->APB1ENR1 &= ~(RCC_APB1ENR1_USART2EN);
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
	
	//Choose System clock as clock source for USART2
	RCC->CCIPR &= ~RCC_CCIPR_USART2SEL;
	RCC->CCIPR |= RCC_CCIPR_USART2SEL_0;
	
	//FIXME CAll dma func and set cmar + DMA-- select channel in dma [DONE?]
	DMA_Init_UARTx(DMA1_Channel7, USART2);
	DMA1_Channel7 -> CMAR = (uint32_t)active; //FIXME?
	DMA1_CSELR->CSELR &= ~DMA_CSELR_C7S;
	DMA1_CSELR->CSELR |= 1 << 25; // no mask available
	
	USART_Init(USART2);
	NVIC_EnableIRQ(USART2_IRQn);
	NVIC_SetPriority(USART2_IRQn , 1);

}

void UART1_GPIO_Init(void) {
	//Enable GPIO clock b
	RCC->AHB2ENR &= ~(RCC_AHB2ENR_GPIOBEN);
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOBEN);

	//Set Alternative function mode
	GPIOB->MODER &= ~(GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
	GPIOB->MODER |= (GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1);

	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL6 | GPIO_AFRL_AFSEL7);
	GPIOB->AFR[0] |= (GPIO_AFRL_AFSEL6_0 | GPIO_AFRL_AFSEL6_1 | GPIO_AFRL_AFSEL6_2 | GPIO_AFRL_AFSEL7_0 | GPIO_AFRL_AFSEL7_1| GPIO_AFRL_AFSEL7_2);
	
	//Set pins 6/7 to very high speed output
	GPIOB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7);
	GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7);
	
	//Set pins 6/7 to push pull
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7);
	
	//Set pins 6/7 to pull-up
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD7);
	GPIOB->PUPDR |= (GPIO_PUPDR_PUPD6_0 | GPIO_PUPDR_PUPD7_0);
}

void UART2_GPIO_Init(void) {
	//Enable GPIO clock a
	RCC->AHB2ENR &= ~(RCC_AHB2ENR_GPIOAEN);
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);
	
	//Set Alternative function mode
	GPIOA->MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
	GPIOA->MODER |= (GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1);
	
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2 | GPIO_AFRL_AFSEL3);
	GPIOA->AFR[0] |= (GPIO_AFRL_AFSEL2_0 | GPIO_AFRL_AFSEL2_1 | GPIO_AFRL_AFSEL2_2 | GPIO_AFRL_AFSEL3_0 | GPIO_AFRL_AFSEL3_1| GPIO_AFRL_AFSEL3_2);
	
	//Set pins 2/3 to very high speed output
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED2 | GPIO_OSPEEDR_OSPEED3);
	GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED2 | GPIO_OSPEEDR_OSPEED3);
	
	
	//Set pins 2/3 to push pull
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3);
	
	//Set pins 2/3 to pull-up
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3);
	GPIOA->PUPDR |= (GPIO_PUPDR_PUPD2_0 | GPIO_PUPDR_PUPD3_0);
	
	
}

void USART_Init(USART_TypeDef* USARTx) {
	// Disable USART of choice
	USARTx->CR1 &= ~USART_CR1_UE;
	//[TODO] Configure baud rate
	USARTx->BRR = 8333;
	//Enable transmitter and reciever
	USARTx->CR1 |= USART_CR1_RE | USART_CR1_TE;
	//Re-enable USART
	USARTx->CR1 |= USART_CR1_TCIE | USART_CR1_RXNEIE;
	USARTx->CR3 |= USART_CR3_DMAT;
	USARTx->CR1 |= USART_CR1_UE;
}

/**
 * This function accepts a string that should be sent through UART
*/
void UART_print(char* data) {
	//TODO
	pending_size = 0;
	if ((DMA1_Channel7->CCR & DMA_CCR_EN) == DMA_CCR_EN) {
		for (int i = 0; i < IO_SIZE; i++) {
			pending[i] = data[i];
			if (data[i] == '\0') break;
			pending_size++;

		}
	} else {
		for (int i = 0; i < IO_SIZE; i++) {
			active[i] = data[i];
			if (data[i] == '\0') break;
			active_size++;
			
		}
		DMA1_Channel7->CNDTR = active_size;
		DMA1_Channel7->CCR |= DMA_CCR_EN;
	}
	//Transfer char array to buffer
	//Check DMA status. If DMA is ready, send data
	//If DMA is not ready, put the data aside
	
	
}
 
/**
 * This function should be invoked when a character is accepted through UART
*/
void transfer_data(char ch) {
	//TODO
	if (ch == '\n') {
		UART_onInput(inputs, input_size);
		input_size = 0;
	} else {
		inputs[input_size] = ch;
		input_size++;
	}
	// Append character to input buffer.
	// If the character is end-of-line, invoke UART_onInput
}

/**
 * This function should be invoked when DMA transaction is completed
*/
void on_complete_transfer(void) {
	//TODO
	// If there are pending data to send, switch active and pending buffer, and send data
	if (pending_size > 0) {
		uint8_t * tmp = active;
		active = pending;
		pending = tmp;
		UART_print(active);
	}
	pending_size = 0;
}

void USART1_IRQHandler(void){
	//TODO
	// When receive a character, invoke transfer_data
	// When complete sending data, invoke on_complete_transfer
}

void USART2_IRQHandler(void){
	//TODO
	NVIC_ClearPendingIRQ(USART2_IRQn);
	if ((USART2->ISR & USART_ISR_RXNE) == USART_ISR_RXNE) {
		transfer_data(USART2->RDR);
	}
	if ((USART2->ISR & USART_ISR_TC) == USART_ISR_TC){
		USART2->ICR |= USART_ICR_TCCF;
		DMA1_Channel7->CCR &= ~DMA_CCR_EN;
		on_complete_transfer();
	}
	// When receive a character, invoke transfer_data
	// When complete sending data, invoke on_complete_transfer
}
