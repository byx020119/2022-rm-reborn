#ifndef __USART8_H__
#define __USART8_H__
# define CHANG 19
#include <stm32f4xx.h>
extern uint8_t   JIE[2][CHANG];
extern uint32_t usart8_this_time_rx_len ;
void jiesuan(uint8_t *pData);
void UART8_Configuration(void);
uint8_t laserExchange(uint8_t data);
uint8_t hexExchange(uint8_t data1,uint8_t data2,uint8_t data3,uint8_t data4);

#endif