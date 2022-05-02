#include "main.h"

/***
������CAN1_Configuration()
���ܣ�����CAN1
��ע��CAN1_RX--->PB8
      CAN1_TX--->PB9
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
    can1.CAN_Mode = CAN_Mode_Normal;     //CAN�Ĺ���ģʽ���г�ʼ��ģʽ������ģʽ��˯��ģʽ
    can1.CAN_SJW  = CAN_SJW_1tq;
    can1.CAN_BS1 = CAN_BS1_9tq;
    can1.CAN_BS2 = CAN_BS2_4tq;
    can1.CAN_Prescaler = 3;            //CAN BaudRate 42/(1+9+4)/3=1Mbps ;820R�����������ͨ�Ų�����Ϊ1Mbps������Ƶ��Ϊ1KHz
    CAN_Init(CAN1, &can1);

	  can1_filter.CAN_FilterNumber=0;               //ѡ��28��ɸѡ���еĵ�0��ɸѡ��
	  can1_filter.CAN_FilterMode=CAN_FilterMode_IdMask;     //ɸѡ��ģʽ����:�б�ʶ������λģʽ�ͱ�ʶ���б�ģʽ
	  can1_filter.CAN_FilterScale=CAN_FilterScale_32bit;    //ɸѡ��λ�����ã�1��32λ��ʶ������λģʽɸѡ����2��16λ��ʶ������λģʽɸѡ����2��32λ��ʶ���б�ģʽɸѡ����4��16λ��ʶ���б�ģʽɸѡ��
	  can1_filter.CAN_FilterIdHigh=0x0000;
	  can1_filter.CAN_FilterIdLow=0x0000;                   //ID���ֵ�32λ�������յ���ID�����β��������32ΪȫΪ0������£��˴�����д����ֵ
	  can1_filter.CAN_FilterMaskIdHigh=0x0000;
	  can1_filter.CAN_FilterMaskIdLow=0x0000;              //���β��������32ΪȫΪ0��˵������ID�ı�ʶ������ͨ��ɸѡ��
	  can1_filter.CAN_FilterFIFOAssignment=0;        //ѡ��ͨ��ɸѡ�������ݴ����ĸ�FIFO�У���FIFO0��FIFO1����ѡ��
	  can1_filter.CAN_FilterActivation=ENABLE;  //ʹ��ɸѡ��
	  CAN_FilterInit(&can1_filter);
    
    CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);       //FMP0�ж�:FIFO0���յ�����Ϣ
    CAN_ITConfig(CAN1,CAN_IT_TME,ENABLE);        //TME�жϣ���������Ϊ��
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
������CAN1�ķ����жϺ���
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
������CAN1�Ľ����жϺ���
*************************************************************************/
void CAN1_RX0_IRQHandler(void)
{   
	CanRxMsg rx_message;	
  if(CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET)
	{
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
		CAN_ClearFlag(CAN1, CAN_FLAG_FF0); 
		CAN_Receive(CAN1, CAN_FIFO0, &rx_message);
		//������������ݴ���;820R�����������ͨ�Ų�����Ϊ1Mbps������Ƶ��Ϊ1KHz������1ms����һ�α���
		CanReceiveMsgProcess(&rx_message);
	}
}



