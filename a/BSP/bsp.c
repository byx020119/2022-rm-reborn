#include "main.h"

void BSP_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	TIM2_Configuration();           //1us��һ��������¼��������ʱ��
	TIM6_Configuration();           //2ms�ж�һ�Σ�ˢ��һ�ο��ƻ�
	TIM3_Configuration();  
	CAN1_Configuration();
	CAN2_Configuration();	          //���͵�����������������̷���������
	USART1_Configuration();         //ң������ʼ��
	usart3_Init(115200);            //�������㴫��������19200
  USART6_Configuration();
	usart7_Init(115200);            //�������ش���������
	//PWM_Configuration();            //Ħ����
		Phototube_Configuration();

}
