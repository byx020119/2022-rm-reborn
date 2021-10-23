#include "usart3.h"
#include "RemoteTask.h"
#include "main.h"


static uint8_t REMOTE_DMA_RX_BUF[2][REMOTE_DMA_RX_BUF_LEN];

/***
函数：USART3_Configuration()
功能：配置USART3,接收遥控器数据
备注：USART3_RX--->PC11
***/

void USART3_Configuration(void)
{	/* -------------- Enable Module Clock Source ----------------------------*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); 
	/* -------------- Configure GPIO & USART1 -------------------------------*/
	{
	  GPIO_InitTypeDef gpio;
		USART_InitTypeDef usart;
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);
		
		GPIO_StructInit(&gpio);
		gpio.GPIO_Pin = GPIO_Pin_11;
		gpio.GPIO_Mode = GPIO_Mode_AF;
		gpio.GPIO_Speed = GPIO_Speed_2MHz;
		gpio.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOC, &gpio);
		
		USART_DeInit(USART3);
		USART_StructInit(&usart);
		usart.USART_BaudRate =100000;
		usart.USART_WordLength = USART_WordLength_8b;
		usart.USART_StopBits = USART_StopBits_1;
		usart.USART_Parity = USART_Parity_Even;
		usart.USART_Mode = USART_Mode_Rx;
		usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_Init(USART3, &usart);
		
		USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
		USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);        //usart rx idle interrupt  enabled
		USART_Cmd(USART3, ENABLE);
		
	}
	/* -------------- Configure DMA1_Stream1 --------------------------------*/
  {
		DMA_InitTypeDef dma;
		
		DMA_DeInit(DMA1_Stream1);
		DMA_StructInit(&dma);
		dma.DMA_Channel = DMA_Channel_4;
		dma.DMA_PeripheralBaseAddr = (uint32_t)(&USART3->DR);
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
		DMA_Init(DMA1_Stream1, &dma);
		
		//配置Memory1,Memory0是第一个使用的Memory
		DMA_DoubleBufferModeConfig(DMA1_Stream1, (uint32_t)&REMOTE_DMA_RX_BUF[1][0], DMA_Memory_0);   //first used memory configuration
		DMA_DoubleBufferModeCmd(DMA1_Stream1, ENABLE);
		
		DMA_Cmd(DMA1_Stream1, ENABLE);
		
	} 
/* -------------- Configure NVIC ----------------------------------------*/
	{
		NVIC_InitTypeDef nvic;
		nvic.NVIC_IRQChannel = USART3_IRQn;                          
		nvic.NVIC_IRQChannelPreemptionPriority = 1;   //pre-emption priority 
		nvic.NVIC_IRQChannelSubPriority = 2;		    //subpriority 
		nvic.NVIC_IRQChannelCmd = ENABLE;			
		NVIC_Init(&nvic);	
	}
}

int test=0;
int a=0;

void USART3_IRQHandler(void)
{
	static uint32_t usart3_this_time_rx_len = 0;
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
	{
		//clear the idle pending flag 
		(void)USART3->SR;
		(void)USART3->DR;
		//Target is Memory0
		if(DMA_GetCurrentMemoryTarget(DMA1_Stream1) == 0)
		{
			DMA_Cmd(DMA1_Stream1, DISABLE);
			usart3_this_time_rx_len = REMOTE_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA1_Stream1);
			DMA1_Stream1->NDTR = (uint16_t)REMOTE_DMA_RX_BUF_LEN;     //relocate the dma memory pointer to the beginning position
			DMA1_Stream1->CR |= (uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 1
			DMA_Cmd(DMA1_Stream1, ENABLE);
      if(usart3_this_time_rx_len == RC_FRAME_LENGTH)
			{				
				RemoteDataPrcess(REMOTE_DMA_RX_BUF[0]);
			}
		}
		else //Target is Memory1
		{
			DMA_Cmd(DMA1_Stream1, DISABLE);
			usart3_this_time_rx_len = REMOTE_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA1_Stream1);
			DMA1_Stream1->NDTR = (uint16_t)REMOTE_DMA_RX_BUF_LEN;      //relocate the dma memory pointer to the beginning position
			DMA1_Stream1->CR &= ~(uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 0
			DMA_Cmd(DMA1_Stream1, ENABLE);
      if(usart3_this_time_rx_len == RC_FRAME_LENGTH)
			{
				RemoteDataPrcess(REMOTE_DMA_RX_BUF[1]);
			}
		}
	}
}

