#ifndef __USART7_H__
#define __USART7_H__
#include <stm32f4xx.h>

#define UART7_MAX_RECV_LEN		400					//�����ջ����ֽ���
#define UART7_MAX_SEND_LEN		400					//����ͻ����ֽ���

#define UART7_DMA1_RX_BUF_LEN    100          //ÿ������ĳ���
#define Robot_State_Rx_Len        17          //������״̬���ݣ�Ӧ���ܵ�������
#define Robot_Hurt_Rx_Len         10          //�������˺����ݣ�Ӧ���ܵ�������
#define Robot_PowerHeat_Rx_Len    29          //�����˹����������ݣ�Ӧ���ܵ�������
#define Robot_Shoot_Rx_Len        15          //������������ݣ�Ӧ���ܵ�������

//����ϵͳһ�η���44��������A5��ͷ			
extern uint8_t UART7_DMA1_RX_BUF[2][UART7_DMA1_RX_BUF_LEN];
extern u16 UART7_RX_STA;
extern uint32_t uart7_this_time_rx_len;
extern uint8_t CRC8_Ref_Value;
extern uint8_t CRC8_Solve_Value;
extern uint16_t CRC16_Ref_Value;
extern uint16_t CRC16_Solve_Value;
extern uint8_t Save_Element_Array[30];
extern uint16_t data_Length;
extern uint16_t Tail_Over_Zero_Value;
extern uint16_t Head_Over_Zero_Value;

void usart7_Init(u32 bound);
#endif

