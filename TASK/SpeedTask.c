//注释了战车识别状态，云台内容等很多，无报错

#include "SpeedTask.h"
#include "WorkState.h"
#include "encoder.h"
#include "Phototube.h"
#include "Freedom_Status.h"
#include "common.h"
#include "math.h"
#include "ShootingTask.h"
#include "timer.h"
#include "usart3.h"
#include "usart6.h"

//调参这里PID
//PID结构体初始化
PID_Regulator CMPositionPID  = CHASSIS_MOTOR_POSITION_PID_DEFAULT;
PID_Regulator CM1SpeedPID    = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator CM2SpeedPID    = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator CM3SpeedPID    = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator CM4SpeedPID    = CHASSIS_MOTOR_SPEED_PID_DEFAULT;
PID_Regulator Speed_Offset   = CHASSIS_SPEED_OFFSET_DEFAULT;
PID_Regulator GMPPositionPID = GIMBAL_MOTOR_PITCH_POSITION_PID_DEFAULT; 
PID_Regulator GMPPositionPID1 = GIMBAL_MOTOR_PITCH_POSITION_PID_DEFAULT;  
PID_Regulator GMPSpeedPID 	 = GIMBAL_MOTOR_PITCH_SPEED_PID_DEFAULT;
PID_Regulator GMYPositionPID = GIMBAL_MOTOR_YAW_POSITION_PID_DEFAULT;			
PID_Regulator GMYSpeedPID 	 = GIMBAL_MOTOR_YAW_SPEED_PID_DEFAULT;

PID_Regulator CM7PositionPID = CM7_POSITION_PID_DEFAULT;
PID_Regulator CM7SpeedPID    = CM7_SPEED_PID_DEFAULT;

//pitch,yaw斜坡结构体初始化
RampGen_t GMPitchRamp = RAMP_GEN_DAFAULT;
RampGen_t GMYawRamp   = RAMP_GEN_DAFAULT;
RampGen_t CMRamp      = RAMP_GEN_DAFAULT;

Gimbal_Ref_t GimbalRef;             //定义双环控制参考值结构体（云台，刹车）刹车因为不经过角度控制，只经过标志位控制所以暂时未使用这个值
CM2_Ref_t CM2Ref;                   //定义单环控制参考值结构体（CM2）
int Brake_flag=0;                   //定义刹车标志位 1为水平时

float CMSpeedRate =1.0f;            //底盘电机速度系数
float Yaw_FeedForward = 0.0;        //yaw识别补偿值
float Chassis_Position_Ref = 0;     //底盘位置目标值
float last_Chassis_Temp_Speed = 0;  //上一次底盘临时速度
float Chassis_Temp_Speed      = 0;  //本次底盘临时速度
float last_Chassis_Temp_Speed_Save = 0;
float Chassis_Temp_Speed_Save = 0;
float Chassis_Speed_Ref       = 0;  //底盘实际速度参考值
u8 pitch_POS_Flag = 0;
float  test1=0;
float  GMY_Previous_Output=0;
float  GMY_Previous_Position_ref=0;
float  GMY_Previous_Position_fdb=0;
float  CR_yaw_Symbol=0;
float  CR_yaw_increment=0;
float  CR_Pitch_Symbol=0;
float  CR_Pitch_increment=0;
int    Dodeg_STATE_Change = 0;
int    Last_Dodeg_STATE_Change=0;
float  sbyaw=0;

