#include "main.h"

/*-----USART6_TX-----PC6-----*/
/*-----USART6_RX-----PC7----*/
uint8_t  JIE[2][CHANG];
 uint32_t usart8_this_time_rx_len = 0;


//uint8_t dafuROLL_FLAG;
void UART8_Configuration(void)
{
	/* -------------- Enable Module Clock Source ----------------------------*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART8,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	/* -------------- Configure GPIO & USART6 -------------------------------*/
	{
		GPIO_InitTypeDef  gpio;
		USART_InitTypeDef usart;
			
		GPIO_PinAFConfig(GPIOE,GPIO_PinSource0,GPIO_AF_UART8);
		GPIO_PinAFConfig(GPIOE,GPIO_PinSource1,GPIO_AF_UART8);
		
		gpio.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1 ;
		gpio.GPIO_Mode = GPIO_Mode_AF;
		gpio.GPIO_OType = GPIO_OType_PP;
		gpio.GPIO_Speed = GPIO_Speed_100MHz;
		gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOE,&gpio);
		
		usart.USART_BaudRate = 115200;
		usart.USART_WordLength = USART_WordLength_8b;
		usart.USART_StopBits = USART_StopBits_1;
		usart.USART_Parity = USART_Parity_No;
		usart.USART_Mode = USART_Mode_Rx;
		usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_Init(UART8,&usart);

		USART_DMACmd(UART8, USART_DMAReq_Rx, ENABLE);
		USART_Cmd(UART8,ENABLE);
		USART_ITConfig(UART8,USART_IT_IDLE,ENABLE);
	}
		/* -------------- Configure DMA2_Stream2 --------------------------------*/
	{
		DMA_InitTypeDef dma;
		
		DMA_DeInit(DMA1_Stream6);
		DMA_StructInit(&dma);
		dma.DMA_Channel = DMA_Channel_5;
		dma.DMA_PeripheralBaseAddr = (uint32_t)(&UART8->DR);
		dma.DMA_Memory0BaseAddr = (uint32_t)&JIE[0][0];
		dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
		dma.DMA_BufferSize = sizeof(CHANG)/2;
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
		DMA_Init(DMA1_Stream6, &dma);
		
		//配置Memory1,Memory0是第一个使用的Memory
		DMA_DoubleBufferModeConfig(DMA1_Stream6, (uint32_t)&JIE[1][0], DMA_Memory_0);   //first used memory configuration
		DMA_DoubleBufferModeCmd(DMA1_Stream6, ENABLE);
		
		DMA_Cmd(DMA1_Stream6, ENABLE);
	}
	/* -------------- Configure NVIC ----------------------------------------*/
	{
		NVIC_InitTypeDef  nvic;
		
		nvic.NVIC_IRQChannel = UART8_IRQn;
		nvic.NVIC_IRQChannelPreemptionPriority = 0;
		nvic.NVIC_IRQChannelSubPriority = 0;
		nvic.NVIC_IRQChannelCmd = ENABLE;

		NVIC_Init(&nvic);
	}
	jiesuan(JIE[0]);
}


void UART8_IRQHandler(void)                	//串口6中断服务程序
{
	if(USART_GetITStatus(UART8, USART_IT_IDLE) != RESET)
	{
		//clear the idle pending flag 
		(void)UART8->SR;
		(void)UART8->DR;

		//Target is Memory0
		if(DMA_GetCurrentMemoryTarget(DMA1_Stream6) == 0)
		{
			DMA_Cmd(DMA1_Stream6, DISABLE);
			usart8_this_time_rx_len = CHANG - DMA_GetCurrDataCounter(DMA1_Stream6);
			DMA1_Stream6->NDTR = (uint16_t)CHANG;     //relocate the dma memory pointer to the beginning position
			DMA1_Stream6->CR |= (uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 1
			DMA_Cmd(DMA1_Stream6, ENABLE);
      
			
				jiesuan(JIE[0]);
			
		}
		else //Target is Memory1
		{
			DMA_Cmd(DMA1_Stream6, DISABLE);
			usart8_this_time_rx_len = CHANG - DMA_GetCurrDataCounter(DMA1_Stream6);
			DMA1_Stream6->NDTR = (uint16_t)CHANG;     //relocate the dma memory pointer to the beginning position
			DMA1_Stream6->CR |= (uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 1
			DMA_Cmd(DMA1_Stream6, ENABLE);
     
			
				jiesuan(JIE[1]);
			
		}
	}       
}
int juli;
void jiesuan(uint8_t *pData)
{
	uint8_t H1,H2,L1,L2;
	
	H1=laserExchange(pData[9]);
	H2=laserExchange(pData[10]);
	L1=laserExchange(pData[11]);
	L2=laserExchange(pData[12]);
	juli=(int16_t)hexExchange(H1,H2,L1,L2);
//	juli=(int16_t)((pData[12])+(pData[11]<<4)+(pData[10]<<8)+(pData[9]<<16));
}
	
uint8_t laserExchange(uint8_t data)
{
	if(data>47&&data<58)
		data=data-48;
	if(data>64&&data<71)
		data=data-55;
	else data=0;
	return data;
}
uint8_t hexExchange(uint8_t data1,uint8_t data2,uint8_t data3,uint8_t data4)
{
	uint8_t distance;
	distance=data1*16*16*16+data2*16*16+data3*16+data4;
  return distance;
}	

	
	
	