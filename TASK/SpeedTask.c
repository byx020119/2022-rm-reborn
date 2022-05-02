//ע����ս��ʶ��״̬����̨���ݵȺܶ࣬�ޱ���

#include "SpeedTask.h"
#include "WorkState.h"
#include "encoder.h"
#include "Phototube.h"
#include "Freedom_Status.h"
#include "common.h"
#include "math.h"
#include "ShootingTask.h"
#include "timer.h"
#include "usart3.h"
#include "usart6.h"

//��������PID
//PID�ṹ���ʼ��
PID_Regulator CMPositionPID  = CHASSIS_MOTOR_POSITION_PID_DEFAULT;
PID_Regulator CM1SpeedPID    = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator CM2SpeedPID    = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator CM3SpeedPID    = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator CM4SpeedPID    = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator Speed_Offset   = CHASSIS_SPEED_OFFSET_DEFAULT;
PID_Regulator GMPPositionPID = GIMBAL_MOTOR_PITCH_POSITION_PID_DEFAULT; 
PID_Regulator GMPPositionPID1 = GIMBAL_MOTOR_PITCH_POSITION_PID_DEFAULT;  
PID_Regulator GMPSpeedPID 	 = GIMBAL_MOTOR_PITCH_SPEED_PID_DEFAULT;
PID_Regulator GMYPositionPID = GIMBAL_MOTOR_YAW_POSITION_PID_DEFAULT;			
PID_Regulator GMYSpeedPID 	 = GIMBAL_MOTOR_YAW_SPEED_PID_DEFAULT;

PID_Regulator CM7PositionPID = CM7_POSITION_PID_DEFAULT;
PID_Regulator CM7SpeedPID    = CM7_SPEED_PID_DEFAULT;

//pitch,yawб�½ṹ���ʼ��
RampGen_t GMPitchRamp = RAMP_GEN_DAFAULT;
RampGen_t GMYawRamp   = RAMP_GEN_DAFAULT;
RampGen_t CMRamp      = RAMP_GEN_DAFAULT;

Gimbal_Ref_t GimbalRef;             //����˫�����Ʋο�ֵ�ṹ�壨��̨��ɲ����ɲ����Ϊ�������Ƕȿ��ƣ�ֻ������־λ����������ʱδʹ�����ֵ
CM2_Ref_t CM2Ref;                   //���嵥�����Ʋο�ֵ�ṹ�壨CM2��
int Brake_flag=0;                   //����ɲ����־λ 1Ϊˮƽʱ

