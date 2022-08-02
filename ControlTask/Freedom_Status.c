//差encoder文件，注释了云台内容,无报错

#include "Freedom_Status.h"
#include "ControlTask.h"
#include "WorkState.h"
#include "SpeedTask.h"
#include "encoder.h"
#include "stdlib.h"
#include "main.h"

float YawCurrentPositionSave   = 0.0f;   //保存当前Yaw轴位置
float PitchCurrentPositionSave = 0.0f;   //保存当前Pitch轴位置
float YawInitPositionSave = 0.0f;

uint8_t DetectTargetFinish_Flag = 0;          //自由状态下，yaw看到目标一闪而过时，在该位置附近巡逻标志
uint8_t RotationDir_Flag=0;                   //精巡逻过程中的旋转方向标志
uint8_t Chassis_Change_Dir_Flag = 0;          //底盘换向标志
uint8_t Chassis_Speed_UP_Flag   = 0;          //底盘加速标志
uint8_t Chassis_Speed_DOWN_Flag = 0;          //底盘减速标志
int16_t  GimbalRefTest =0;               //什么标志
int32_t YAW_Round_Cnt     = 0;           //整数
int16_t YAW_Angle_YuShu   = 0;           //余数
int32_t Attacked_YAW_Pos_Ref = 0;        //什么标志 被攻击时，yaw轴的位置
uint8_t Attacked_YAW_Rotation_Dir = 0;        //被攻击时，YAW与被攻击装甲的方向
uint8_t Chassis_Freedom_i  = 0;               //什么标志
int Yaw_rotate_flag=0;                   //y轴自由旋转方向标志
int pitch_rotate_flag=0;                 //p轴自由旋转方向标志
int count_i =0;                          //对什么计数
int attack_dally_count = 0;
int Speed_change=0;
int32_t time_CNT_i = 0;
float TimeCount = 0;
int Recognition_YAW_Rotation_Dir=0;      //固定摄像头识别时，云台需要旋转的方向
int32_t Recognition_YAW_Pos_Ref = 0;     //什么标志 识别状态yaw轴的位置
int Freedom_Rotation_flag=0;             //自由状态 0未识别到对方装甲板 1识别到对方装甲板
int32_t Recognition_Delay_count = 0;     //识别状态计数初值为0
int32_t time_track_left = 0;  //跑导轨计时
int32_t time_track_left_max = 0;  
int32_t time_track_right = 0;  //跑导轨计时
int32_t time_track_right_max = 0;

double yaw_ecd_angle_flag =0;             //2022加  自由模式y轴一侧限位值
double yaw_ecd_angle_flag1 =200;          //2022加  自由模式y轴另一侧限位值
const float Chassis_speed=0;						//2022加  底盘电机速度常量
const float Chassis_Position=2000;        //2022加  底盘电机位置常量
double freedom_randspeed = 0;             //2022加  自由模式曲线变化量,暂时没有用到
int freedom_randspeed_flag = 0;        //2022加  自由模式曲线变化量标志，暂时没有用到 
/***
函数：YawFreeRoation()
功能：自由状态时YAW自由旋转
***/
void YawFreeRoation(void)
{
	if(GetWorkState()==Freedom_STATE)//粗巡逻状态 自由状态
	{
		if(GMPitchEncoder.ecd_angle-37>=0)   //38//35//30
		{
		pitch_rotate_flag = 1;   //转向在某个位置，p轴转向标志位1？
		}
		if(GMPitchEncoder.ecd_angle-3<=0)//-3
		{
		pitch_rotate_flag=0;    //转向在某个位置，p轴转向标志位0？
		}
		if(pitch_rotate_flag==1)
		{
	  GimbalRef.pitch_angle_dynamic_ref=GimbalRef.pitch_angle_dynamic_ref + 0.1f;   //p轴标志位1，p轴转向角度+0.1？
		}
		if(pitch_rotate_flag==0)
		{
	  GimbalRef.pitch_angle_dynamic_ref=GimbalRef.pitch_angle_dynamic_ref - 0.1f;   //p轴标志位0，p轴转向角度-0.1？
		}
		
		
		
		
		if(Freedom_Rotation_flag==0)
		{
			if(GMYawEncoder.ecd_angle-yaw_ecd_angle_flag>=0)//Y在一个范围内不断加减，实现转圈 半圈
			{
			Yaw_rotate_flag=1;
			}
			if(GMYawEncoder.ecd_angle+yaw_ecd_angle_flag1<=0)
			{
			Yaw_rotate_flag=0;
			}
			if(Yaw_rotate_flag==1)
			{
			GimbalRef.yaw_angle_dynamic_ref=GimbalRef.yaw_angle_dynamic_ref - 0.2f;//0.2//1
			}
			if(Yaw_rotate_flag==0)
			{
			GimbalRef.yaw_angle_dynamic_ref=GimbalRef.yaw_angle_dynamic_ref + 0.2f;//0.2//1
			}
   	}

		
		
		
   

	}
	
	//被攻击时云台状态
	if(GetWorkState() == Attacked_STATE)
	{
		attack_dally_count++;
		if(attack_dally_count>5000)
		{
		  attack_dally_count=0;
			Attacked_Flag=0;
		}
		
		
		if(GMPitchEncoder.ecd_angle-20>0)
		{
		pitch_rotate_flag=1;
		}
		if(GMPitchEncoder.ecd_angle+10<0)
		{
		pitch_rotate_flag=0;
		}
		if(pitch_rotate_flag==1)
		{
	  GimbalRef.pitch_angle_dynamic_ref=GimbalRef.pitch_angle_dynamic_ref + 0.07f;//点头慢点
		}
		if(pitch_rotate_flag==0)
		{
	  GimbalRef.pitch_angle_dynamic_ref=GimbalRef.pitch_angle_dynamic_ref - 0.07f;
		}
		
		
			if(Attacked_YAW_Rotation_Dir == 1)
			{
				GimbalRef.yaw_angle_dynamic_ref=GimbalRef.yaw_angle_dynamic_ref+0.8f;//0.4f
				if(GimbalRef.yaw_angle_dynamic_ref - Attacked_YAW_Pos_Ref>0)
				{
					GimbalRef.yaw_angle_dynamic_ref = Attacked_YAW_Pos_Ref;
					Attacked_Flag = 0;
					Attacked_YAW_Rotation_Dir = 0;
				}
			}
			else if(Attacked_YAW_Rotation_Dir == 2)
			{
				GimbalRef.yaw_angle_dynamic_ref=GimbalRef.yaw_angle_dynamic_ref-0.8f;//0.1f
				if(GimbalRef.yaw_angle_dynamic_ref - Attacked_YAW_Pos_Ref<0)
				{
					GimbalRef.yaw_angle_dynamic_ref = Attacked_YAW_Pos_Ref;
					Attacked_Flag = 0;
					Attacked_YAW_Rotation_Dir = 0;
				}
			}
			else if(Attacked_YAW_Rotation_Dir == 0)  //参考值到达目标位置后，等待电机到位
			{
				if(abs(GimbalRef.yaw_angle_dynamic_ref - (-GMYawEncoder.ecd_angle))<1)
				{
					Freedom_Flag = 1;
				}
			}
	  
		//如果云台在摆向装甲板的过程中识别到目标，那么立刻结束该过程，等待进入识别模式
		if(CameraDetectTarget_Flag ==1)
		{
			Attacked_YAW_Rotation_Dir = 0;
			Attacked_Flag = 0;
			CameraDetectTarget_Flag = 1;
		}
	}
	
	
}
/***
函数：RandSpeed()
功能：在一个范围内随机给已经赋值的速度添加增量，达到曲线速度行驶
备注：无 
***/
int random_speed = 0;
int random_time = 0;
int speed_ref =0;

