#include "main.h"

int16_t GunHeart_Value = 0;      //��ǰǹ������
int Doget_bleed_time=0;
uint32_t Dodeg_Delay_Count = 0;
uint32_t Dodeg_Delay__Count_2 = 0;
int  test_HP=0;
int Count_flag=0;
int HP_error=0;                 //Ѫ����ֵ
int16_t HP_error__time[60];     //�������汻����ʱ�������
int16_t HP__error_time_count=0; //��ʱ����ÿ0.11���������һ�Σ��������Լ�1��
int HP__error_count=0;
int Doget_HP_Flag=0;
int Big_armor_recognition_count=0;
int Big_armor_recognition_flag=0;
int Aerocraft_attack_flag=0;        //��������������־
int Aerocraft_flag=0;
uint32_t Aerocraft_doget_count_delay=0; //������������ʱ�Ķ��
int Aerocraft_Phototube_flag=0;         //�������������Ժ���û�е������Ҷ˱�־
int time_test=0;

/*********************************************
�ڱ������ˣ�(17mm����)
1.�������ޣ�30m/s
2.�������ޣ�360
3.ÿ����ȴֵ��72
4.��ʼѪ��&����Ѫ��ֵ��600
**********************************************/
void Bleed_Monitor(void)
{
   if(Chassis_Power_On_Flag == 1)
		{

 		RobotHP = robotState.remainHP;
			HP_error=RobotHP-lastRobotHP;	
			 HP__error_time_count++;
		  	if(HP_error<=-10)
		  	{
		  	  HP_error__time[HP__error_count++]=HP__error_time_count;
		  		Doget_HP_Flag=1;
					//Aerocraft_flag=1;
		  	}
			 //��0.6���ڱ�����������Σ�Ǩ������ͷδʶ�𵽲�����������ܷ�����ģʽ?
//				if(HP__error_count>=5&&Aerocraft_flag==1)
//		  	{  time_test=HP_error__time[HP__error_count-1]-HP_error__time[HP__error_count-5];
//		     	if((HP_error__time[HP__error_count-1]-HP_error__time[HP__error_count-5]<6)&&GM_Rotete_flag_Before!='X')//&&(CM1Encoder.ecd_angle>-2500&&CM1Encoder.ecd_angle<500))
//		    	{
//						Dodeg_Delay_Count = 0; //��ܼ�������
//						Aerocraft_doget_count_delay = 0;
//			    	DodgeTarget_Flag = 1;
//				  	Aerocraft_flag=0;
//						Aerocraft_attack_flag=1;
//						Aerocraft_Phototube_flag=1;
//			    }
//		    }
			if(RobotHP>120)  
			{
					//��5�����ڱ�����15�Σ�������״̬
		  	if(HP__error_count>=15&&Doget_HP_Flag==1)
		  	{
					time_test=HP_error__time[HP__error_count-1]-HP_error__time[HP__error_count-15];
		     	if((HP_error__time[HP__error_count-1]-HP_error__time[HP__error_count-15]<60)&&(HP_error__time[HP__error_count-1]-HP_error__time[HP__error_count-5]>6))
		    	{
			      Dodeg_Delay_Count = 0; //��ܼ�������
			    	DodgeTarget_Flag = 1;
				  	Doget_HP_Flag=0;
			    }
		    }
				
				//��Ӣ�۵ڶ��ι������ٴν�����״̬������м�Σ�//
				 Doget_bleed_time++;
		  	if(Doget_bleed_time<60&&Doget_bleed_time>10)
			  {
			  	if(HP_error<=-100) 
			    {
	      	Random_change_flag=1;			
			  	Doget_Change_Flag=1;			
			    }		
			  }
				//��Ӣ�۹���һ�Σ�������״̬ ��������߶Σ�//
		  	if(HP_error<=-100) 
		  	{
	      	Dodeg_Delay_Count = 0; //��ܼ�������
		  		DodgeTarget_Flag = 1;
			  	Doget_Change_Flag=1;
			  	Doget_bleed_time=0;
//					Big_armor_recognition_count=0;
//   				Big_armor_recognition_flag=1;
		  	}
				Big_armor_recognition_count++;
  			if(Big_armor_recognition_count>300)
  			{ 
  			  Big_armor_recognition_flag=0;
  			}
			}
			
			//Ѫ������120��ֱ�ӽ�����״̬  �ڶ��״̬������ʱ���Ƕ�ܵ����߶ι��10�룬֮��20��ȫ������ܣ�������Ҳ����ȥ���߶Σ�20��֮������ٴν������߶ζ��//
			if(RobotHP<120)  
			{
				Dodeg_Delay_Count = 0;
				DodgeTarget_Flag = 1;
				if(HP_error<=-10&&Count_flag==0)
				{
				  Doget_Change_Flag=1;
					Dodeg_Delay__Count_2=0;
					Random_change__flag_2=1;
					Count_flag=1;
				}
				Dodeg_Delay__Count_2++;
				if(Dodeg_Delay__Count_2>100)
				{
				   Random_change__flag_2=0;
				}
				if(Dodeg_Delay__Count_2>300)
				{
				  Count_flag=0;
				}
			  if(HP_error<=-10)
			  {
			    Big_armor_recognition_count=0;
//				  Big_armor_recognition_flag=1;
		  	}
			  Big_armor_recognition_count++;
		   	if(Big_armor_recognition_count>300)
			  {
			    Big_armor_recognition_flag=0;
			  }
			}
			
			if(HP_error<=-10 && HP_error>=-20) //��С���蹥��һ��
			{
				Attacked_Flag = 1; //��������־��Ϊ1
			}
		lastRobotHP = RobotHP;		
	  } 
		
		
	}
