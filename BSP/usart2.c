#include "main.h"
#include "JudgingSystemTask.h"
#include "stm32f4xx.h"                  // Device header
#include "usart2.h"

//C 板
//USART6_RX接PG9--->DMA2数据流2通道5  A C板均为此端口
//USART6_TX-PG14        A C板均为此端口
//串口6裁判系统

//A 板
//USART2_RX接PD6--->DMA2数据流2通道5  
//USART2_TX-PD5        
//串口二是裁判系统


//2022改为串口7-UART7 PE7,PE8
//2022改为串口2-USART2 PA2,PA3

#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
	USART2->DR = (u8) ch;      
	return ch;
}
#endif


uint8_t USART2_DMA1_RX_BUF[2][USART2_DMA1_RX_BUF_LEN]; //2行LEN列

uint8_t CRC8_Ref_Value;
uint8_t CRC8_Solve_Value;
uint16_t CRC16_Ref_Value;    //收到的CRC16校验值
uint16_t CRC16_Solve_Value;  //计算得到的CRC16校验值

uint8_t Save_Element_Array[30];
uint16_t data_Length;
uint16_t Tail_Over_Zero_Value =0;   //尾指针通过零点
uint16_t Head_Over_Zero_Value =0;   //头指针通过零点

uint32_t usart2_this_time_rx_len = 0;

void usart2_Init(u32 bound)
{
	/* -------------- Enable Module Clock Source ----------------------------*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	/* -------------- Configure GPIO & USART2 -------------------------------*/
	{
		GPIO_InitTypeDef gpio;
		USART_InitTypeDef usart;
		DMA_InitTypeDef dma;
		
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
		
		GPIO_StructInit(&gpio);
		gpio.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8;
		gpio.GPIO_Mode = GPIO_Mode_AF;
		gpio.GPIO_Speed = GPIO_Speed_50MHz;
		gpio.GPIO_PuPd = GPIO_OType_PP;
		GPIO_Init(GPIOA, &gpio);
		
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
		USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);        //usart rx idle interrupt  enabled;IDLE是串口空闲中断
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
		dma.DMA_BufferSize = sizeof(USART2_DMA1_RX_BUF)/2;  //表示这个数组总的大小，除以2表示每次传输的数量是这个数组大小的一半，即USART2_DMA1_RX_BUF[0]或者USART2_DMA1_RX_BUF[1]的大小
		                                                    //相当于设置NDTR(每次传输的数据量)的值
		dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
		dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		dma.DMA_Mode = DMA_Mode_Circular;                //循环模式：DMA传完一批数据后，DMA_BufferSize(每次传输的数据量)会重新加载为初始化时的值，有新的数据来时，可继续传输
		                                                 //普通模式：DMA传完一批数据后，DMA_BufferSize(每次传输的数据量)不会重新加载为初始化时的值，若要重新传数，需重新初始化
		                                                 //DMA传输一包数据时，每传输一包中的一个数据，DMA_BufferSize会自动减1，直至减为0，该包传输完成
		dma.DMA_Priority = DMA_Priority_Medium;
		dma.DMA_FIFOMode = DMA_FIFOMode_Disable;
		dma.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
		dma.DMA_MemoryBurst = DMA_MemoryBurst_Single;
		dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
		DMA_Init(DMA1_Stream5, &dma);
		
		//配置Memory1,Memory0是第一个使用的Memory
		//下面的两句是开启双缓存模式
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
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)       //USART的IDLE中断;IDLE是空闲中断
	{
		//clear the idle pending flag                             //清空IDLE标志位
		(void)USART2->SR;
		(void)USART2->DR;

		//Target is Memory0
		if(DMA_GetCurrentMemoryTarget(DMA1_Stream5) == 0)         //DMA_GetCurrentMemoryTarget():得到当前DMA的内存地址是0还是1
		{
			DMA_Cmd(DMA1_Stream5, DISABLE);
			usart2_this_time_rx_len = USART2_DMA1_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA1_Stream5);//判断0传输情况，已传输数量
			DMA1_Stream5->NDTR = (uint16_t)USART2_DMA1_RX_BUF_LEN;         //设定1传输量
			DMA1_Stream5->CR |= (uint32_t)(DMA_SxCR_CT);                  //使能1传输
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
				Tail_Over_Zero_Value=0;   //未过零点
			else                                           
				Tail_Over_Zero_Value=100; //通过零点
			
			if(buffer.headPosition>=96&&buffer.headPosition<=99)  
				Head_Over_Zero_Value = 100;//读五个元素头指针过零点
			else                                                 
				Head_Over_Zero_Value = 0;  //读五个元素头指针未过零点
				
			for(j=0;j<5;j++)   //取出帧头
			{
				RingBuffer_Read(Save_Element_Array+j);
			}
			CRC8_Ref_Value   = Save_Element_Array[4];
			CRC8_Solve_Value = Get_CRC8_Check_Sum(Save_Element_Array,4,0xff);
			if(CRC8_Ref_Value == CRC8_Solve_Value)  //帧头通过CRC8校验
			{
				data_Length = Save_Element_Array[1]|Save_Element_Array[2]<<8;//本该用两个变量，为了方便用1个
				data_Length = Transform_Hex_To_Oct(data_Length,16);
					
				if(buffer.tailPosition+Tail_Over_Zero_Value-(Head_Over_Zero_Value+buffer.headPosition-5)>=5+2+data_Length+2)
				{
					for(j=0;j<data_Length+2+2;j++)
					{
						RingBuffer_Read(Save_Element_Array+5+j);
					}
					CRC16_Ref_Value   = Save_Element_Array[5+2+data_Length+2-2]|Save_Element_Array[5+2+data_Length+2-1]<<8;
					CRC16_Solve_Value = Get_CRC16_Check_Sum(Save_Element_Array,7+data_Length+2-2,0xffff);
					if(CRC16_Ref_Value == CRC16_Solve_Value)  //通过CRC16校验
					{
						if(Save_Element_Array[5]==0x01&&Save_Element_Array[6]==0x02)
						{
							getRobotState(Save_Element_Array);//读取机器人血量值
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
							getRobotPowerHeat(Save_Element_Array);//读取17mm弹丸枪口热量
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