int pitchRefFdbErrorFlag = 0;//      识别模式下p轴错位度数标志
/***
函数：GMBrakeControlLoop()
功能：利用PID计算出brake电机的输出量
备注：串级控制
***/

	 
/***
函数：GMPitchControlLoop()
功能：利用PID计算出pitch电机的输出量
备注：串级控制
***/
void GMPitchControlLoop(void)
{
    VAL_LIMIT(GimbalRef.pitch_angle_dynamic_ref , -20, 13);

	  //VAL_LIMIT(ChariotRecognition_pitch , -4, 32)//P轴限位95 145
	
	if(GetWorkState() == Dodeg_STATE && CameraDetectTarget_Flag ==1 )
	{
	 Dodeg_STATE_Change = 1;
	}
	if(GetWorkState() == Dodeg_STATE && CameraDetectTarget_Flag ==0 )
	{
	 Dodeg_STATE_Change = 2;
	}


	//战车识别状态
  if(GetWorkState() == ChariotRecognition_STATE )
	{ 

		CR_Pitch_Symbol= ChariotRecognition_pitch/fabs(ChariotRecognition_pitch);
		

//	if(fabs(ChariotRecognition_pitch)>1.5)
//	{
//		GMPPositionPID.ref = -(1-exp(-fabs(ChariotRecGMPPositionPID.Calc(&GMPPositionPID);   //得到pitch轴位置环输出控制量;
//		GMPPositionPID.fdb = (1-exp(-fabs(ChariotRecognition_pitch)))*(ChariotRecognition_pitch)*GMYawRamp.Calc(&GMYawRamp);
//	}
//		
//	if(fabs(ChariotRecognition_pitch)<1.5)
	{
		GMPPositionPID.ref = -ChariotRecognition_pitch;	//
		GMPPositionPID.fdb = GMPitchEncoder.ecd_angle;
		//是否错位判断
		if(abs(GMPPositionPID.ref-GMPPositionPID.fdb>5)){
			pitchRefFdbErrorFlag = 1;
		}
		else{
			pitchRefFdbErrorFlag = 0;
		}
	}
	
		GMPPositionPID.kp = 80+3*(1-exp(-0.1*fabs(GMPPositionPID.ref - GMPPositionPID.fdb)));//100+120*(1-exp(-0.1*fabs(GMPPositionPID.ref - GMPPositionPID.fdb)))
		GMPPositionPID.ki = 0.001;//0.001;//0.05//0.02//0.1;//0.001;
		GMPPositionPID.kd = 1;//6;//0//6
						
		GMPSpeedPID.kp = 100;//40;//1;//2.5//30
		GMPSpeedPID.ki = 0;
		GMPSpeedPID.kd = 2;//2;//0//2

			
		GMPPositionPID.Calc(&GMPPositionPID);   //得到pitch轴位置环输出控制量
		GMPPositionPID.output = GMPPositionPID.output;// - GMPitchEncoder.ecd_angle*6;//- GMPitchEncoder.ecd_angle*10;
				
	}


	//准备状态、自由状态、测试状态、被攻击状态和躲避状态
	if(GetWorkState() == PREPARE_STATE || GetWorkState() == Freedom_STATE || GetWorkState() == Test_STATE || GetWorkState() == Attacked_STATE  || Dodeg_STATE_Change == 2 )
	{	
		GMPPositionPID.kp = 40;//100;//40//60//80
		GMPPositionPID.ki = 0.01;//0.03
		GMPPositionPID.kd = 2;//0//2
			
		GMPSpeedPID.kp = 20;//3 30//20
		GMPSpeedPID.ki = 0;//0.002
		GMPSpeedPID.kd = 0;//0
		if(GetWorkState() == PREPARE_STATE||GetWorkState() == STOP_STATE)
		{
			
			GMPPositionPID.ref = 0.0f;   //GMPitchRamp.Calc(&GMPitchRamp);
			GMPPositionPID.fdb = GMPitchEncoder.ecd_angle* GMPitchRamp.Calc(&GMPitchRamp);
      GMPPositionPID.Calc(&GMPPositionPID);   //得到pitch轴位置环输出控制量			
		}
		else
		{
		  GMPPositionPID.ref = - GimbalRef.pitch_angle_dynamic_ref;
			GMPPositionPID.fdb = GMPitchEncoder.ecd_angle * GMPitchRamp.Calc(&GMPitchRamp); 
			GMPPositionPID.Calc(&GMPPositionPID);   //得到pitch轴位置环输出控制量
		}
					
	}
  //GMPPositionPID.Calc(&GMPPositionPID);   //得到pitch轴位置环输出控制量
	GMPSpeedPID.ref = 0.45*GMPPositionPID.output;
	GMPSpeedPID.fdb = 0.3*GMPitchEncoder.filter_rate;//-Gyro[1]/10;                     //p轴抖动修改系数
	GMPSpeedPID.Calc(&GMPSpeedPID);
	GMPSpeedPID.output =GMPSpeedPID.output+GMPSpeedPID.output/fabs(GMPSpeedPID.output);
	
//	GMPSpeedPID.output =GMPSpeedPID.output+GMPSpeedPID.output/fabs(GMPSpeedPID.output)*0.5;
}

