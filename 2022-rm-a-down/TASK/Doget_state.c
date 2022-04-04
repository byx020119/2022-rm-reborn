#include "Doget_state.h"
#include "Freedom_Status.h"
#include "WorkState.h"
#include "SpeedTask.h"
#include "GunHeartAndBleed.h"
#include "JudgingSystemTask.h"

int Doget_Change_Flag=0;             // 位置可变躲避状态标志
int Destination_Flag = 0;            //是否到达两端标志
int16_t last_Random_MotionRange = 0; //底盘上一次随机数位置
int16_t Random_MotionRange = 0;      //本次随机数位置
int Doget_twice=0;                   //二次躲避
int Random_change_flag=0;            //随机数产生函数变化标志
int Random_change__flag_2=0;            //随机数产生函数变化标志
int Doget_rego_flag=0;
int test_ecd=0;

void YawFreeRoation_Doget(void)
{
  if( GetWorkState()==Dodeg_STATE  )
	{
		
		if(Last_CameraDetectTarget_Flag==1 && CameraDetectTarget_Flag==0)
		{
		   YawCurrentPositionSave = -GMYawEncoder.ecd_angle;           //保存当前yaw码盘值
		   GimbalRef.yaw_angle_dynamic_ref = YawCurrentPositionSave;
		}
		
	//慢慢点头
		if(GMPitchEncoder.ecd_angle-35>0)
		{
		pitch_rotate_flag=1;
		}
		if(GMPitchEncoder.ecd_angle-8<0)
		{
		pitch_rotate_flag=0;
		}
		if(pitch_rotate_flag==1)
		{
	  GimbalRef.pitch_angle_dynamic_ref=GimbalRef.pitch_angle_dynamic_ref + 0.06f;
		}
		if(pitch_rotate_flag==0)
		{
	  GimbalRef.pitch_angle_dynamic_ref=GimbalRef.pitch_angle_dynamic_ref - 0.06f;
		}
	

		//固定摄像头未识别
		if(Freedom_Rotation_flag==0)
		{
			if(GMYawEncoder.ecd_angle-yaw_ecd_angle_flag<=0)
			{
			Yaw_rotate_flag=1;
			}
			if(GMYawEncoder.ecd_angle+yaw_ecd_angle_flag1>=0)
			{
			Yaw_rotate_flag=0;
			}
			if(Yaw_rotate_flag==1)
			{
			GimbalRef.yaw_angle_dynamic_ref=GimbalRef.yaw_angle_dynamic_ref - 0.08f;
			}
			if(Yaw_rotate_flag==0)
			{
			GimbalRef.yaw_angle_dynamic_ref=GimbalRef.yaw_angle_dynamic_ref + 0.08f;
			}
   	}
		
		
		if(GM_Rotete_flag_Before=='X'||GM_Rotete_flag_Behind=='X')
		{
			Recognition_Delay_count++;
			if(Recognition_Delay_count>3000)
			{
			   Freedom_Rotation_flag=1; //识别一段时间后，固定摄像头识别
			}
		}
		
   
	 //固定摄像头识别到敌方装甲板，将云台旋转过去
	if(Freedom_Rotation_flag==1)
	{
		Yaw_encoder = -GMYawEncoder.ecd_angle-Yaw_encoder_s;
		YAW_Round_Cnt   = (int)(Yaw_encoder)/1852;
		YAW_Angle_YuShu = (int)(Yaw_encoder)%1852;
		if(YAW_Angle_YuShu>=0)
		{
			if(GM_Rotete_flag_Before=='X') //前固定摄像头识别到目标
			{
				if(YAW_Angle_YuShu<=703)        //926+223=1249   926-223=703  1852-223=1629  1852-446=1406 926-446=480 1852+446=2298 1249+223=1472
					{
						Recognition_YAW_Pos_Ref = YAW_Round_Cnt*1852 -446;
					}
				if(YAW_Angle_YuShu>703)
				  {
				    Recognition_YAW_Pos_Ref = YAW_Round_Cnt*1852 +1852;
				  }

			}
			else if(GM_Rotete_flag_Behind=='X') //后固定摄像头识别到目标
			{
	  		if(YAW_Angle_YuShu<=703)       
					{
						Recognition_YAW_Pos_Ref = YAW_Round_Cnt*1852 +926;
					}
				if(YAW_Angle_YuShu>703)
				  {
				    Recognition_YAW_Pos_Ref = YAW_Round_Cnt*1852 +480;
				  }
			}
		}
		else
		{
			if(GM_Rotete_flag_Before=='X') //前固定摄像头识别到目标
			{
	  		if(YAW_Angle_YuShu>=-1249)
					{
						Recognition_YAW_Pos_Ref = YAW_Round_Cnt*1852 +0;
					}
				if(YAW_Angle_YuShu<-1249)
				 {
				   Recognition_YAW_Pos_Ref = YAW_Round_Cnt*1852 -2298;
				 }

	  	}
			else if(GM_Rotete_flag_Behind=='X') //后固定摄像头识别到目标
				{
					if(YAW_Angle_YuShu>=-1249)
					{
						Recognition_YAW_Pos_Ref = YAW_Round_Cnt*1852 -1472;
					}
				if(YAW_Angle_YuShu<-1249)
				 {
				   Recognition_YAW_Pos_Ref = YAW_Round_Cnt*1852 -926;
				 }
				}
		}
		
//？？？
		if(Recognition_YAW_Pos_Ref+Yaw_encoder_s - (-GMYawEncoder.ecd_angle)>=0)
			{
				Recognition_YAW_Rotation_Dir = 1;   //向位置环参考值增大的方向转
			}
		else
			{
				Recognition_YAW_Rotation_Dir = 2;  //向位置环参考值减小的方向转
			}
			
			
		if(Recognition_YAW_Rotation_Dir == 1)
			{
				GimbalRef.yaw_angle_dynamic_ref=GimbalRef.yaw_angle_dynamic_ref+0.8f;//0.4f
				if(GimbalRef.yaw_angle_dynamic_ref - Recognition_YAW_Pos_Ref>0)
				{
					GimbalRef.yaw_angle_dynamic_ref = Recognition_YAW_Pos_Ref;
					Recognition_YAW_Rotation_Dir = 0;
					Freedom_Rotation_flag=0;
					Recognition_Delay_count=0;
				}
			}
		else if(Recognition_YAW_Rotation_Dir == 2)
			{
				GimbalRef.yaw_angle_dynamic_ref=GimbalRef.yaw_angle_dynamic_ref-0.8f;//0.1f
				if(GimbalRef.yaw_angle_dynamic_ref - Recognition_YAW_Pos_Ref<0)
				{
					GimbalRef.yaw_angle_dynamic_ref = Recognition_YAW_Pos_Ref;
					Recognition_YAW_Rotation_Dir = 0;
					Freedom_Rotation_flag=0;
					Recognition_Delay_count=0;
				}
			}
//		else if(Recognition_YAW_Rotation_Dir == 0)  //参考值到达目标位置后，等待电机到位
//			{
//				if(abs(GimbalRef.yaw_angle_dynamic_ref - (-GMYawEncoder.ecd_angle))<1)
//				{
//					Freedom_Flag = 1;
//				}
//			}
	  
		//如果云台在摆向固定摄像头的过程中识别到目标，那么立刻结束该过程，等待进入识别模式
		if(CameraDetectTarget_Flag ==1)
		{
			Recognition_YAW_Rotation_Dir = 0;
			Freedom_Rotation_flag=0;
		}
	}
	
	}
}



