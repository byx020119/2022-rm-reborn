#include "main.h"

typedef __packed struct
{
	uint32_t RCC_GPIOx;
	TIM_TypeDef* TIMx;
	uint8_t  APBx_Flag;
	uint32_t RCC_APB1_TIMx;
	uint32_t RCC_APB2_TIMx;
}my_pwm_set;

void Check_APBxTIMx(my_pwm_set *pwm)
{
	if((uint64_t)pwm->TIMx >= APB2PERIPH_BASE)
	{
		pwm->APBx_Flag=0xa2;
		switch((uint32_t)pwm->TIMx - APB2PERIPH_BASE)
		{
			//TIM1
			case 0x0000:
			{
				pwm->RCC_APB2_TIMx = RCC_APB2Periph_TIM1;
			}break;
			//TIM8
			case 0x0400:
			{
				pwm->RCC_APB2_TIMx = RCC_APB2Periph_TIM8;
			}break;
			//TIM9
			case 0x4000:
			{
				pwm->RCC_APB2_TIMx = RCC_APB2Periph_TIM9;
			}break;
			//TIM10
			case 0x4400:
			{
				pwm->RCC_APB2_TIMx = RCC_APB2Periph_TIM10;
			}break;
			//TIM11
			case 0x4800:
			{
				pwm->RCC_APB2_TIMx = RCC_APB2Periph_TIM11;
			}break;
			//TIM7
			default:
			{

			}
		}
	}
	else
	{
		pwm->APBx_Flag=0xa1;
		switch((uint32_t)pwm->TIMx - APB1PERIPH_BASE)
		{
			//TIM2
			case 0x0000:
			{
				pwm->RCC_APB1_TIMx = RCC_APB1Periph_TIM2;
			}break;
			//TIM3
			case 0x0400:
			{
				pwm->RCC_APB1_TIMx = RCC_APB1Periph_TIM3;
			}break;
			//TIM4
			case 0x0800:
			{
				pwm->RCC_APB1_TIMx = RCC_APB1Periph_TIM4;
			}break;
			//TIM5
			case 0x0C00:
			{
				pwm->RCC_APB1_TIMx = RCC_APB1Periph_TIM5;
			}break;
			//TIM6
			case 0x1000:
			{
				pwm->RCC_APB1_TIMx = RCC_APB1Periph_TIM6;
			}break;
			//TIM7
			case 0x1400:
			{
				pwm->RCC_APB1_TIMx = RCC_APB1Periph_TIM7;
			}break;
			//TIM12
			case 0x1800:
			{
				pwm->RCC_APB1_TIMx = RCC_APB1Periph_TIM12;
			}break;
			//TIM13
			case 0x1C00:
			{
				pwm->RCC_APB1_TIMx = RCC_APB1Periph_TIM13;
			}break;
			//TIM14
			case 0x2000:
			{
				pwm->RCC_APB1_TIMx = RCC_APB1Periph_TIM14;
			}break;
			default:
			{
				
			}
		}
	}

}

void PWM_Configuration0(my_pwm_set* pwm)
{
	/* -------------- Check The Parameter -----------------------------------*/
	{
		Check_APBxTIMx(pwm);
		assert_param(IS_RCC_AHB1_CLOCK_PERIPH(pwm->RCC_GPIOx));
		assert_param(IS_TIM_LIST1_PERIPH(pwm->TIMx)); 
	}
	/* -------------- Enable Module Clock Source ----------------------------*/
	RCC_AHB1PeriphClockCmd(pwm->RCC_GPIOx, ENABLE);
	if(pwm->APBx_Flag == 0xa1)
	{
		RCC_APB1PeriphClockCmd(pwm->RCC_APB1_TIMx , ENABLE);
	}
	else
	{
		RCC_APB2PeriphClockCmd(pwm->RCC_APB2_TIMx , ENABLE);
	}
	/* -------------- Configure GPIO ----------------------------------------*/
	{
		
	}
}