float CMSpeedRate =1.0f;            //���̵���ٶ�ϵ��
float Yaw_FeedForward = 0.0;        //yawʶ�𲹳�ֵ
float Chassis_Position_Ref = 0;     //����λ��Ŀ��ֵ
float last_Chassis_Temp_Speed = 0;  //��һ�ε�����ʱ�ٶ�
float Chassis_Temp_Speed      = 0;  //���ε�����ʱ�ٶ�
float last_Chassis_Temp_Speed_Save = 0;
float Chassis_Temp_Speed_Save = 0;
float Chassis_Speed_Ref       = 0;  //����ʵ���ٶȲο�ֵ
u8 pitch_POS_Flag = 0;
float  test1=0;
float  GMY_Previous_Output=0;
float  GMY_Previous_Position_ref=0;
float  GMY_Previous_Position_fdb=0;
float  CR_yaw_Symbol=0;
float  CR_yaw_increment=0;
float  CR_Pitch_Symbol=0;
float  CR_Pitch_increment=0;
int    Dodeg_STATE_Change = 0;
int    Last_Dodeg_STATE_Change=0;
float  sbyaw=0;
/***
������GMBrakeControlLoop()
���ܣ�����PID�����brake����������
��ע����������
***/

	 
/***
������GMPitchControlLoop()
���ܣ�����PID�����pitch����������
��ע����������
***/
void GMPitchControlLoop(void)
{
    VAL_LIMIT(GimbalRef.pitch_angle_dynamic_ref , -31, 0);

	  //VAL_LIMIT(ChariotRecognition_pitch , -4, 32)//P����λ95 145
	
	if(GetWorkState() == Dodeg_STATE && CameraDetectTarget_Flag ==1 )
	{
	 Dodeg_STATE_Change = 1;
	}
	if(GetWorkState() == Dodeg_STATE && CameraDetectTarget_Flag ==0 )
	{
	 Dodeg_STATE_Change = 2;
	}


	//ս��ʶ��״̬
  if(GetWorkState() == ChariotRecognition_STATE )
	{ 
		CR_Pitch_Symbol= ChariotRecognition_pitch/fabs(ChariotRecognition_pitch);
		

//	if(fabs(ChariotRecognition_pitch)>1.5)
//	{
//		GMPPositionPID.ref = -(1-exp(-fabs(ChariotRecGMPPositionPID.Calc(&GMPPositionPID);   //�õ�pitch��λ�û����������;
//		GMPPositionPID.fdb = (1-exp(-fabs(ChariotRecognition_pitch)))*(ChariotRecognition_pitch)*GMYawRamp.Calc(&GMYawRamp);
//	}
//		
//	if(fabs(ChariotRecognition_pitch)<1.5)
	{
		GMPPositionPID.ref = ChariotRecognition_pitch;
		GMPPositionPID.fdb = GMPitchEncoder.ecd_angle;
	}
	
		GMPPositionPID.kp = 60+30*(1-exp(-0.1*fabs(GMPPositionPID.ref - GMPPositionPID.fdb)));//100+120*(1-exp(-0.1*fabs(GMPPositionPID.ref - GMPPositionPID.fdb)))
		GMPPositionPID.ki = 0.001;//0.001;//0.05//0.02//0.1;//0.001;
		GMPPositionPID.kd = 6;//0//6
						
		GMPSpeedPID.kp = 40;//1;//2.5//30
		GMPSpeedPID.ki = 0;
		GMPSpeedPID.kd = 2;//0//2

			
		GMPPositionPID.Calc(&GMPPositionPID);   //�õ�pitch��λ�û����������
		GMPPositionPID.output = GMPPositionPID.output - GMPitchEncoder.ecd_angle*6;//- GMPitchEncoder.ecd_angle*10;
				
	}


	//׼��״̬������״̬������״̬��������״̬�Ͷ��״̬
	if(GetWorkState() == PREPARE_STATE || GetWorkState() == Freedom_STATE || GetWorkState() == Test_STATE || GetWorkState() == Attacked_STATE  || Dodeg_STATE_Change == 2 )
	{	
		GMPPositionPID.kp = 80;//100;//40//60//80
		GMPPositionPID.ki = 0.01;//0.03
		GMPPositionPID.kd = 2;//0//2
			
		GMPSpeedPID.kp = 20;//3 30//20
		GMPSpeedPID.ki = 0;//0.002
		GMPSpeedPID.kd = 0;//0
		if(GetWorkState() == PREPARE_STATE||GetWorkState() == STOP_STATE)
		{
			
			GMPPositionPID.ref = 0.0f;   //GMPitchRamp.Calc(&GMPitchRamp);
			GMPPositionPID.fdb = GMPitchEncoder.ecd_angle* GMPitchRamp.Calc(&GMPitchRamp);
      GMPPositionPID.Calc(&GMPPositionPID);   //�õ�pitch��λ�û����������			
		}
		else
		{
		  GMPPositionPID.ref = - GimbalRef.pitch_angle_dynamic_ref;
			GMPPositionPID.fdb = GMPitchEncoder.ecd_angle * GMPitchRamp.Calc(&GMPitchRamp); 
			GMPPositionPID.Calc(&GMPPositionPID);   //�õ�pitch��λ�û����������
		}
					
	}
  //GMPPositionPID.Calc(&GMPPositionPID);   //�õ�pitch��λ�û����������
	GMPSpeedPID.ref = GMPPositionPID.output;
	GMPSpeedPID.fdb = 0.10*GMPitchEncoder.filter_rate;//-Gyro[1]/10;                     //p�ᶶ���޸�ϵ��
	GMPSpeedPID.Calc(&GMPSpeedPID);
	GMPSpeedPID.output =GMPSpeedPID.output+GMPSpeedPID.output/fabs(GMPSpeedPID.output);
	
//	GMPSpeedPID.output =GMPSpeedPID.output+GMPSpeedPID.output/fabs(GMPSpeedPID.output)*0.5;
}

