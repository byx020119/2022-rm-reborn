#include "main.h"
#include "ShootingTask.h"
#include "WorkState.h"
#include "uart1.h"
#include "RemoteTask.h"
#include "Freedom_Status.h"
#include "JudgingSystemTask.h"
#include "usart6.h"

/***
下列程序用于UART1打印数据，视觉，印的UART2
***/
//#if 1
//#pragma import(__use_no_semihosting)             
////标准库需要的支持函数                 
//struct __FILE 
//{ 
//	int handle; 
//}; 

//FILE __stdout;       
////定义_sys_exit()以避免使用半主机模式    
//void _sys_exit(int x) 
//{ 
//	x = x; 
//} 
////重定义fputc函数 
//int fputc(int ch, FILE *f)
//{ 	
//	while((USART3->SR&0X40)==0);//循环发送,直到发送完毕   
//	USART3->DR = (u8) ch;      
//	return ch;
//}
//#endif


__align(8) u8 USART1_TX_BUF[USART1_MAX_SEND_LEN]; 	//发送缓冲,最大USART1_MAX_SEND_LEN(400)字节
u8 USART1_RX_BUF[USART1_MAX_RECV_LEN];//接收缓冲,最大USART_REC_LEN(400)个字节.
u16 USART1_RX_STA=0;						 //接收状态标记	



/***
共收到8个字节，前两个'R','M';之后Y轴低8位，Y轴高8位；之后P轴低8位，P轴高8位；最后两个字节空闲
先Y轴，后P轴
***/
uint32_t usart1_this_time_rx_len = 0;                                    //USART3收到的数据个数
uint8_t ChariotRecognition_data[2][ChariotRecognition_data_dma_buf_len]; //DMA接收妙算数据的双缓存数组
int16_t ChariotRecognitionTemp[3];                                       //解析角度内容的数组
int16_t ChariotRecognitionDirection[2];                                  //接受摄像头传来的大装甲数据
int16_t Chariot_Rec_Dir_rotate[2];                                       //接受摄像头传来的大装甲和小装甲数据
float last_ChariotRecognition_pitch = 0.0f;
float ChariotRecognition_pitch = 0.0f;                                   //pitch角度值
float last_ChariotRecognition_yaw = 0.0f;
float ChariotRecognition_yaw = 0.0f;                                     //yaw角度值
uint16_t last_Target_Distance = 0;
uint16_t Target_Distance = 150;                                       //摄像头与目标的距离
uint16_t Distance_buf[10];
uint8_t Dis_Buf_Index = 0;
uint8_t Pitch_Add_Angle = 0;
int Armor_R_Flag_Before=0;
int Armor_R_Flag_Behind=0;
int GM_Rotete_flag_Before=0;    //前固定摄像头识别目标
int GM_Rotete_flag_Behind=0;    //后固定摄像头识别目标
int Time_count=0;

CRringBuffer_t CR_ringBuffer;

float CR_yaw_Angle[20];
u8 CR_yaw_Angle_Index = 0;
u8 CR_yaw_Angle_CNT   = 0;
int8_t loop_j;

char Sendtosight[12];///发送给视觉
static int count_Sendtosight=0;
void USART1_DMA_TX_config();
void DMA_TX_cmd(DMA_Stream_TypeDef *DMAx_Streamx,u16 datasize);
int friction_wheel_count = 0;
float kalman_yaw = 0;
float kalman_pitch = 0;
float kalman_yaw_feedforward = 0;
uint8_t update_flag = 1;
uint8_t enter_CNT = 0;
int Last_CameraDetectTarget_Flag=0;
float E_TEST=0;
float E_TEST1=0;
float E_TEST2=0;
float E_TEST3=0;
int camere_count=0;
int colorflag = 0;

