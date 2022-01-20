//Î´¸Ä

#ifndef Encoder_H
#define Encoder_H
#include <stm32f4xx.h>
#include "CanBusTask.h"

extern int dtm123[6];//2022¼Ó

void GetEncoderBias(volatile Encoder *v, CanRxMsg * msg);
void Motor_3508_EncoderProcess(volatile Encoder *v, CanRxMsg * msg);
void Motor_6623_EncoderProcess(volatile Encoder *v, CanRxMsg * msg);
void Motor_2310_EncoderProcess(volatile Encoder *v, CanRxMsg * msg);
void Motor_6020_EncoderProcess(volatile Encoder *v, CanRxMsg * msg);
void Down_to_Up_Flag(CanRxMsg * msg);
#endif

