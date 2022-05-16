#include "main.h"
#include "encoder.h"

uint32_t can_count = 0;
int16_t  pitch_ecd_bias = -3000;//600
int16_t  yaw_ecd_bias   = 0;
int16_t  brake_ecd_bias = 0;//有减速比，每次刷新电机圈数清零，无法通过给定值回正，须手动

//关键字volatile:提醒编译器它后面所定义的变量随时都有可能改变，
//因此编译后的程序每次需要存储或读取这个变量的时候，都会直接从变量地址中读取数据。
//如果没有volatile关键字，则编译器可能优化读取和存储，可能暂时使用寄存器中的值，如果这个变量由别的程序更新的话，将出现不一致现象。

volatile Encoder CM1Encoder			= {0,0,0,0,0,0,0,{0,0,0,0,0,0},0,0,0};
volatile Encoder CM2Encoder			= {0,0,0,0,0,0,0,{0,0,0,0,0,0},0,0,0};
volatile Encoder CM3Encoder			= {0,0,0,0,0,0,0,{0,0,0,0,0,0},0,0,0};
volatile Encoder CM4Encoder			= {0,0,0,0,0,0,0,{0,0,0,0,0,0},0,0,0};
volatile Encoder GMYawEncoder 	= {0,0,0,0,0,0,0,{0,0,0,0,0,0},0,0,0};
volatile Encoder GMPitchEncoder = {0,0,0,0,0,0,0,{0,0,0,0,0,0},0,0,0};
volatile Encoder CM6Encoder 		= {0,0,0,0,0,0,0,{0,0,0,0,0,0},0,0,0};//2022加
volatile Encoder CM7Encoder 		= {0,0,0,0,0,0,0,{0,0,0,0,0,0},0,0,0};


uint16_t Shooter_17_Heat = 0; //17mm弹丸枪口热量
uint16_t lastRobotHP     = 600; //机器人上一次血量值
uint16_t RobotHP         = 0; //机器人血量值
uint16_t Hurt_COUNT      = 0; //装甲被攻击次数
uint8_t Amor_ID          = 0; //被攻击装甲ID

