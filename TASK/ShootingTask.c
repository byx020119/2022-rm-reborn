#include "main.h"


/************************************
Speed_Friction    Real_Speed(m/s)
800               24.8
900               26.5        
      
*************************************/

/*************************************
1s���䵯������     Vmax(m/s)
    6~10              7.2
    5                 14.4
    4                 14.4
    3                 21.6
    2                 36
    1                 72
*************************************/

/****************************************
420     25m/s      3
****************************************/

FrictionWheelState_e friction_wheel_state = FRICTION_WHEEL_OFF; //Ħ����״̬
FrictionWheelState_e friction_wheel_state_flag =0;                   //
uint32_t FrictionWheel_COUNT=0;                                 //Ħ����������ʱ
uint16_t Friction_Output_Speed = 350;                           //Ħ�����ٶ�,1000ֹͣ,2000���
RampGen_t frictionRamp = RAMP_GEN_DAFAULT;                      //Ħ����б��

Shoot_State_e shootState = NOSHOOTING; //�������״̬
uint8_t TempShootingFlag=0;                 //������־,�޸ĸñ�־����ѡ���Ƿ񷢵�

//Ħ�������ֲ�ͬ�ٶȽṹ���ʼ��
#define FRICTION_SPEED_INIT_DEFAULT \
{\
	300,\
	300,\
}\

//�ӵ�3�ֲ�ͬ���ٻ������ʼ��
#define BULLET_SPEED_REF_DEFAULT \
{\
	20,\
	25,\
}\

//�ӵ�3�ֲ�ͬ��Ƶ�ṹ���ʼ��
#define BULLET_FREQ_REF_DEFAULT \
{\
	40,\
	40,\
}\

//�ӵ�3�ֲ�ͬ�����µ����������ṹ���ʼ��
//6,3,3
#define Gravity_Add_Angle_DEFAULT \
{\
	5,\
	2,\
}\


Friction_Speed_t Friction_Speed       = FRICTION_SPEED_INIT_DEFAULT;  //Ħ����3�ֲ�ͬ�ٶȽṹ��
Bullet_Speed_Ref_t Bullet_Speed_Ref   = BULLET_SPEED_REF_DEFAULT;     //�ӵ�3�ֲ�ͬ���ٽṹ��
Bullet_Freq_Ref_t Bullet_Freq_Ref     = BULLET_FREQ_REF_DEFAULT;      //�ӵ�3�в�ͬ��Ƶ�ṹ��
uint16_t Bullet_17_real_Speed         = 0;                            //17mm����ʵ������
u8 Bullet_Ouput_Freq                  = 0;
Gravity_Add_Angle_t Gravity_Add_Angle = Gravity_Add_Angle_DEFAULT;    //���ֵ��������µ���������ֵ


/***
������FrictionWheelControl()
���ܣ�Ħ���ֵ����������
***/
void FrictionWheelControl(void)
{
if(friction_wheel_state_flag ==1)
	{
//			if(Target_Distance<=30)
//				{
				CM3SpeedPID.ref =  -Friction_Speed.low_speed ;
			  CM4SpeedPID.ref = Friction_Speed.low_speed ;
//				}
//			else
//			  {
//				CM3SpeedPID.ref = -Friction_Speed.high_speed ;
//			  CM4SpeedPID.ref = Friction_Speed.high_speed ;
//					
//			  }	
	}
	
if(friction_wheel_state_flag ==0)
	{
				CM3SpeedPID.ref = 0;//���̳���
  			CM4SpeedPID.ref = 0 ;
	}

}

/***
������BoLunMotorControl()
���ܣ����ֵ����������
***/
void BoLunMotorControl(void)
{
	
	/***
	1.ֻ��ʶ��״̬��ң��������״̬���Է���
	2.ǹ������С��400�ſ��Է���
	***/
	if(GetWorkState() == ChariotRecognition_STATE || GetWorkState() == Test_STATE||GetWorkState() == Dodeg_STATE)  
	{
		if(Shooter_17_Heat<400)
		{
			if(TempShootingFlag==1)//�򿪲��ַ���
			{
				shot_frequency_limt.time_now = Get_Time_Micros();//TIM2->CNT
				shot_frequency_limt.time_error = shot_frequency_limt.time_now - shot_frequency_limt.time_last;
				if(shot_frequency_limt.time_error > (int)(650000/Bullet_Ouput_Freq))
				{
					count_temp++;
					shot_frequency_limt.time_last = Get_Time_Micros();
				}	
			}
	  }
  }
}


/***
������HeatControl_Task()
���ܣ�ǹ�������ջ�
��ע��Զ����ʱ�����ٶ�СƵ��(>=4)
      �о���ʱ�����ٶ���Ƶ��(2-4)
      ������ʱ��С�ٶȴ�Ƶ��(<=2)
      Target_DistanceΪ�Ӿ����صľ��룬Bullet_17_real_Speed�ǴӲ���ϵͳ�����ĵ�������
***/

void HeatControl_Task(void)
{
//	if(Target_Distance<=30)         //������
//	{
		Bullet_Ouput_Freq     = Bullet_Freq_Ref.low_speed;
//		Pitch_Add_Angle       = Gravity_Add_Angle.low_speed;
//	}
//	else                           //Զ����
//	{
//		Bullet_Ouput_Freq     = Bullet_Freq_Ref.high_speed;
//		Pitch_Add_Angle       = Gravity_Add_Angle.high_speed;
//	}
//	if(Shooter_17_Heat>300) 
//	{
//		Bullet_Ouput_Freq = Bullet_Ouput_Freq-2;
//		Bullet_Ouput_Freq = Bullet_Ouput_Freq<=1 ? 1 : Bullet_Ouput_Freq;
//	}
}

