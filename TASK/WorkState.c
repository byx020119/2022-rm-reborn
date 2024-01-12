//ע���˲�����̨����

#include "WorkState.h"
#include "ControlTask.h"
#include "SpeedTask.h"
#include "math.h"
#include "CanBusTask.h"
#include "Freedom_Status.h"
#include "usart1.h"
#include "encoder.h"
#include "JudgingSystemTask.h"

WorkState_e lastWorkState = STOP_STATE;   //ö�ٱ�����ʼ��
WorkState_e workState = STOP_STATE;       //ö�ٱ�����ʼ��������д�ں��������

uint8_t CameraDetectTarget_Flag = 0;   //����ͷ����Ŀ���־
uint8_t DodgeTarget_Flag        = 0;   //���Ŀ���־
uint8_t RemoteTest_Flag         = 0;   //ң�������Ա�־
uint8_t Waiting_Flag            = 0;   //�ȴ���־������״̬��ʶ��״̬�л�ʱ���Ƚ���ȴ�״̬��ʹ��̨����ͣ��
uint8_t Chassis_Power_On_Flag   = 0;   //�����ϵ��־
uint8_t Attacked_Flag           = 0;   //��������־
uint8_t Freedom_Flag            = 0;   //����״̬ת����־

uint16_t Waiting_COUNT = 0;
int16_t danliang_16 = 0;
int danliang = 0;
uint32_t Dodge_time_count = 0;         //2022�Ӷ��ģʽ��ʱ


