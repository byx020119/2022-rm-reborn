//#include "main.h"

///***
//函数：PWM_Configuration()
//功能：摩擦轮PWM初始化
//备注：PA0,PA1摩擦轮
//			PA0--->TIM5_CH1
//			PA1--->TIM5_CH2
//			电调信号:1ms 0%油门, 2ms100%油门,对应1000~2000
//***/
//void PWM_Configuration(void)
//{
//		PWM_Set PA0_FrictionWheel,PA1_FrictionWheel;

//		PA0_FrictionWheel.GPIOx						=	GPIOA;
//		PA0_FrictionWheel.GPIO_Pin_x			=	GPIO_Pin_0;
//		PA0_FrictionWheel.GPIO_AF					=	GPIO_AF_TIM5;
//		PA0_FrictionWheel.TIMx						=	TIM5;
//		PA0_FrictionWheel.Chx							=	1;
//		PA0_FrictionWheel.Period					=	10000;
//		PA0_FrictionWheel.Prescaler				=	84-1;
//		PA0_FrictionWheel.TIM_Pulse				=	1000;
//		PWM_Configuration_AUTO(&PA0_FrictionWheel);

//		PA1_FrictionWheel.GPIOx						=	GPIOA;
//		PA1_FrictionWheel.GPIO_Pin_x			=	GPIO_Pin_1;
//		PA1_FrictionWheel.GPIO_AF					=	GPIO_AF_TIM5;
//		PA1_FrictionWheel.TIMx						=	TIM5;
//		PA1_FrictionWheel.Chx							=	2;
//		PA1_FrictionWheel.Period					=	10000;
//		PA1_FrictionWheel.Prescaler				=	84-1;
//		PA1_FrictionWheel.TIM_Pulse				=	1000;
//		PWM_Configuration_AUTO(&PA1_FrictionWheel);
//}


///***
//函数：void PWM_Configuration_AUTO(PWM_Set* pwm)
//功能：配置PWM
//备注：无
//***/
//void PWM_Configuration_AUTO(PWM_Set* pwm)
//{
//		/*
//		pwm->GPIO_Pin_x
//		pwm->GPIOx
//		pwm->GPIO_PinSource
//		pwm->GPIO_AF
//		pwm->Prescaler
//		pwm->Period
//		pwm->TIMx
//		pwm->TIM_Pulse;  
//		pwm->Chx
//		*/
//	
//	  GPIO_InitTypeDef          gpio;
//    TIM_TimeBaseInitTypeDef   tim;
//    TIM_OCInitTypeDef         oc;
//	
//		if(pwm->TIMx == TIM1 || pwm->TIMx == TIM8)
//	{
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
//		
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8 , ENABLE);

//		gpio.GPIO_Mode	= GPIO_Mode_AF;
//		gpio.GPIO_OType	= GPIO_OType_PP;
//		gpio.GPIO_Pin		= pwm->GPIO_Pin_x;//GPIO_Pin_x
//		gpio.GPIO_PuPd	= GPIO_PuPd_NOPULL;
//		gpio.GPIO_Speed	= GPIO_Speed_100MHz;
//		GPIO_Init(pwm->GPIOx,&gpio);//GPIOx

//		Switch_GPIO_PinSource(pwm);
//		GPIO_PinAFConfig(pwm->GPIOx,pwm->GPIO_PinSource,pwm->GPIO_AF);//GPIOx 
//																																	//GPIO_PinSource 
//																																	//GPIO_AF
//		
//		tim.TIM_Prescaler					=	pwm->Prescaler;//Prescaler
//		tim.TIM_CounterMode				=	TIM_CounterMode_Up;
//		tim.TIM_Period						= pwm->Period;//Period
//		tim.TIM_ClockDivision			=	TIM_CKD_DIV1;
//		tim.TIM_RepetitionCounter	=0;
//		TIM_TimeBaseInit(pwm->TIMx,&tim);//TIMx
//		
//		oc.TIM_OCMode = TIM_OCMode_PWM1;
//    oc.TIM_OutputState = TIM_OutputState_Enable;
//    oc.TIM_OutputNState = TIM_OutputNState_Enable;
//    oc.TIM_OCPolarity = TIM_OCPolarity_High;
//    oc.TIM_OCNPolarity = TIM_OCPolarity_High;
//    oc.TIM_OCIdleState = TIM_OCIdleState_Set;
//    oc.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
//		oc.TIM_Pulse = pwm->TIM_Pulse;

//		//Chx TIMx
//		switch (pwm->Chx)
//		{
//			case 1:
//			{
//				TIM_OC1Init(pwm->TIMx,&oc);
//			}break;
//			case 2:
//			{
//				TIM_OC2Init(pwm->TIMx,&oc);
//			}break;
//			case 3:
//			{
//				TIM_OC3Init(pwm->TIMx,&oc);
//			}break;
//		
//			default:
//			{
//				TIM_OC4Init(pwm->TIMx,&oc);
//			}break;
//		}
//		TIM_CtrlPWMOutputs(pwm->TIMx,ENABLE);
//		TIM_Cmd(pwm->TIMx,ENABLE);//TIMx
//	}
//	else
//	{
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
//	
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5 , ENABLE);
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13 , ENABLE);

