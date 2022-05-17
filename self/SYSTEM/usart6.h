#ifndef __USART6_H__
#define __USART6_H__
#include "stm32f4xx.h"

#define IMU_DMA_RX_BUF_LEN 60u
#define IMU_FRAME_LENGTH   29u

extern int16_t Gyro[3];
extern float Eular[3];
extern float Angles;

void USART6_Configuration(void);
void IMUDataProcess(uint8_t *pData);
void AngleAdd(volatile float * Angles,volatile float * angles);

#endif
