#include "main.h"

int16_t GunHeart_Value = 0;      //当前枪口热量
int Doget_bleed_time=0;
uint32_t Dodeg_Delay_Count = 0;
uint32_t Dodeg_Delay__Count_2 = 0;
int  test_HP=0;
int Count_flag=0;
int HP_error=0;                 //血量差值
int16_t HP_error__time[60];     //用来保存被攻击时间的数组
int16_t HP__error_time_count=0; //计时量（每0.11秒程序运行一次，即程序自加1）
int HP__error_count=0;
int Doget_HP_Flag=0;
int Big_armor_recognition_count=0;
int Big_armor_recognition_flag=0;
int Aerocraft_attack_flag=0;        //被飞行器攻击标志
int Aerocraft_flag=0;
uint32_t Aerocraft_doget_count_delay=0; //被飞行器攻击时的躲避
int Aerocraft_Phototube_flag=0;         //被飞行器攻击以后有没有到达最右端标志
int time_test=0;

/*********************************************
哨兵机器人：(17mm弹丸)
1.射速上限：30m/s
2.热量上限：360
3.每秒冷却值：72
4.初始血量&上限血量值：600
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
			 //若0.6秒内被攻击超过五次，迁且摄像头未识别到步兵，则进入躲避飞行器模式?
//				if(HP__error_count>=5&&Aerocraft_flag==1)
//		  	{  time_test=HP_error__time[HP__error_count-1]-HP_error__time[HP__error_count-5];
//		     	if((HP_error__time[HP__error_count-1]-HP_error__time[HP__error_count-5]<6)&&GM_Rotete_flag_Before!='X')//&&(CM1Encoder.ecd_angle>-2500&&CM1Encoder.ecd_angle<500))
//		    	{
//						Dodeg_Delay_Count = 0; //躲避计数清零
//						Aerocraft_doget_count_delay = 0;
//			    	DodgeTarget_Flag = 1;
//				  	Aerocraft_flag=0;
//						Aerocraft_attack_flag=1;
//						Aerocraft_Phototube_flag=1;
//			    }
//		    }
			if(RobotHP>120)  
			{
					//若5秒钟内被攻击15次，进入躲避状态
		  	if(HP__error_count>=15&&Doget_HP_Flag==1)
		  	{
					time_test=HP_error__time[HP__error_count-1]-HP_error__time[HP__error_count-15];
		     	if((HP_error__time[HP__error_count-1]-HP_error__time[HP__error_count-15]<60)&&(HP_error__time[HP__error_count-1]-HP_error__time[HP__error_count-5]>6))
		    	{
			      Dodeg_Delay_Count = 0; //躲避计数清零
			    	DodgeTarget_Flag = 1;
				  	Doget_HP_Flag=0;
			    }
		    }
				
				//被英雄第二次攻击，再次进入躲避状态（轨道中间段）//
				 Doget_bleed_time++;
		  	if(Doget_bleed_time<60&&Doget_bleed_time>10)
			  {
			  	if(HP_error<=-100) 
			    {
	      	Random_change_flag=1;			
			  	Doget_Change_Flag=1;			
			    }		
			  }
				//被英雄攻击一次，进入躲避状态 （轨道两边段）//
		  	if(HP_error<=-100) 
		  	{
	      	Dodeg_Delay_Count = 0; //躲避计数清零
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
			
			//血量少于120，直接进入躲避状态  在躲避状态被攻击时先是躲避到两边段轨道10秒，之后20秒全轨道乱跑（被击打也不会去两边段）20秒之后可以再次进入两边段躲避//
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
			
			if(HP_error<=-10 && HP_error>=-20) //被小弹丸攻击一次
			{
				Attacked_Flag = 1; //被攻击标志变为1
			}
		lastRobotHP = RobotHP;		
	  } 
		
		
	}
/**********************
功能：被攻击状况监控
参数：
	
**********************/
	
