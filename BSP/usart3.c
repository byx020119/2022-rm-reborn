#include "main.h"
#include "ShootingTask.h"
#include "WorkState.h"
#include "usart3.h"
#include "usart6.h"
#include "RemoteTask.h"
#include "Freedom_Status.h"
#include "JudgingSystemTask.h"
#include "encoder.h"
/***
���г�������UART3��ӡ���ݣ��Ӿ���ӡ��UART2
***/
//#if 1
//#pragma import(__use_no_semihosting)             
////��׼����Ҫ��֧�ֺ���                 
//struct __FILE 
//{ 
//	int handle; 
//}; 

//FILE __stdout;       
////����_sys_exit()�Ա���ʹ�ð�����ģʽ    
//void _sys_exit(int x) 
//{ 
//	x = x; 
//} 
////�ض���fputc���� 
//int fputc(int ch, FILE *f)
//{ 	
//	while((USART3->SR&0X40)==0);//ѭ������,ֱ���������   
//	USART3->DR = (u8) ch;      
//	return ch;
//}
//#endif


__align(8) u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 	//���ͻ���,���USART1_MAX_SEND_LEN(400)�ֽ�
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN];//���ջ���,���USART_REC_LEN(400)���ֽ�.
u16 USART3_RX_STA=0;						 //����״̬���	



/***
���յ�8���ֽڣ�ǰ����'R','M';֮��Y���8λ��Y���8λ��֮��P���8λ��P���8λ����������ֽڿ���
��Y�ᣬ��P��
***/
uint32_t usart3_this_time_rx_len = 0;                                    //USART3�յ������ݸ���
uint8_t ChariotRecognition_data[2][ChariotRecognition_data_dma_buf_len]; //DMA�����������ݵ�˫��������
int16_t ChariotRecognitionTemp[3];                                       //�����Ƕ����ݵ�����
int16_t ChariotRecognitionDirection[2];                                  //��������ͷ�����Ĵ�װ������
int16_t Chariot_Rec_Dir_rotate[2];                                       //��������ͷ�����Ĵ�װ�׺�Сװ������
float last_ChariotRecognition_pitch = 0.0f;
float ChariotRecognition_pitch = 0.0f;                                   //pitch�Ƕ�ֵ
float last_ChariotRecognition_yaw = 0.0f;
float ChariotRecognition_yaw = 0.0f;                                     //yaw�Ƕ�ֵ
uint16_t last_Target_Distance = 0;
uint16_t Target_Distance = 150;                                       //����ͷ��Ŀ��ľ���
uint16_t Distance_buf[10];
uint8_t Dis_Buf_Index = 0;
uint8_t Pitch_Add_Angle = 0;
int Armor_R_Flag_Before=0;
int Armor_R_Flag_Behind=0;
int GM_Rotete_flag_Before=0;    //ǰ�̶�����ͷʶ��Ŀ��
int GM_Rotete_flag_Behind=0;    //��̶�����ͷʶ��Ŀ��
int Time_count=0;

CRringBuffer_t CR_ringBuffer;

float CR_yaw_Angle[20];
u8 CR_yaw_Angle_Index = 0;
u8 CR_yaw_Angle_CNT   = 0;
int8_t loop_j;

char Sendtosight[16];///���͸��Ӿ�
static int count_Sendtosight=0;
void USART3_DMA_TX_config(void);
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
C ��
������usart1_Init(bound)
���ܣ�����USART1��������������
��ע��USART1_TX--->PA9 A C���Ϊ�˶˿�
      USART1_RX--->PB7 A C���Ϊ�˶˿�
      �������ӡ����
