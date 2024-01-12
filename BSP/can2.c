#include "can2.h"
#include "CanBusTask.h"

/***
������CAN2_Configuration()
���ܣ�����CAN2�����ֵ����
��ע��CAN2_RX--->PB12 A �� C��ΪPB5
      CAN2_TX--->PB13  A �� C��ΪPB6
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
    can2.CAN_Mode = CAN_Mode_Normal;     //CAN�Ĺ���ģʽ���г�ʼ��ģʽ������ģʽ��˯��ģʽ
    can2.CAN_SJW  = CAN_SJW_1tq;
    can2.CAN_BS1 = CAN_BS1_9tq;
    can2.CAN_BS2 = CAN_BS2_4tq;
    can2.CAN_Prescaler = 3;            //CAN BaudRate 42/(1+9+4)/3=1Mbps ;820R�����������ͨ�Ų�����Ϊ1Mbps������Ƶ��Ϊ1KHz
    CAN_Init(CAN2, &can2);

	  can2_filter.CAN_FilterNumber=14;               //ѡ��28��ɸѡ���еĵ�0��ɸѡ�� can1Ϊ0��can2Ϊ14
	  can2_filter.CAN_FilterMode=CAN_FilterMode_IdMask;     //ɸѡ��ģʽ����:�б�ʶ������λģʽ�ͱ�ʶ���б�ģʽ
	  can2_filter.CAN_FilterScale=CAN_FilterScale_32bit;    //ɸѡ��λ�����ã�1��32λ��ʶ������λģʽɸѡ����2��16λ��ʶ������λģʽɸѡ����2��32λ��ʶ���б�ģʽɸѡ����4��16λ��ʶ���б�ģʽɸѡ��
	  can2_filter.CAN_FilterIdHigh=0x0000;
	  can2_filter.CAN_FilterIdLow=0x0000;                   //ID���ֵ�32λ�������յ���ID�����β��������32ΪȫΪ0������£��˴�����д����ֵ
	  can2_filter.CAN_FilterMaskIdHigh=0x0000;
	  can2_filter.CAN_FilterMaskIdLow=0x0000;              //���β��������32ΪȫΪ0��˵������ID�ı�ʶ������ͨ��ɸѡ��
	  can2_filter.CAN_FilterFIFOAssignment=0;        //ѡ��ͨ��ɸѡ�������ݴ����ĸ�FIFO�У���FIFO0��FIFO1����ѡ��
	  can2_filter.CAN_FilterActivation=ENABLE;  //ʹ��ɸѡ��
	  CAN_FilterInit(&can2_filter);
    
    CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);       //FMP0�ж�:FIFO0���յ�����Ϣ
    CAN_ITConfig(CAN2,CAN_IT_TME,ENABLE);        //TME�жϣ���������Ϊ��
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
������CAN2�ķ����жϺ���
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
������CAN2�Ľ����жϺ���
*************************************************************************/
void CAN2_RX0_IRQHandler(void)
{   
	CanRxMsg rx_message;	
  if(CAN_GetITStatus(CAN2,CAN_IT_FMP0)!= RESET)
	{
		CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
		CAN_ClearFlag(CAN2, CAN_FLAG_FF0); 
		CAN_Receive(CAN2, CAN_FIFO0, &rx_message);
		//������������ݴ���;820R�����������ͨ�Ų�����Ϊ1Mbps������Ƶ��Ϊ1KHz������1ms����һ�α���
		//����ͨ�����ݴ���
		CanReceiveMsgProcess1(&rx_message);
	}
}