void CanReceiveMsgProcess(CanRxMsg * msg)
{
    can_count++;
		switch(msg->StdId)
		{
			case CAN_BUS1_MOTOR1_FEEDBACK_MSG_ID:
			{
				 Chassis_Power_On_Flag = 1;
				(can_count<=50) ? GetEncoderBias(&CM1Encoder ,msg):Motor_3508_EncoderProcess(&CM1Encoder ,msg);       //获取到编码器的初始偏差值    
			}break;
			case CAN_BUS1_MOTOR2_FEEDBACK_MSG_ID:
			{
				 Chassis_Power_On_Flag = 1;
				(can_count<=50) ? GetEncoderBias(&CM2Encoder ,msg):Motor_3508_EncoderProcess(&CM2Encoder ,msg);   //测试用
			}break;
			case CAN_BUS1_MOTOR3_FEEDBACK_MSG_ID:
			{  
				Chassis_Power_On_Flag = 1;
				(can_count<=50) ? GetEncoderBias(&CM3Encoder ,msg):Motor_3508_EncoderProcess(&CM3Encoder ,msg);       //获取到编码器的初始偏差值    
			}break;
			case CAN_BUS1_MOTOR4_FEEDBACK_MSG_ID:
			{
				 Chassis_Power_On_Flag = 1;
				(can_count<=50) ? GetEncoderBias(&CM4Encoder ,msg):Motor_3508_EncoderProcess(&CM4Encoder ,msg);
			}break;
			
			case 0x30:       //左侧光电管数据处理
		  {
				
		  }break;
			
			case 0x31:       //中间光电管数据处理
		  {
				
		  }break;
			
			case 0x32:       //右侧光电管数据处理
		  {
				
		  }break;
			
			case 0x40:       //CAN接收17mm枪口热量数据
		  {
				
		  }break;
			
			case 0x50:       //CAN接收机器人血量数据
		  {
				
		  }break;
			
			case 0x60:       //CAN接收机器人伤害数据
		  {

		  }break;
			
			case 0x70:     //CAN接收弹丸射速数据
			{
				
			}break;
			//y轴电机6020 ID：7
			
			case CAN_BUS1_MOTOR7_FEEDBACK_MSG_ID:
			{
				//y轴不要调初值，初始在哪就以哪为初值
//				(can_count<=50)?GetEncoderBias(&GMYawEncoder ,msg):Motor_3508_EncoderProcess(&GMYawEncoder ,msg);
				if(can_count<50)
				{
					GetEncoderBias(&GMYawEncoder ,msg);
					if(GMYawEncoder.ecd_bias-yaw_ecd_bias>4096)
						yaw_ecd_bias+=8192;
					else if(GMYawEncoder.ecd_bias-yaw_ecd_bias<-4096)
						yaw_ecd_bias-=8192;
				}
				else 
				{
					GMYawEncoder.ecd_bias = yaw_ecd_bias;
					Motor_6020_EncoderProcess(&GMYawEncoder ,msg);
				}
			}break;
//			p轴6020 ID：5
			case CAN_BUS1_MOTOR5_FEEDBACK_MSG_ID:
			{
//				(can_count<=50)?GetEncoderBias(&GMPitchEncoder ,msg):Motor_6623_EncoderProcess(&GMPitchEncoder ,msg);
				if(can_count<50)
				{
					GetEncoderBias(&GMPitchEncoder ,msg);
					if(GMPitchEncoder.ecd_bias-pitch_ecd_bias>4096)
						pitch_ecd_bias+=8192;
					else if(GMPitchEncoder.ecd_bias-pitch_ecd_bias<-4096)
						pitch_ecd_bias-=8192;
				}
				else
				{
					GMPitchEncoder.ecd_bias = pitch_ecd_bias;
					Motor_6020_EncoderProcess(&GMPitchEncoder ,msg);
				}
			}break;
			
			case CAN_BUS1_MOTOR6_FEEDBACK_MSG_ID://波轮电机 6
			{
				(can_count<=50) ? GetEncoderBias(&CM7Encoder ,msg):Motor_2310_EncoderProcess(&CM7Encoder ,msg);
			}break;
			
			default:
			{
			}
	}
}

//can2波轮电机ID同Y轴207，故写此函数
void CanReceiveMsgProcess1(CanRxMsg * msg)
{
    can_count++;
	switch(msg->StdId)
	{
//		case CAN_BUS1_MOTOR7_FEEDBACK_MSG_ID://波轮电机 ID 7
//		{
//			(can_count<=50) ? GetEncoderBias(&CM7Encoder ,msg):Motor_2310_EncoderProcess(&CM7Encoder ,msg);
//			break;
//		}
//	  case  CAN_BUS1_MOTOR6_FEEDBACK_MSG_ID://刹车机构 2022加 ID 6
//			{
//				if(can_count<50)
//				{
//					GetEncoderBias(&CM6Encoder ,msg);
//					if(CM6Encoder.ecd_bias-brake_ecd_bias>4096)
//						brake_ecd_bias+=8192;
//					else if(CM6Encoder.ecd_bias-brake_ecd_bias<-4096)
//						brake_ecd_bias-=8192;
//				}
//				else
//				{
//					CM6Encoder.ecd_bias = brake_ecd_bias;
//					Motor_2310_EncoderProcess(&CM6Encoder ,msg);
//				}
//			}break;
		case 0x90://板间通信,地址任意,但需要与发送是给定stdid一致
		{
			Down_to_Up_Flag(msg);//接受之后对数据的处理
		}break;
		default:
		{
		}				
	}
	
}

void Set_CM_Speed(CAN_TypeDef *CANx, int16_t cm1_iq, int16_t cm2_iq,int16_t cm3_iq, int16_t cm4_iq)
{
	  CanTxMsg tx_message;
    tx_message.StdId = 0x200;
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;
    
    tx_message.Data[0] = (uint8_t)(cm1_iq >> 8);
    tx_message.Data[1] = (uint8_t)cm1_iq;
    tx_message.Data[2] = (uint8_t)(cm2_iq >> 8);
    tx_message.Data[3] = (uint8_t)cm2_iq;
    tx_message.Data[4] = (uint8_t)(cm3_iq >> 8);
    tx_message.Data[5] = (uint8_t)cm3_iq;
    tx_message.Data[6] = (uint8_t)(cm4_iq >> 8);
    tx_message.Data[7] = (uint8_t)cm4_iq;
    CAN_Transmit(CANx,&tx_message);
}