void Chassis_Motion_Switch_Doget(void)
{
	//被飞行器攻击时的躲避
if(GetWorkState()== Dodeg_STATE && Aerocraft_attack_flag==1)
  {
		if(Aerocraft_Phototube_flag==1)
		  {
	    	Chassis_Position_Ref = -10000; 
	  	}
		if(Aerocraft_Phototube_flag==0)
		{
			//依靠光电管进行换向
				if(Dir_Change_Flag==1)
			{
			   switch(Chassis_Freedom_i)
				{
					case 0:
					{
						Chassis_Position_Ref = 10000;
						Dir_Change_Flag=0;
					};break;
					case 1:
					{
						Chassis_Position_Ref = -10000; 
						Dir_Change_Flag=0;
					};break;
				}
			}
			//依靠码盘进行换向
			test_ecd=fabs(Chassis_Position_Ref - CM1Encoder.ecd_angle);
				if((fabs(Chassis_Position_Ref - CM1Encoder.ecd_angle)<5)&((Chassis_Position_Ref - CM1Encoder.ecd_angle)>-5))
			{
				switch(Chassis_Freedom_i)
				{
					case 0:
					{
						Chassis_Position_Ref = 10000;
					};break;
					case 1:
					{
						Chassis_Position_Ref = -10000; 
					};break;
				}
				Chassis_Freedom_i++;
				if(Chassis_Freedom_i>=2) Chassis_Freedom_i = 0;
			}
		}
			
	  	if(Chassis_Change_Dir_Flag==0)    //计算运动速度
			{
			last_Chassis_Temp_Speed = Chassis_Temp_Speed;
		  if(Aerocraft_Phototube_flag==1)
	      	{
		       Chassis_Temp_Speed=-300;
	      	}
			if(Aerocraft_Phototube_flag==0)
	     {
				if(Chassis_Position_Ref <CM1Encoder.ecd_angle)
				{
					Chassis_Temp_Speed = -300;
				}
				else if(Chassis_Position_Ref > CM1Encoder.ecd_angle)
				{	
					Chassis_Temp_Speed = 300;
				}
		 	}
				
				if(last_Chassis_Temp_Speed*Chassis_Temp_Speed<0)    //底盘换向
				{
				  Chassis_Change_Dir_Flag = 1;
				}
		  }
  }
	
	//未被飞行器攻击时的躲避
if(GetWorkState()== Dodeg_STATE && Aerocraft_attack_flag==0)   
	{
		
		if(Random_change__flag_2==1)                    //躲避模式被攻击换向的标志位
			{				
			int tt2=0;//自增变量，每被打到，就进行换向					
			tt2++;
			if(tt2>=0&&tt2<=10)	//位置0l'ji环ref只赋值一次  //notice
			{
		      if( fabs(Chassis_Position_Ref - CM1Encoder.ecd_angle ) < 400 ) //跑一段距离再换向，原地换向慢
			  {
		       Chassis_Position_Ref = Chassis_Position_Ref;
				  Destination_Flag =1;
			   //当前位置变向要给当前一个位置ref，但是由于延迟，往往CM1会剧烈转动到ref位置
			  }
			   else
			  {				
			   Chassis_Position_Ref = ( Chassis_Position_Ref - CM1Encoder.ecd_angle ) * 0.005  + CM1Encoder.ecd_angle;
				  Destination_Flag=1;
			  }	  
			}	
              //last_Chassis_Temp_Speed = Chassis_Temp_Speed;	//前后速度不同会换向
             Random_change__flag_2 = 0;	//避免重复换向	
			}	
			
			//	英雄打，换向+随机位置
			if(Random_change__flag_2==2)                               //被英雄攻击，进入躲避状态,给一个随机位置
			{
			getRandom_MotionRange();
		    Chassis_Position_Ref = Random_MotionRange;
		       switch(Chassis_Freedom_i)
				  {
				  	 case 0:
					  {
					  Chassis_Position_Ref = abs(Random_MotionRange);
					  };break;
					 case 1:
				  	  {
				  	   Chassis_Position_Ref = -abs(Random_MotionRange); 
				      };break;
			      }	
               Random_change__flag_2 = 0;	//避免重复换向				  
		   }
		//被英雄大弹丸攻击，进入躲避状态
//		
//		if(Amor_ID==0&&Doget_Change_Flag==1)
//		{
//		  Chassis_Position_Ref = -10000;     //触发右侧光电管
//			Doget_Change_Flag = 0;
//			Destination_Flag = 0;
////			Attacked_Flag=0;
//		}
//		
//		if(Amor_ID==1&&Doget_Change_Flag==1) 
//		{
//		  Chassis_Position_Ref = 10000;       //触发左侧光电管
//			Doget_Change_Flag = 0;
//			Destination_Flag = 0;
////			Attacked_Flag=0;
//		}
//	 
//			
//		//固定摄像头辅助躲避
//		if(Doget_rego_flag==1)
//		{
//		  Chassis_Position_Ref = -10000;     //触发右侧光电管
//			Doget_Change_Flag = 0;
//			Destination_Flag = 0;
//			Doget_rego_flag=0;
////			Attacked_Flag=0;
//		}
//		
//		if(Doget_rego_flag==2)
//		{
//		  Chassis_Position_Ref = 10000;       //触发左侧光电管
//			Doget_Change_Flag = 0;
//			Destination_Flag = 0;
//			Doget_rego_flag=0;
////			Attacked_Flag=0;
//		}
		
		
		if(Chassis_Power_On_Flag == 1)  //底盘已经上电
		{
		 if(Destination_Flag==1)         //已经抵达了两端
		 {
			   //依靠光电管进行换向
			 
			if(Dir_Change_Flag==1)                         
			{
			  getRandom_MotionRange();
				switch(Chassis_Freedom_i)
				{
					case 0:
					{
						Chassis_Position_Ref = abs(Random_MotionRange);
						Dir_Change_Flag=0;
					};break;
					case 1:
					{
						Chassis_Position_Ref = -abs(Random_MotionRange); 
						Dir_Change_Flag=0;
					};break;
				}		
			}
			//固定摄像头辅助进行躲避
		 if(RobotHP>=120&&Big_armor_recognition_flag==1)
				{
				 if(Dodeg_Delay_Count<5000)
			    	{
					   if((Armor_R_Flag_Before=='R'||Armor_R_Flag_Before=='L')&&Destination_Flag==1)   
						 { 
							 switch(Chassis_Freedom_i)
				       {
					       case 0:
				       	{
						      Doget_rego_flag=2;
				       	};break;
					       case 1:
					      {
						      Doget_rego_flag=1;
					      };break;
				       }		
							 
							 Dodeg_Delay_Count=0;
						 }
						 if((Armor_R_Flag_Behind=='R'||Armor_R_Flag_Behind=='L')&&Destination_Flag==1)
						 {
							  switch(Chassis_Freedom_i)
				       {
					       case 0:
				       	{
						      Doget_rego_flag=1;
				       	};break;
					       case 1:
					      {
						      Doget_rego_flag=2;
					      };break;
				       }
							 Dodeg_Delay_Count=0;
						 }
			    	}
				 if(Dodeg_Delay_Count>5000 & Dodeg_Delay_Count<16000)
				 {
           if(Armor_R_Flag_Before=='R')         //相当于大弹丸正面攻击
						 {
						   Doget_rego_flag=2;
						 }
					if(Armor_R_Flag_Before=='L')         //相当于大弹丸背面攻击
						 {
						   Doget_rego_flag=1;             
						 }
					if(Armor_R_Flag_Behind=='R')         //相当于大弹丸背面攻击
						 {
						   Doget_rego_flag=1;
						 }
					if(Armor_R_Flag_Behind=='L')         //相当于大弹丸正面攻击
						 {
						   Doget_rego_flag=2;             
						 }
				 }
				}
			if(RobotHP<120)//&&Big_armor_recognition_flag==1)
				{
				  if(Dodeg_Delay__Count_2<=100)
			    	{
					   if((Armor_R_Flag_Before=='R'||Armor_R_Flag_Before=='L')&&Destination_Flag==1)   
						 { 
							 switch(Chassis_Freedom_i)
				       {
					       case 0:
				       	{
						      Doget_rego_flag=2;
				       	};break;
					       case 1:
					      {
						      Doget_rego_flag=1;
					      };break;
				       }		
							 
							 Dodeg_Delay_Count=0;
						 }
						 if((Armor_R_Flag_Behind=='R'||Armor_R_Flag_Behind=='L')&&Destination_Flag==1)
						 {
							  switch(Chassis_Freedom_i)
				       {
					       case 0:
				       	{
						      Doget_rego_flag=1;
				       	};break;
					       case 1:
					      {
						      Doget_rego_flag=2;
					      };break;
				       }
							 Dodeg_Delay_Count=0;
						 }
			    	}
				 if(Dodeg_Delay__Count_2>100)
				 {
           if(Armor_R_Flag_Before=='R')         //相当于大弹丸正面攻击
						 {
						   Doget_rego_flag=2;
						 }
					if(Armor_R_Flag_Before=='L')         //相当于大弹丸背面攻击
						 {
						   Doget_rego_flag=1;             
						 }
					if(Armor_R_Flag_Behind=='R')         //相当于大弹丸背面攻击
						 {
						   Doget_rego_flag=1;
						 }
					if(Armor_R_Flag_Behind=='L')         //相当于大弹丸正面攻击
						 {
						   Doget_rego_flag=2;             
						 }
				 }
				}
				
			if(fabs(Chassis_Position_Ref - CM1Encoder.ecd_angle)<5)    //计算目标位置
			{
			
				if(RobotHP>=120)
				{
				if(Dodeg_Delay_Count<5000)
				{	
			  	if(Random_change_flag==0)                              //被英雄攻击，进入躲避状态
					{
					getRandom_MotionRange();
		      Chassis_Position_Ref = Random_MotionRange;
					}
					if(Random_change_flag==1)                             //被英雄第二次攻击，进入二次躲避状态
					{
					getRandom_MotionRange();
				  switch(Chassis_Freedom_i)
				   {
				  	case 0:
					  {
					  	Chassis_Position_Ref = abs(Random_MotionRange);
					  };break;
					  case 1:
				  	{
				  		Chassis_Position_Ref = -abs(Random_MotionRange); 
				   	};break;
			     }		
					}
				}
				
				
				
//				if(Dodeg_Delay_Count>5000&&Dodeg_Delay_Count<16000)
//				{
//				  switch(Chassis_Freedom_i)
//				   {
//				  	case 0:
//					  {
//					  	Chassis_Position_Ref = 10000;
//					  };break;
//					  case 1:
//				  	{
//				  		Chassis_Position_Ref = -10000; 
//				   	};break;
//			     }		
//				}
				}
				if(RobotHP<120)
				{
					if(Random_change__flag_2==1)                    //被攻击时，躲避到两边段10s
					{
					getRandom_MotionRange();
		      Chassis_Position_Ref = Random_MotionRange;
					}
					
					if(Random_change__flag_2==0)                    //进行全场躲避至少20s
					{
			     getRandom_MotionRange();
				   switch(Chassis_Freedom_i)
				   {
				  	case 0:
					  {
					  	Chassis_Position_Ref = abs(Random_MotionRange);
					  };break;
					  case 1:
				  	{
				  		Chassis_Position_Ref = -abs(Random_MotionRange); 
				   	};break;
			     }		
          }
				}	
			}
		 }
			if(Chassis_Change_Dir_Flag==0)    //计算运动速度
			{
				
				last_Chassis_Temp_Speed = Chassis_Temp_Speed;
				
				if(Chassis_Position_Ref <CM1Encoder.ecd_angle)
				{
					if(fabs(Chassis_Position_Ref - CM1Encoder.ecd_angle)>500)
					{
						if(RobotHP>=120)
						{
							if(Dodeg_Delay_Count<=5000)
			      	{
				          Chassis_Temp_Speed = -300;//-700;	2022 -750
			      	}
//						  if(Dodeg_Delay_Count>5000&&Dodeg_Delay_Count<16000)
//			      	{
//				          Chassis_Temp_Speed = -600;// 2022 -700;
//			      	}
						}
						if(RobotHP<120)
						{
						  Chassis_Temp_Speed = -300;//2022 -700;
						}
					}
					else
					{
						Chassis_Temp_Speed = -300;//2022 -600
					}
				}
				else if(Chassis_Position_Ref > CM1Encoder.ecd_angle)
				{
					if(fabs(Chassis_Position_Ref - CM1Encoder.ecd_angle)>500)
					{
						if(RobotHP>=120)
						{
						 if(Dodeg_Delay_Count<=5000)
			      	{
				          Chassis_Temp_Speed = 300;//2022 750;
			      	}
//						  if(Dodeg_Delay_Count>5000&&Dodeg_Delay_Count<16000)
//			      	{
//				          Chassis_Temp_Speed = 600;//-700;
//			      	}
						}
						if(RobotHP<120)
						{
						  Chassis_Temp_Speed = 300;//2022 700;
						}
					}
					else
					{
						Chassis_Temp_Speed = 300;//2022 600
					}
				}
				
				if(fabs(Chassis_Temp_Speed - last_Chassis_Temp_Speed)>150 && last_Chassis_Temp_Speed*Chassis_Temp_Speed>0) //同号，同向变化
				{
					last_Chassis_Temp_Speed_Save = last_Chassis_Temp_Speed;
					Chassis_Temp_Speed_Save      = Chassis_Temp_Speed;
					if(fabs(Chassis_Temp_Speed)>fabs(last_Chassis_Temp_Speed))  //加速
					{
						Chassis_Speed_UP_Flag = 1;
						CMRamp.ResetCounter(&CMRamp);  //斜坡清零
					}
					else if(fabs(Chassis_Temp_Speed)<fabs(last_Chassis_Temp_Speed))
					{
						Chassis_Speed_DOWN_Flag = 1;
						CMRamp.ResetCounter(&CMRamp);  //斜坡清零
					}
				}
			
				if(Chassis_Speed_UP_Flag == 1)
				{
					Chassis_Temp_Speed = last_Chassis_Temp_Speed_Save +(Chassis_Temp_Speed_Save - last_Chassis_Temp_Speed_Save)*CMRamp.Calc(&CMRamp);
					if(CMRamp.Calc(&CMRamp) == 1)  Chassis_Speed_UP_Flag = 0;
				}
				else if(Chassis_Speed_DOWN_Flag == 1)
				{
					Chassis_Temp_Speed = last_Chassis_Temp_Speed_Save + (Chassis_Temp_Speed_Save - last_Chassis_Temp_Speed_Save)*CMRamp.Calc(&CMRamp);
					if(CMRamp.Calc(&CMRamp) == 1)  Chassis_Speed_DOWN_Flag = 0;
				}
				
				if(last_Chassis_Temp_Speed*Chassis_Temp_Speed<0)    //底盘换向
				{
				  Chassis_Change_Dir_Flag = 1;
				}
		  }
	  }
	}
}


