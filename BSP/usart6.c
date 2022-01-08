#include "main.h"
#include "usart6.h"

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
//int fputc(int ch, FILE *f)
//{ 	
//	while((USART6->SR&0X40)==0);//循环发送,直到发送完毕   
//	USART6->DR = (u8) ch;      
//	return ch;
//}
#endif

uint8_t USART6_DMA1_RX_BUF[2][IMU_DMA_RX_BUF_LEN]; //2行LEN列

uint8_t CRC8_Ref_Value;
uint8_t CRC8_Solve_Value;
uint16_t CRC16_Ref_Value;    //收到的CRC16校验值
uint16_t CRC16_Solve_Value;  //计算得到的CRC16校验值

uint8_t Save_Element_Array[30];
uint16_t data_Length;
uint16_t Tail_Over_Zero_Value =0;   //尾指针通过零点
uint16_t Head_Over_Zero_Value =0;   //头指针通过零点

uint32_t usart6_this_time_rx_len = 0;

void usart6_Init(u32 bound)
{
	/* -------------- Enable Module Clock Source ----------------------------*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE); 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
	/* -------------- Configure GPIO & USART6 -------------------------------*/
	{
		GPIO_InitTypeDef gpio;
		USART_InitTypeDef usart;
		DMA_InitTypeDef dma;
		
		GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_USART6);
		GPIO_PinAFConfig(GPIOG, GPIO_PinSource9, GPIO_AF_USART6);
		
		GPIO_StructInit(&gpio);
		gpio.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_9;
		gpio.GPIO_Mode = GPIO_Mode_AF;
		gpio.GPIO_Speed = GPIO_Speed_100MHz;
		gpio.GPIO_PuPd = GPIO_OType_PP;
		GPIO_Init(GPIOG, &gpio);
		
		USART_DeInit(USART6);
		USART_StructInit(&usart);
		usart.USART_BaudRate =bound;
		usart.USART_WordLength = USART_WordLength_8b;
		usart.USART_StopBits = USART_StopBits_1;
		usart.USART_Parity = USART_Parity_No;//USART_Parity_Even;
		usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_Init(USART6, &usart);
		
		USART_DMACmd(USART6, USART_DMAReq_Rx, ENABLE);
		USART_ITConfig(USART6, USART_IT_IDLE, ENABLE);        //usart rx idle interrupt  enabled;IDLE是串口空闲中断
//		USART_ITConfig(USART2, USART_IT_TC, ENABLE);
		USART_Cmd(USART6, ENABLE);
	}
	/* -------------- Configure DMA1_Stream5_ch4 --------------------------------*/
	{
		DMA_InitTypeDef dma;
		
		DMA_DeInit(DMA2_Stream1);
		DMA_StructInit(&dma);
		dma.DMA_Channel = DMA_Channel_5;
		dma.DMA_PeripheralBaseAddr = (uint32_t)(&USART6->DR);
		dma.DMA_Memory0BaseAddr = (uint32_t)&USART6_DMA1_RX_BUF[0][0];
		dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
		dma.DMA_BufferSize = sizeof(USART6_DMA1_RX_BUF)/2;  //sizeof(USART2_DMA1_RX_BUF)表示这个数组总的大小，除以2表示每次传输的数量是这个数组大小的一半，即USART2_DMA1_RX_BUF[0]或者USART2_DMA1_RX_BUF[1]的大小
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
		DMA_Init(DMA2_Stream1, &dma);
		
		//配置Memory1,Memory0是第一个使用的Memory
		//下面的两句是开启双缓存模式
		DMA_DoubleBufferModeConfig(DMA2_Stream1, (uint32_t)&USART6_DMA1_RX_BUF[1][0], DMA_Memory_0);   //first used memory configuration
		DMA_DoubleBufferModeCmd(DMA2_Stream1, ENABLE);
		
		DMA_Cmd(DMA2_Stream1, ENABLE);
	}
	/* -------------- Configure NVIC ----------------------------------------*/
	{
		NVIC_InitTypeDef nvic;
		nvic.NVIC_IRQChannel = USART6_IRQn;                          
		nvic.NVIC_IRQChannelPreemptionPriority = 0;   //pre-emption priority 
		nvic.NVIC_IRQChannelSubPriority = 3;		    //subpriority 
		nvic.NVIC_IRQChannelCmd = ENABLE;			
		NVIC_Init(&nvic);	
	}
}
//void USART6_SendChar(char b)
//{
//    while( USART_GetFlagStatus(USART6,USART_FLAG_TC) == RESET);
//	  USART_SendData(USART6,b);
//}

	int i,j;
