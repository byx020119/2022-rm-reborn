#include "main.h"
#include "ShootingTask.h"


/************************************
Speed_Friction    Real_Speed(m/s)
800               24.8
900               26.5        
      
*************************************/

/*************************************
1s发射弹丸数量     Vmax(m/s)
    6~10              7.2
    5                 14.4
    4                 14.4
    3                 21.6
    2                 36
    1                 72
*************************************/

/****************************************
420     25m/s      3
****************************************/

FrictionWheelState_e friction_wheel_state = FRICTION_WHEEL_OFF; //摩擦轮状态
FrictionWheelState_e friction_wheel_state_flag =0;                   //
uint32_t FrictionWheel_COUNT=0;                                 //摩擦轮启动延时
uint16_t Friction_Output_Speed = 350;                           //摩擦论速度,1000停止,2000最大
RampGen_t frictionRamp = RAMP_GEN_DAFAULT;                      //摩擦轮斜坡

Shoot_State_e shootState = NOSHOOTING; //波轮射击状态
int TempShootingFlag=0;                 //发弹标志,修改该标志，可选择是否发弹

//摩擦轮三种不同速度结构体初始化
#define FRICTION_SPEED_INIT_DEFAULT \
{\
	250,\
	250,\
}\

//子弹3种不同射速机构体初始化
#define BULLET_SPEED_REF_DEFAULT \
{\
	15,\
	25,\
}\

//子弹3种不同射频结构体初始化
#define BULLET_FREQ_REF_DEFAULT \
{\
	6,\
	6,\
}\

//子弹3种不同射速下的重力补偿结构体初始化
//6,3,3
#define Gravity_Add_Angle_DEFAULT \
{\
	5,\
	2,\
}\


Friction_Speed_t Friction_Speed       = FRICTION_SPEED_INIT_DEFAULT;  //摩擦轮3种不同速度结构体
Bullet_Speed_Ref_t Bullet_Speed_Ref   = BULLET_SPEED_REF_DEFAULT;     //子弹3种不同射速结构体
Bullet_Freq_Ref_t Bullet_Freq_Ref     = BULLET_FREQ_REF_DEFAULT;      //子弹3中不同射频结构体
uint16_t Bullet_17_real_Speed         = 0;                            //17mm弹丸实际射速
u8 Bullet_Ouput_Freq                  = 0;
Gravity_Add_Angle_t Gravity_Add_Angle = Gravity_Add_Angle_DEFAULT;    //三种弹丸射速下的重力补偿值


/***
函数：FrictionWheelControl()
功能：摩擦轮电机控制任务
***/
void FrictionWheelControl(void)
{
if(friction_wheel_state_flag ==1)
	{
//			if(Target_Distance<=30)
//				{
				CM3SpeedPID.ref =  -Friction_Speed.low_speed ;
			  CM4SpeedPID.ref = Friction_Speed.low_speed ;
//				}
//			else
//			  {
//				CM3SpeedPID.ref = -Friction_Speed.high_speed ;
//			  CM4SpeedPID.ref = Friction_Speed.high_speed ;
//					
//			  }	
	}
	
if(friction_wheel_state_flag ==0)
	{
				CM3SpeedPID.ref = 0;//底盘程序
  			CM4SpeedPID.ref = 0 ;
	}

}

/***
函数：BoLunMotorControl()
功能：波轮电机控制任务
***/
void BoLunMotorControl(void)
{
	
	/***
	1.只在识别状态、遥控器测试状态可以发弹
	2.枪口热量小于400才可以发弹
	***/
	if(GetWorkState() == ChariotRecognition_STATE || GetWorkState() == Test_STATE||GetWorkState() == Dodeg_STATE)  
	{
		if(Shooter_17_Heat<400)
		{
			if(TempShootingFlag==1)//打开波轮发弹
			{
				shot_frequency_limt.time_now = Get_Time_Micros();//TIM2->CNT
				shot_frequency_limt.time_error = shot_frequency_limt.time_now - shot_frequency_limt.time_last;
				if(shot_frequency_limt.time_error > (int)(650000/Bullet_Ouput_Freq))
				{
					count_temp++;
					shot_frequency_limt.time_last = Get_Time_Micros();
				}	
			}
	  }
  }
}


/***
函数：HeatControl_Task()
功能：枪口热量闭环
备注：远距离时，大速度小频率(>=4)
      中距离时，中速度中频率(2-4)
      近距离时，小速度大频率(<=2)
      Target_Distance为视觉传回的距离，Bullet_17_real_Speed是从裁判系统读出的弹丸射速
***/

void HeatControl_Task(void)
{
//	if(Target_Distance<=30)         //近距离
//	{
		Bullet_Ouput_Freq     = Bullet_Freq_Ref.low_speed;
//		Pitch_Add_Angle       = Gravity_Add_Angle.low_speed;
//	}
//	else                           //远距离
//	{
//		Bullet_Ouput_Freq     = Bullet_Freq_Ref.high_speed;
//		Pitch_Add_Angle       = Gravity_Add_Angle.high_speed;
//	}
//	if(Shooter_17_Heat>300) 
//	{
//		Bullet_Ouput_Freq = Bullet_Ouput_Freq-2;
//		Bullet_Ouput_Freq = Bullet_Ouput_Freq<=1 ? 1 : Bullet_Ouput_Freq;
//	}
}

