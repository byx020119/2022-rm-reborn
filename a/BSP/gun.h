#ifndef GUN_H
#define GUN_H
#include <stm32f4xx.h>

#define PA0_PWM1_FrictionWheel  TIM5->CCR1
#define PA1_PWM1_FrictionWheel  TIM5->CCR2

//1ms 0%����, 2ms100%����,��Ӧ1000~2000
#define InitFrictionWheel()     \
        PA0_PWM1_FrictionWheel = 1000;             \
        PA1_PWM1_FrictionWheel = 1000;
				
//������ȵ�Ħ�����ٶȣ�����ֹͣĦ����
#define SetEqualFrictionWheelSpeed(x) \
        PA0_PWM1_FrictionWheel = x;                \
        PA1_PWM1_FrictionWheel = x;
				
//���ò��ȵ�Ħ�����ٶȣ����ڵ���ˮƽ������ڵ�Ư��
#define SetInequalFrictionWheelSpeed(x) \
        PA0_PWM1_FrictionWheel = x ;                \
        PA1_PWM1_FrictionWheel = x ;


typedef struct PWM_Set
{
	uint32_t RCC_AHB1Periph;
	uint32_t GPIO_Pin_x;
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_PinSource;
  uint8_t GPIO_AF;
	uint32_t Prescaler;
	uint32_t Period;
	TIM_TypeDef* TIMx;
	uint32_t TIM_Pulse;
	uint32_t Chx;
}PWM_Set;

void PWM_Configuration(void);
void PWM_Configuration_AUTO(PWM_Set* PWM);
void Switch_GPIO_PinSource(PWM_Set* PWM);

#endif
