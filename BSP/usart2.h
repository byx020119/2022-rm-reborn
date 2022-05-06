#ifndef __USART2_H__
#define __USART2_H__
#include <stm32f4xx.h>

#define USART2_MAX_RECV_LEN		400					//最大接收缓存字节数
#define USART2_MAX_SEND_LEN		400					//最大发送缓存字节数

#define USART2_DMA1_RX_BUF_LEN    100          //每级数组的长度
#define Robot_State_Rx_Len        17          //机器人状态数据，应接受的数据量
#define Robot_Hurt_Rx_Len         10          //机器人伤害数据，应接受的数据量
#define Robot_PowerHeat_Rx_Len    29          //机器人功率热量数据，应接受的数据量
#define Robot_Shoot_Rx_Len        15          //机器人射击数据，应接受的数据量

//裁判系统一次发来44个数，以A5开头			
extern uint8_t USART2_DMA1_RX_BUF[2][USART2_DMA1_RX_BUF_LEN];
extern u16 USART2_RX_STA;
extern uint32_t usart2_this_time_rx_len;
extern uint8_t CRC8_Ref_Value;
extern uint8_t CRC8_Solve_Value;
extern uint16_t CRC16_Ref_Value;
extern uint16_t CRC16_Solve_Value;
extern uint8_t Save_Element_Array[30];
extern uint16_t data_Length;
extern uint16_t Tail_Over_Zero_Value;
extern uint16_t Head_Over_Zero_Value;

void usart2_Init(u32 bound);
#endif