double Yawangle = 0.0f;
int Yawsent = 0;
double t =0.0f;
double z =0.0f;
/***
函数：usart1_Init(bound)
功能：配置USART1，接收妙算数据
备注：USART1_TX--->PA9
      USART1_RX--->PB7
      可向外打印数据
***/
void usart1_Init(u32 bound)
{
	/* -------------- Enable Module Clock Source ----------------------------*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);	//使能GPIOA\B时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);//使能DMA2时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 /* -------------- Configure GPIO & USART3 -------------------------------*/
	{
		GPIO_InitTypeDef gpio;
		USART_InitTypeDef usart;
		//串口1对应引脚复用映射
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_USART1); //GPIOB7复用为USART1 
		
		//USART1端口配置
		gpio.GPIO_Pin = GPIO_Pin_9; //GPIOA9
		gpio.GPIO_Mode = GPIO_Mode_AF;//复用功能
		gpio.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
		gpio.GPIO_OType = GPIO_OType_PP; //推挽复用输出
		gpio.GPIO_PuPd = GPIO_PuPd_UP; //上拉
		GPIO_Init(GPIOA,&gpio); //初始化，PA9
		
		gpio.GPIO_Pin = GPIO_Pin_7;
		gpio.GPIO_Mode = GPIO_Mode_AF;//复用功能
		gpio.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
		gpio.GPIO_OType = GPIO_OType_PP; //推挽复用输出
		gpio.GPIO_PuPd = GPIO_PuPd_UP; //上拉
		GPIO_Init(GPIOB,&gpio); //初始化PB7


		//USART1 初始化设置
		USART_DeInit(USART1);
		USART_StructInit(&usart);
		usart.USART_BaudRate = bound;//波特率设置
		usart.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
		usart.USART_StopBits = USART_StopBits_1;//一个停止位
		usart.USART_Parity = USART_Parity_No;//无奇偶校验位
		usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		usart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//收发模式
		USART_Init(USART1, &usart); //初始化串口1
		
		USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
		//USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
		USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//开启相关中断
		USART_Cmd(USART1, ENABLE);  //使能串口1
	}
	/* -------------- Configure DMA1_Stream1 --------------------------------*/
	{
		DMA_InitTypeDef dma;
		
		DMA_DeInit(DMA2_Stream5);
		DMA_StructInit(&dma);
		dma.DMA_Channel = DMA_Channel_4;
		dma.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);
		dma.DMA_Memory0BaseAddr = (uint32_t)&ChariotRecognition_data[0][0];        //定义DMA在内存的目标位置，即DMA即将要读取或者写入的位置
		dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
		dma.DMA_BufferSize = sizeof(ChariotRecognition_data)/2;                    //数组长度
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
		DMA_DoubleBufferModeConfig(DMA2_Stream5, (uint32_t)&ChariotRecognition_data[1][0], DMA_Memory_0);   //first used memory configuration
		DMA_DoubleBufferModeCmd(DMA2_Stream5, ENABLE);
		
		DMA_Cmd(DMA2_Stream5, ENABLE);
	}
	/* -------------- Configure NVIC ----------------------------------------*/
	{
		NVIC_InitTypeDef nvic;
		//串口1 NVIC 配置
		nvic.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
		nvic.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级0
		nvic.NVIC_IRQChannelSubPriority =2;		//子优先级0
		nvic.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&nvic);	//根据指定的参数初始化VIC寄存器、
	}
}

////////////////////////usart2 DMA send,
/*1，和接受使用了不同的DMA通道，所以需要重新配，参考技术手册，找到外设对应映射的数据流和通道
2， 
*/

void USART1_DMA_TX_config()
{


}
///////不同于上面的初始化函数，这个函数只有在需要发数的时候才调用//
void DMA_TX_cmd(DMA_Stream_TypeDef *DMAx_Streamx,u16 datasize)
{

}


