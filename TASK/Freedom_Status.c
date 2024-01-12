//��encoder�ļ���ע������̨����,�ޱ���

#include "Freedom_Status.h"
#include "ControlTask.h"
#include "WorkState.h"
#include "SpeedTask.h"
#include "encoder.h"
#include "stdlib.h"


float YawCurrentPositionSave   = 0.0f;   //���浱ǰYaw��λ��
float PitchCurrentPositionSave = 0.0f;   //���浱ǰPitch��λ��
float YawInitPositionSave = 0.0f;

uint8_t DetectTargetFinish_Flag = 0;          //����״̬�£�yaw����Ŀ��һ������ʱ���ڸ�λ�ø���Ѳ�߱�־
uint8_t RotationDir_Flag=0;                   //��Ѳ�߹����е���ת�����־
uint8_t Chassis_Change_Dir_Flag = 0;          //���̻����־
uint8_t Chassis_Speed_UP_Flag   = 0;          //���̼��ٱ�־
uint8_t Chassis_Speed_DOWN_Flag = 0;          //���̼��ٱ�־
int16_t  GimbalRefTest =0;               //ʲô��־
int32_t YAW_Round_Cnt     = 0;           //����
int16_t YAW_Angle_YuShu   = 0;           //����
int32_t Attacked_YAW_Pos_Ref = 0;        //ʲô��־ ������ʱ��yaw���λ��
uint8_t Attacked_YAW_Rotation_Dir = 0;        //������ʱ��YAW�뱻����װ�׵ķ���
uint8_t Chassis_Freedom_i  = 0;               //ʲô��־
int Yaw_rotate_flag=0;                   //y��������ת�����־
int pitch_rotate_flag=0;                 //p��������ת�����־
int count_i =0;                          //��ʲô����
int attack_dally_count = 0;
int Speed_change=0;
int32_t time_CNT_i = 0;
float TimeCount = 0;
int Recognition_YAW_Rotation_Dir=0;      //�̶�����ͷʶ��ʱ����̨��Ҫ��ת�ķ���
int32_t Recognition_YAW_Pos_Ref = 0;     //ʲô��־ ʶ��״̬yaw���λ��
int Freedom_Rotation_flag=0;             //����״̬ 0δʶ�𵽶Է�װ�װ� 1ʶ�𵽶Է�װ�װ�
int32_t Recognition_Delay_count = 0;     //ʶ��״̬������ֵΪ0
int32_t time_track_left = 0;  //�ܵ����ʱ
int32_t time_track_left_max = 0;  
int32_t time_track_right = 0;  //�ܵ����ʱ
int32_t time_track_right_max = 0;

double yaw_ecd_angle_flag = 10;    //-20;    //2022��  ����ģʽy��һ����λֵ
double yaw_ecd_angle_flag1 =-110;  //-150    //2022��  ����ģʽy����һ����λֵ
const int chassis_speed = 200;               //2022�� ����ģʽ�����ٶ�

/***
������YawFreeRoation()
���ܣ�����״̬ʱYAW������ת
***/
void YawFreeRoation(void)
{
	if(GetWorkState()==Freedom_STATE)//��Ѳ��״̬ ����״̬
	{
		if(GMPitchEncoder.ecd_angle-30>=0)   //38//35//30
		{
		pitch_rotate_flag = 1;   //ת����ĳ��λ�ã�p��ת���־λ1��
		}
		if(GMPitchEncoder.ecd_angle-8<=0)//-3
		{
		pitch_rotate_flag=0;    //ת����ĳ��λ�ã�p��ת���־λ0��
		}
		if(pitch_rotate_flag==1)
		{
	  GimbalRef.pitch_angle_dynamic_ref=GimbalRef.pitch_angle_dynamic_ref + 0.1f;   //p���־λ1��p��ת��Ƕ�+0.1��
		}
		if(pitch_rotate_flag==0)
		{
	  GimbalRef.pitch_angle_dynamic_ref=GimbalRef.pitch_angle_dynamic_ref - 0.1f;   //p���־λ0��p��ת��Ƕ�-0.1��
		}
		
		
		
		
		if(Freedom_Rotation_flag==0)
		{
			if(GMYawEncoder.ecd_angle-yaw_ecd_angle_flag<=0)//Y��һ����Χ�ڲ��ϼӼ���ʵ��תȦ ��Ȧ
			{
			Yaw_rotate_flag=1;
			}
			if(GMYawEncoder.ecd_angle+yaw_ecd_angle_flag1>=0)
			{
			Yaw_rotate_flag=0;
			}
			if(Yaw_rotate_flag==1)
			{
			GimbalRef.yaw_angle_dynamic_ref=GimbalRef.yaw_angle_dynamic_ref + 0.12f;//0.2//1
			}
			if(Yaw_rotate_flag==0)
			{
			GimbalRef.yaw_angle_dynamic_ref=GimbalRef.yaw_angle_dynamic_ref - 0.12f;//0.2//1
			}
   	}

		
		
		
   

	}
	
	//������ʱ��̨״̬
	if(GetWorkState() == Attacked_STATE)
	{
		attack_dally_count++;
		if(attack_dally_count>5000)
		{
		  attack_dally_count=0;
			Attacked_Flag=0;
		}
		
		
		if(GMPitchEncoder.ecd_angle-30>0)
		{
		pitch_rotate_flag=1;
		}
		if(GMPitchEncoder.ecd_angle-8<0)
		{
		pitch_rotate_flag=0;
		}
		if(pitch_rotate_flag==1)
		{
	  GimbalRef.pitch_angle_dynamic_ref=GimbalRef.pitch_angle_dynamic_ref + 0.07f;//��ͷ����
		}
		if(pitch_rotate_flag==0)
		{
	  GimbalRef.pitch_angle_dynamic_ref=GimbalRef.pitch_angle_dynamic_ref - 0.07f;
		}
		
		
			if(Attacked_YAW_Rotation_Dir == 1)
			{
				//if(GMYawEncoder.ecd_angle-yaw_ecd_angle_flag<=0)
				GimbalRef.yaw_angle_dynamic_ref=GimbalRef.yaw_angle_dynamic_ref+0.4f;//0.4f
				if(GimbalRef.yaw_angle_dynamic_ref - Attacked_YAW_Pos_Ref>0)
				{
					GimbalRef.yaw_angle_dynamic_ref = Attacked_YAW_Pos_Ref;
					Attacked_Flag = 0;
					Attacked_YAW_Rotation_Dir = 0;
				}
			}
			else if(Attacked_YAW_Rotation_Dir == 2)
			{
				//if(GMYawEncoder.ecd_angle+yaw_ecd_angle_flag1>=0)
				GimbalRef.yaw_angle_dynamic_ref=GimbalRef.yaw_angle_dynamic_ref-0.4f;//0.1f
				if(GimbalRef.yaw_angle_dynamic_ref - Attacked_YAW_Pos_Ref<0)
				{
					GimbalRef.yaw_angle_dynamic_ref = Attacked_YAW_Pos_Ref;
					Attacked_Flag = 0;
					Attacked_YAW_Rotation_Dir = 0;
				}
			}
			else if(Attacked_YAW_Rotation_Dir == 0)  //�ο�ֵ����Ŀ��λ�ú󣬵ȴ������λ
			{
				if(abs(GimbalRef.yaw_angle_dynamic_ref - (-GMYawEncoder.ecd_angle))<1)
				{
					Freedom_Flag = 1;
				}
			}
	  
		//�����̨�ڰ���װ�װ�Ĺ�����ʶ��Ŀ�꣬��ô���̽����ù��̣��ȴ�����ʶ��ģʽ
		if(CameraDetectTarget_Flag ==1)
		{
			Attacked_YAW_Rotation_Dir = 0;
			Attacked_Flag = 0;
			CameraDetectTarget_Flag = 1;
		}
	}
	
	
}