/***
������WorkStateFSM()
���ܣ����ƹ���ģʽ
��ע������״̬�������ƶ���Y����ת��P����ֵλ��
      ս��ʶ��״̬��P,Y�ᰴ������ͷ���ص������˶������̲�������ʼ����
***/
void WorkStateFSM(void)
{
	danliang_16 =  ( utm123[3]<<8 | utm123[4] );
  danliang = Transform_Hex_To_Oct(danliang_16,16);

	//���ڲ���״̬��ʶ��״̬ת��
	/////////////////////////////////////
	
	lastWorkState = workState;//���µ�ǰ����״̬
	switch(workState)
	{
		case PREPARE_STATE://׼��״̬
		{		
			if(time_tick_2ms > PREPARE_TIME_TICK_MS)//׼��״̬ά��2s
			{
				workState = Freedom_STATE;//���Զ��״̬�޸Ĵ˴�Ϊ�� Dodeg_STATE ����ģʽ ��Freedom_STATE;
			}
				
		}break;
		
		case Freedom_STATE:     //����״̬�������ƶ���yaw��ת
		{	
//      if( utm123[2] == 1 && danliang >0 )//(danliang >= 300) ��ǹʶ�𵽣�����ʶ��״̬
//			{
//				workState = ChariotRecognition_STATE;
//			}
      if(CameraDetectTarget_Flag == 1)   //(CameraDetectTarget_Flag == 1 && danliang > 0 )//����ͷʶ�𣬽���ʶ��״̬
			{
				workState = ChariotRecognition_STATE;
			}
				if(RemoteTest_Flag == 0)   //ң����ֹͣ���ԣ�����ֹͣ״̬
			{
				workState = STOP_STATE;	
			}
			if(RemoteTest_Flag == 2)   //ң������ʼ���ԣ��������״̬
			{
				workState = Test_STATE;	
			}
			if(Attacked_Flag == 1)   //Ѳ��ʱ��װ�ױ���������Ϊ������״̬
			{
				workState = Attacked_STATE;        
			}
			 if(DodgeTarget_Flag == 1)		//Ѫ������̫���Ѫ�����٣���Ϊ���״̬
			{
				workState = Dodeg_STATE;  
			}			
		}break;
		
		
		case Attacked_STATE:    //������״̬
		{
		  if(Attacked_Flag  == 0)		//ת�򱻹���װ�װ���̽�����Ϊ����״̬
			{
				workState = Freedom_STATE;  //����״̬
			}		
			if(CameraDetectTarget_Flag == 1)   //ת�����������ͷʶ��Ŀ�꣬��Ϊʶ��״̬
			{
				workState = ChariotRecognition_STATE;
			}
			if(RemoteTest_Flag == 0)   //ң����ֹͣ���ԣ������Զ�ģʽ�µ�ֹͣ״̬
			{
				workState = STOP_STATE;
			}	
			if(DodgeTarget_Flag == 1 || utm123[5] == 1)		//Ѫ������̫���Ѫ�����٣���Ϊ���״̬
			{
				workState = Dodeg_STATE;  
			}	
			if(RemoteTest_Flag == 0)   //ң����ֹͣ���ԣ�����ֹͣ״̬
			{
				workState = STOP_STATE;	
			}

		}break;
		
		case Test_STATE:    //ң��������״̬
		{
			if(RemoteTest_Flag == 0)   //ң����ֹͣ���ԣ������Զ�ģʽ�µ�ֹͣ״̬
			{
				workState = STOP_STATE;
			}		
			if(RemoteTest_Flag == 1)
			{
				workState = PREPARE_STATE; //ң����ֹͣ���ԣ�����׼��״̬������������״̬
			}
      if(CameraDetectTarget_Flag == 1)   //����ͷʶ��Ŀ�꣬��Ϊʶ��״̬
			{
				workState = ChariotRecognition_STATE;
			}			
			if( (utm123[5] != 1) && (utm123[2] == 1 ) )	// && danliang >= 300
			{
				workState = ChariotRecognition_STATE;
			}	
		}break;
		
		case ChariotRecognition_STATE:      //ս��ʶ��״̬
		{	
			if(RemoteTest_Flag == 0)   //ң����ֹͣ���ԣ������Զ�ģʽ�µ�ֹͣ״̬
			{
				workState = STOP_STATE;
			}	
			 if(CameraDetectTarget_Flag == 0 && utm123[2] == 0 )   //����ͷδʶ��Ŀ�꣬��������״̬
			{
				workState = Freedom_STATE;
			}	
			 if(DodgeTarget_Flag == 1)		//Ѫ������̫�죬��Ϊ���״̬
			{
				workState = Dodeg_STATE;
			}		
			if(RemoteTest_Flag == 0)   //ң����ֹͣ���ԣ�����ֹͣ״̬
			{
				workState = STOP_STATE;
        CameraDetectTarget_Flag = 0;				
			}
						
		}break;
		
		case Dodeg_STATE:      //���״̬
		{	
      if(DodgeTarget_Flag == 0 && CameraDetectTarget_Flag == 0)		//��ܽ�������δʶ��Ŀ�꣬��������״̬
			{
				 workState = Freedom_STATE;  
			}				
			if(DodgeTarget_Flag == 0 && CameraDetectTarget_Flag == 1)   //����ʶ��״̬
			{
				workState = ChariotRecognition_STATE;
			}
			if(RemoteTest_Flag == 2)   //�������״̬
			{
				workState = Test_STATE;	
			}      
			if(RemoteTest_Flag == 0)   //ң����ֹͣ����
			{
				workState = STOP_STATE;
				DodgeTarget_Flag = 0;
			}	
		}break;
		
		
		
		case STOP_STATE:   //ֹͣ״̬
		{
			if(RemoteTest_Flag == 1)
			{
				workState = PREPARE_STATE; 
			}
		
		}break;
		default:
		{
			
		}
	}	
}


