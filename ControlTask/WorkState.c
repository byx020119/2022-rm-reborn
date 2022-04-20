//注释了部分云台内容

#include "WorkState.h"
#include "ControlTask.h"
#include "SpeedTask.h"
#include "math.h"
#include "CanBusTask.h"
#include "Freedom_Status.h"
#include "uart1.h"

WorkState_e lastWorkState = STOP_STATE;   //枚举变量初始化
WorkState_e workState = STOP_STATE;       //枚举变量初始化，可以写在函数的外边

uint8_t CameraDetectTarget_Flag = 0;   //摄像头发现目标标志
uint8_t DodgeTarget_Flag        = 0;   //躲避目标标志
uint8_t RemoteTest_Flag         = 0;   //遥控器测试标志
uint8_t Waiting_Flag            = 0;   //等待标志，自由状态向识别状态切换时，先进入等待状态，使云台立刻停下
uint8_t Chassis_Power_On_Flag   = 0;   //底盘上电标志
uint8_t Attacked_Flag           = 0;   //被攻击标志
uint8_t Freedom_Flag            = 0;   //自由状态转换标志

uint16_t Waiting_COUNT = 0;
uint32_t Dodge_time_count = 0;         //2022加躲避模式计时
/***
函数：WorkStateFSM()
功能：控制工作模式
备注：自由状态：底盘移动，Y轴旋转，P轴中值位置
      战车识别状态：P,Y轴按照摄像头传回的数据运动，底盘不动，开始发弹
***/
void WorkStateFSM(void)
{
	
	//能在测试状态和识别状态转换
	/////////////////////////////////////
	
	lastWorkState = workState;//更新当前工作状态
	switch(workState)
	{
		case PREPARE_STATE://准备状态
		{		
			if(time_tick_2ms > PREPARE_TIME_TICK_MS)//准备状态维持2s
			{
				workState = Freedom_STATE ;//测试躲避状态修改此处为： Dodeg_STATE 自由模式 ：Freedom_STATE;
			}
				
		}break;
		
		case Freedom_STATE:     //自由状态，底盘移动，yaw旋转
		{
      if(CameraDetectTarget_Flag == 1)   //摄像头识别，进入识别状态
			{
				workState = ChariotRecognition_STATE;
			}
				if(RemoteTest_Flag == 0)   //遥控器停止测试，进入停止状态
			{
				workState = STOP_STATE;	
			}
			if(RemoteTest_Flag == 2)   //遥控器开始测试，进入测试状态
			{
				workState = Test_STATE;	
			}
			if(Attacked_Flag == 1)   //巡逻时，装甲被攻击，变为被攻击状态
			{
				workState = Attacked_STATE;
				          
			}
			 if(DodgeTarget_Flag == 1)		//血量减少太快或血量过少，变为躲避状态
			{
				workState = Dodeg_STATE;  
			}			
		}break;
		
		
		case Attacked_STATE:    //被攻击状态
		{
		  if(Attacked_Flag  == 0)		//转向被攻击装甲板过程结束变为自由状态
			{
				workState = Freedom_STATE;  //自由状态
			}		
			if(CameraDetectTarget_Flag == 1)   //转向过程中摄像头识别到目标，变为识别状态
			{
				workState = ChariotRecognition_STATE;
			}
			if(RemoteTest_Flag == 0)   //遥控器停止测试，进入自动模式下的停止状态
			{
				workState = STOP_STATE;
			}	
			if(DodgeTarget_Flag == 1)		//血量减少太快或血量过少，变为躲避状态
			{
				workState = Dodeg_STATE;  
			}	
		}break;
		
		case Test_STATE:    //遥控器测试状态
		{
			if(RemoteTest_Flag == 0)   //遥控器停止测试，进入自动模式下的停止状态
			{
				workState = STOP_STATE;
			}		
      if(CameraDetectTarget_Flag == 1)   //摄像头识别到目标，变为识别状态
			{
				workState = ChariotRecognition_STATE;
			}			
		}break;
		
		case ChariotRecognition_STATE:      //战车识别状态
		{	
			if(RemoteTest_Flag == 0)   //遥控器停止测试，进入自动模式下的停止状态
			{
				workState = STOP_STATE;
			}	
			 if(CameraDetectTarget_Flag == 0)   //摄像头未识别到目标，进入自由状态
			{
				workState = Freedom_STATE;
			}	
			 if(DodgeTarget_Flag == 1)		//血量减少太快，变为躲避状态
			{
				workState = Dodeg_STATE;
			}		
						
		}break;
		
		case Dodeg_STATE:      //躲避状态
		{	
      if(DodgeTarget_Flag == 0)		//躲避结束，变为自由状态
			{
				 workState = Freedom_STATE;  
			}				
			if(RemoteTest_Flag == 2)   //进入测试状态
			{
				workState = Test_STATE;	
			}
			if(RemoteTest_Flag == 0)   //遥控器停止测试
			{
				workState = STOP_STATE;
			}	
		}break;
		
		
		
		case STOP_STATE:   //停止状态
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
函数：WorkStateSwitchProcess()
功能：工作状态切换时，更改运行参数
***/
void WorkStateSwitchProcess(void)
{
	/***
	  状态变化：其他状态变为准备状态，
	  操    作：重新初始化控制环
	  备    注：测试状态变为准备状态时，以当前位置的yaw码盘值初始化yaw位置环参考值
	***/
	if((lastWorkState != PREPARE_STATE) && (workState == PREPARE_STATE))  
	{
		YawCurrentPositionSave = GMYawEncoder.ecd_angle;
		YawInitPositionSave = GMYawEncoder.ecd_angle;  //Y轴的初始位置，保证自由模式Y轴旋转方向和角度始终不变
		ControtLoopTaskInit();//重新初始化控制环
	}
		/***
	  状态变化：其他状态变为躲避状态   2022加
	  操    作：计时标志，到一定时间躲避清零，另一部分在躲避模式
	  备    注：无
	***/
	if((lastWorkState != Dodeg_STATE) && (workState == Dodeg_STATE))  
	{	
		Dodge_time_count = time_tick_2ms;
	}
	/***
	  状态变化：准备状态变为自由状态
	  操    作：开摩擦轮，不发弹
	  备    注：无
	***/
	if((lastWorkState == PREPARE_STATE) && (workState == Freedom_STATE))  
	{
		YawCurrentPositionSave = GMYawEncoder.ecd_angle;           //保存当前yaw码盘值
		GimbalRef.yaw_angle_dynamic_ref = YawCurrentPositionSave;
		PitchCurrentPositionSave= -GMPitchEncoder.ecd_angle;           //保存当前pitch码盘值
		GimbalRef.pitch_angle_dynamic_ref = PitchCurrentPositionSave;
		Last_Dodeg_STATE_Change = 0;	
	}
	

	
	/***
	  状态变化：识别状态变为自由状态
	  操    作：
	  备    注：无
	***/
	if((lastWorkState == ChariotRecognition_STATE) && (workState == Freedom_STATE))  
	{
		YawCurrentPositionSave = GMYawEncoder.ecd_angle;           //保存当前yaw码盘值
		GimbalRef.yaw_angle_dynamic_ref = YawCurrentPositionSave;
		PitchCurrentPositionSave= -GMPitchEncoder.ecd_angle;           //保存当前pitch码盘值
		GimbalRef.pitch_angle_dynamic_ref = PitchCurrentPositionSave;
	}
	
	/***
	  状态变化: 躲避状态变为自由状态
	  操    作：
	  备    注：无
	***/
	if((lastWorkState == Dodeg_STATE) && (workState == Freedom_STATE))  
	{
		Chassis_Position_Ref = Chassis_Position_Ref/fabs(Chassis_Position_Ref)*10000;
	}
	/***
	  状态变化：其他状态变为战车识别状态
	  操    作：开始发弹，yaw加入斜坡缓慢靠近目标
	  备    注：无
	***/
	if((lastWorkState != ChariotRecognition_STATE) && (workState == ChariotRecognition_STATE))  
	{
		GMYawRamp.SetScale(&GMYawRamp, 20); //50
		GMYawRamp.ResetCounter(&GMYawRamp);
		YawCurrentPositionSave = GMYawEncoder.ecd_angle;
		PitchCurrentPositionSave = - GMPitchEncoder.ecd_angle;
	}
		/***
	  状态变化：其他状态变为停止状态        2022加
	  操    作：被攻击标志位，被飞机攻击标志位，躲避标志位清零
	  备    注：无
	***/
	if((lastWorkState != STOP_STATE) && (workState == STOP_STATE))  
	{
		Attacked_Flag = 0;
		DodgeTarget_Flag = 0;

		
		YawCurrentPositionSave = GMYawEncoder.ecd_angle;
		YawInitPositionSave = GMYawEncoder.ecd_angle;  //Y轴的初始位置，保证自由模式Y轴旋转方向和角度始终不变
		ControtLoopTaskInit();//重新初始化控制环
	}

	
	
}
	


/***
函数：SetWorkState(WorkState_e state)
功能：返回当前工作状态
备注：整个工程没用过
***/
void SetWorkState(WorkState_e state)
{
	if(workState != PREPARE_STATE)
	{
    workState = state;
	}
}

/***
函数：WorkState_e GetWorkState(void)
功能：返回工作状态
备注：用return值返回工作状态
***/
WorkState_e GetWorkState(void)
{
	return workState;
}

WorkState_e GetlastWorkState(void)
{
	return lastWorkState;
}