/***
函数：GMYawControlLoop()
功能：利用PID计算出yaw电机的输出量
备注：串级控制
***/
void GMYawControlLoop(void)
{
	//战车识别状态
	if(GetWorkState() == ChariotRecognition_STATE || Dodeg_STATE_Change == 1)
	{
		test1= last_ChariotRecognition_yaw - ChariotRecognition_yaw;//当时调参测试加的变量

		
    Last_Dodeg_STATE_Change = Dodeg_STATE_Change;
		Dodeg_STATE_Change = 0;
		
		
		CR_yaw_Symbol = (ChariotRecognition_yaw)/fabs(ChariotRecognition_yaw);

////分情况调，拓展一下调参思路，数据实在感觉差点意思可以分段等方法		
//		if(fabs(ChariotRecognition_yaw)>1.2)//&& fabs(ChariotRecognition_yaw)<3 )
//		{
//		CR_yaw_increment = 6*CR_yaw_Symbol*(1-exp(-0.3*fabs(ChariotRecognition_yaw)))-0.5;//0.95   0.6
//		GMYPositionPID.ref = (1-exp(-fabs(ChariotRecognition_yaw)))*(ChariotRecognition_yaw) +CR_yaw_increment;
//		GMYPositionPID.fdb = -(1-exp(-fabs(ChariotRecognition_yaw)))*(ChariotRecognition_yaw)*GMYawRamp.Calc(&GMYawRamp);
//		}
//	
//		if(fabs(ChariotRecognition_yaw)<1.2)
//		{
//		CR_yaw_increment = 0.5*(ChariotRecognition_yaw)*(ChariotRecognition_yaw)*(ChariotRecognition_yaw);
//		GMYPositionPID.ref = (1-exp(-fabs(ChariotRecognition_yaw)))*(ChariotRecognition_yaw);//-CR_yaw_increment ;
//		GMYPositionPID.fdb = -(1-exp(-fabs(ChariotRecognition_yaw)))*(ChariotRecognition_yaw)*GMYawRamp.Calc(&GMYawRamp);
//		}
//		 if(fabs(ChariotRecognition_yaw)>1.2)
//		{
//		GMYPositionPID.ref = (1-exp(-fabs(ChariotRecognition_yaw)))*(ChariotRecognition_yaw);//-CR_yaw_increment ;
//		GMYPositionPID.fdb = -(1-exp(-fabs(ChariotRecognition_yaw)))*(ChariotRecognition_yaw)*GMYawRamp.Calc(&GMYawRamp);	
//		}
//		
//	   if(fabs(ChariotRecognition_yaw)<1.2)
	
	//调参待测
//		GMYPositionPID.ref = (1-exp(-fabs(ChariotRecognition_yaw)))*(ChariotRecognition_yaw);//-CR_yaw_increment ;
//		GMYPositionPID.fdb = -(1-exp(-fabs(ChariotRecognition_yaw)))*(ChariotRecognition_yaw)*GMYawRamp.Calc(&GMYawRamp);	
	
  	GMYPositionPID.ref = Eular[2]+ camera;//-CR_yaw_increment ;
		GMYPositionPID.fdb = Eular[2];//GMYawEncoder.ecd_angle;	

		GMYPositionPID.kp = 80+2*(1-exp(-0.3*fabs(GMYPositionPID.ref - GMYPositionPID.fdb)));//60+30//160+150*(1-exp(-0.3*fabs(GMYPositionPID.ref - GMYPositionPID.fdb)))
		GMYPositionPID.ki = 0.001*exp(-0.3*fabs(GMYPositionPID.ref - GMYPositionPID.fdb));//0.1;//0.005;//0.01*exp(-0.3*fabs(GMYPositionPID.ref - GMYPositionPID.fdb));//0.1//5
		GMYPositionPID.kd = 0;//5;//-5*(1-exp(-0.3*fabs(GMYPositionPID.ref - GMYPositionPID.fdb)));//10-5//3;//0;
		
		GMYSpeedPID.kp = 70;//65;//10//15
		GMYSpeedPID.ki = 0;// 0.001;//0.005;
		GMYSpeedPID.kd = 0;// 2;//1//2//5
		
		GMYPositionPID.Calc(&GMYPositionPID);    //得到yaw轴位置环输出控制量
		
		
     last_ChariotRecognition_yaw   = ChariotRecognition_yaw;
	
	}

	//准备状态
	if(GetWorkState() == PREPARE_STATE ||GetWorkState() == STOP_STATE  ) 
	{	
		GMYPositionPID.kp = 80;//0
		GMYPositionPID.ki =	0.01;//0
		GMYPositionPID.kd = 5;//0
			
		GMYSpeedPID.kp = 10;//0
		GMYSpeedPID.ki = 0;//0
		GMYSpeedPID.kd = 0;//0
		
		GMYPositionPID.ref = GMYawEncoder.ecd_angle ;
		GMYPositionPID.fdb = GMYawEncoder.ecd_angle;   
		GMYPositionPID.Calc(&GMYPositionPID); 
	}
	
	//测试状态
	if( GetWorkState() == Test_STATE) 
	{
		GMYPositionPID.kp = 80;//50;//60;//100
		GMYPositionPID.ki =	0.005;//0.05;//0.005
		GMYPositionPID.kd = 5;//100;5//0
			
		GMYSpeedPID.kp = 25;//10//5//50（会抖）
		GMYSpeedPID.ki = 0;//5
		GMYSpeedPID.kd = 0;//2
	
		GMYPositionPID.ref = GimbalRef.yaw_angle_dynamic_ref;
		GMYPositionPID.fdb = GMYawEncoder.ecd_angle;	
    if(time_tick_2ms%8!=0)		
		{GMYPositionPID.Calc(&GMYPositionPID);} 
	}
	
	//自由状态、被攻击状态、准备状态
	if(GetWorkState() ==Freedom_STATE || GetWorkState() ==Attacked_STATE ||Dodeg_STATE_Change ==2) 
	{
    Last_Dodeg_STATE_Change = Dodeg_STATE_Change;
		Dodeg_STATE_Change = 0;
		
		GMYPositionPID.kp = 80;//60//100
		GMYPositionPID.ki =	0.005;//0
		GMYPositionPID.kd = 0;//0
			
		GMYSpeedPID.kp = 30;//5//50
		GMYSpeedPID.ki = 0;//0.005;
		GMYSpeedPID.kd = 0;//2
			
		GMYPositionPID.ref = GimbalRef.yaw_angle_dynamic_ref;
		GMYPositionPID.fdb = GMYawEncoder.ecd_angle;   
		GMYPositionPID.Calc(&GMYPositionPID); 
	}
	
	GMYSpeedPID.ref = GMYPositionPID.output*0.5;                            //y轴抖动修改系数
	GMYSpeedPID.fdb = Gyro[2]/3;	//Gyro[2]/5; //GMYawEncoder.filter_rate;               //y轴抖动修改系数
	GMYSpeedPID.Calc(&GMYSpeedPID);
//		
}

