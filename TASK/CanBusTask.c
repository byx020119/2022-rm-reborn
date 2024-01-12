#include "main.h"
#include "encoder.h"

uint32_t can_count = 0;
int16_t  pitch_ecd_bias = 600;//200
int16_t  yaw_ecd_bias   = 0;
int16_t  brake_ecd_bias = 0;//�м��ٱȣ�ÿ��ˢ�µ��Ȧ�����㣬�޷�ͨ������ֵ���������ֶ�

//�ؼ���volatile:���ѱ�����������������ı�����ʱ���п��ܸı䣬
//��˱����ĳ���ÿ����Ҫ�洢���ȡ���������ʱ�򣬶���ֱ�Ӵӱ�����ַ�ж�ȡ���ݡ�
//���û��volatile�ؼ��֣�������������Ż���ȡ�ʹ洢��������ʱʹ�üĴ����е�ֵ�������������ɱ�ĳ�����µĻ��������ֲ�һ������

volatile Encoder CM1Encoder			= {0,0,0,0,0,0,0,{0,0,0,0,0,0},0,0,0};
volatile Encoder CM2Encoder			= {0,0,0,0,0,0,0,{0,0,0,0,0,0},0,0,0};
volatile Encoder CM3Encoder			= {0,0,0,0,0,0,0,{0,0,0,0,0,0},0,0,0};
volatile Encoder CM4Encoder			= {0,0,0,0,0,0,0,{0,0,0,0,0,0},0,0,0};
volatile Encoder GMYawEncoder 	= {0,0,0,0,0,0,0,{0,0,0,0,0,0},0,0,0};
volatile Encoder GMPitchEncoder = {0,0,0,0,0,0,0,{0,0,0,0,0,0},0,0,0};
volatile Encoder CM6Encoder 		= {0,0,0,0,0,0,0,{0,0,0,0,0,0},0,0,0};//2022��
volatile Encoder CM7Encoder 		= {0,0,0,0,0,0,0,{0,0,0,0,0,0},0,0,0};


uint16_t Shooter_17_Heat = 0; //17mm����ǹ������
uint16_t lastRobotHP     = 600; //��������һ��Ѫ��ֵ
uint16_t RobotHP         = 0; //������Ѫ��ֵ
uint16_t Hurt_COUNT      = 0; //װ�ױ���������
uint8_t Amor_ID          = 0; //������װ��ID

