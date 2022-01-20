#include "bsp.h"
#include "timer.h"
#include "can1.h"
#include "can2.h"
#include "usart3.h"
#include "Phototube.h"
#include "uart1.h"
#include "usart2.h"
#include "usart6.h"

void BSP_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断优先级设置为2组
	TIM2_Configuration();           //1us记一个数，记录程序运行时间
	TIM6_Configuration();           //2ms中断一次，刷新一次控制环
	CAN1_Configuration();           //发送电机控制量，接收码盘返回来的数
	CAN2_Configuration();           //发送电机控制量，接收码盘返回来的数
	USART1_Configuration();         //遥控器初始化
	usart3_Init(115200);            //接收妙算传来的数据19200
  USART6_Configuration();         //接收九轴传来的数据
	usart2_Init(115200);            //接收主控传来的数据
//	PWM_Configuration();            //摩擦轮
	Phototube_Configuration();
}

