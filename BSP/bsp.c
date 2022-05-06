#include "main.h"

void BSP_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	TIM2_Configuration();           //1us记一个数，记录程序运行时间
	TIM6_Configuration();           //2ms中断一次，刷新一次控制环
	TIM3_Configuration();  
	CAN1_Configuration();
	CAN2_Configuration();	          //发送电机控制量，接收码盘返回来的数
	USART1_Configuration();         //遥控器初始化
	usart3_Init(115200);            //接收妙算传来的数据19200
  USART6_Configuration();
	usart2_Init(115200);            //接收主控传来的数据
	//PWM_Configuration();            //摩擦轮
		Phototube_Configuration();

}
