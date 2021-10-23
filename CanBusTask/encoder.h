//Î´¸Ä

#ifndef Encoder_H
#define Encoder_H
#include <stm32f4xx.h>
#include "CanBusTask.h"

void GetEncoderBias(volatile Encoder *v, CanRxMsg * msg);
void Motor_3508_EncoderProcess(volatile Encoder *v, CanRxMsg * msg);
void Motor_6623_EncoderProcess(volatile Encoder *v, CanRxMsg * msg);
void Motor_2310_EncoderProcess(volatile Encoder *v, CanRxMsg * msg);
void Motor_6020_EncoderProcess(volatile Encoder *v, CanRxMsg * msg);

#endif

