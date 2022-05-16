//差encoder文件，注释了云台内容,无报错

#include "Freedom_Status.h"
#include "ControlTask.h"
#include "WorkState.h"
#include "SpeedTask.h"
#include "encoder.h"
#include "stdlib.h"


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

double yaw_ecd_angle_flag = -230;    //-20;    //2022加  自由模式y轴一侧限位值
double yaw_ecd_angle_flag1 = -110;  //-150    //2022加  自由模式y轴另一侧限位值
const int chassis_speed = 50;               //2022加 自由模式底盘速度

/***
函数：YawFreeRoation()
功能：自由状态时YAW自由旋转
***/
void YawFreeRoation(void)
{
	if(GetWorkState()==Freedom_STATE)//粗巡逻状态 自由状态
	{
		if(GMPitchEncoder.ecd_angle - 15 >=0)   //38//35//30
		{
		pitch_rotate_flag = 1;   //转向在某个位置，p轴转向标志位1？
		}
		if(GMPitchEncoder.ecd_angle + 12 <=0)//-3
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
			if(GMYawEncoder.ecd_angle+yaw_ecd_angle_flag<=0)//Y在一个范围内不断加减，实现转圈 半圈
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
	
	//被攻击时云台状态
	if(GetWorkState() == Attacked_STATE)
	{
		attack_dally_count++;
		if(attack_dally_count>5000)
		{
		  attack_dally_count=0;
			Attacked_Flag=0;
		}
		
		if(GMPitchEncoder.ecd_angle +12 >0)
		{
		pitch_rotate_flag=1;
		}
		if(GMPitchEncoder.ecd_angle - 19<0)
		{
		pitch_rotate_flag=0;
		}
		if(pitch_rotate_flag==1)
		{
	  GimbalRef.pitch_angle_dynamic_ref=GimbalRef.pitch_angle_dynamic_ref - 0.07f;//点头慢点
		}
		if(pitch_rotate_flag==0)
		{
	  GimbalRef.pitch_angle_dynamic_ref=GimbalRef.pitch_angle_dynamic_ref + 0.07f;
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
函数：Chassis_Motion_Switch()
功能：停止的位置随机变化，巡逻模式底盘换向
备注：无
***/
void Chassis_Motion_Switch(void)
{
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
						Chassis_Position_Ref = 10000;//底盘位置赋值
						time_track_left++;
						time_track_right_max = time_track_right;
						Dir_Change_Flag=0;//避免刷新时重复换向
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
		if(Chassis_Power_On_Flag == 1)  //底盘已经上电
		{
			
			if(abs(Chassis_Position_Ref - CM1Encoder.ecd_angle)<5)
			{
				switch(Chassis_Freedom_i)//底盘运动方向
				{
					GimbalRefTest++;//记录什么
					case 0:
					{
						Chassis_Position_Ref = 10000;//底盘位置赋值
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
				 if(Chassis_Position_Ref > CM1Encoder.ecd_angle)//光电管出问题，一直在一个方向上，就反向
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