/***
������GMYawControlLoop()
���ܣ�����PID�����yaw����������
��ע����������
***/
void GMYawControlLoop(void)
{
	//ս��ʶ��״̬
	if(GetWorkState() == ChariotRecognition_STATE || Dodeg_STATE_Change == 1)
	{
		test1= last_ChariotRecognition_yaw - ChariotRecognition_yaw;//��ʱ���β��Լӵı���

		
    Last_Dodeg_STATE_Change = Dodeg_STATE_Change;
		Dodeg_STATE_Change = 0;
		
		
		CR_yaw_Symbol = (ChariotRecognition_yaw)/fabs(ChariotRecognition_yaw);

////�����������չһ�µ���˼·������ʵ�ڸо������˼���Էֶεȷ���		
//		if(fabs(ChariotRecognition_yaw)>1.2)//&& fabs(ChariotRecognition_yaw)<3 )
//		{
//		CR_yaw_increment = 6*CR_yaw_Symbol*(1-exp(-0.3*fabs(ChariotRecognition_yaw)))-0.5;//0.95   0.6
//		GMYPositionPID.ref = (1-exp(-fabs(ChariotRecognition_yaw)))*(ChariotRecognition_yaw) +CR_yaw_increment;
//		GMYPositionPID.fdb = -(1-exp(-fabs(ChariotRecognition_yaw)))*(ChariotRecognition_yaw)*GMYawRamp.Calc(&GMYawRamp);
//		}
//	
//		if(fabs(ChariotRecognition_yaw)<1.2)
//		{
//		CR_yaw_increment = 0.5*(ChariotRecognition_yaw)*(ChariotRecognition_yaw)*(ChariotRecognition_yaw);
//		GMYPositionPID.ref = (1-exp(-fabs(ChariotRecognition_yaw)))*(ChariotRecognition_yaw);//-CR_yaw_increment ;
//		GMYPositionPID.fdb = -(1-exp(-fabs(ChariotRecognition_yaw)))*(ChariotRecognition_yaw)*GMYawRamp.Calc(&GMYawRamp);
//		}
//		 if(fabs(ChariotRecognition_yaw)>1.2)
//		{
//		GMYPositionPID.ref = (1-exp(-fabs(ChariotRecognition_yaw)))*(ChariotRecognition_yaw);//-CR_yaw_increment ;
//		GMYPositionPID.fdb = -(1-exp(-fabs(ChariotRecognition_yaw)))*(ChariotRecognition_yaw)*GMYawRamp.Calc(&GMYawRamp);	
//		}
//		
//	   if(fabs(ChariotRecognition_yaw)<1.2)
	
	//���δ���
//		GMYPositionPID.ref = (1-exp(-fabs(ChariotRecognition_yaw)))*(ChariotRecognition_yaw);//-CR_yaw_increment ;
//		GMYPositionPID.fdb = -(1-exp(-fabs(ChariotRecognition_yaw)))*(ChariotRecognition_yaw)*GMYawRamp.Calc(&GMYawRamp);	
	
  	GMYPositionPID.ref = -ChariotRecognition_yaw;//-CR_yaw_increment ;
		GMYPositionPID.fdb = -Angles;//GMYawEncoder.ecd_angle;	

		GMYPositionPID.kp = 70+60*(1-exp(-0.3*fabs(GMYPositionPID.ref - GMYPositionPID.fdb)));//60+30//160+150*(1-exp(-0.3*fabs(GMYPositionPID.ref - GMYPositionPID.fdb)))
		GMYPositionPID.ki = 0.005*exp(-0.3*fabs(GMYPositionPID.ref - GMYPositionPID.fdb));//0.1;//0.005;//0.01*exp(-0.3*fabs(GMYPositionPID.ref - GMYPositionPID.fdb));//0.1//5
		GMYPositionPID.kd = 5;//-5*(1-exp(-0.3*fabs(GMYPositionPID.ref - GMYPositionPID.fdb)));//10-5//3;//0;
		
		GMYSpeedPID.kp = 65;//10//15
		GMYSpeedPID.ki = 0.001;//0.005;
		GMYSpeedPID.kd = 2;//1//2//5
		
		GMYPositionPID.Calc(&GMYPositionPID);    //�õ�yaw��λ�û����������
		
		
     last_ChariotRecognition_yaw   = ChariotRecognition_yaw;
	
	}

	//׼��״̬
	if(GetWorkState() == PREPARE_STATE ||GetWorkState() == STOP_STATE  ) 
	{	
		GMYPositionPID.kp = 100;//0
		GMYPositionPID.ki =	0.01;//0
		GMYPositionPID.kd = 5;//0
			
		GMYSpeedPID.kp = 10;//0
		GMYSpeedPID.ki = 0;//0
		GMYSpeedPID.kd = 2;//0
		
		GMYPositionPID.ref = GMYawEncoder.ecd_angle ;
		GMYPositionPID.fdb = GMYawEncoder.ecd_angle;   
		GMYPositionPID.Calc(&GMYPositionPID); 
	}
	
	//����״̬
	if( GetWorkState() == Test_STATE) 
	{
		GMYPositionPID.kp = 100;//50;//60;//100
		GMYPositionPID.ki =	0.005;//0.05;//0.005
		GMYPositionPID.kd = 5;//100;5//0
			
		GMYSpeedPID.kp = 25;//10//5//50���ᶶ��
		GMYSpeedPID.ki = 0;//5
		GMYSpeedPID.kd = 2;//2
	
		GMYPositionPID.ref = GimbalRef.yaw_angle_dynamic_ref;
		GMYPositionPID.fdb = GMYawEncoder.ecd_angle;	
    if(time_tick_2ms%8!=0)		
		{GMYPositionPID.Calc(&GMYPositionPID);} 
	}
	
	//����״̬��������״̬��׼��״̬
	if(GetWorkState() ==Freedom_STATE || GetWorkState() ==Attacked_STATE ||Dodeg_STATE_Change ==2) 
	{
    Last_Dodeg_STATE_Change = Dodeg_STATE_Change;
		Dodeg_STATE_Change = 0;
		
		GMYPositionPID.kp = 100;//60//100
		GMYPositionPID.ki =	0.005;//0
		GMYPositionPID.kd = 0;//0
			
		GMYSpeedPID.kp = 30;//5//50
		GMYSpeedPID.ki = 0;//0.005;
		GMYSpeedPID.kd = 2;//2
			
		GMYPositionPID.ref = GimbalRef.yaw_angle_dynamic_ref;
		GMYPositionPID.fdb = GMYawEncoder.ecd_angle;   
		GMYPositionPID.Calc(&GMYPositionPID); 
	}
	
	GMYSpeedPID.ref = GMYPositionPID.output*0.3;                            //y�ᶶ���޸�ϵ��
	GMYSpeedPID.fdb = -Gyro[2]/5;//GMYawEncoder.filter_rate;                //y�ᶶ���޸�ϵ��
	GMYSpeedPID.Calc(&GMYSpeedPID);
//		
}

