#include "main.h"

Time_Count system_micrsecond	 		=	TIME_COUNT_INIT;   //ϵͳʱ�� ��λus
Time_Count shot_frequency_limt 		= TIME_COUNT_INIT;   //�������Ƶ�ʿ���ʱ�� ��λus
Time_Count Remote_microsecond     = TIME_COUNT_INIT;   //����״̬��׼��״̬�л�����ʱ��
Time_Count usart3_microsecond     = TIME_COUNT_INIT;   //�����������η���ʱ���
Time_Count mpu6050_micrsecond 		= TIME_COUNT_INIT;   //mpu6050���ϵͳʱ�� ��λus
int time_testtt=0;
int time_test2=0;
/***
������TIM2_Configuration()
���ܣ�ʹ��TIM2����Ϊϵͳʱ��
��ע����
***/
void TIM2_Configuration(void)
{
	/* -------------- Enable Module Clock Source ----------------------------*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	/* -------------- Configure TIM2 ----------------------------------------*/
  {
		TIM_TimeBaseInitTypeDef tim;
    
    tim.TIM_Period = 0xFFFFFFFF;
    tim.TIM_Prescaler = 84 - 1;	 //1M ��ʱ�� ;1us��һ����
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
������TIM6_Configuration()
���ܣ�����TIM6��2ms�ж�һ��
��ע����
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
		Control_Task();         //���ƻ�
		time_testtt++;
	}
}
/***
������TIM3_Configuration()
���ܣ�ʹ��TIM3����ΪС����ͨ��ʱ��
��ע����
***/
void TIM3_Configuration(void)
{
	/* -------------- Enable Module Clock Source ----------------------------*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	/* -------------- Configure TIM3 ----------------------------------------*/
  {
		TIM_TimeBaseInitTypeDef tim;
    
    tim.TIM_Period = 3000-1;//1ms����һ��
    tim.TIM_Prescaler = 84 - 1;	 //1M ��ʱ�� ;1us��һ����
    tim.TIM_ClockDivision = TIM_CKD_DIV1;	
    tim.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM3, &tim);
		
		TIM_ARRPreloadConfig(TIM3,ENABLE);
    TIM_Cmd(TIM3,ENABLE);	
	}
	
		/* -------------- Configure NVIC ----------------------------------------*/
	{
		NVIC_InitTypeDef         nvic;

    nvic.NVIC_IRQChannel = TIM3_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority =1;
    nvic.NVIC_IRQChannelSubPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
	}
	

}

void TIM3_Start(void)
{
    TIM_Cmd(TIM3, ENABLE);	 
    TIM_ITConfig(TIM3, TIM_IT_Update,ENABLE);
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);	
}
   
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!= RESET) 
	{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		TIM_ClearFlag(TIM3, TIM_FLAG_Update);
		
		Sendtosightway(12);	
		time_test2++;
	}
} 
