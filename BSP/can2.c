#include "can2.h"
#include "CanBusTask.h"

/***
函数：CAN2_Configuration()
功能：配置CAN2（波轮电机）
备注：CAN2_RX--->PB12 A 板/self C板为PB5
      CAN2_TX--->PB13  A 板/self C板为PB6
***/

void CAN2_Configuration(void)
{
	/* -------------- Enable Module Clock Source ----------------------------*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
	/* -------------- Configure GPIO & CAN1 ---------------------------------*/
	{
		GPIO_InitTypeDef       gpio;
		CAN_InitTypeDef        can2;
		CAN_FilterInitTypeDef  can2_filter;
		
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_CAN2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_CAN2);

    gpio.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOB, &gpio);
		
		CAN_DeInit(CAN2);
    CAN_StructInit(&can2);
    
    can2.CAN_TTCM = DISABLE;
    can2.CAN_ABOM = DISABLE;
    can2.CAN_AWUM = DISABLE;
    can2.CAN_NART = DISABLE;
    can2.CAN_RFLM = DISABLE;
    can2.CAN_TXFP = ENABLE;
    can2.CAN_Mode = CAN_Mode_Normal;     //CAN的工作模式，有初始化模式、正常模式、睡眠模式
    can2.CAN_SJW  = CAN_SJW_1tq;
    can2.CAN_BS1 = CAN_BS1_9tq;
    can2.CAN_BS2 = CAN_BS2_4tq;
    can2.CAN_Prescaler = 3;            //CAN BaudRate 42/(1+9+4)/3=1Mbps ;820R电调反馈报文通信波特率为1Mbps，发送频率为1KHz
    CAN_Init(CAN2, &can2);

	  can2_filter.CAN_FilterNumber=14;               //选中28个筛选器中的第0个筛选器 can1为0，can2为14
	  can2_filter.CAN_FilterMode=CAN_FilterMode_IdMask;     //筛选器模式配置:有标识符屏蔽位模式和标识符列表模式
	  can2_filter.CAN_FilterScale=CAN_FilterScale_32bit;    //筛选器位宽配置：1个32位标识符屏蔽位模式筛选器，2个16位标识符屏蔽位模式筛选器和2个32位标识符列表模式筛选器，4个16位标识符列表模式筛选器
	  can2_filter.CAN_FilterIdHigh=0x0000;
	  can2_filter.CAN_FilterIdLow=0x0000;                   //ID部分的32位是期望收到的ID，屏蔽部分掩码的32为全为0的情况下，此处可以写任意值
	  can2_filter.CAN_FilterMaskIdHigh=0x0000;
	  can2_filter.CAN_FilterMaskIdLow=0x0000;              //屏蔽部分掩码的32为全为0，说明各种ID的标识符均可通过筛选器
	  can2_filter.CAN_FilterFIFOAssignment=0;        //选择通过筛选器的数据存入哪个FIFO中，有FIFO0和FIFO1两个选项
	  can2_filter.CAN_FilterActivation=ENABLE;  //使能筛选器
	  CAN_FilterInit(&can2_filter);
    
    CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);       //FMP0中断:FIFO0接收到新消息
    CAN_ITConfig(CAN2,CAN_IT_TME,ENABLE);        //TME中断：发送邮箱为空
	}
  /* -------------- Configure NVIC ----------------------------------------*/
	{
		NVIC_InitTypeDef       nvic;

    nvic.NVIC_IRQChannel = CAN2_RX0_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
    
    nvic.NVIC_IRQChannel = CAN2_TX_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);    
	}
}
/*************************************************************************
                          CAN2_TX_IRQHandler
描述：CAN2的发数中断函数
*************************************************************************/
void CAN2_TX_IRQHandler(void) //CAN TX
{
	if (CAN_GetITStatus(CAN2,CAN_IT_TME)!= RESET) 
	{
		CAN_ClearITPendingBit(CAN2,CAN_IT_TME);
  }
}
/*************************************************************************
                          CAN2_RX0_IRQHandler
描述：CAN2的接收中断函数
*************************************************************************/
void CAN2_RX0_IRQHandler(void)
{   
	CanRxMsg rx_message;	
  if(CAN_GetITStatus(CAN2,CAN_IT_FMP0)!= RESET)
	{
		CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
		CAN_ClearFlag(CAN2, CAN_FLAG_FF0); 
		CAN_Receive(CAN2, CAN_FIFO0, &rx_message);
		//电机编码器数据处理;820R电调反馈报文通信波特率为1Mbps，发送频率为1KHz，即：1ms发送一次报文
		//与板间通信数据处理
		CanReceiveMsgProcess1(&rx_message);
	}
}