/***
������SetGimbalMotorOutput()
���ܣ�ͨ��can����̨�����������������
��ע����
***/


void SetGimbalMotorOutput(void)
{
	//��̨�������								
	if((GetWorkState() == STOP_STATE))   
	{
		Set_Gimbal_Current(CAN1, 0, 0);     //yaw + pitch
	}
	//ʶ�𡢶��
	
		//����,����
	else
	{	
//		Set_Gimbal_Current(CAN1,2000,3000,(int16_t)(CM7SpeedPID.output));     //yaw + pitch+BoLunMotor	
  		Set_Gimbal_Current(CAN1,(int16_t)(GMYSpeedPID.output),(int16_t)(GMPSpeedPID.output));     //yaw + pitch	
		  Set_Gimbal_Current1(CAN2,0,(int16_t)(CM7SpeedPID.output));   //BoLunMotor + Brake

		//  	  Set_Gimbal_Current(CAN1,0,(int16_t)(GMPSpeedPID.output),(int16_t)(CM7SpeedPID.output));    
//   	  Set_Gimbal_Current(CAN1,(int16_t)(GMYSpeedPID.output),0,(int16_t)(CM7SpeedPID.output));
//		Set_Gimbal_Current(CAN1,0,0,(int16_t)(CM7SpeedPID.output));
	}
	
/**scc ��Ϊû�������ǣ���ʱע����ʶ��Ͷ��
	else if(GetWorkState() == ChariotRecognition_STATE || GetWorkState() == Dodeg_STATE)   
	{
		
		Set_Gimbal_Current(CAN1,(int16_t)(GMYSpeedPID.output),(int16_t)(GMPSpeedPID.output),(int16_t)(CM7SpeedPID.output)); 	
	}
		
	 
**/


}


