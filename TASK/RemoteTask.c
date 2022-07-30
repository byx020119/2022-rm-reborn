//注释了波轮和摩擦轮，无报错

#include "RemoteTask.h"
#include "stdio.h"
#include "SpeedTask.h"
#include "WorkState.h"
#include "ShootingTask.h"

RC_Ctl_t RC_CtrlData;
double pitch_err = 0;//2022加，p轴数据修正,传给视觉的
int pitch_err_flag = 0;//2022加 p轴数据修正标志位，只有标志位为1或-1且ch0为0时，pitch_err值加或减一个单位
char mini_pc = 'I';
/**
函数：RemoteDataPrcess(pData)
功能：对遥控器数据进行处理
备注：
        1                               1
   (s1) 3                          (s2) 3
	      2                               2
				
			(ch3)              (ch1)
       |                  |
       |                  |
	-----------(ch2)  -------------(ch0)     
			 |                  |
			 |                  |
		 遥控器测试模式时s2最上：
		 1.ch1控制底盘前后移动
		 2.ch2控制yaw水平旋转(累加、限位)
		 3.ch3控制pitch竖直摆动(累加、限位)
		 4.s1扳到最下面，全关,刹车回正
		 5.s2扳到中间，摩擦轮开，拨轮关
		 6.s2扳到最上，全开
		 
		 //遥控器测试模式s2=1：
		 1.ch1控制底盘前后移动
		 2.ch2控制yaw水平旋转(累加、限位)
		 3.ch3控制pitch竖直摆动(累加、限位)
		 4.s2=2,停止状态 s2=3,测试状态 s2=1,准备状态，2s后进入自由状态
		 5.s1控制刹车块
		 
		 遥控器集停：
		 1.左拨杆最上为关小电脑
		 
		 服务器模式：
		 1.右拨杆最上为集停
		 2.左右拨杆中间为关小电脑
		 
		 
**/
void RemoteDataPrcess(uint8_t *pData)
{

	if(pData == NULL)
	{
			return;
	}

//	GetRemoteSwitchAction(switch1, s1);
//	RemoteTest_Flag = 1;//遥控器打开时，遥控器测试标志变为1。
	Remote_microsecond.time_last = Get_Time_Micros();
	
	//ch0~ch3:max=1684,min=364,|error|=660
	RC_CtrlData.rc.ch0 = ((int16_t)pData[0] | ((int16_t)pData[1] << 8)) & 0x07FF; 
	RC_CtrlData.rc.ch1 = (((int16_t)pData[1] >> 3) | ((int16_t)pData[2] << 5)) & 0x07FF;
	RC_CtrlData.rc.ch2 = (((int16_t)pData[2] >> 6) | ((int16_t)pData[3] << 2) | ((int16_t)pData[4] << 10)) & 0x07FF;
	RC_CtrlData.rc.ch3 = (((int16_t)pData[4] >> 1) | ((int16_t)pData[5]<<7)) & 0x07FF;
	
	RC_CtrlData.rc.s1 = ((pData[5] >> 4) & 0x000C) >> 2;
	RC_CtrlData.rc.s2 = ((pData[5] >> 4) & 0x0003);
	
  RC_CtrlData.mouse.x = ((int16_t)pData[6]) | ((int16_t)pData[7] << 8);//鼠标左右
	RC_CtrlData.mouse.y = ((int16_t)pData[8]) | ((int16_t)pData[9] << 8);//鼠标上下
	RC_CtrlData.mouse.z = ((int16_t)pData[10]) | ((int16_t)pData[11] << 8);    //没用到

	RC_CtrlData.mouse.press_l = pData[12];//鼠标左键
	RC_CtrlData.mouse.press_r = pData[13];//鼠标右键

	RC_CtrlData.key.v = ((int16_t)pData[14]) | ((int16_t)pData[15] << 8);//每一位对应一个按键
	
	Chassis_Speed_Ref = (RC_CtrlData.rc.ch1-1024);
	GimbalRef.pitch_angle_dynamic_ref +=(RC_CtrlData.rc.ch3-1024)*0.003;//*0.01   /*原0.005*/
	GimbalRef.yaw_angle_dynamic_ref  +=(RC_CtrlData.rc.ch2-1024)*0.00285;//0.0085
	
	/****************修正数据函数************************/
	if((RC_CtrlData.rc.ch0-1024)>0){
		pitch_err_flag = 1;
	}
	else if((RC_CtrlData.rc.ch0-1024)<0){
		pitch_err_flag = -1;
	}
	if(pitch_err_flag==1&&(RC_CtrlData.rc.ch0-1024)<=0){
		pitch_err_flag=0;
		pitch_err+=0.5;
	}
	else if(pitch_err_flag==-1&&(RC_CtrlData.rc.ch0-1024)>=0){
		pitch_err_flag=0;
		pitch_err-=0.5;
	}
	
	/****************************************************/
	
//	if((RC_CtrlData.rc.s1==2)&&(GetFrictionState()==FRICTION_WHEEL_OFF))//打开摩擦轮

	
	//测发射延迟使用
//	if(RC_CtrlData.mouse.press_l==1 && RC_CtrlData.mouse.last_press_l ==0)
//	{
//		SetShootState(SHOOTING);
//		count_temp++;
//	}
//	RC_CtrlData.mouse.last_press_l = RC_CtrlData.mouse.press_l;
	
	if(RC_CtrlData.rc.s2==1)
	{
		RemoteTest_Flag = 2;//遥控器S2拨杆拨到1位置时，遥控器标志变为2，与工作状态对应
	}
	
	if(RC_CtrlData.rc.s2==3)
	{
		RemoteTest_Flag = 1;//遥控器打开时，遥控器测试标志变为1。拨杆拨到中间位置
	}
	
	if(RC_CtrlData.rc.s2==2)
	{
		RemoteTest_Flag = 0;//遥控器关闭时，遥控器测试状态变为0。拨杆拨到最下面
		Brake_flag=0;//刹车回正
	}

	
	
 if(RC_CtrlData.rc.s2==1)//测试状态中，拨杆拨到最上面
 {
	 
	 if(RC_CtrlData.rc.s1==3|| RC_CtrlData.rc.s1==1)//打开摩擦轮，1.3
	 {
			friction_wheel_state_flag = 1;
	 }
	
	
 	 if(RC_CtrlData.rc.s1==2)  //关闭摩擦轮 2 刹车模块置于平衡
	 {
			friction_wheel_state_flag = 0;
		  
	 }  
	
	 if(RC_CtrlData.rc.s1==1)//打开波轮 1
	 {
			TempShootingFlag=1;
	 }
	

	 if(RC_CtrlData.rc.s1==2||RC_CtrlData.rc.s1==3)//关闭波轮 2.3
	 {
		  TempShootingFlag=0;
	 }
	 switch(RC_CtrlData.rc.s1)//刹车模块位置测试（方向从板子方向向电池方向看）
	 {
		 case 2://(中间)
		 { 
			 Brake_flag=0;
			 break;
		 }
		 case 3://（左面）
		 {			
			 Brake_flag=-1;
			 break;
		 }
		 case 1://(最右面)
		 {
			 Brake_flag=1;
			 break;
		 }
	 }
	 
	
}	
 
 if(RC_CtrlData.rc.s2==3)//准备->自由，之后可进行模式的转换  左边3是自由状态
 {
 	 if(RC_CtrlData.rc.s1== 3)
	 {
		 mini_pc = 'O';
	 }
	 else{
		 mini_pc = 'I';
	 }
//	 Attacked_Flag = 0;
//   CameraDetectTarget_Flag = 0;
//	 DodgeTarget_Flag = 0;
	}
	 

 }
 
