#include "main.h"
#include "GunHeartAndBleed.h"
#include "ShootingTask.h"
#include "JudgingSystemTask.h"

int16_t GunHeart_Value = 0;      //µ±Ç°Ç¹¿ÚÈÈÁ¿
int Doget_bleed_time=0;
uint32_t Dodeg_Delay_Count = 0;
uint32_t Dodeg_Delay__Count_2 = 0;
int  test_HP=0;
int Count_flag=0;
int HP_error=0;                 //ÑªÁ¿²îÖµ
int16_t HP_error__time[60];     //ÓÃÀ´±£´æ±»¹¥»÷Ê±¼äµÄÊı×é
int16_t HP__error_time_count=0; //¼ÆÊ±Á¿£¨Ã¿0.11Ãë³ÌĞòÔËĞĞÒ»´Î£¬¼´³ÌĞò×Ô¼Ó1£©
int HP__error_count=0;
int Doget_HP_Flag=0;
int Big_armor_recognition_count=0;
int Big_armor_recognition_flag=0;
int Aerocraft_attack_flag=0;        //±»·ÉĞĞÆ÷¹¥»÷±êÖ¾
int Aerocraft_flag=0;
uint32_t Aerocraft_doget_count_delay=0; //±»·ÉĞĞÆ÷¹¥»÷Ê±µÄ¶ã±Ü
int Aerocraft_Phototube_flag=0;         //±»·ÉĞĞÆ÷¹¥»÷ÒÔºóÓĞÃ»ÓĞµ½´ï×îÓÒ¶Ë±êÖ¾
int time_test=0;

/*********************************************
ÉÚ±ø»úÆ÷ÈË£º(17mmµ¯Íè)
1.ÉäËÙÉÏÏŞ£º30m/s
2.ÈÈÁ¿ÉÏÏŞ£º360
3.Ã¿ÃëÀäÈ´Öµ£º72
4.³õÊ¼ÑªÁ¿&ÉÏÏŞÑªÁ¿Öµ£º600
**********************************************/
void Bleed_Monitor(void)
{
   if(Chassis_Power_On_Flag == 1)
		{

 		RobotHP = robotState.remainHP;
			HP_error=RobotHP-lastRobotHP;	
			 HP__error_time_count++;
		  	if(HP_error<=-10)
		  	{
		  	  HP_error__time[HP__error_count++]=HP__error_time_count;
		  		Doget_HP_Flag=1;
					Aerocraft_flag=1;
		  	}
			 //Èô0.6ÃëÄÚ±»¹¥»÷³¬¹ıÎå´Î£¬Ç¨ÇÒÉãÏñÍ·Î´Ê¶±ğµ½²½±ø£¬Ôò½øÈë¶ã±Ü·ÉĞĞÆ÷Ä£Ê½£
				if(HP__error_count>=5&&Aerocraft_flag==1)
		  	{  time_test=HP_error__time[HP__error_count-1]-HP_error__time[HP__error_count-5];
		     	if((HP_error__time[HP__error_count-1]-HP_error__time[HP__error_count-5]<6)&&GM_Rotete_flag_Before!='X')//&&(CM1Encoder.ecd_angle>-2500&&CM1Encoder.ecd_angle<500))
		    	{
						Dodeg_Delay_Count = 0; //¶ã±Ü¼ÆÊıÇåÁã
						Aerocraft_doget_count_delay = 0;
			    	DodgeTarget_Flag = 1;
				  	Aerocraft_flag=0;
						Aerocraft_attack_flag=1;
						Aerocraft_Phototube_flag=1;
			    }
		    }
			if(RobotHP>120)  
			{
					//Èô5ÃëÖÓÄÚ±»¹¥»÷15´Î£¬½øÈë¶ã±Ü×´Ì¬
		  	if(HP__error_count>=15&&Doget_HP_Flag==1)
		  	{
					time_test=HP_error__time[HP__error_count-1]-HP_error__time[HP__error_count-15];
		     	if((HP_error__time[HP__error_count-1]-HP_error__time[HP__error_count-15]<60)&&(HP_error__time[HP__error_count-1]-HP_error__time[HP__error_count-5]>6))
		    	{
			      Dodeg_Delay_Count = 0; //¶ã±Ü¼ÆÊıÇåÁã
			    	DodgeTarget_Flag = 1;
				  	Doget_HP_Flag=0;
			    }
		    }
				
				//±»Ó¢ĞÛµÚ¶ş´Î¹¥»÷£¬ÔÙ´Î½øÈë¶ã±Ü×´Ì¬£¨¹ìµÀÖĞ¼ä¶Î£©//
				 Doget_bleed_time++;
		  	if(Doget_bleed_time<60&&Doget_bleed_time>10)
			  {
			  	if(HP_error<=-100) 
			    {
	      	Random_change_flag=1;			
			  	Doget_Change_Flag=1;			
			    }		
			  }
				//±»Ó¢ĞÛ¹¥»÷Ò»´Î£¬½øÈë¶ã±Ü×´Ì¬ £¨¹ìµÀÁ½±ß¶Î£©//
		  	if(HP_error<=-100) 
		  	{
	      	Dodeg_Delay_Count = 0; //¶ã±Ü¼ÆÊıÇåÁã
		  		DodgeTarget_Flag = 1;
			  	Doget_Change_Flag=1;
			  	Doget_bleed_time=0;
//					Big_armor_recognition_count=0;
//   				Big_armor_recognition_flag=1;
		  	}
				Big_armor_recognition_count++;
  			if(Big_armor_recognition_count>300)
  			{ 
  			  Big_armor_recognition_flag=0;
  			}
			}
			
			//ÑªÁ¿ÉÙÓÚ120£¬Ö±½Ó½øÈë¶ã±Ü×´Ì¬  ÔÚ¶ã±Ü×´Ì¬±»¹¥»÷Ê±ÏÈÊÇ¶ã±Üµ½Á½±ß¶Î¹ìµÀ10Ãë£¬Ö®ºó20ÃëÈ«¹ìµÀÂÒÅÜ£¨±»»÷´òÒ²²»»áÈ¥Á½±ß¶Î£©20ÃëÖ®ºó¿ÉÒÔÔÙ´Î½øÈëÁ½±ß¶Î¶ã±Ü//
			if(RobotHP<120)  
			{
				Dodeg_Delay_Count = 0;
				DodgeTarget_Flag = 1;
				if(HP_error<=-10&&Count_flag==0)
				{
				  Doget_Change_Flag=1;
					Dodeg_Delay__Count_2=0;
					Random_change__flag_2=1;
					Count_flag=1;
				}
				Dodeg_Delay__Count_2++;
				if(Dodeg_Delay__Count_2>100)
				{
				   Random_change__flag_2=0;
				}
				if(Dodeg_Delay__Count_2>300)
				{
				  Count_flag=0;
				}
			  if(HP_error<=-10)
			  {
			    Big_armor_recognition_count=0;
//				  Big_armor_recognition_flag=1;
		  	}
			  Big_armor_recognition_count++;
		   	if(Big_armor_recognition_count>300)
			  {
			    Big_armor_recognition_flag=0;
			  }
			}
			
			if(HP_error<=-10 && HP_error>=-20) //±»Ğ¡µ¯Íè¹¥»÷Ò»´Î
			{
				Attacked_Flag = 1; //±»¹¥»÷±êÖ¾±äÎª1
			}
		lastRobotHP = RobotHP;		
	  } 
		
		
	}