/***
������CMControlLoop()
���ܣ����̿�������������̵������
��ע���ٶȿ���
***/
void CMControlLoop(void)
{  
	//CM1����
	  CM1SpeedPID.kp = 100+100*(1-exp(-0.1*fabs(CM1SpeedPID.ref - CM1SpeedPID.fdb)));//80
		CM1SpeedPID.ki = 0;//0.5
		CM1SpeedPID.kd = 0;//20
	    

//CM3��4Ħ����
	  CM3SpeedPID.kp = CM4SpeedPID.kp = 60 ;//40
		CM3SpeedPID.ki = CM4SpeedPID.ki = 0.5;//0.5
		CM3SpeedPID.kd = CM4SpeedPID.kd = 10;  //20
	
		//ֹͣ״̬��׼��״̬
		if(GetWorkState()==STOP_STATE || GetWorkState()==PREPARE_STATE ) 
		{
			CM1SpeedPID.kp = 40 ;//40
		  CM1SpeedPID.ki = 0.5;//0.5
	   	CM1SpeedPID.kd = 5;//20
			
     
			CM1SpeedPID.ref = 0;
			CM1SpeedPID.fdb = CM1Encoder.filter_rate;//0;
			CM1SpeedPID.Calc(&CM1SpeedPID);
		}
		//ʶ��
		if(GetWorkState()==ChariotRecognition_STATE)
		{
			if(Speed_change==0)
				{
				
				CM1SpeedPID.ref = -0.1*Chassis_Speed_Ref ;//- Speed_Offset.output*Chassis_Speed_Ref/fabs(Chassis_Speed_Ref) ;
			  }
			if(Speed_change==1)
				{
			 
				CM1SpeedPID.ref = -0.1*Chassis_Speed_Ref ;//- Speed_Offset.output*Chassis_Speed_Ref/fabs(Chassis_Speed_Ref);
			  }
			
			CM1SpeedPID.fdb = CM1Encoder.filter_rate;
			CM1SpeedPID.Calc(&CM1SpeedPID);

			
			CM3SpeedPID.fdb =  CM3Encoder.filter_rate;
			CM3SpeedPID.Calc(&CM3SpeedPID); 
			

			CM4SpeedPID.fdb = CM4Encoder.filter_rate;
			CM4SpeedPID.Calc(&CM4SpeedPID);
			
		}
		//����״̬������״̬��������״̬
		if(GetWorkState()== Freedom_STATE || GetWorkState() == Test_STATE || GetWorkState() == Attacked_STATE)
		{
			
			CM1SpeedPID.ref = -Chassis_Speed_Ref ;//-  Speed_Offset.output*Chassis_Speed_Ref/fabs(Chassis_Speed_Ref) ;
			CM1SpeedPID.fdb = CM1Encoder.filter_rate;
			CM1SpeedPID.Calc(&CM1SpeedPID);

			CM3SpeedPID.fdb =  CM3Encoder.filter_rate;
			CM3SpeedPID.Calc(&CM3SpeedPID); 
			
			CM4SpeedPID.fdb = CM4Encoder.filter_rate;
			CM4SpeedPID.Calc(&CM4SpeedPID);
			
		}
		
		//���
		if(GetWorkState()== Dodeg_STATE)
		{	
			
			CM1SpeedPID.ref = Chassis_Speed_Ref ;//+ Speed_Offset.output*Chassis_Speed_Ref/fabs(Chassis_Speed_Ref) ;
			CM1SpeedPID.fdb = CM1Encoder.filter_rate;
			CM1SpeedPID.Calc(&CM1SpeedPID);
			
			
		  CM3SpeedPID.fdb =  CM3Encoder.filter_rate;
			CM3SpeedPID.Calc(&CM3SpeedPID); 
			
			CM4SpeedPID.fdb = CM4Encoder.filter_rate;
			CM4SpeedPID.Calc(&CM4SpeedPID);
		}
		
		
		
		if(GetWorkState() == STOP_STATE || GetWorkState() == PREPARE_STATE ) 
		{
			//1+��+3+4����Ҫ��Ӧ�����ܰѿյ�ɾȥ
			if((CM1SpeedPID.fdb <= 10)&&(CM1SpeedPID.fdb >= -10))

			Set_CM_Speed(CAN1, 0,0,0,0);
			else
			  Set_CM_Speed(CAN1, CM1SpeedPID.output,0,0,0);
		}
		else if(GetWorkState()==ChariotRecognition_STATE)
		{ 
		Set_CM_Speed(CAN1, CM1SpeedPID.output, 0,CM3SpeedPID.output, CM4SpeedPID.output);	
		}
		else //���ɣ����ԣ���ܣ�ʶ�𣬱�����
		{
//	Set_CM_Speed(CAN1, 0, 0,CM3SpeedPID.output, CM4SpeedPID.output);		
		Set_CM_Speed(CAN1, CM1SpeedPID.output, 0,CM3SpeedPID.output, CM4SpeedPID.output);	 		 
		}
}