void USART1_IRQHandler(void)  	//串口1中断服务程序
{
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)      //接收到数据
	{
//		//clear the idle pending flag 清除闲置待定标志
		(void)USART1->SR;
		(void)USART1->DR;

		//Target is Memory0目标是内存0
		if(DMA_GetCurrentMemoryTarget(DMA2_Stream5) == 0)
		{
			DMA_Cmd(DMA2_Stream5, DISABLE);
			usart1_this_time_rx_len = ChariotRecognition_data_dma_buf_len - DMA_GetCurrDataCounter(DMA2_Stream5);
			DMA2_Stream5->NDTR = (uint16_t)ChariotRecognition_data_dma_buf_len;     //relocate the dma memory pointer to the beginning position将dma内存指点移到始置位置
			DMA2_Stream5->CR |= (uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 1启用当前选定内存是内存1
			DMA_Cmd(DMA2_Stream5, ENABLE);
      if(usart1_this_time_rx_len == ChariotRecognition_data_len)
			{
				ChariotRecognition_Mes_Process(ChariotRecognition_data[0]);
			}
		}
		else //Target is Memory1目标是内存1
		{
			DMA_Cmd(DMA2_Stream5, DISABLE);
			usart1_this_time_rx_len = ChariotRecognition_data_dma_buf_len - DMA_GetCurrDataCounter(DMA2_Stream5);
			DMA2_Stream5->NDTR = (uint16_t)ChariotRecognition_data_dma_buf_len;      //relocate the dma memory pointer to the beginning position将dma内存指点移到始置位
			DMA2_Stream5->CR &= ~(uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 0启用当前选定内存是内存1
			DMA_Cmd(DMA2_Stream5, ENABLE);
      if(usart1_this_time_rx_len == ChariotRecognition_data_len)
			{
				ChariotRecognition_Mes_Process(ChariotRecognition_data[1]);
			}
		}
	}
} 


/////////////////////////////////////////
State_distance state_distacne=closedistance;


void Sendtosightway(int value)//与视觉商定
{
	//对于陀螺仪值，取到小数点后一位，故抬高十倍，将四位数拆分0-9间逐一以字符形式发送，故在数值基础上+48（0的ASCII码）
	
//	t= (  ( (int)GMYawEncoder.ecd_angle % 360 )+360 ) % 360; //每一圈里的码盘数值，保证是正的
//	int Yawsent =  (int)t % 360 ;
//	
//  int GMYawtemp_Eular=(Yawsent)*10;    //-180，精确度到小数点后一位，乘10，发送四位
//	int GMPitchtemp_Encoder=(GMPitchEncoder.ecd_angle+90)*10;	
	
//  if(robotState.robot_id>=1&&robotState.robot_id<=9)//判断敌方我方 ，1-9为red，101-109为blue
//	{
//	Sendtosight[0]='B';	
//	}
//	if(robotState.robot_id>=101&&robotState.robot_id<=109)
//	{
//	Sendtosight[0]='R';	
//	}
	
//	colorflag=PCin(6);
////没有的时候：利用开关
//	if( PCin(6) == 1 )
//	{
//		Sendtosight[0] ='B';	
//	}
//	if( PCin(6) == 0)
//	{
//		Sendtosight[0]='R';	
//	}
	int GMYawtemp_Eular=(Angles+180)*10;
	int GMPitchtemp_Encoder=(Eular[0]+90)*10;

	Sendtosight[0]='R';	
	
	Sendtosight[1]='M';//开始发送
	Sendtosight[2]='N';
	Sendtosight[3]='L';
	
	Sendtosight[4]=(uint8_t)(GMYawtemp_Eular/1000+48);			 //千位
	Sendtosight[5]=(uint8_t)(GMYawtemp_Eular%1000/100+48);	 //百位
	Sendtosight[6]=(uint8_t)(GMYawtemp_Eular%100/10+48);		 //十位
	Sendtosight[7]=(uint8_t)(GMYawtemp_Eular%10+48);				 //个位
	
	Sendtosight[8]=(uint8_t)(GMPitchtemp_Encoder/1000+48);		//千位
	Sendtosight[9]=(uint8_t)(GMPitchtemp_Encoder%1000/100+48);//百位
	Sendtosight[10]=(uint8_t)(GMPitchtemp_Encoder%100/10+48);	//十位
	Sendtosight[11]=(uint8_t)(GMPitchtemp_Encoder%10+48);			//个位
	
  while(count_Sendtosight<value)
  {
	  USART_SendData(USART1, Sendtosight[count_Sendtosight]);
	  count_Sendtosight++;
	  delay_us(150);
  }
 
  {
		count_Sendtosight=0;
  }
}

/***
函数：ChariotRecognition_Mes_Process(p)
功能：妙算数据处理
备注：无
***/
void ChariotRecognition_Mes_Process(uint8_t *p)
{
	  ChariotRecognitionTemp[0] = ((p[3]<<8) | p[2]);
		ChariotRecognitionTemp[1] = ((p[5]<<8) | p[4]);
		ChariotRecognitionTemp[2] =  p[6];
	  ChariotRecognitionDirection[0]= p[7];
	  ChariotRecognitionDirection[1]= p[8];
	  Chariot_Rec_Dir_rotate[0]= p[9];
	  Chariot_Rec_Dir_rotate[1]= p[10];
	
	Last_CameraDetectTarget_Flag=CameraDetectTarget_Flag;
	
	if(p[0]=='R'&&p[1]=='M'&&p[2]!='A')    //能识别到
	{
	 E_TEST3++;
	 E_TEST=ChariotRecognitionTemp[0];
	 E_TEST1 =ChariotRecognitionTemp[1];	
		
  	//UART1收到数据时，摄像头发现目标标志变为1。
	  CameraDetectTarget_Flag =1;	
		CR_ringBuffer.lost_COUNT =0;	
		
	  if(RC_CtrlData.rc.s1==3)//  if(RC_CtrlData.rc.s1==3 && gameState.game_progress == 4游戏开始，且识别到，开波轮
	  {
		  TempShootingFlag=1;//发弹标志位
	  }
		
		usart1_microsecond.time_now = Get_Time_Micros();//本次收到数据的时刻
		usart1_microsecond.time_error = usart1_microsecond.time_now - usart1_microsecond.time_last;//计算相邻两次的时间
		
		enter_CNT++;
		if(enter_CNT == 1)
		{
			YawCurrentPositionSave = GMYawEncoder.ecd_angle;
			PitchCurrentPositionSave = GMPitchEncoder.ecd_angle;
		}
		
		if(enter_CNT >30) enter_CNT = 30;
		
		//停止，准备，测试和躲避状态不识别，如果识别到之后变为躲避模式，不接受视觉发来的数据，故云台保持识别的数据，要在状态里手动关闭摩擦轮
		if(GetWorkState() == STOP_STATE || GetWorkState() == PREPARE_STATE || GetWorkState() == Test_STATE|| GetWorkState() == Dodeg_STATE)
		{
			ChariotRecognition_yaw = GMYawEncoder.ecd_angle;
			ChariotRecognition_pitch= GMPitchEncoder.ecd_angle;
		}
		else  //自由、识别、精巡逻和躲避状态
		{
			ChariotRecognition_yaw  = ChariotRecognitionTemp[0]/100.0 ;//接收浮点数  // GMYawEncoder.ecd_angle + ChariotRecognitionTemp[0]/100.0 

//			if(ChariotRecognition_yaw==0)//滤掉视觉发来的所有0，0附近的数就可以保持平衡
//			{
//				ChariotRecognition_yaw = last_ChariotRecognition_yaw;
//			}
			
			if(CR_ringBuffer.lost_COUNT<35)  
			{
				CR_ringBuffer.ringBuf[CR_ringBuffer.tailPosition++] = ChariotRecognition_yaw;//yaw角度入列
				if(CR_ringBuffer.tailPosition >= BUFFER_SIZE) CR_ringBuffer.tailPosition = 0;//形成环形队列
				CR_ringBuffer.lost_COUNT = 0;//丢失目标次数清零
			}
			else //如果很多次没识别到，突然识别到一次，不往环形队列中存储该值，并将环形队列清零，尾指针清零
			{
				for(u8 loop_i = 0;loop_i<5;loop_i++)
				{
				  CR_ringBuffer.ringBuf[loop_i] = 0;
				}
				CR_ringBuffer.tailPosition = 0;
				CR_ringBuffer.lost_COUNT = 0;
			}
			
			Distance_buf[Dis_Buf_Index++] = ChariotRecognitionTemp[2];
			if(Dis_Buf_Index >= Dis_buf_Size) Dis_Buf_Index = 0;  //索引循环
			Target_Distance = filter(Distance_buf);

			// Eular[0]+
			ChariotRecognition_pitch = ChariotRecognitionTemp[1]/100.0;//(0.0088*Target_Distance+1.9604);//GMPitchEncoder.ecd_angle + ChariotRecognitionTemp[1]/100.0;

			
			last_ChariotRecognition_yaw   = ChariotRecognition_yaw;
			last_ChariotRecognition_pitch = ChariotRecognition_pitch;
		}
		usart1_microsecond.time_last = Get_Time_Micros();//记录上一次收到数据的时刻
	}
	
	if(p[0]=='R'&&p[1]=='M'&&p[2]=='A'&&p[3]=='A'&&p[4]=='A')//识别不到
	{
		
			E_TEST=0;
      E_TEST1=0;
		
		CR_ringBuffer.lost_COUNT++;
		
		if(CR_ringBuffer.lost_COUNT>=35)//如果改这个数据一定要全改，很容易卡在识别模式里
		{
			CR_ringBuffer.lost_COUNT = 35;
		}
		
		if(CR_ringBuffer.lost_COUNT<=35)//连续丢失目标的次数小于3次，进行预测
		{

			ChariotRecognition_yaw = GMYawEncoder.ecd_angle;
			ChariotRecognition_pitch = GMPitchEncoder.ecd_angle;			
   	}	
	}
	
	if(CR_ringBuffer.lost_COUNT>=35)
	{
		enter_CNT = 0;
		ChariotRecognition_yaw = GMYawEncoder.ecd_angle;
		ChariotRecognition_pitch = GMPitchEncoder.ecd_angle;
		TempShootingFlag = 0;
		CameraDetectTarget_Flag = 0;//如果连续?帧没识别到，则换状态
	}
	
		if(  RC_CtrlData.rc.s1== 3) //&& gameState.game_progress == 4游戏开始
	{
		friction_wheel_state_flag = 1; //游戏开始，s1 放到中间，摩擦轮就开
	}

	if(GetWorkState()== ChariotRecognition_STATE && RC_CtrlData.rc.s1==2)//识别状态下，将左拨杆放到最下仍无法关闭摩擦轮  //5/2
	{
		TempShootingFlag = 0;//关拨轮
		friction_wheel_state_flag = 0;//关摩擦轮
	}

}


uint16_t filter(uint16_t *distance_buf)//滤波其实视觉做了
{
	int i,j,t;
	for(i=0;i<Dis_buf_Size-1;i++)
	{
		for(j=0;j<Dis_buf_Size-i-1;j++)
		{
			if(distance_buf[j]>distance_buf[j+1])
			{
				t = distance_buf[j+1];
				distance_buf[j+1] = distance_buf[j];
				distance_buf[j] = t;
			}
		}
	}
	
	return (int)(distance_buf[Dis_buf_Size/2]);
}
