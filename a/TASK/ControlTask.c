//注释了部分内容,包括云台

#include "ControlTask.h"
#include "Phototube.h"
#include "timer.h"
#include "SpeedTask.h"
#include "WorkState.h"
#include "Freedom_Status.h"
#include "Doget_state.h"
#include "ShootingTask.h"

uint32_t time_tick_2ms = 0;     //2ms刷新一次，初值为0
u16 adc_Value = 0;


/***
函数：Control_Task()
功能：控制环，2ms刷新一次
备注：无
***/
void Control_Task(void)
{
	time_tick_2ms += 2;								//记录运行时间
	WorkStateFSM();										//工作状态选择
	WorkStateSwitchProcess();					//检测工作状态切换
	
	/***云台控制任务***/
	YawFreeRoation();                 //自由状态时，云台自由旋转
  YawFreeRoation_Doget();           //躲避状态时，云台自由旋转
	GMYawControlLoop();								//计算Y轴电机输出量,0x205
	GMPitchControlLoop();							//计算P轴电机输出量,0x206
	
	
	/***发弹控制任务***/
	HeatControl_Task();
  FrictionWheelControl();             //摩擦轮拨弹电机控制任务
	BoLunMotorControl();
	ShooterMControlLoop();      		  //发射机构控制任务,0x207
	//SetGimbalMotorOutput();						//将云台、波轮、刹车电机输出量发送
	
	
  /***底盘控制任务***/
  Chassis_Motion_Switch();      //自由状态，底盘运动到边界后，换向(正交编码器)
  Chassis_Motion_Switch_Doget();//躲避状态，底盘运动到边界后，换向
	GraduallyChangeCMSpeed();     //底盘电机速度平滑换向
	//CMControlLoop();							//底盘控制任务 发送底盘电机输出量
  Phototube_Handle();           //光电管控制任务	
	


		
	/***遥控器测试模式切换***/
	Remote_microsecond.time_now = Get_Time_Micros();
	Remote_microsecond.time_error = Remote_microsecond.time_now - Remote_microsecond.time_last;
	if(Remote_microsecond.time_error>500000) RemoteTest_Flag = 0;
		
}

/***
函数：ControtLoopTaskInit()
功能：控制环初始化
备注：无
***/
void ControtLoopTaskInit(void)
{
	//计数初始化
	time_tick_2ms = 0;   
	
	//5个电机PID初始化
	//P、Y轴位置、速度pid初始化
	CM1SpeedPID.Reset(&CM1SpeedPID);
	CM2SpeedPID.Reset(&CM2SpeedPID);//没用
	CM3SpeedPID.Reset(&CM3SpeedPID);//   
	CM4SpeedPID.Reset(&CM4SpeedPID);
	
	GMPPositionPID.Reset(&GMPPositionPID);
	GMPSpeedPID.Reset(&GMPSpeedPID);
	GMYPositionPID.Reset(&GMYPositionPID);
	GMYSpeedPID.Reset(&GMYSpeedPID);
	
	CM7PositionPID.Reset(&CM7PositionPID);
	CM7SpeedPID.Reset(&CM7SpeedPID);
	
	CMRamp.SetScale(&CMRamp,400);
	CMRamp.ResetCounter(&CMRamp);
	
	//P,Y轴电机斜坡初始化
	GMPitchRamp.SetScale(&GMPitchRamp, PREPARE_TIME_TICK_MS); //设定斜坡步数
	GMYawRamp.SetScale(&GMYawRamp, PREPARE_TIME_TICK_MS);
	GMPitchRamp.ResetCounter(&GMPitchRamp);                   //斜坡计数值清零
	GMYawRamp.ResetCounter(&GMYawRamp);
	
	//云台给定值初始化
	/***
	  首次云台参考值初始化时，pitch、yaw为0
	  遥控器测试结束再次初始化时，pitch为0,yaw为当前电机码盘值，保证在自由状态时，yaw从当前位置旋转
	***/
	Chassis_Speed_Ref = 0;//底盘速度初始化
	GimbalRef.pitch_angle_dynamic_ref = 0.0f;
	GimbalRef.yaw_angle_dynamic_ref = YawCurrentPositionSave;
	
	//摩擦轮斜坡初始化
//	frictionRamp.SetScale(&frictionRamp, FRICTION_RAMP_TICK_COUNT);
//	frictionRamp.ResetCounter(&frictionRamp);
//	//摩擦轮运行状态初始化
//	InitFrictionWheel();
}

//GPIO初始化，可控电源
void engineerpower_Init()
{		GPIO_InitTypeDef gpio;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE); 
		
		gpio.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_2;//2、3、4、5管脚
		gpio.GPIO_Mode = GPIO_Mode_OUT;//输出模式
		gpio.GPIO_OType = GPIO_OType_PP;//推挽
		gpio.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
		gpio.GPIO_PuPd = GPIO_PuPd_UP;//初始上拉
		GPIO_Init(GPIOH, &gpio);
	
	GPIO_SetBits(GPIOH,GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_2);
}

