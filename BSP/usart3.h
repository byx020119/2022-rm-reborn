#include "stdint.h"

#ifndef UART3_H
#define UART3_H
//#include "main.h"
#define USART3_MAX_RECV_LEN		400					//�����ջ����ֽ���
#define USART3_MAX_SEND_LEN		400					//����ͻ����ֽ���
	  	
			
extern uint8_t  USART3_RX_BUF[USART3_MAX_RECV_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern uint16_t USART3_RX_STA;
extern int Armor_R_Flag_Before;
extern int Armor_R_Flag_Behind;

#define ChariotRecognition_data_dma_buf_len   12u
#define ChariotRecognition_data_len           8u
#define A_Size  60
#define Dis_buf_Size 10

#define BUFFER_SIZE 5

typedef enum
{
    closedistance,
	middledistance,
	longdistance
}State_distance;
extern State_distance state_distacne;

typedef struct CRringBuffer_t
{
	float ringBuf[BUFFER_SIZE];
	int16_t tailPosition;
	float lineBuf[BUFFER_SIZE-1];
	float errBuf[BUFFER_SIZE-2];
	float err_Average;
	float predict_Val;
	int16_t lost_COUNT;
	int16_t out_Point;
}CRringBuffer_t;

extern uint8_t ChariotRecognition_data[2][ChariotRecognition_data_dma_buf_len];
extern uint32_t usart3_this_time_rx_len;
extern float last_ChariotRecognition_pitch;
extern float ChariotRecognition_pitch;
extern float last_ChariotRecognition_yaw;
extern float ChariotRecognition_yaw;

extern uint16_t last_Target_Distance;
extern uint16_t Target_Distance;
extern uint16_t Distance_Buf[Dis_buf_Size];
extern uint8_t Dis_Buf_Index;
extern uint8_t Pitch_Add_Angle;
extern CRringBuffer_t CR_ringBuffer;
extern int  GM_Rotete_flag_Before;
extern int  GM_Rotete_flag_Behind;
extern int camere_count;

extern float CR_yaw_Angle[20];
extern uint8_t CR_yaw_Angle_Index;
extern uint8_t CR_yaw_Angle_CNT;
extern int Last_CameraDetectTarget_Flag;


void Sendtosightway(int value);
void usart3_Init(uint32_t bound);
void ChariotRecognition_Mes_Process(uint8_t *p);
uint16_t filter(uint16_t *distance_buf);


#endif