void USART6_IRQHandler(void)
{
	if(USART_GetITStatus(USART6, USART_IT_IDLE) != RESET)       //USART的IDLE中断;IDLE是空闲中断
	{
		//clear the idle pending flag                             //清空IDLE标志位
		(void)USART6->SR;
		(void)USART6->DR;

		//Target is Memory0
		if(DMA_GetCurrentMemoryTarget(DMA2_Stream1) == 0)         //DMA_GetCurrentMemoryTarget():得到当前DMA的内存地址是0还是1
		{
			DMA_Cmd(DMA2_Stream1, DISABLE);
			usart6_this_time_rx_len = IMU_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA2_Stream1);//判断0传输情况，已传输数量
			DMA2_Stream1->NDTR = (uint16_t)IMU_DMA_RX_BUF_LEN;         //设定1传输量
			DMA2_Stream1->CR |= (uint32_t)(DMA_SxCR_CT);                  //使能1传输
			DMA_Cmd(DMA2_Stream1, ENABLE);
			
			if(usart6_this_time_rx_len == IMU_FRAME_LENGTH)
			{
				IMUDataProcess(USART6_DMA1_RX_BUF[0]);//IMU惯性测量单元
			}
			DMA_Cmd(DMA2_Stream1, ENABLE);

		}
		else //Target is Memory1
		{
			DMA_Cmd(DMA2_Stream1, DISABLE);
			usart6_this_time_rx_len = IMU_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA2_Stream1);
			DMA2_Stream1->NDTR = (uint16_t)IMU_DMA_RX_BUF_LEN;      //relocate the dma memory pointer to the beginning position
			DMA2_Stream1->CR &= ~(uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 0
			DMA_Cmd(DMA2_Stream1, ENABLE);
			
      if(usart6_this_time_rx_len == IMU_FRAME_LENGTH)
			{
				IMUDataProcess(USART6_DMA1_RX_BUF[1]);
			}
			DMA_Cmd(DMA2_Stream1, ENABLE);
		}
	}
}

float Angles;
int16_t Gyro[3];//加速度
float Eular[3]; //欧拉角

void IMUDataProcess(uint8_t *pData)
{
	static  int   yawCount       = 0;
	static  int   pitchCount       = 0;
	static  float LastAngle      = 0;
	static  float NowAngle       = 0;
	static  float lastPitchAngle = 0;
	static  float nowPitchAngle  = 0;
	
	mpu6050_micrsecond.time_last = Get_Time_Micros();
	
	if(pData == NULL)
	{
		return;
	}
	Gyro[0] = ((float)(int16_t)(pData[16] + (pData[17]<<8)));//roll
	Gyro[1] = ((float)(int16_t)(pData[18] + (pData[19]<<8)));//pitch
	Gyro[2] = ((float)(int16_t)(pData[20] + (pData[21]<<8)));//yaw

	Eular[0] = ((float)(int16_t)(pData[23] + (pData[24]<<8)))/100;//pitch
	Eular[1] = -((float)(int16_t)(pData[25] + (pData[26]<<8)))/100;//roll	
	
	Angles = ((float)(int16_t)(pData[27] + (pData[28]<<8)))/10;//yaw
	
	lastPitchAngle = nowPitchAngle;
	nowPitchAngle = Eular[1];

	LastAngle=NowAngle;
	NowAngle=Angles;
	
	if((NowAngle-LastAngle)>300)
		yawCount--;
	if((NowAngle-LastAngle)<-300)
		yawCount++;
	Eular[2]  = NowAngle+yawCount*360;              //已经考虑到了加圈数
	
	if((nowPitchAngle-lastPitchAngle)>300)
		pitchCount--;
	if((nowPitchAngle-lastPitchAngle)<-300)
		pitchCount++;
	Eular[1]  = nowPitchAngle+pitchCount*360;
}
