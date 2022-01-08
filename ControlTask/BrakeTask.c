#include "BrakeTask.h"
#include "Phototube.h"
#include "SpeedTask.h"
#include "main.h"

int change_brake=0;

/***
功能：刹车电机赋值，检测底盘电机状态，修改刹车电机位置标志
备注：无
***/
void Brake_Handle(void)
{
	//通过上一次的ref值及本次ref值判断是否刹车
	if(RC_CtrlData.rc.s2!=1&&RC_CtrlData.rc.s2!=2)//测试以及停止状态下不进此函数
		{
			if(((last_Chassis_Temp_Speed<0&&Chassis_Temp_Speed >0)||(last_Chassis_Temp_Speed<0&&Chassis_Temp_Speed ==0))&&change_brake==0)//左侧检测换向
			{
				Brake_flag=-1;

				change_brake=1;
				
				if(brake_limt.time_error==0)brake_limt.time_last=Get_Time_Micros();		 
		
			}
			if(((last_Chassis_Temp_Speed>0&&Chassis_Temp_Speed <0)||(last_Chassis_Temp_Speed>0&&Chassis_Temp_Speed ==0))&&change_brake==0)//右侧检测换向
			{
				Brake_flag=1;
			
				change_brake=1;
				
				if(brake_limt.time_error==0)brake_limt.time_last=Get_Time_Micros();
		
			}
			if(change_brake==1)
			{
			 brake_limt.time_now = Get_Time_Micros();//TIM2->CNT
		 	 brake_limt.time_error = brake_limt.time_now - brake_limt.time_last;
			 if(brake_limt.time_error>500000)//设置刹车时间 单位us
	   		{
					Brake_flag=0;
					brake_limt.time_error=0;
					change_brake=0;
		  	}
			}
		}

	
}
