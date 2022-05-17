#ifndef Encoder_H
#define Encoder_H
#include <stm32f4xx.h>
#include "CanBusTask.h"

extern int utm123[6];//2022¼Ó

void GetEncoderBias(volatile Encoder *v, CanRxMsg * msg);
void Motor_3508_EncoderProcess(volatile Encoder *v, CanRxMsg * msg);
void Motor_6623_EncoderProcess(volatile Encoder *v, CanRxMsg * msg);
void Motor_2310_EncoderProcess(volatile Encoder *v, CanRxMsg * msg);
void Motor_6020_EncoderProcess(volatile Encoder *v, CanRxMsg * msg);

void Up_to_Down_Flag(CanRxMsg * msg);//2022¼Ó

#endif