void Set_Gimbal_Current(CAN_TypeDef *CANx, int16_t gimbal_yaw_iq ,int16_t cm7_iq , int16_t gimbal_pitch_iq)
{
    CanTxMsg tx_message;    
    tx_message.StdId = 0x1FF;
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;
    
    tx_message.Data[0] = (unsigned char)(gimbal_pitch_iq >> 8);
    tx_message.Data[1] = (unsigned char)gimbal_pitch_iq;
    tx_message.Data[2] = (unsigned char)(cm7_iq >> 8);
    tx_message.Data[3] = (unsigned char)cm7_iq;
    tx_message.Data[4] = (unsigned char)(gimbal_yaw_iq >> 8);//原波轮(unsigned char)(cm7_iq >> 8);
    tx_message.Data[5] = (unsigned char)gimbal_yaw_iq;//原波轮(unsigned char)cm7_iq;
    tx_message.Data[6] = 0xff;
    tx_message.Data[7] = 0xff;
    CAN_Transmit(CANx,&tx_message);
}
//void Set_Gimbal_Current1(CAN_TypeDef *CANx,int16_t cm7_iq)//can2波轮控制		int16_t cm6_iq,
//{
//    CanTxMsg tx_message;    
//    tx_message.StdId = 0x1FF;
//    tx_message.IDE = CAN_Id_Standard;
//    tx_message.RTR = CAN_RTR_Data;
//    tx_message.DLC = 0x08;
//    
//    tx_message.Data[0] = 0xff;
//    tx_message.Data[1] = 0xff;
//    tx_message.Data[2] = 0xff;//(unsigned char)(cm6_iq >> 8);//2022加
//    tx_message.Data[3] = 0xff;//(unsigned char)cm6_iq;//2022加
//    tx_message.Data[4] = (unsigned char)(cm7_iq >> 8);
//    tx_message.Data[5] = (unsigned char)cm7_iq;
//    tx_message.Data[6] = 0xff;
//    tx_message.Data[7] = 0xff;
//    CAN_Transmit(CANx,&tx_message);
//}
void Set_Gimbal_CALI_STATE(CAN_TypeDef *CANx)
{
    CanTxMsg tx_message;    
    tx_message.StdId = 0x3f0;
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;
    
    tx_message.Data[0] = 'c';
    tx_message.Data[1] = 0x00;
    tx_message.Data[2] = 0x00;
    tx_message.Data[3] = 0x00;
    tx_message.Data[4] = 0x00;
    tx_message.Data[5] = 0x00;
    tx_message.Data[6] = 0x00;
    tx_message.Data[7] = 0x00;
    CAN_Transmit(CANx,&tx_message);
}
//2022加 板间数据交换函数 Up to Down  引用位置暂时未安排，暂时无数据发送 参数可根据需求扩展
void Send_Up_to_Down_Message(CAN_TypeDef *CANx,int8_t color_iq, int8_t qianshao_iq ,int8_t recog_iq,int16_t danliang_iq,int8_t doget_iq)
{
    CanTxMsg tx_message;    
    tx_message.StdId = 0x100;
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;
    
    tx_message.Data[0] = (uint8_t)color_iq;
    tx_message.Data[1] = (uint8_t)qianshao_iq;
    tx_message.Data[2] = (uint8_t)recog_iq;
    tx_message.Data[3] = (uint8_t)(danliang_iq >> 8);
    tx_message.Data[4] = (uint8_t)(danliang_iq );
    tx_message.Data[5] = (uint8_t)doget_iq;
    tx_message.Data[6] = 0x00;
    tx_message.Data[7] = 0x00;
    CAN_Transmit(CANx,&tx_message);
}