/***
������ShooterMControlLoop()	
���ܣ������������������
***/
int16_t count_temp = 0;


void ShooterMControlLoop(void)	
{
	//����pid
	CM7PositionPID.kp = 0;//40
	CM7PositionPID.ki =	0;//0
	CM7PositionPID.kd = 0;//0
	
	CM7SpeedPID.kp = 60;//50//55//60
	CM7SpeedPID.ki = 0;//0
	CM7SpeedPID.kd = 0;//10//25//20//25
	
	//ʶ��ģʽ��s1�Ƶ��м��Ħ���ֲŻ�򿪲���
	if(TempShootingFlag==1)//||(GetWorkState() == ChariotRecognition_STATE&&RC_CtrlData.rc.s1==3))//�򿪲��ַ���
	{
		CM7PositionPID.ref = 60*count_temp;
		CM7PositionPID.fdb = CM7Encoder.ecd_angle;
		CM7PositionPID.Calc(&CM7PositionPID);
		
		CM7SpeedPID.ref = 300;//CM7PositionPID.output;//CM7SpeedPID.ref = 40;����ת��������û��//400//250
	}
//	//�ſ���
	if(CM7Encoder.ecd_raw_rate < 10)
	{ 
		CM7SpeedPID.ref = -600;
	}
	if(CM7Encoder.ecd_raw_rate < -5 )
	{			
		CM7SpeedPID.ref = 300;//
	}
////	if(CM7Encoder.ecd_raw_rate<-5 &&)
//	{
//		kadanflag = 0 ;
//		kadantt = 10 ;
//	}
	
	//����״̬Ҫ�رղ��֣����״̬��bug����д�˹رղ��ֺ�Ħ���ֵĴ���
	if(TempShootingFlag==0)
	{
		CM7SpeedPID.ref = 0;
	}
	
	CM7SpeedPID.fdb = CM7Encoder.filter_rate;  //CM7SpeedPID.fdb = CM7Encoder.ecd_raw_rate
	CM7SpeedPID.Calc(&CM7SpeedPID);
	
//	if(CM7PositionPID.ref-CM7PositionPID.fdb>200)//������ת
//		count_temp=count_temp-5;
}


/***
������GraduallyChangeCMSpeed()
���ܣ�����״̬�Ͷ��״̬���̻���ʱ�����𽥼����ٷ����𽥼���
��ע����
***/
void GraduallyChangeCMSpeed(void)
{
	if(GetWorkState()== Freedom_STATE || GetWorkState()== Dodeg_STATE||GetWorkState()== ChariotRecognition_STATE||GetWorkState()== Attacked_STATE) //����״̬�Ͷ��״̬
	{
		if(Chassis_Change_Dir_Flag==1) //1->0���ٶ��𽥼�С
		{
			Chassis_Speed_UP_Flag = 0;
			Chassis_Speed_DOWN_Flag = 0;
			
			CMSpeedRate=0;//CMSpeedRate-0.05f;
			if(CMSpeedRate<=0) 
			{
				CMSpeedRate=0.0f;
				Chassis_Change_Dir_Flag=2;
			}
			Chassis_Speed_Ref = last_Chassis_Temp_Speed*CMSpeedRate;
		}
		else if(Chassis_Change_Dir_Flag==2) //0->1���ٶ�������
		{
			CMSpeedRate=1;//CMSpeedRate+0.05f;
			if(CMSpeedRate>=1) 
			{
				CMSpeedRate=1.0f;
				Chassis_Change_Dir_Flag = 0;
			}
			Chassis_Speed_Ref = Chassis_Temp_Speed*CMSpeedRate;
		}
		else
		{
			Chassis_Speed_Ref = Chassis_Temp_Speed;
		}
  }
}