void CanReceiveMsgProcess(CanRxMsg * msg)
{
    can_count++;
		switch(msg->StdId)
		{
			case CAN_BUS1_MOTOR1_FEEDBACK_MSG_ID:
			{
				 Chassis_Power_On_Flag = 1;
				(can_count<=50) ? GetEncoderBias(&CM1Encoder ,msg):Motor_3508_EncoderProcess(&CM1Encoder ,msg);       //��ȡ���������ĳ�ʼƫ��ֵ    
			}break;
			case CAN_BUS1_MOTOR2_FEEDBACK_MSG_ID:
			{
				 Chassis_Power_On_Flag = 1;
				(can_count<=50) ? GetEncoderBias(&CM2Encoder ,msg):Motor_3508_EncoderProcess(&CM2Encoder ,msg);   //������
			}break;
			case CAN_BUS1_MOTOR3_FEEDBACK_MSG_ID:
			{  
				Chassis_Power_On_Flag = 1;
				(can_count<=50) ? GetEncoderBias(&CM3Encoder ,msg):Motor_3508_EncoderProcess(&CM3Encoder ,msg);       //��ȡ���������ĳ�ʼƫ��ֵ    
			}break;
			case CAN_BUS1_MOTOR4_FEEDBACK_MSG_ID:
			{
				 Chassis_Power_On_Flag = 1;
				(can_count<=50) ? GetEncoderBias(&CM4Encoder ,msg):Motor_3508_EncoderProcess(&CM4Encoder ,msg);
			}break;
			
			case 0x30:       //���������ݴ���
		  {
				
		  }break;
			
			case 0x31:       //�м�������ݴ���
		  {
				
		  }break;
			
			case 0x32:       //�Ҳ�������ݴ���
		  {
				
		  }break;
			
			case 0x40:       //CAN����17mmǹ����������
		  {
				
		  }break;
			
			case 0x50:       //CAN���ջ�����Ѫ������
		  {
				
		  }break;
			
			case 0x60:       //CAN���ջ������˺�����
		  {

		  }break;
			
			case 0x70:     //CAN���յ�����������
			{
				
			}break;
			//y����6020 ID��7
			
			case CAN_BUS1_MOTOR7_FEEDBACK_MSG_ID:
			{
				//y�᲻Ҫ����ֵ����ʼ���ľ�����Ϊ��ֵ
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
//			p��6020 ID��5
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
			/*case CAN_BUS1_MOTOR7_FEEDBACK_MSG_ID://���ֵ��
			{
				(can_count<=50) ? GetEncoderBias(&CM7Encoder ,msg):Motor_2310_EncoderProcess(&CM7Encoder ,msg);
			}break;*/
			default:
			{
			}
	}
}

//can2���ֵ��IDͬY��207����д�˺���
void CanReceiveMsgProcess1(CanRxMsg * msg)
{
    can_count++;
	switch(msg->StdId)
	{
		case CAN_BUS1_MOTOR7_FEEDBACK_MSG_ID://���ֵ�� ID 7
		{
			(can_count<=50) ? GetEncoderBias(&CM7Encoder ,msg):Motor_2310_EncoderProcess(&CM7Encoder ,msg);
			break;
		}
	  case  CAN_BUS1_MOTOR6_FEEDBACK_MSG_ID://ɲ������ 2022�� ID 6
			{
				if(can_count<50)
				{
					GetEncoderBias(&CM6Encoder ,msg);
					if(CM6Encoder.ecd_bias-brake_ecd_bias>4096)
						brake_ecd_bias+=8192;
					else if(CM6Encoder.ecd_bias-brake_ecd_bias<-4096)
						brake_ecd_bias-=8192;
				}
				else
				{
					CM6Encoder.ecd_bias = brake_ecd_bias;
					Motor_2310_EncoderProcess(&CM6Encoder ,msg);
				}
			}break;
		case 0x100://���ͨ��,��ַ����,����Ҫ�뷢���Ǹ���stdidһ��
		{
			Up_to_Down_Flag(msg);//����֮������ݵĴ���
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


void Set_Gimbal_Current(CAN_TypeDef *CANx, int16_t gimbal_yaw_iq,int16_t gimbal_pitch_iq)
{
    CanTxMsg tx_message;    
    tx_message.StdId = 0x1FF;
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;
    
    tx_message.Data[0] = (unsigned char)(gimbal_pitch_iq >> 8);
    tx_message.Data[1] = (unsigned char)gimbal_pitch_iq;
    tx_message.Data[2] = 0xff;
    tx_message.Data[3] = 0xff;
    tx_message.Data[4] = (unsigned char)(gimbal_yaw_iq >> 8);//ԭ����(unsigned char)(cm7_iq >> 8);
    tx_message.Data[5] = (unsigned char)gimbal_yaw_iq;//ԭ����(unsigned char)cm7_iq;
    tx_message.Data[6] = 0xff;
    tx_message.Data[7] = 0xff;
    CAN_Transmit(CANx,&tx_message);
}
void Set_Gimbal_Current1(CAN_TypeDef *CANx,int16_t cm6_iq,int16_t cm7_iq)//can2���ֿ���
{
    CanTxMsg tx_message;    
    tx_message.StdId = 0x1FF;
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;
    
    tx_message.Data[0] = 0xff;
    tx_message.Data[1] = 0xff;
    tx_message.Data[2] = (unsigned char)(cm6_iq >> 8);//2022��
    tx_message.Data[3] = (unsigned char)cm6_iq;//2022��
    tx_message.Data[4] = (unsigned char)(cm7_iq >> 8);
    tx_message.Data[5] = (unsigned char)cm7_iq;
    tx_message.Data[6] = 0xff;
    tx_message.Data[7] = 0xff;
    CAN_Transmit(CANx,&tx_message);
}
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
//2022�� ������ݽ������� Down to Up  ����λ����ʱδ���ţ���ʱ�����ݷ��� �����ɸ���������չ
void Send_Down_to_Up_Message(CAN_TypeDef *CANx)
{
    CanTxMsg tx_message;    
    tx_message.StdId = 0x90;
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;
    
    tx_message.Data[0] = 0x00;
    tx_message.Data[1] = 0x00;
    tx_message.Data[2] = 0x00;
    tx_message.Data[3] = 0x00;
    tx_message.Data[4] = 0x00;
    tx_message.Data[5] = 0x00;
    tx_message.Data[6] = 0x00;
    tx_message.Data[7] = 0x00;
    CAN_Transmit(CANx,&tx_message);
}
