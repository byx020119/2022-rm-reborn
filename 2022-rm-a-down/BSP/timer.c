#include "main.h"

Time_Count system_micrsecond	 		=	TIME_COUNT_INIT;   //系统时间 单位us
Time_Count shot_frequency_limt 		= TIME_COUNT_INIT;   //发射机构频率控制时间 单位us
Time_Count Remote_microsecond     = TIME_COUNT_INIT;   //测试状态向准备状态切换控制时间
Time_Count usart3_microsecond     = TIME_COUNT_INIT;   //妙算相邻两次发数时间差
Time_Count mpu6050_micrsecond 		= TIME_COUNT_INIT;   //mpu6050监控系统时间 单位us
int time_testtt=0;
/***
函数：TIM2_Configuration()
功能：使能TIM2，作为系统时钟
备注：无
***/
void TIM2_Configuration(void)
{
	/* -------------- Enable Module Clock Source ----------------------------*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	/* -------------- Configure TIM2 ----------------------------------------*/
  {
		TIM_TimeBaseInitTypeDef tim;
    
    tim.TIM_Period = 0xFFFFFFFF;
    tim.TIM_Prescaler = 84 - 1;	 //1M 的时钟 ;1us记一个数
    tim.TIM_ClockDivision = TIM_CKD_DIV1;	
    tim.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM2, &tim);
		
		TIM_ARRPreloadConfig(TIM2,ENABLE);
    TIM_Cmd(TIM2,ENABLE);	
	}
}
   
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)!= RESET) 
	{
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	}
} 

uint32_t Get_Time_Micros(void)
{
	return TIM2->CNT;
}


/***
函数：TIM6_Configuration()
功能：配置TIM6，2ms中断一次
备注：无
***/
void TIM6_Configuration(void)
{
	/* -------------- Enable Module Clock Source ----------------------------*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	/* -------------- Configure TIM6 ----------------------------------------*/
	{
		TIM_TimeBaseInitTypeDef  tim;
    
    tim.TIM_Prescaler = 84-1;        //84M internal clock
    tim.TIM_CounterMode = TIM_CounterMode_Up;
    tim.TIM_ClockDivision = TIM_CKD_DIV1;
    tim.TIM_Period = 2000;  //2ms,500Hz
    TIM_TimeBaseInit(TIM6,&tim);
	}
	/* -------------- Configure NVIC ----------------------------------------*/
	{
		NVIC_InitTypeDef         nvic;

    nvic.NVIC_IRQChannel = TIM6_DAC_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority =0;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
	}
}

void TIM6_Start(void)
{
    TIM_Cmd(TIM6, ENABLE);	 
    TIM_ITConfig(TIM6, TIM_IT_Update,ENABLE);
    TIM_ClearFlag(TIM6, TIM_FLAG_Update);	
}

void TIM6_DAC_IRQHandler(void)  
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)!= RESET) 
	{
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
		TIM_ClearFlag(TIM6, TIM_FLAG_Update);
		Control_Task();         //控制环
		time_testtt++;
	}
}

