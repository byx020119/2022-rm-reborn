//Î´¸Ä

#ifndef TIMER_H
#define TIMER_H
#include <stm32f4xx.h>

#define TIME_COUNT_INIT \
{\
	0,\
	0,\
	0,\
	0,\
}\


typedef __packed struct
{
	uint32_t time_last;
	uint32_t time_now;
	int32_t time_error;
	uint8_t flag;
}Time_Count;

extern Time_Count system_micrsecond;
extern Time_Count shot_frequency_limt;
extern Time_Count Yaw_Correction;
extern Time_Count Remote_microsecond;
extern Time_Count usart3_microsecond;
extern Time_Count mpu6050_micrsecond;
extern Time_Count brake_limt ;
void TIM2_Configuration(void);
uint32_t Get_Time_Micros(void);
void TIM6_Configuration(void);
void TIM6_Start(void);

#endif