/***
函数：SetGimbalMotorOutput()
功能：通过can将云台电机的输出量传给电机
备注：无
***/


void SetGimbalMotorOutput(void)
{
	//云台控制输出								
	if((GetWorkState() == STOP_STATE))   
	{
		Set_Gimbal_Current(CAN1, 0, 0, 0);     //yaw + pitch
	}
	//识别、躲避
	
		//自由,测试
	else
	{	
//		Set_Gimbal_Current(CAN1,2000,3000,(int16_t)(CM7SpeedPID.output));     //yaw + pitch+BoLunMotor	
  		Set_Gimbal_Current(CAN1,(int16_t)(GMYSpeedPID.output),(int16_t)(CM7SpeedPID.output), (int16_t)(GMPSpeedPID.output));     //yaw + pitch	
//		  Set_Gimbal_Current1(CAN2,(int16_t)(CM7SpeedPID.output));   //BoLunMotor + Brake	,0

		//  	  Set_Gimbal_Current(CAN1,0,(int16_t)(GMPSpeedPID.output),(int16_t)(CM7SpeedPID.output));    
//   	  Set_Gimbal_Current(CAN1,(int16_t)(GMYSpeedPID.output),0,(int16_t)(CM7SpeedPID.output));
//		Set_Gimbal_Current(CAN1,0,0,(int16_t)(CM7SpeedPID.output));
	}
	
/**scc 因为没有陀螺仪，暂时注掉了识别和躲避
	else if(GetWorkState() == ChariotRecognition_STATE || GetWorkState() == Dodeg_STATE)   
	{
		
		Set_Gimbal_Current(CAN1,(int16_t)(GMYSpeedPID.output),(int16_t)(GMPSpeedPID.output),(int16_t)(CM7SpeedPID.output)); 	
	}
		
	 
**/


}


