//ע���˲�������,������̨

#include "ControlTask.h"
#include "Phototube.h"
#include "timer.h"
#include "SpeedTask.h"
#include "WorkState.h"
#include "Freedom_Status.h"
#include "Doget_state.h"
#include "ShootingTask.h"

uint32_t time_tick_2ms = 0;     //2msˢ��һ�Σ���ֵΪ0
u16 adc_Value = 0;

/***
������Control_Task()
���ܣ����ƻ���2msˢ��һ��
��ע����
***/
void Control_Task(void)
{
	time_tick_2ms += 2;								//��¼����ʱ��
	WorkStateFSM();										//����״̬ѡ��
	WorkStateSwitchProcess();					//��⹤��״̬�л�
	
	/***��̨��������***/
	YawFreeRoation();                 //����״̬ʱ����̨������ת
  YawFreeRoation_Doget();           //���״̬ʱ����̨������ת
	GMYawControlLoop();								//����Y���������,0x205
	GMPitchControlLoop();							//����P���������,0x206
	
	
	/***������������***/
	HeatControl_Task();
  FrictionWheelControl();             //Ħ���ֲ��������������
	BoLunMotorControl();
	ShooterMControlLoop();      		  //���������������,0x207
	SetGimbalMotorOutput();						//����̨�����֡�ɲ��������������
	
	
  /***���̿�������***/
  Chassis_Motion_Switch();      //����״̬�������˶����߽�󣬻���(����������)
  Chassis_Motion_Switch_Doget();//���״̬�������˶����߽�󣬻���
	GraduallyChangeCMSpeed();     //���̵���ٶ�ƽ������
	CMControlLoop();							//���̿������� ���͵��̵�������
  Phototube_Handle();           //���ܿ�������	
	


		
	/***ң��������ģʽ�л�***/
	Remote_microsecond.time_now = Get_Time_Micros();
	Remote_microsecond.time_error = Remote_microsecond.time_now - Remote_microsecond.time_last;
	if(Remote_microsecond.time_error>500000) RemoteTest_Flag = 0;
		
}

/***
������ControtLoopTaskInit()
���ܣ����ƻ���ʼ��
��ע����
***/
void ControtLoopTaskInit(void)
{
	//������ʼ��
	time_tick_2ms = 0;   
	
	//5�����PID��ʼ��
	//P��Y��λ�á��ٶ�pid��ʼ��
	CM1SpeedPID.Reset(&CM1SpeedPID);
	CM2SpeedPID.Reset(&CM2SpeedPID);//û��
	CM3SpeedPID.Reset(&CM3SpeedPID);//   
	CM4SpeedPID.Reset(&CM4SpeedPID);
	
	GMPPositionPID.Reset(&GMPPositionPID);
	GMPSpeedPID.Reset(&GMPSpeedPID);
	GMYPositionPID.Reset(&GMYPositionPID);
	GMYSpeedPID.Reset(&GMYSpeedPID);
	
	CM7PositionPID.Reset(&CM7PositionPID);
	CM7SpeedPID.Reset(&CM7SpeedPID);
	
	CMRamp.SetScale(&CMRamp,400);
	CMRamp.ResetCounter(&CMRamp);
	
	//P,Y����б�³�ʼ��
	GMPitchRamp.SetScale(&GMPitchRamp, PREPARE_TIME_TICK_MS); //�趨б�²���
	GMYawRamp.SetScale(&GMYawRamp, PREPARE_TIME_TICK_MS);
	GMPitchRamp.ResetCounter(&GMPitchRamp);                   //б�¼���ֵ����
	GMYawRamp.ResetCounter(&GMYawRamp);
	
	//��̨����ֵ��ʼ��
	/***
	  �״���̨�ο�ֵ��ʼ��ʱ��pitch��yawΪ0
	  ң�������Խ����ٴγ�ʼ��ʱ��pitchΪ0,yawΪ��ǰ�������ֵ����֤������״̬ʱ��yaw�ӵ�ǰλ����ת
	***/
	Chassis_Speed_Ref = 0;//�����ٶȳ�ʼ��
	GimbalRef.pitch_angle_dynamic_ref = 0.0f;
	GimbalRef.yaw_angle_dynamic_ref = YawCurrentPositionSave;
	
	//Ħ����б�³�ʼ��
//	frictionRamp.SetScale(&frictionRamp, FRICTION_RAMP_TICK_COUNT);
//	frictionRamp.ResetCounter(&frictionRamp);
//	//Ħ��������״̬��ʼ��
//	InitFrictionWheel();
}

//GPIO��ʼ�����ɿص�Դ
void engineerpower_Init()
{		GPIO_InitTypeDef gpio;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE); 
		
		gpio.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_2;//2��3��4��5�ܽ�
		gpio.GPIO_Mode = GPIO_Mode_OUT;//���ģʽ
		gpio.GPIO_OType = GPIO_OType_PP;//����
		gpio.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
		gpio.GPIO_PuPd = GPIO_PuPd_UP;//��ʼ����
		GPIO_Init(GPIOH, &gpio);
	
	GPIO_SetBits(GPIOH,GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_2);
}

