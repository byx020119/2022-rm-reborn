#include "main.h"
#include "JudgingSystemTask.h"

//USART2_RX��PA2--->DMA2������2ͨ��4


#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART2->SR&0X40)==0);//ѭ������,ֱ���������   
	USART2->DR = (u8) ch;      
	return ch;
}
#endif


uint8_t USART2_DMA1_RX_BUF[2][USART2_DMA1_RX_BUF_LEN];

uint8_t CRC8_Ref_Value;
uint8_t CRC8_Solve_Value;
uint16_t CRC16_Ref_Value;    //�յ���CRC16У��ֵ
uint16_t CRC16_Solve_Value;  //����õ���CRC16У��ֵ

uint8_t Save_Element_Array[30];
uint16_t data_Length;
uint16_t Tail_Over_Zero_Value =0;   //βָ��ͨ�����
uint16_t Head_Over_Zero_Value =0;   //ͷָ��ͨ�����

uint32_t usart2_this_time_rx_len = 0;

void usart2_Init(u32 bound)
{
	/* -------------- Enable Module Clock Source ----------------------------*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	/* -------------- Configure GPIO & USART2 -------------------------------*/
	{
		GPIO_InitTypeDef gpio;
		USART_InitTypeDef usart;
		DMA_InitTypeDef dma;
		
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);
		
		GPIO_StructInit(&gpio);
		gpio.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;
		gpio.GPIO_Mode = GPIO_Mode_AF;
		gpio.GPIO_Speed = GPIO_Speed_50MHz;
		gpio.GPIO_PuPd = GPIO_OType_PP;
		GPIO_Init(GPIOD, &gpio);
		
		USART_DeInit(USART2);
		USART_StructInit(&usart);
		usart.USART_BaudRate =bound;
		usart.USART_WordLength = USART_WordLength_8b;
		usart.USART_StopBits = USART_StopBits_1;
		usart.USART_Parity = USART_Parity_No;//USART_Parity_Even;
		usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_Init(USART2, &usart);
		
		USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
		USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);        //usart rx idle interrupt  enabled;IDLE�Ǵ��ڿ����ж�
//		USART_ITConfig(USART2, USART_IT_TC, ENABLE);
		USART_Cmd(USART2, ENABLE);
	}
	/* -------------- Configure DMA1_Stream5_ch4 --------------------------------*/
	{
		DMA_InitTypeDef dma;
		
		DMA_DeInit(DMA1_Stream5);
		DMA_StructInit(&dma);
		dma.DMA_Channel = DMA_Channel_4;
		dma.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);
		dma.DMA_Memory0BaseAddr = (uint32_t)&USART2_DMA1_RX_BUF[0][0];
		dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
		dma.DMA_BufferSize = sizeof(USART2_DMA1_RX_BUF)/2;  //sizeof(USART2_DMA1_RX_BUF)��ʾ��������ܵĴ�С������2��ʾÿ�δ������������������С��һ�룬��USART2_DMA1_RX_BUF[0]����USART2_DMA1_RX_BUF[1]�Ĵ�С
		                                                    //�൱������NDTR(ÿ�δ����������)��ֵ
		dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
		dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		dma.DMA_Mode = DMA_Mode_Circular;                //ѭ��ģʽ��DMA����һ�����ݺ�DMA_BufferSize(ÿ�δ����������)�����¼���Ϊ��ʼ��ʱ��ֵ�����µ�������ʱ���ɼ�������
		                                                 //��ͨģʽ��DMA����һ�����ݺ�DMA_BufferSize(ÿ�δ����������)�������¼���Ϊ��ʼ��ʱ��ֵ����Ҫ���´����������³�ʼ��
		                                                 //DMA����һ������ʱ��ÿ����һ���е�һ�����ݣ�DMA_BufferSize���Զ���1��ֱ����Ϊ0���ð��������
		dma.DMA_Priority = DMA_Priority_Medium;
		dma.DMA_FIFOMode = DMA_FIFOMode_Disable;
		dma.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
		dma.DMA_MemoryBurst = DMA_MemoryBurst_Single;
		dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
		DMA_Init(DMA1_Stream5, &dma);
		
		//����Memory1,Memory0�ǵ�һ��ʹ�õ�Memory
		//����������ǿ���˫����ģʽ
		DMA_DoubleBufferModeConfig(DMA1_Stream5, (uint32_t)&USART2_DMA1_RX_BUF[1][0], DMA_Memory_0);   //first used memory configuration
		DMA_DoubleBufferModeCmd(DMA1_Stream5, ENABLE);
		
		DMA_Cmd(DMA1_Stream5, ENABLE);
	}
	/* -------------- Configure NVIC ----------------------------------------*/
	{
		NVIC_InitTypeDef nvic;
		nvic.NVIC_IRQChannel = USART2_IRQn;                          
		nvic.NVIC_IRQChannelPreemptionPriority = 0;   //pre-emption priority 
		nvic.NVIC_IRQChannelSubPriority = 3;		    //subpriority 
		nvic.NVIC_IRQChannelCmd = ENABLE;			
		NVIC_Init(&nvic);	
	}
}