void getRandom_MotionRange(void)
{
	if(RobotHP>=120)
	{
	  last_Random_MotionRange = Random_MotionRange;//保存上次产生的随机数
		
	  if(Random_change_flag==0)                //被英雄攻击，进入躲避状态
	    {
	      Random_MotionRange = rand()%2500;//1000~2500
	
	      if(abs(Random_MotionRange)<1000)
	        {
	          Random_MotionRange = Random_MotionRange+Random_MotionRange/abs(Random_MotionRange)*1000;//>1000
	        }
       }
	
	   if(Random_change_flag==1)              //被英雄第二次攻击，进入二次躲避状态
    	{
	      Random_MotionRange = rand()%3000+3500; //3500~6500//past-1000~past+1000之外
	      if(last_Random_MotionRange-Random_MotionRange<1000)
	        {
	          Random_MotionRange = Random_MotionRange-1000;
	        }
		
		    if(Random_MotionRange-last_Random_MotionRange<1000)
	        {
	          Random_MotionRange = Random_MotionRange+1000;
	        }
      }
  }
	if(RobotHP<120)
	{
		
		if(Random_change__flag_2==0)            //进行全场躲避至少20s
	  	{
	      Random_MotionRange = rand()%8000;//2000~8000
	      if(abs(Random_MotionRange)<2000)
	        {
	          Random_MotionRange = Random_MotionRange+Random_MotionRange/abs(Random_MotionRange)*2000;
	        }
	    }
		if(Random_change__flag_2==1)          //被攻击时，躲避到两边段10s 
		  {
	      Random_MotionRange = rand()%2500;//1000~2500
	      if(abs(Random_MotionRange)<1000)
	      {
	          Random_MotionRange = Random_MotionRange+Random_MotionRange/abs(Random_MotionRange)*1000;
	      }
	    }
  }
}