/***
函数：CMControlLoop()
功能：底盘控制任务，输出底盘电机电流
备注：速度控制
***/
void CMControlLoop(void)
{  
	//CM1底盘
	  CM1SpeedPID.kp = 100+100*(1-exp(-0.1*fabs(CM1SpeedPID.ref - CM1SpeedPID.fdb)));//80
		CM1SpeedPID.ki = 0;//0.5
		CM1SpeedPID.kd = 0;//20
	    

//CM3、4摩擦轮
	  CM3SpeedPID.kp = CM4SpeedPID.kp = 60 ;//40
		CM3SpeedPID.ki = CM4SpeedPID.ki = 0.5;//0.5
		CM3SpeedPID.kd = CM4SpeedPID.kd = 10;  //20
	
		//停止状态、准备状态
		if(GetWorkState()==STOP_STATE || GetWorkState()==PREPARE_STATE ) 
		{
			CM1SpeedPID.kp = 40 ;//40
		  CM1SpeedPID.ki = 0.5;//0.5
	   	CM1SpeedPID.kd = 5;//20
			
     
			CM1SpeedPID.ref = 0;
			CM1SpeedPID.fdb = CM1Encoder.filter_rate;//0;
			CM1SpeedPID.Calc(&CM1SpeedPID);
		}
		//识别
		if(GetWorkState()==ChariotRecognition_STATE)
		{
			if(Speed_change==0)
				{
				
				CM1SpeedPID.ref = -0.3*Chassis_Speed_Ref ;//- Speed_Offset.output*Chassis_Speed_Ref/fabs(Chassis_Speed_Ref) ;
			  }
			if(Speed_change==1)
				{
			 
				CM1SpeedPID.ref = -0.3*Chassis_Speed_Ref ;//- Speed_Offset.output*Chassis_Speed_Ref/fabs(Chassis_Speed_Ref);
			  }
			
			CM1SpeedPID.fdb = CM1Encoder.filter_rate;
			CM1SpeedPID.Calc(&CM1SpeedPID);

			
			CM3SpeedPID.fdb =  CM3Encoder.filter_rate;
			CM3SpeedPID.Calc(&CM3SpeedPID); 
			

			CM4SpeedPID.fdb = CM4Encoder.filter_rate;
			CM4SpeedPID.Calc(&CM4SpeedPID);
			
		}
		//自由状态、测试状态、被攻击状态
		if(GetWorkState()== Freedom_STATE || GetWorkState() == Test_STATE || GetWorkState() == Attacked_STATE)
		{
			
			CM1SpeedPID.ref = -Chassis_Speed_Ref ;//-  Speed_Offset.output*Chassis_Speed_Ref/fabs(Chassis_Speed_Ref) ;
			CM1SpeedPID.fdb = CM1Encoder.filter_rate;
			CM1SpeedPID.Calc(&CM1SpeedPID);

			CM3SpeedPID.fdb =  CM3Encoder.filter_rate;
			CM3SpeedPID.Calc(&CM3SpeedPID); 
			
			CM4SpeedPID.fdb = CM4Encoder.filter_rate;
			CM4SpeedPID.Calc(&CM4SpeedPID);
			
		}
		
		//躲避
		if(GetWorkState()== Dodeg_STATE)
		{	
			
			CM1SpeedPID.ref = Chassis_Speed_Ref ;//+ Speed_Offset.output*Chassis_Speed_Ref/fabs(Chassis_Speed_Ref) ;
			CM1SpeedPID.fdb = CM1Encoder.filter_rate;
			CM1SpeedPID.Calc(&CM1SpeedPID);
			
			
		  CM3SpeedPID.fdb =  CM3Encoder.filter_rate;
			CM3SpeedPID.Calc(&CM3SpeedPID); 
			
			CM4SpeedPID.fdb = CM4Encoder.filter_rate;
			CM4SpeedPID.Calc(&CM4SpeedPID);
		}
		
		
		
		if(GetWorkState() == STOP_STATE || GetWorkState() == PREPARE_STATE ) 
		{
			//1+空+3+4，需要对应，不能把空的删去
			if((CM1SpeedPID.fdb <= 10)&&(CM1SpeedPID.fdb >= -10))

			Set_CM_Speed(CAN1, 0,0,0,0);
			else
			  Set_CM_Speed(CAN1, CM1SpeedPID.output,0,0,0);
		}
		else if(GetWorkState()==ChariotRecognition_STATE)
		{ 
		Set_CM_Speed(CAN1, CM1SpeedPID.output, 0,CM3SpeedPID.output, CM4SpeedPID.output);	
		}
		else //自由，测试，躲避，识别，被攻击
		{
//	Set_CM_Speed(CAN1, 0, 0,CM3SpeedPID.output, CM4SpeedPID.output);		
		Set_CM_Speed(CAN1, CM1SpeedPID.output, 0,CM3SpeedPID.output, CM4SpeedPID.output);	 		 
		}
}

