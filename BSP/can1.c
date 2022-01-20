#include "can1.h"
#include "CanBusTask.h"


/***
函数：CAN1_Configuration()
功能：配置CAN1（电机们）
备注：CAN1_RX--->PD0  A C 板均为此端口
      CAN1_TX--->PD1  A C 板均为此端口
***/
void CAN1_Configuration(void)
{
	/* -------------- Enable Module Clock Source ----------------------------*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	/* -------------- Configure GPIO & CAN1 ---------------------------------*/
	{
		GPIO_InitTypeDef       gpio;
		CAN_InitTypeDef        can1;
		CAN_FilterInitTypeDef  can1_filter;
		
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_CAN1);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_CAN1);

    gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOD, &gpio);
		
		CAN_DeInit(CAN1);
    CAN_StructInit(&can1);
    
    can1.CAN_TTCM = DISABLE;
    can1.CAN_ABOM = DISABLE;
    can1.CAN_AWUM = DISABLE;
    can1.CAN_NART = DISABLE;
    can1.CAN_RFLM = DISABLE;
    can1.CAN_TXFP = ENABLE;
    can1.CAN_Mode = CAN_Mode_Normal;     //CAN的工作模式，有初始化模式、正常模式、睡眠模式
    can1.CAN_SJW  = CAN_SJW_1tq;
    can1.CAN_BS1 = CAN_BS1_9tq;
    can1.CAN_BS2 = CAN_BS2_4tq;
    can1.CAN_Prescaler = 3;            //CAN BaudRate 42/(1+9+4)/3=1Mbps ;820R电调反馈报文通信波特率为1Mbps，发送频率为1KHz
    CAN_Init(CAN1, &can1);

	  can1_filter.CAN_FilterNumber=0;               //选中28个筛选器中的第0个筛选器
	  can1_filter.CAN_FilterMode=CAN_FilterMode_IdMask;     //筛选器模式配置:有标识符屏蔽位模式和标识符列表模式
	  can1_filter.CAN_FilterScale=CAN_FilterScale_32bit;    //筛选器位宽配置：1个32位标识符屏蔽位模式筛选器，2个16位标识符屏蔽位模式筛选器和2个32位标识符列表模式筛选器，4个16位标识符列表模式筛选器
	  can1_filter.CAN_FilterIdHigh=0x0000;
	  can1_filter.CAN_FilterIdLow=0x0000;                   //ID部分的32位是期望收到的ID，屏蔽部分掩码的32为全为0的情况下，此处可以写任意值
	  can1_filter.CAN_FilterMaskIdHigh=0x0000;
	  can1_filter.CAN_FilterMaskIdLow=0x0000;              //屏蔽部分掩码的32为全为0，说明各种ID的标识符均可通过筛选器
	  can1_filter.CAN_FilterFIFOAssignment=0;        //选择通过筛选器的数据存入哪个FIFO中，有FIFO0和FIFO1两个选项
	  can1_filter.CAN_FilterActivation=ENABLE;  //使能筛选器
	  CAN_FilterInit(&can1_filter);
    
    CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);       //FMP0中断:FIFO0接收到新消息
    CAN_ITConfig(CAN1,CAN_IT_TME,ENABLE);        //TME中断：发送邮箱为空
	}
  /* -------------- Configure NVIC ----------------------------------------*/
	{
		NVIC_InitTypeDef       nvic;

    nvic.NVIC_IRQChannel = CAN1_RX0_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
    
    nvic.NVIC_IRQChannel = CAN1_TX_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);    
	}
}
/*************************************************************************
                          CAN1_TX_IRQHandler
描述：CAN1的发数中断函数
*************************************************************************/
void CAN1_TX_IRQHandler(void) //CAN TX
{
	if (CAN_GetITStatus(CAN1,CAN_IT_TME)!= RESET) 
	{
		CAN_ClearITPendingBit(CAN1,CAN_IT_TME);
  }
}
/*************************************************************************
                          CAN1_RX0_IRQHandler
描述：CAN1的接收中断函数
*************************************************************************/
void CAN1_RX0_IRQHandler(void)
{   
	CanRxMsg rx_message;	
  if(CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET)
	{
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
		CAN_ClearFlag(CAN1, CAN_FLAG_FF0); 
		CAN_Receive(CAN1, CAN_FIFO0, &rx_message);
		//电机编码器数据处理;820R电调反馈报文通信波特率为1Mbps，发送频率为1KHz，即：1ms发送一次报文
		CanReceiveMsgProcess(&rx_message);
	}
}