/**********************
¹¦ÄÜ£º±»¹¥»÷×´¿ö¼à¿Ø
²ÎÊı£º
	
**********************/
	
void Attacked_Monitor(void)
{
	
	Amor_ID = robotHurt.armorType;    //1ºÅÎªÕıÃæ×°¼×£¬0ºÅÎª±³Ãæ×°¼×
	if(Attacked_Flag == 0)  //ÔÚÏìÓ¦µ±´Î×°¼×°å¹¥»÷Ê±£¬²»»áÔÙÏìÓ¦ĞÂµÄÉËº¦£¬·ÀÖ¹·èµô
	{
//		Amor_ID = robotHurt.armorType;
		Yaw_encoder = -GMYawEncoder.ecd_angle-Yaw_encoder_s;
		YAW_Round_Cnt   = (int)(Yaw_encoder)/1852;
		YAW_Angle_YuShu = (int)(Yaw_encoder)%1852;
		if(YAW_Angle_YuShu>=0)
		{
			if(Amor_ID == 1) //1ºÅ×°¼×±»¹¥»÷   Ç°×°¼×°å
			{
				if(YAW_Angle_YuShu<=703)        //926+223=1249   926-223=703  1852-223=1629 1852+223=2085
					{
						Attacked_YAW_Pos_Ref = YAW_Round_Cnt*1852 -223;
					}
				if(YAW_Angle_YuShu>703)
				  {
				    Attacked_YAW_Pos_Ref = YAW_Round_Cnt*1852 +1629;
				  }
				else if(YAW_Angle_YuShu>=703&&YAW_Angle_YuShu<=1629)
					{
						Attacked_YAW_Pos_Ref = YAW_Round_Cnt*1852 + 1629;
					}
			}
			else if(Amor_ID == 0) //0ºÅ×°¼×±»¹¥»÷
			{
	  			Attacked_YAW_Pos_Ref = YAW_Round_Cnt*1852 + 703;
			}
		}
		else
		{
			if(Amor_ID == 1) //1ºÅ×°¼×±»¹¥»÷
			{
	  		if(YAW_Angle_YuShu>=-1249)
					{
						Attacked_YAW_Pos_Ref = YAW_Round_Cnt*1852 -223;
					}
				if(YAW_Angle_YuShu<-1249)
				 {
				  Attacked_YAW_Pos_Ref = YAW_Round_Cnt*1852 -2085;
				 }
				else if (YAW_Angle_YuShu<=-703&&YAW_Angle_YuShu>=-1629)
					{
			  		 Attacked_YAW_Pos_Ref = YAW_Round_Cnt*1852 - 2085;
					}
	  	}
			else if(Amor_ID == 0) //0ºÅ×°¼×±»¹¥»÷
				{
					Attacked_YAW_Pos_Ref = YAW_Round_Cnt*1852 - 1249;
				}
		}
		if(Attacked_YAW_Pos_Ref+Yaw_encoder_s - (-GMYawEncoder.ecd_angle)>=0)
			{
				Attacked_YAW_Rotation_Dir = 1;   //ÏòÎ»ÖÃ»·²Î¿¼ÖµÔö´óµÄ·½Ïò×ª
			}
			else
			{
				Attacked_YAW_Rotation_Dir = 2;  //ÏòÎ»ÖÃ»·²Î¿¼Öµ¼õĞ¡µÄ·½Ïò×ª
			}
			Hurt_COUNT++;
	}
}