void RandSpeed(){
	//另一种曲线速度变化方式
//	int temp = 0;
//	temp = (int)rand()%2;
//	
//	if(freedom_randspeed > 300 )
//		freedom_randspeed_flag = 1;
//	else if (freedom_randspeed <= 0)
//		freedom_randspeed_flag = 0;
//	
//	if(freedom_randspeed_flag == 1)
//		freedom_randspeed -= temp;
//  else if(freedom_randspeed_flag == 0)
//		freedom_randspeed += temp;
	//ADRC曲线速度变化方式
	random_time++;
	if(random_time % 100 == 0)
	{
		random_speed = 200 + rand()%121;
		ADRC_SPEED_Controller.x2 = 0;
	}
	ADRC_Control_SPEED(&ADRC_SPEED_Controller,(float)random_speed);
	speed_ref = ADRC_SPEED_Controller.x1;
}

/***
函数：Chassis_Motion_Switch()
功能：停止的位置随机变化，巡逻模式底盘换向
备注：无
***/
void Chassis_Motion_Switch(void)
{
	RandSpeed();
	if(GetWorkState()== Freedom_STATE||GetWorkState()== ChariotRecognition_STATE||GetWorkState()== Attacked_STATE)   
	{
		if(Chassis_Power_On_Flag == 1)  //底盘已经上电
		{
			
			if(Dir_Change_Flag==1)//底盘换向标志
			{
			   switch(Chassis_Freedom_i)//底盘自由状态方向标志
				{
					case 0:
					{
						Chassis_Position_Ref = Chassis_Position;//底盘位置赋值
						time_track_left++;
						time_track_right_max = time_track_right;
						Dir_Change_Flag=0;//避免刷新时重复换向
					};break;
					case 1:
					{
						Chassis_Position_Ref = -Chassis_Position;
            time_track_right++;	
            time_track_left_max = time_track_left;						
						Dir_Change_Flag=0;
					};break;
				}
			}
		}
		if(Chassis_Power_On_Flag == 1)  //底盘已经上电
		{
			
			if(abs(Chassis_Position_Ref - CM1Encoder.ecd_angle)<5)
			{
				switch(Chassis_Freedom_i)//底盘运动方向
				{
					GimbalRefTest++;//记录什么
					case 0:
					{
						Chassis_Position_Ref = Chassis_Position;//底盘位置赋值
					};break;
					case 1:
					{
						Chassis_Position_Ref = -Chassis_Position; 
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
				 if(Chassis_Position_Ref > CM1Encoder.ecd_angle)//光电管出问题，一直在一个方向上，就反向
				   {
					   Chassis_Temp_Speed = -speed_ref;//-(Chassis_speed+freedom_randspeed) ;
				   }
				 else if(Chassis_Position_Ref < CM1Encoder.ecd_angle)
				   {
					   Chassis_Temp_Speed =  speed_ref;//Chassis_speed+freedom_randspeed ;
				   }
			  }
				if(Speed_change==1)
				{
				 if(Chassis_Position_Ref > CM1Encoder.ecd_angle)
				   {
					   Chassis_Temp_Speed = -speed_ref;//-(Chassis_speed+freedom_randspeed) ;
				   }
				 else if(Chassis_Position_Ref < CM1Encoder.ecd_angle)
				   {
					   Chassis_Temp_Speed = speed_ref;//Chassis_speed+freedom_randspeed ;
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