void Attacked_Monitor(void)
{
	
	Amor_ID = robotHurt.armorType;    //1号为正面装甲，0号为背面装甲
	if(Attacked_Flag == 0)  //在响应当次装甲板攻击时，不会再响应新的伤害，防止疯掉
	{
//		Amor_ID = robotHurt.armorType;
		Yaw_encoder = GMYawEncoder.ecd_angle;//-Yaw_encoder_s;
		YAW_Round_Cnt   = (int)(Yaw_encoder)/360;
		YAW_Angle_YuShu = (((int)(Yaw_encoder)%360)+360)%360;

			if(Amor_ID == 0) //0号装甲被攻击 前装甲板
			{

					
				if(YAW_Angle_YuShu>=240 && YAW_Angle_YuShu<=335)   //左后半面    
					{
						Attacked_YAW_Pos_Ref = YAW_Round_Cnt*360+90;//YAW_Round_Cnt*1852 -223;
					}
				else if(YAW_Angle_YuShu>=155&& YAW_Angle_YuShu<=240)   //右后半面    
				   {
						Attacked_YAW_Pos_Ref = YAW_Round_Cnt*360+180;//YAW_Round_Cnt*1852 -223;
				   }

			}
			else if(Amor_ID == 1) //1号装甲被攻击
			{
				if(YAW_Angle_YuShu>=335&& YAW_Angle_YuShu<=60)   //左前半面    
					{
						Attacked_YAW_Pos_Ref = YAW_Round_Cnt*360-90;//YAW_Round_Cnt*1852 -223;
					}
				else if(YAW_Angle_YuShu>=60&& YAW_Angle_YuShu<=155)   //右前半面   
				   {
						Attacked_YAW_Pos_Ref = YAW_Round_Cnt*360-180;//YAW_Round_Cnt*1852 -223;
				   }

			}
		
		if(Attacked_YAW_Pos_Ref+Yaw_encoder_s - (-GMYawEncoder.ecd_angle)>=0)
			{
				Attacked_YAW_Rotation_Dir = 1;   //向位置环参考值增大的方向转
			}
			else
			{
				Attacked_YAW_Rotation_Dir = 2;  //向位置环参考值减小的方向转
			}
			Hurt_COUNT++;
	}
}

/***************************************************************************
功能：枪口热量计算
入口参数：17mm弹丸射速
出口参数：无
***************************************************************************/
void BulletSpeed_Monitor(void)
{
 
				
				/***根据弹丸实际射速实时调整最小摩擦轮速度***/
	if(Target_Distance<=30)
	{
		if(Bullet_17_real_Speed - Bullet_Speed_Ref.low_speed>1)       //实际射速大于目标射速
		{
	 		Friction_Speed.low_speed = Friction_Speed.low_speed - 10;
		}
		else if(Bullet_17_real_Speed - Bullet_Speed_Ref.low_speed<-1) //实际射速小于目标射速
		{
			Friction_Speed.low_speed = Friction_Speed.low_speed + 10;
		}
	}
				/***根据弹丸实际射速实时调整最大摩擦轮速度***/
	if(Target_Distance>30)  
	{
		if(Bullet_17_real_Speed - Bullet_Speed_Ref.high_speed>1)       //实际射速大于目标射速
		{
	  	if(Bullet_17_real_Speed - Bullet_Speed_Ref.high_speed<3)
			{
	 		   Friction_Speed.high_speed = Friction_Speed.high_speed - 10;
			}
			else
			{
							/***
							速度超过28m/s一次，三档摩擦轮速度均减去100，保证不超热量
							***/
		    	Friction_Speed.low_speed = Friction_Speed.low_speed - 100;
					Friction_Speed.high_speed = Friction_Speed.high_speed - 100;
							
					if(Friction_Speed.low_speed<100) Friction_Speed.low_speed = 100;
			}
	}
	else if(Bullet_17_real_Speed - Bullet_Speed_Ref.high_speed<-1) //实际射速小于目标射速
		{
  			Friction_Speed.high_speed = Friction_Speed.high_speed + 10;
		}
  }
}

/***********************************************************
功能：枪口热量冷却计算
入口参数：无
出口参数：无
备注：10Hz冷却线程
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