/***
������Chassis_Motion_Switch()
���ܣ�ֹͣ��λ������仯��Ѳ��ģʽ���̻���
��ע����
***/
void Chassis_Motion_Switch(void)
{
	if(GetWorkState()== Freedom_STATE||GetWorkState()== ChariotRecognition_STATE||GetWorkState()== Attacked_STATE)   
	{
		if(Chassis_Power_On_Flag == 1)  //�����Ѿ��ϵ�
		{
			
			if(Dir_Change_Flag==1)//���̻����־
			{
			   switch(Chassis_Freedom_i)//��������״̬�����־
				{
					case 0:
					{
						Chassis_Position_Ref = 10000;//����λ�ø�ֵ
						time_track_left++;
						time_track_right_max = time_track_right;
						Dir_Change_Flag=0;//����ˢ��ʱ�ظ�����
					};break;
					case 1:
					{
						Chassis_Position_Ref = -10000;
            time_track_right++;	
            time_track_left_max = time_track_left;						
						Dir_Change_Flag=0;
					};break;
				}
			}
		}
		if(Chassis_Power_On_Flag == 1)  //�����Ѿ��ϵ�
		{
			
			if(abs(Chassis_Position_Ref - CM1Encoder.ecd_angle)<5)
			{
				switch(Chassis_Freedom_i)//�����˶�����
				{
					GimbalRefTest++;//��¼ʲô
					case 0:
					{
						Chassis_Position_Ref = 10000;//����λ�ø�ֵ
					};break;
					case 1:
					{
						Chassis_Position_Ref = -10000; 
					};break;
				}
				Chassis_Freedom_i++;
				if(Chassis_Freedom_i>=2) Chassis_Freedom_i = 0;
			}
			
			
			time_CNT_i++;
			if(Chassis_Change_Dir_Flag==0)
			{
				last_Chassis_Temp_Speed = Chassis_Temp_Speed;
				time_CNT_i++;	
				if(time_CNT_i > 150000) 
				{
					Speed_change = 1; 
				}
				if(Speed_change==0)
				{
				 if(Chassis_Position_Ref > CM1Encoder.ecd_angle)//���ܳ����⣬һֱ��һ�������ϣ��ͷ���
				   {
					   Chassis_Temp_Speed = -chassis_speed ;
				   }
				 else if(Chassis_Position_Ref < CM1Encoder.ecd_angle)
				   {
					   Chassis_Temp_Speed = chassis_speed ;
				   }
			  }
				if(Speed_change==1)
				{
				 if(Chassis_Position_Ref > CM1Encoder.ecd_angle)
				   {
					   Chassis_Temp_Speed = -chassis_speed ;
				   }
				 else if(Chassis_Position_Ref < CM1Encoder.ecd_angle)
				   {
					   Chassis_Temp_Speed = chassis_speed ;
				   }
			  }
				
				if(last_Chassis_Temp_Speed*Chassis_Temp_Speed<0)
				{
					Chassis_Change_Dir_Flag = 1;
				}
		  }
	  }
  }
}

