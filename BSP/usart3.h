#ifndef USART3_H
#define USART3_H
#include "stm32f4xx.h"

#define REMOTE_DMA_RX_BUF_LEN 30u

#define RC_FRAME_LENGTH 18u

void USART3_Configuration(void);
void USART3_IRQHandler(void);
	
#endif