***/
/***
A ��
������usart3_Init(bound)
���ܣ�����USART3��������������
��ע��USART3_TX--->PD9
      USART3_RX--->PD8
      �������ӡ����
***/
void usart3_Init(u32 bound)
{
	/* -------------- Enable Module Clock Source ----------------------------*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);	//ʹ��GPIODʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);//ʹ��DMA2ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ʹ��USART3ʱ��
 /* -------------- Configure GPIO & USART3 -------------------------------*/
	{
		GPIO_InitTypeDef gpio;
		USART_InitTypeDef usart;
		//����3��Ӧ���Ÿ���ӳ��
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3); //GPIOD8����ΪUSART3
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3); //GPIOD9����ΪUSART3
		
		//USART3�˿�����
		gpio.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9; //GPIOD8 9
		gpio.GPIO_Mode = GPIO_Mode_AF;//���ù���
		gpio.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
		gpio.GPIO_OType = GPIO_OType_PP; //���츴�����
		gpio.GPIO_PuPd = GPIO_PuPd_UP; //����
		GPIO_Init(GPIOD,&gpio); //��ʼ����PD8 9


		//USART3 ��ʼ������
		USART_DeInit(USART3);
		USART_StructInit(&usart);
		usart.USART_BaudRate = 115200;//����������
		usart.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
		usart.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		usart.USART_Parity = USART_Parity_No;//����żУ��λ
		usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
		usart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//�շ�ģʽ
		USART_Init(USART3, &usart); //��ʼ������3
		
		USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
		//USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
		USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//��������ж�
		USART_Cmd(USART3, ENABLE);  //ʹ�ܴ���3
	}
	/* -------------- Configure DMA1_Stream1 --------------------------------*/
	{
		DMA_InitTypeDef dma;
		
		DMA_DeInit(DMA1_Stream1);
		DMA_StructInit(&dma);
		dma.DMA_Channel = DMA_Channel_4;
		dma.DMA_PeripheralBaseAddr = (uint32_t)(&USART3->DR);
		dma.DMA_Memory0BaseAddr = (uint32_t)&ChariotRecognition_data[0][0];        //����DMA���ڴ��Ŀ��λ�ã���DMA����Ҫ��ȡ����д���λ��
		dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
		dma.DMA_BufferSize = sizeof(ChariotRecognition_data)/2;                    //���鳤��
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
		
		//����Memory1,Memory0�ǵ�һ��ʹ�õ�Memory
		DMA_DoubleBufferModeConfig(DMA1_Stream1, (uint32_t)&ChariotRecognition_data[1][0], DMA_Memory_0);   //first used memory configuration
		DMA_DoubleBufferModeCmd(DMA1_Stream1, ENABLE);
		
		DMA_Cmd(DMA1_Stream1, ENABLE);
	}
	/* -------------- Configure NVIC ----------------------------------------*/
	{
		NVIC_InitTypeDef nvic;
		//����1 NVIC ����
		nvic.NVIC_IRQChannel = USART3_IRQn;//����1�ж�ͨ��
		nvic.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�0
		nvic.NVIC_IRQChannelSubPriority =2;		//�����ȼ�0
		nvic.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&nvic);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
	}
}

////////////////////////usart2 DMA send,
/*1���ͽ���ʹ���˲�ͬ��DMAͨ����������Ҫ�����䣬�ο������ֲᣬ�ҵ������Ӧӳ�����������ͨ��
2�� 
*/

void USART3_DMA_TX_config()
{


}
///////��ͬ������ĳ�ʼ���������������ֻ������Ҫ������ʱ��ŵ���//
void DMA_TX_cmd(DMA_Stream_TypeDef *DMAx_Streamx,u16 datasize)
{

}


