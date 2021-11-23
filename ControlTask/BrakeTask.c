#include "BrakeTask.h"
#include "Phototube.h"
#include "SpeedTask.h"
#include "main.h"



/***
功能：刹车电机赋值，检测光电管状态及底盘电机状态，修改刹车电机位置标志
备注：无
***/
void Brake_Handle()
{
	//光电管检测刹车
	if(Phototube_brake==1)//左侧检测换向
	{
		Brake_flag=-1;
		Phototube_brake=0;//光电管检测刹车标志清零
		brake_limt.time_now = Get_Time_Micros();//TIM2->CNT
	  brake_limt.time_error = brake_limt.time_now - brake_limt.time_last;
		if(brake_limt.time_error>1000000)
		{
			Brake_flag=0;
			brake_limt.time_last=Get_Time_Micros();
		}
		
	}
	if(Phototube_brake==1)//右侧检测换向
	{
		Brake_flag=1;
		Phototube_brake=0;
		brake_limt.time_now = Get_Time_Micros();//TIM2->CNT
	  brake_limt.time_error = brake_limt.time_now - brake_limt.time_last;
		if(brake_limt.time_error>1000000)
		{
			Brake_flag=0;
			brake_limt.time_last=Get_Time_Micros();
		}
		
	}
}