/**********************
���ܣ�������״�����
������
	
**********************/
	
void Attacked_Monitor(void)
{
	
	Amor_ID = robotHurt.armorType;    //1��Ϊ����װ�ף�0��Ϊ����װ��
	if(Attacked_Flag == 0)  //����Ӧ����װ�װ幥��ʱ����������Ӧ�µ��˺�����ֹ���
	{
//		Amor_ID = robotHurt.armorType;
		Yaw_encoder = GMYawEncoder.ecd_angle;//-Yaw_encoder_s;
		YAW_Round_Cnt   = (int)(Yaw_encoder)/360;
		YAW_Angle_YuShu = (((int)(Yaw_encoder)%360)+360)%360;

			if(Amor_ID == 0) //0��װ�ױ����� ǰװ�װ�
			{

					
				if(YAW_Angle_YuShu>=240 && YAW_Angle_YuShu<=335)   //������    
					{
						Attacked_YAW_Pos_Ref = YAW_Round_Cnt*360+90;//YAW_Round_Cnt*1852 -223;
					}
				else if(YAW_Angle_YuShu>=155&& YAW_Angle_YuShu<=240)   //�Һ����    
				   {
						Attacked_YAW_Pos_Ref = YAW_Round_Cnt*360+180;//YAW_Round_Cnt*1852 -223;
				   }
				else Attacked_YAW_Pos_Ref =  GMYawEncoder.ecd_angle;

			}
			if(Amor_ID == 1) //1��װ�ױ�����
			{
				if(YAW_Angle_YuShu>=335&& YAW_Angle_YuShu<=60)   //��ǰ����    
					{
						Attacked_YAW_Pos_Ref = YAW_Round_Cnt*360-90;//YAW_Round_Cnt*1852 -223;
					}
				else if(YAW_Angle_YuShu>=60&& YAW_Angle_YuShu<=155)   //��ǰ����   
				   {
						Attacked_YAW_Pos_Ref = YAW_Round_Cnt*360-180;//YAW_Round_Cnt*1852 -223;
				   }
			  else Attacked_YAW_Pos_Ref =  GMYawEncoder.ecd_angle;


			}
		
		if(Attacked_YAW_Pos_Ref - GMYawEncoder.ecd_angle > 1)
			{
				Attacked_YAW_Rotation_Dir = 1;   //��λ�û��ο�ֵ����ķ���ת
			}
		else
			{
				Attacked_YAW_Rotation_Dir = 2;  //��λ�û��ο�ֵ��С�ķ���ת
			}
			Hurt_COUNT++;
	}
}

/***************************************************************************
���ܣ�ǹ����������
��ڲ�����17mm��������
���ڲ�������
***************************************************************************/
void BulletSpeed_Monitor(void)
{
 
				
				/***���ݵ���ʵ������ʵʱ������СĦ�����ٶ�***/
	if(Target_Distance<=30)
	{
		if(Bullet_17_real_Speed - Bullet_Speed_Ref.low_speed>1)       //ʵ�����ٴ���Ŀ������
		{
	 		Friction_Speed.low_speed = Friction_Speed.low_speed - 10;
		}
		else if(Bullet_17_real_Speed - Bullet_Speed_Ref.low_speed<-1) //ʵ������С��Ŀ������
		{
			Friction_Speed.low_speed = Friction_Speed.low_speed + 10;
		}
	}
				/***���ݵ���ʵ������ʵʱ�������Ħ�����ٶ�***/
	if(Target_Distance>30)  
	{
		if(Bullet_17_real_Speed - Bullet_Speed_Ref.high_speed>1)       //ʵ�����ٴ���Ŀ������
		{
	  	if(Bullet_17_real_Speed - Bullet_Speed_Ref.high_speed<3)
			{
	 		   Friction_Speed.high_speed = Friction_Speed.high_speed - 10;
			}
//��ע ��ʹĦ����refͣ��100�ٶ�
//			else
//			{
//							/***
//							�ٶȳ���28m/sһ�Σ�����Ħ�����ٶȾ���ȥ100����֤��������
//							***/
//		    	Friction_Speed.low_speed = Friction_Speed.low_speed - 100;
//					Friction_Speed.high_speed = Friction_Speed.high_speed - 100;
//							
//					if(Friction_Speed.low_speed<100) Friction_Speed.low_speed = 100;
//			}
	}
	else if(Bullet_17_real_Speed - Bullet_Speed_Ref.high_speed<-1) //ʵ������С��Ŀ������
		{
  			Friction_Speed.high_speed = Friction_Speed.high_speed + 10;
		}
  }
}

/***********************************************************
���ܣ�ǹ��������ȴ����
��ڲ�������
���ڲ�������
��ע��10Hz��ȴ�߳�
***********************************************************/

void GunHeart_CalcProcess(uint16_t BoomSpeed)
{
  GunHeart_Value = GunHeart_Value + BoomSpeed;
}

void GunHeart_CoolingProcess(void)
{
	GunHeart_Value = (int)(GunHeart_Value - Cooling_Value_PerSec/10);
	GunHeart_Value = (GunHeart_Value<0 ? 0 : GunHeart_Value);
}