/***************************************************************************
¹¦ÄÜ£ºÇ¹¿ÚÈÈÁ¿¼ÆËã
Èë¿Ú²ÎÊı£º17mmµ¯ÍèÉäËÙ
³ö¿Ú²ÎÊı£ºÎŞ
***************************************************************************/
void BulletSpeed_Monitor(void)
{
 
				
				/***¸ù¾İµ¯ÍèÊµ¼ÊÉäËÙÊµÊ±µ÷Õû×îĞ¡Ä¦²ÁÂÖËÙ¶È***/
	if(Target_Distance<=30)
	{
		if(Bullet_17_real_Speed - Bullet_Speed_Ref.low_speed>1)       //Êµ¼ÊÉäËÙ´óÓÚÄ¿±êÉäËÙ
		{
	 		Friction_Speed.low_speed = Friction_Speed.low_speed - 10;
		}
		else if(Bullet_17_real_Speed - Bullet_Speed_Ref.low_speed<-1) //Êµ¼ÊÉäËÙĞ¡ÓÚÄ¿±êÉäËÙ
		{
			Friction_Speed.low_speed = Friction_Speed.low_speed + 10;
		}
	}
				/***¸ù¾İµ¯ÍèÊµ¼ÊÉäËÙÊµÊ±µ÷Õû×î´óÄ¦²ÁÂÖËÙ¶È***/
	if(Target_Distance>30)  
	{
		if(Bullet_17_real_Speed - Bullet_Speed_Ref.high_speed>1)       //Êµ¼ÊÉäËÙ´óÓÚÄ¿±êÉäËÙ
		{
	  	if(Bullet_17_real_Speed - Bullet_Speed_Ref.high_speed<3)
			{
	 		   Friction_Speed.high_speed = Friction_Speed.high_speed - 10;
			}
			else
			{
							/***
							ËÙ¶È³¬¹ı28m/sÒ»´Î£¬ÈıµµÄ¦²ÁÂÖËÙ¶È¾ù¼õÈ¥100£¬±£Ö¤²»³¬ÈÈÁ¿
							***/
		    	Friction_Speed.low_speed = Friction_Speed.low_speed - 100;
					Friction_Speed.high_speed = Friction_Speed.high_speed - 100;
							
					if(Friction_Speed.low_speed<100) Friction_Speed.low_speed = 100;
			}
	}
	else if(Bullet_17_real_Speed - Bullet_Speed_Ref.high_speed<-1) //Êµ¼ÊÉäËÙĞ¡ÓÚÄ¿±êÉäËÙ
		{
  			Friction_Speed.high_speed = Friction_Speed.high_speed + 10;
		}
  }
}

/***********************************************************
¹¦ÄÜ£ºÇ¹¿ÚÈÈÁ¿ÀäÈ´¼ÆËã
Èë¿Ú²ÎÊı£ºÎŞ
³ö¿Ú²ÎÊı£ºÎŞ
±¸×¢£º10HzÀäÈ´Ïß³Ì
***********************************************************/

void GunHeart_CalcProcess(uint16_t BoomSpeed)
{
  GunHeart_Value = GunHeart_Value + BoomSpeed;
}

void GunHeart_CoolingProcess(void)
{
	GunHeart_Value = (int)(GunHeart_Value - Cooling_Value_PerSec/10);
	GunHeart_Value = (GunHeart_Value<0 ? 0 : GunHeart_Value);
}