void USART3_IRQHandler(void)  	//����1�жϷ������
{
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)      //���յ�����
	{
//		//clear the idle pending flag ������ô�����־
		(void)USART3->SR;
		(void)USART3->DR;

		//Target is Memory0Ŀ�����ڴ�0
		if(DMA_GetCurrentMemoryTarget(DMA1_Stream1) == 0)
		{
			DMA_Cmd(DMA1_Stream1, DISABLE);
			usart3_this_time_rx_len = ChariotRecognition_data_dma_buf_len - DMA_GetCurrDataCounter(DMA1_Stream1);
			DMA1_Stream1->NDTR = (uint16_t)ChariotRecognition_data_dma_buf_len;     //relocate the dma memory pointer to the beginning position��dma�ڴ�ָ���Ƶ�ʼ��λ��
			DMA1_Stream1->CR |= (uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 1���õ�ǰѡ���ڴ����ڴ�1
			DMA_Cmd(DMA1_Stream1, ENABLE);
      if(usart3_this_time_rx_len == ChariotRecognition_data_len)
			{
				ChariotRecognition_Mes_Process(ChariotRecognition_data[0]);
			}
		}
		else //Target is Memory1Ŀ�����ڴ�1
		{
			DMA_Cmd(DMA1_Stream1, DISABLE);
			usart3_this_time_rx_len = ChariotRecognition_data_dma_buf_len - DMA_GetCurrDataCounter(DMA1_Stream1);
			DMA1_Stream1->NDTR = (uint16_t)ChariotRecognition_data_dma_buf_len;      //relocate the dma memory pointer to the beginning position��dma�ڴ�ָ���Ƶ�ʼ��λ
			DMA1_Stream1->CR &= ~(uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 0���õ�ǰѡ���ڴ����ڴ�1
			DMA_Cmd(DMA1_Stream1, ENABLE);
      if(usart3_this_time_rx_len == ChariotRecognition_data_len)
			{
				ChariotRecognition_Mes_Process(ChariotRecognition_data[1]);
			}
		}
	}
} 


/////////////////////////////////////////
State_distance state_distacne=closedistance;


void Sendtosightway(int value)//���Ӿ��̶�
{
	//����������ֵ��ȡ��С�����һλ����̧��ʮ��������λ�����0-9����һ���ַ���ʽ���ͣ�������ֵ������+48��0��ASCII�룩
	int GMYawtemp_Eular=(Angles+180)*10;
	int GMPitchtemp_Encoder=(GMPitchEncoder.ecd_angle+90+pitch_err)*10;

//	int GMPitchtemp_Encoder=(Eular[0]+90)*10;
	int rolltly = (Eular[1]+90)*10;
	
//�в���ϵͳ�����ߵ�ʱ��
  if(robotState.robot_id>=1&&robotState.robot_id<=9)//�жϵз��ҷ� ��1-9Ϊred��101-109Ϊblue
	{
	Sendtosight[0]='R';	
	}
	if(robotState.robot_id>=101&&robotState.robot_id<=109)
	{
	Sendtosight[0]='B';	
	}
	
//	colorflag=PCin(6);
////û�е�ʱ�����ÿ���
//	if( PCin(6) == 1 )
//	{
//		Sendtosight[0] ='B';	
//	}
//	if( PCin(6) == 0)
//	{
//		Sendtosight[0]='R';	
//	}

//up to down������
//	if(utm123[0] == 0)
//	{
//	Sendtosight[0] ='B';//��Ӫ
//	}
//	if(utm123[0] == 1)
//	{
//	Sendtosight[0] ='R';
//	}
//	
	
	Sendtosight[1]='M';//��ʼ����
	Sendtosight[2]='N';
	Sendtosight[3]='L';
	
	Sendtosight[4]=(uint8_t)(GMYawtemp_Eular/1000+48);			 //ǧλ
	Sendtosight[5]=(uint8_t)(GMYawtemp_Eular%1000/100+48);	 //��λ
	Sendtosight[6]=(uint8_t)(GMYawtemp_Eular%100/10+48);		 //ʮλ
	Sendtosight[7]=(uint8_t)(GMYawtemp_Eular%10+48);				 //��λ
	
	Sendtosight[8]=(uint8_t)(GMPitchtemp_Encoder/1000+48);		//ǧλ
	Sendtosight[9]=(uint8_t)(GMPitchtemp_Encoder%1000/100+48);//��λ
	Sendtosight[10]=(uint8_t)(GMPitchtemp_Encoder%100/10+48);	//ʮλ
	Sendtosight[11]=(uint8_t)(GMPitchtemp_Encoder%10+48);			//��λ

	Sendtosight[12]=(uint8_t)(rolltly/1000+48);		//ǧλ
	Sendtosight[13]=(uint8_t)(rolltly%1000/100+48);//��λ
	Sendtosight[14]=(uint8_t)(rolltly%100/10+48);	//ʮλ
	Sendtosight[15]=(uint8_t)(rolltly%10+48);			//��λ

  while(count_Sendtosight<value)
  {
	  USART_SendData(USART3, Sendtosight[count_Sendtosight]);
	  count_Sendtosight++;
	  delay_us(150);
  }
 
  {
		count_Sendtosight=0;
  }
}

/***
������ChariotRecognition_Mes_Process(p)
���ܣ��������ݴ���
��ע����
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
	
	if(p[0]=='R'&&p[1]=='M'&&p[2]!='A')    //��ʶ��
	{
	 E_TEST3++;
	 E_TEST=ChariotRecognitionTemp[0];
	 E_TEST1 =ChariotRecognitionTemp[1];	
		
  	//UART1�յ�����ʱ������ͷ����Ŀ���־��Ϊ1��
	  CameraDetectTarget_Flag =1;	
		CR_ringBuffer.lost_COUNT =0;	
		
	  if(RC_CtrlData.rc.s1==3)//  if(RC_CtrlData.rc.s1==3 && gameState.game_progress == 4��Ϸ��ʼ����ʶ�𵽣�������
	  {
		  TempShootingFlag=1;//������־λ
	  }
		
		usart3_microsecond.time_now = Get_Time_Micros();//�����յ����ݵ�ʱ��
		usart3_microsecond.time_error = usart3_microsecond.time_now - usart3_microsecond.time_last;//�����������ε�ʱ��
		
		enter_CNT++;
		if(enter_CNT == 1)
		{
			YawCurrentPositionSave = GMYawEncoder.ecd_angle;
			PitchCurrentPositionSave = GMPitchEncoder.ecd_angle;
		}
		
		if(enter_CNT >30) enter_CNT = 30;
		
		//ֹͣ��׼�������ԺͶ��״̬��ʶ�����ʶ��֮���Ϊ���ģʽ���������Ӿ����������ݣ�����̨����ʶ������ݣ�Ҫ��״̬���ֶ��ر�Ħ����
		if(GetWorkState() == STOP_STATE || GetWorkState() == PREPARE_STATE || GetWorkState() == Test_STATE|| GetWorkState() == Dodeg_STATE || GetWorkState() == Freedom_STATE)
		{
			ChariotRecognition_yaw = GMYawEncoder.ecd_angle;
			ChariotRecognition_pitch= GMPitchEncoder.ecd_angle;
		}
		else  //ʶ�𡢾�Ѳ�ߺͶ��״̬
		{
			ChariotRecognition_yaw  = ChariotRecognitionTemp[0]/100.0 ;//���ո�����  // GMYawEncoder.ecd_angle + ChariotRecognitionTemp[0]/100.0 

//			if(ChariotRecognition_yaw==0)//�˵��Ӿ�����������0��0���������Ϳ��Ա���ƽ��
//			{
//				ChariotRecognition_yaw = last_ChariotRecognition_yaw;
//			}
			
			if(CR_ringBuffer.lost_COUNT<35)  
			{
				CR_ringBuffer.ringBuf[CR_ringBuffer.tailPosition++] = ChariotRecognition_yaw;//yaw�Ƕ�����
				if(CR_ringBuffer.tailPosition >= BUFFER_SIZE) CR_ringBuffer.tailPosition = 0;//�γɻ��ζ���
				CR_ringBuffer.lost_COUNT = 0;//��ʧĿ���������
			}
			else //����ܶ��ûʶ�𵽣�ͻȻʶ��һ�Σ��������ζ����д洢��ֵ���������ζ������㣬βָ������
			{
				for(u8 loop_i = 0;loop_i<5;loop_i++)
				{
				  CR_ringBuffer.ringBuf[loop_i] = 0;
				}
				CR_ringBuffer.tailPosition = 0;
				CR_ringBuffer.lost_COUNT = 0;
			}
			
			Distance_buf[Dis_Buf_Index++] = ChariotRecognitionTemp[2];
			if(Dis_Buf_Index >= Dis_buf_Size) Dis_Buf_Index = 0;  //����ѭ��
			Target_Distance = filter(Distance_buf);

			// Eular[0]+
			ChariotRecognition_pitch = ChariotRecognitionTemp[1]/100.0;//(0.0088*Target_Distance+1.9604);//GMPitchEncoder.ecd_angle + ChariotRecognitionTemp[1]/100.0;

			
			last_ChariotRecognition_yaw   = ChariotRecognition_yaw;
			last_ChariotRecognition_pitch = ChariotRecognition_pitch;
		}
		usart3_microsecond.time_last = Get_Time_Micros();//��¼��һ���յ����ݵ�ʱ��
	}
	
	if(p[0]=='R'&&p[1]=='M'&&p[2]=='A'&&p[3]=='A'&&p[4]=='A')//ʶ�𲻵�
	{
		
			E_TEST=0;
      E_TEST1=0;
		
		CR_ringBuffer.lost_COUNT++;
		
		if(CR_ringBuffer.lost_COUNT>=35)//������������һ��Ҫȫ�ģ������׿���ʶ��ģʽ��
		{
			CR_ringBuffer.lost_COUNT = 35;
		}
		
		if(CR_ringBuffer.lost_COUNT<=35)//������ʧĿ��Ĵ���С��35�Σ�����Ԥ��
		{

			ChariotRecognition_yaw = Angles;//GMYawEncoder.ecd_angle;
		//	ChariotRecognition_pitch = GMPitchEncoder.ecd_angle;			
   	}	
	}
	
	if(CR_ringBuffer.lost_COUNT>=35)
	{
		enter_CNT = 0;
		ChariotRecognition_yaw = Angles;
		ChariotRecognition_pitch = GMPitchEncoder.ecd_angle;
		TempShootingFlag = 0;
		CameraDetectTarget_Flag = 0;//�������?֡ûʶ�𵽣���״̬
	}
	
		if(  RC_CtrlData.rc.s1== 3) //&& gameState.game_progress == 4��Ϸ��ʼ
	{
		friction_wheel_state_flag = 1; //��Ϸ��ʼ��s1 �ŵ��м䣬Ħ���־Ϳ�
	}

	if(GetWorkState()== ChariotRecognition_STATE && RC_CtrlData.rc.s1==2)//ʶ��״̬�£����󲦸˷ŵ��������޷��ر�Ħ����  //5/2
	{
		TempShootingFlag = 0;//�ز���
		friction_wheel_state_flag = 0;//��Ħ����
	}

}


uint16_t filter(uint16_t *distance_buf)//�˲���ʵ�Ӿ�����
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