/***
������WorkStateSwitchProcess()
���ܣ�����״̬�л�ʱ���������в���
***/
void WorkStateSwitchProcess(void)
{
	/***
	  ״̬�仯������״̬��Ϊ׼��״̬��
	  ��    �������³�ʼ�����ƻ�
	  ��    ע������״̬��Ϊ׼��״̬ʱ���Ե�ǰλ�õ�yaw����ֵ��ʼ��yawλ�û��ο�ֵ
	***/
	if((lastWorkState != PREPARE_STATE) && (workState == PREPARE_STATE))  
	{
		YawCurrentPositionSave = GMYawEncoder.ecd_angle;
		YawInitPositionSave = GMYawEncoder.ecd_angle;  //Y��ĳ�ʼλ�ã���֤����ģʽY����ת����ͽǶ�ʼ�ղ���
		ControtLoopTaskInit();//���³�ʼ�����ƻ�
	}
	/***
	  ״̬�仯��׼��״̬��Ϊ����״̬
	  ��    ������Ħ���֣�������
	  ��    ע����
	***/
	if((lastWorkState == PREPARE_STATE) && (workState == Freedom_STATE))  
	{
		YawCurrentPositionSave = GMYawEncoder.ecd_angle;           //���浱ǰyaw����ֵ
		GimbalRef.yaw_angle_dynamic_ref = YawCurrentPositionSave;
		PitchCurrentPositionSave= -GMPitchEncoder.ecd_angle;           //���浱ǰpitch����ֵ
		GimbalRef.pitch_angle_dynamic_ref = PitchCurrentPositionSave;
		Last_Dodeg_STATE_Change = 0;	
	}
	

	
	/***
	  ״̬�仯��ʶ��״̬��Ϊ����״̬
	  ��    ����
	  ��    ע����
	***/
	if((lastWorkState == ChariotRecognition_STATE) && (workState == Freedom_STATE))  
	{
		YawCurrentPositionSave = GMYawEncoder.ecd_angle;           //���浱ǰyaw����ֵ
		GimbalRef.yaw_angle_dynamic_ref = YawCurrentPositionSave;
		PitchCurrentPositionSave= -GMPitchEncoder.ecd_angle;           //���浱ǰpitch����ֵ
		GimbalRef.pitch_angle_dynamic_ref = PitchCurrentPositionSave;
	}
		/***
	  ״̬�仯������״̬��Ϊ���״̬   2022��
	  ��    ������ʱ��־����һ��ʱ�������㣬��һ�����ڶ��ģʽ
	  ��    ע����
	***/
	if((lastWorkState != Dodeg_STATE) && (workState == Dodeg_STATE))  
	{	
		Dodge_time_count = time_tick_2ms;
	}
	
	/***
	  ״̬�仯: ���״̬��Ϊ����״̬
	  ��    ����
	  ��    ע����
	***/
	if((lastWorkState == Dodeg_STATE) && (workState == Freedom_STATE))  
	{
		Chassis_Position_Ref = Chassis_Position_Ref/fabs(Chassis_Position_Ref)*10000;
	}
	/***
	  ״̬�仯������״̬��Ϊս��ʶ��״̬
	  ��    ������ʼ������yaw����б�»�������Ŀ��
	  ��    ע����
	***/
	if((lastWorkState != ChariotRecognition_STATE) && (workState == ChariotRecognition_STATE))  
	{
		GMYawRamp.SetScale(&GMYawRamp, 20); //50
		GMYawRamp.ResetCounter(&GMYawRamp);
		YawCurrentPositionSave = GMYawEncoder.ecd_angle;
		PitchCurrentPositionSave = - GMPitchEncoder.ecd_angle;
	}
		/***
	  ״̬�仯������״̬��Ϊֹͣ״̬        2022��
	  ��    ������������־λ�����ɻ�������־λ����ܱ�־λ����
	  ��    ע����
	***/
	if((lastWorkState != STOP_STATE) && (workState == STOP_STATE))  
	{
		Attacked_Flag = 0;
		DodgeTarget_Flag = 0;
		Aerocraft_attack_flag = 0;
		pitch_err = 0;
		speed_rand_flag=0;
		
		YawCurrentPositionSave = GMYawEncoder.ecd_angle;
		YawInitPositionSave = GMYawEncoder.ecd_angle;  //Y��ĳ�ʼλ�ã���֤����ģʽY����ת����ͽǶ�ʼ�ղ���
		ControtLoopTaskInit();//���³�ʼ�����ƻ�
	}

	
	
}
	


/***
������SetWorkState(WorkState_e state)
���ܣ����ص�ǰ����״̬
��ע����������û�ù�
***/
void SetWorkState(WorkState_e state)
{
	if(workState != PREPARE_STATE)
	{
    workState = state;
	}
}

/***
������WorkState_e GetWorkState(void)
���ܣ����ع���״̬
��ע����returnֵ���ع���״̬
***/
WorkState_e GetWorkState(void)
{
	return workState;
}

WorkState_e GetlastWorkState(void)
{
	return lastWorkState;
}