void USART2_IRQHandler(void)
{
	int i,j;
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)       //USART��IDLE�ж�;IDLE�ǿ����ж�
	{
		//clear the idle pending flag                             //���IDLE��־λ
		(void)USART2->SR;
		(void)USART2->DR;

		//Target is Memory0
		if(DMA_GetCurrentMemoryTarget(DMA1_Stream5) == 0)         //DMA_GetCurrentMemoryTarget():�õ���ǰDMA���ڴ��ַ��0����1
		{
			DMA_Cmd(DMA1_Stream5, DISABLE);
			usart2_this_time_rx_len = USART2_DMA1_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA1_Stream5);//�ж�0����������Ѵ�������
			DMA1_Stream5->NDTR = (uint16_t)USART2_DMA1_RX_BUF_LEN;         //�趨1������
			DMA1_Stream5->CR |= (uint32_t)(DMA_SxCR_CT);                  //ʹ��1����
			DMA_Cmd(DMA1_Stream5, ENABLE);
			
			for(i=0;i<usart2_this_time_rx_len;i++)
			{
				RingBuffer_Write(USART2_DMA1_RX_BUF[0][i]);
			}
		}
		else //Target is Memory1
		{
			DMA_Cmd(DMA1_Stream5, DISABLE);
			usart2_this_time_rx_len = USART2_DMA1_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA1_Stream5);
			DMA1_Stream5->NDTR = (uint16_t)USART2_DMA1_RX_BUF_LEN;      //relocate the dma memory pointer to the beginning position
			DMA1_Stream5->CR &= ~(uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 0
			DMA_Cmd(DMA1_Stream5, ENABLE);
			
			for(i=0;i<usart2_this_time_rx_len;i++)
			{
				RingBuffer_Write(USART2_DMA1_RX_BUF[1][i]);
			}
		}
		while(buffer.tailPosition!=buffer.headPosition)
		{
			if(buffer.tailPosition-buffer.headPosition>=0) 
				Tail_Over_Zero_Value=0;   //δ�����
			else                                           
				Tail_Over_Zero_Value=100; //ͨ�����
			
			if(buffer.headPosition>=96&&buffer.headPosition<=99)  
				Head_Over_Zero_Value = 100;//�����Ԫ��ͷָ������
			else                                                 
				Head_Over_Zero_Value = 0;  //�����Ԫ��ͷָ��δ�����
				
			for(j=0;j<5;j++)   //ȡ��֡ͷ
			{
				RingBuffer_Read(Save_Element_Array+j);
			}
			CRC8_Ref_Value   = Save_Element_Array[4];
			CRC8_Solve_Value = Get_CRC8_Check_Sum(Save_Element_Array,4,0xff);
			if(CRC8_Ref_Value == CRC8_Solve_Value)  //֡ͷͨ��CRC8У��
			{
				data_Length = Save_Element_Array[1]|Save_Element_Array[2]<<8;//����������������Ϊ�˷�����1��
				data_Length = Transform_Hex_To_Oct(data_Length,16);
					
				if(buffer.tailPosition+Tail_Over_Zero_Value-(Head_Over_Zero_Value+buffer.headPosition-5)>=5+2+data_Length+2)
				{
					for(j=0;j<data_Length+2+2;j++)
					{
						RingBuffer_Read(Save_Element_Array+5+j);
					}
					CRC16_Ref_Value   = Save_Element_Array[5+2+data_Length+2-2]|Save_Element_Array[5+2+data_Length+2-1]<<8;
					CRC16_Solve_Value = Get_CRC16_Check_Sum(Save_Element_Array,7+data_Length+2-2,0xffff);
					if(CRC16_Ref_Value == CRC16_Solve_Value)  //ͨ��CRC16У��
					{
						if(Save_Element_Array[5]==0x01&&Save_Element_Array[6]==0x02)
						{
							getRobotState(Save_Element_Array);//��ȡ������Ѫ��ֵ
						}
						
						if(Save_Element_Array[5]==0x06&&Save_Element_Array[6]==0x02)
						{
							getRobotHurt(Save_Element_Array);
						}
						
						if(Save_Element_Array[5]==0x07&&Save_Element_Array[6]==0x02)
						{
							getRobotShootData(Save_Element_Array);
						}
						
						if(Save_Element_Array[5]==0x02&&Save_Element_Array[6]==0x02)
						{
							getRobotPowerHeat(Save_Element_Array);//��ȡ17mm����ǹ������
						}
					}
				}
				else
				{
					buffer.headPosition = Head_Over_Zero_Value+buffer.headPosition-5;
					break;
				}
			}
	  }
	}
}