/***
函数：ShooterMControlLoop()	
功能：发射机构射击电机任务
***/
int16_t count_temp = 0;


void ShooterMControlLoop(void)	
{
	//拨轮pid
	CM7PositionPID.kp = 0;//40
	CM7PositionPID.ki =	0;//0
	CM7PositionPID.kd = 0;//0
	
	CM7SpeedPID.kp = 60;//50//55//60
	CM7SpeedPID.ki = 0;//0
	CM7SpeedPID.kd = 0;//10//25//20//25
	
	//识别模式且s1推到中间打开摩擦轮才会打开波轮
	if(TempShootingFlag==1)//||(GetWorkState() == ChariotRecognition_STATE&&RC_CtrlData.rc.s1==3))//打开波轮发弹
	{
		CM7PositionPID.ref = 60*count_temp;
		CM7PositionPID.fdb = CM7Encoder.ecd_angle;
		CM7PositionPID.Calc(&CM7PositionPID);
		
		CM7SpeedPID.ref = 600;//CM7PositionPID.output;//CM7SpeedPID.ref = 40;均匀转动，但是没劲//400//250
	}
//	//放卡弹
	if(CM7Encoder.ecd_raw_rate < 10)
	{ 
		CM7SpeedPID.ref = -900;
	}
	if(CM7Encoder.ecd_raw_rate < -5 )
	{			
		CM7SpeedPID.ref = 600;//
	}
////	if(CM7Encoder.ecd_raw_rate<-5 &&)
//	{
//		kadanflag = 0 ;
//		kadantt = 10 ;
//	}
	
	//其他状态要关闭拨轮，躲避状态有bug，又写了关闭拨轮和摩擦轮的代码
	if(TempShootingFlag==0)
	{
		CM7SpeedPID.ref = 0;
	}
	
	CM7SpeedPID.fdb = CM7Encoder.filter_rate;  //CM7SpeedPID.fdb = CM7Encoder.ecd_raw_rate
	CM7SpeedPID.Calc(&CM7SpeedPID);
	
//	if(CM7PositionPID.ref-CM7PositionPID.fdb>200)//卡弹反转
//		count_temp=count_temp-5;
}


/***
函数：GraduallyChangeCMSpeed()
功能：自由状态和躲避状态底盘换向时，先逐渐减速再反向逐渐加速
备注：无
***/
void GraduallyChangeCMSpeed(void)
{
	if(GetWorkState()== Freedom_STATE || GetWorkState()== Dodeg_STATE||GetWorkState()== ChariotRecognition_STATE||GetWorkState()== Attacked_STATE) //自由状态和躲避状态
	{
		if(Chassis_Change_Dir_Flag==1) //1->0，速度逐渐减小
		{
			Chassis_Speed_UP_Flag = 0;
			Chassis_Speed_DOWN_Flag = 0;
			
			CMSpeedRate=0;//CMSpeedRate-0.05f;
			if(CMSpeedRate<=0) 
			{
				CMSpeedRate=0.0f;
				Chassis_Change_Dir_Flag=2;
			}
			Chassis_Speed_Ref = last_Chassis_Temp_Speed*CMSpeedRate;
		}
		else if(Chassis_Change_Dir_Flag==2) //0->1，速度逐渐增大
		{
			CMSpeedRate=1;//CMSpeedRate+0.05f;
			if(CMSpeedRate>=1) 
			{
				CMSpeedRate=1.0f;
				Chassis_Change_Dir_Flag = 0;
			}
			Chassis_Speed_Ref = Chassis_Temp_Speed*CMSpeedRate;
		}
		else
		{
			Chassis_Speed_Ref = Chassis_Temp_Speed;
		}
  }
}

