#include "gun.h"


/***
函数：PWM_Configuration()
功能：摩擦轮PWM初始化
备注：PA0,PA1摩擦轮
			PA0--->TIM5_CH1
			PA1--->TIM5_CH2
			电调信号:1ms 0%油门, 2ms100%油门,对应1000~2000
***/

