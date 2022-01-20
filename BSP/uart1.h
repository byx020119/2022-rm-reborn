
#ifndef USART1_H
#define USART1_H
#include "stm32f4xx.h"

#define REMOTE_DMA_RX_BUF_LEN 30u

#define RC_FRAME_LENGTH 18u

void USART1_Configuration(void);
void USART1_IRQHandler(void);
	
#endif
