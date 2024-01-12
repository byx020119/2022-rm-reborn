#include "main.h"

static uint8_t REMOTE_DMA_RX_BUF[2][REMOTE_DMA_RX_BUF_LEN];

/***
函数：USART1_Configuration()
功能：配置USART1，接收遥控器数据
备注：USART1_RX--->PB7
***/
void USART1_Configuration(void)
{
	/* -------------- Enable Module Clock Source ----------------------------*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
	/* -------------- Configure GPIO & USART1 -------------------------------*/
	{
		GPIO_InitTypeDef gpio;
		USART_InitTypeDef usart;
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
		
		GPIO_StructInit(&gpio);
		gpio.GPIO_Pin = GPIO_Pin_7;
		gpio.GPIO_Mode = GPIO_Mode_AF;
		gpio.GPIO_Speed = GPIO_Speed_2MHz;
		gpio.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOB, &gpio);
		
		USART_DeInit(USART1);
		USART_StructInit(&usart);
		usart.USART_BaudRate =100000;
		usart.USART_WordLength = USART_WordLength_8b;
		usart.USART_StopBits = USART_StopBits_1;
		usart.USART_Parity = USART_Parity_Even;
		usart.USART_Mode = USART_Mode_Rx;
		usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_Init(USART1, &usart);
		
		USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
		USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);        //usart rx idle interrupt  enabled
		USART_Cmd(USART1, ENABLE);
	}
	/* -------------- Configure DMA2_Stream5 --------------------------------*/
	{
		DMA_InitTypeDef dma;
		
		DMA_DeInit(DMA2_Stream5);
		DMA_StructInit(&dma);
		dma.DMA_Channel = DMA_Channel_4;
		dma.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);
		dma.DMA_Memory0BaseAddr = (uint32_t)&REMOTE_DMA_RX_BUF[0][0];
		dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
		dma.DMA_BufferSize = sizeof(REMOTE_DMA_RX_BUF)/2;
		dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
		dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		dma.DMA_Mode = DMA_Mode_Circular;
		dma.DMA_Priority = DMA_Priority_Medium;
		dma.DMA_FIFOMode = DMA_FIFOMode_Disable;
		dma.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
		dma.DMA_MemoryBurst = DMA_MemoryBurst_Single;
		dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
		DMA_Init(DMA2_Stream5, &dma);
		
		//配置Memory1,Memory0是第一个使用的Memory
		DMA_DoubleBufferModeConfig(DMA2_Stream5, (uint32_t)&REMOTE_DMA_RX_BUF[1][0], DMA_Memory_0);   //first used memory configuration
		DMA_DoubleBufferModeCmd(DMA2_Stream5, ENABLE);
		
		DMA_Cmd(DMA2_Stream5, ENABLE);
	}
	/* -------------- Configure NVIC ----------------------------------------*/
	{
		NVIC_InitTypeDef nvic;
		nvic.NVIC_IRQChannel = USART1_IRQn;                          
		nvic.NVIC_IRQChannelPreemptionPriority = 1;   //pre-emption priority 
		nvic.NVIC_IRQChannelSubPriority = 2;		    //subpriority 
		nvic.NVIC_IRQChannelCmd = ENABLE;			
		NVIC_Init(&nvic);	
	}
}
int test=0;
int a=0;
void USART1_IRQHandler(void)
{
	static uint32_t usart1_this_time_rx_len = 0;
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{
		//clear the idle pending flag 
		(void)USART1->SR;
		(void)USART1->DR;
		//Target is Memory0
		if(DMA_GetCurrentMemoryTarget(DMA2_Stream5) == 0)
		{
			DMA_Cmd(DMA2_Stream5, DISABLE);
			usart1_this_time_rx_len = REMOTE_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA2_Stream5);
			DMA2_Stream5->NDTR = (uint16_t)REMOTE_DMA_RX_BUF_LEN;     //relocate the dma memory pointer to the beginning position
			DMA2_Stream5->CR |= (uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 1
			DMA_Cmd(DMA2_Stream5, ENABLE);
      if(usart1_this_time_rx_len == RC_FRAME_LENGTH)
			{				
				RemoteDataPrcess(REMOTE_DMA_RX_BUF[0]);
			}
		}
		else //Target is Memory1
		{
			DMA_Cmd(DMA2_Stream5, DISABLE);
			usart1_this_time_rx_len = REMOTE_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA2_Stream5);
			DMA2_Stream5->NDTR = (uint16_t)REMOTE_DMA_RX_BUF_LEN;      //relocate the dma memory pointer to the beginning position
			DMA2_Stream5->CR &= ~(uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 0
			DMA_Cmd(DMA2_Stream5, ENABLE);
      if(usart1_this_time_rx_len == RC_FRAME_LENGTH)
			{
				RemoteDataPrcess(REMOTE_DMA_RX_BUF[1]);
			}
		}
	}
}