//		gpio.GPIO_Mode	= GPIO_Mode_AF;
//		gpio.GPIO_OType	= GPIO_OType_PP;
//		gpio.GPIO_Pin		= pwm->GPIO_Pin_x;//GPIO_Pin_x
//		gpio.GPIO_PuPd	= GPIO_PuPd_NOPULL;
//		gpio.GPIO_Speed	= GPIO_Speed_100MHz;
//		GPIO_Init(pwm->GPIOx,&gpio);//GPIOx

//		Switch_GPIO_PinSource(pwm);
//		GPIO_PinAFConfig(pwm->GPIOx,pwm->GPIO_PinSource,pwm->GPIO_AF);//GPIOx 
//																																	//GPIO_PinSource 
//																																	//GPIO_AF
//		
//		tim.TIM_Prescaler					=	pwm->Prescaler;//Prescaler
//		tim.TIM_CounterMode				=	TIM_CounterMode_Up;
//		tim.TIM_Period						= pwm->Period;//Period
//		tim.TIM_ClockDivision			=	TIM_CKD_DIV1;
//		tim.TIM_RepetitionCounter	=0;
//		TIM_TimeBaseInit(pwm->TIMx,&tim);//TIMx
//		
//		oc.TIM_OCMode = TIM_OCMode_PWM1;
//    oc.TIM_OutputState = TIM_OutputState_Enable;
//    oc.TIM_OutputNState = TIM_OutputNState_Enable;
//    oc.TIM_OCPolarity = TIM_OCPolarity_High;
//    oc.TIM_OCNPolarity = TIM_OCPolarity_High;
//    oc.TIM_OCIdleState = TIM_OCIdleState_Set;
//    oc.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
//		oc.TIM_Pulse = pwm->TIM_Pulse;

//		//Chx TIMx
//		switch (pwm->Chx)
//		{
//			case 1:
//			{
//				TIM_OC1Init(pwm->TIMx,&oc);
//				TIM_OC1PreloadConfig(pwm->TIMx,TIM_OCPreload_Enable);
//			}break;
//			case 2:
//			{
//				TIM_OC2Init(pwm->TIMx,&oc);
//				TIM_OC2PreloadConfig(pwm->TIMx,TIM_OCPreload_Enable);
//			}break;
//			case 3:
//			{
//				TIM_OC3Init(pwm->TIMx,&oc);
//				TIM_OC3PreloadConfig(pwm->TIMx,TIM_OCPreload_Enable);
//			}break;
//		
//			default:
//			{
//				TIM_OC4Init(pwm->TIMx,&oc);
//				TIM_OC4PreloadConfig(pwm->TIMx,TIM_OCPreload_Enable);
//			}break;
//		}
//		TIM_ARRPreloadConfig(pwm->TIMx,ENABLE);//TIMx
//		TIM_Cmd(pwm->TIMx,ENABLE);//TIMx
//	}
//}

//void Switch_GPIO_PinSource(PWM_Set* pwm)
//{
////			switch(pwm->GPIO_Pin_x)
////		{
////			case GPIO_Pin_0: 
////			{
////				pwm->GPIO_PinSource = GPIO_PinSource0;
////			}break;
////			case GPIO_Pin_1: 
////			{
////				pwm->GPIO_PinSource = GPIO_PinSource1;
////			}break;
////			case GPIO_Pin_2: 
////			{
////				pwm->GPIO_PinSource = GPIO_PinSource2;
////			}break;
////			case GPIO_Pin_3: 
////			{
////				pwm->GPIO_PinSource = GPIO_PinSource3;
////			}break;
////			case GPIO_Pin_4: 
////			{
////				pwm->GPIO_PinSource = GPIO_PinSource4;
////			}break;
////			case GPIO_Pin_5: 
////			{
////				pwm->GPIO_PinSource = GPIO_PinSource5;
////			}break;
////			case GPIO_Pin_6: 
////			{
////				pwm->GPIO_PinSource = GPIO_PinSource6;
////			}break;
////			case GPIO_Pin_7: 
////			{
////				pwm->GPIO_PinSource = GPIO_PinSource7;
////			}break;
////			case GPIO_Pin_8: 
////			{
////				pwm->GPIO_PinSource = GPIO_PinSource8;
////			}break;
////			case GPIO_Pin_9: 
////			{
////				pwm->GPIO_PinSource = GPIO_PinSource9;
////			}break;
////			case GPIO_Pin_10: 
////			{
////				pwm->GPIO_PinSource = GPIO_PinSource10;
////			}break;
////			case GPIO_Pin_11: 
////			{
////				pwm->GPIO_PinSource = GPIO_PinSource11;
////			}break;
////			case GPIO_Pin_12: 
////			{
////				pwm->GPIO_PinSource = GPIO_PinSource12;
////			}break;
////			case GPIO_Pin_13: 
////			{
////				pwm->GPIO_PinSource = GPIO_PinSource13;
////			}break;
////			case GPIO_Pin_14: 
////			{
////				pwm->GPIO_PinSource = GPIO_PinSource14;
////			}break;
////			default: 
////			{
////				pwm->GPIO_PinSource = GPIO_PinSource15;
////			}break;
////		}
//}

