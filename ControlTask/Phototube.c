//未改

#include "Phototube.h"
#include "encoder.h"
#include "ControlTask.h"
#include "stdio.h"
#include "FreeDom_Status.h"
#include "DogetState.h"
#include "stdlib.h"
#include "common.h"

/***
光电管的理解
光电管实际是挂在GPIO端口上的，所以按照端口处理就可。
先初始化端口，再得到现在端口的高低电平值，
代表光电管的亮或灭，修改底盘换向标志。
***/


uint8_t lastPI7_Level = 0;
uint8_t PI7_Level     = 0;
uint8_t lastPC6_Level = 0;
uint8_t PC6_Level     = 0;
uint8_t lastPI6_Level = 0;
uint8_t PI6_Level     = 0;
uint16_t Dir_Change_Flag=0;
int Phototube_Flag = 0;
uint8_t Phototube_Left_Flag  = 0;
uint8_t Phototube_Middle_Flag     = 0;  //中间光电管检测到柱子标志
uint8_t Phototube_Right_Flag    = 0;    //右侧光电管检测到柱子标志
int Phototube_Time_Count_R=0;      //光电管计时 右
int Phototube_Time_Count_L=0;      //光电管计时 左
int Phototube_brake=0;             //左侧传给刹车的标志，此标志的复位在BrakeTask中
int Phototube_brake1=0;  




/***
函数：Phototube_Configuration()
功能：光电管IO初始化
备注：
		 PI7：左侧光电管
		 PA1：中间光电管//已拆卸
		 PI6：右侧光电管
***/

void Phototube_Configuration(void)
{

	GPIO_InitTypeDef GPIO_PI7_InitStructure;
	GPIO_InitTypeDef GPIO_PI6_InitStructure;
	GPIO_InitTypeDef GPIO_PC6_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI,ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);

	//备用
	GPIO_PC6_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_PC6_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_PC6_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_PC6_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_PC6_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;	
	GPIO_Init(GPIOC,&GPIO_PC6_InitStructure);
	
	/***---左侧光电管**/
	//G
	GPIO_PI7_InitStructure.GPIO_Pin=GPIO_Pin_7;
	GPIO_PI7_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_PI7_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_PI7_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_PI7_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;	
	GPIO_Init(GPIOI,&GPIO_PI7_InitStructure);
	
	/***---右侧光电管***/
	// F
	GPIO_PI6_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_PI6_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_PI6_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_PI6_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_PI6_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;	
	GPIO_Init(GPIOI,&GPIO_PI6_InitStructure);
}

void Phototube_Handle(void)
{
	
	
/***
功能：光电管赋值，检测光电管状态，修改底盘换向标志
备注：先检测PI7电平值，如果是高电平到低电平，则认为是检测到柱子，修改标志，初始化计时。
***/
/***左侧光电管**/
	//PI7见背面丝印

	  PI7_Level = PCin(6);
		if(lastPI7_Level==1 && PI7_Level==0)
		{
			Phototube_Left_Flag = 1;//左侧光电管检测到柱子标志
			Phototube_Time_Count_R=0;
			Phototube_brake=1;//刹车
		}
		if(Phototube_Left_Flag == 1)  
		{
			Dir_Change_Flag=1;//底盘换向标志
			Chassis_Freedom_i=0;//底盘自由状态方向标志
			CM1Encoder.round_cnt = 0;        //将CM1码盘的圈数清零
			CM1Encoder.raw_value = 0;        //将CM1码盘的码盘现有值清零
			Phototube_Left_Flag = 0;       //标志清零
			Destination_Flag = 1;
	//scc		Aerocraft_Phototube_flag=0;
		}
		if(PI7_Level==0)
		{
			Phototube_Time_Count_R++;
			if(Phototube_Time_Count_R>500)
			{
			 Chassis_Freedom_i=0;//0是左，1是右
	     Dir_Change_Flag=1;
			}
		}
		lastPI7_Level = PI7_Level; //上次PI7的电平值
	/***************/

/***
功能：光电管赋值
备注：无
***/
/**右侧光电管**/
		//PI6见背面丝印
		PI6_Level = PIin(7);  //本次PI7电平值
		if(lastPI6_Level==1 && PI6_Level==0)
		{
			Phototube_Right_Flag = 1;//右侧光电管检测到柱子标志1
			Phototube_Time_Count_L=0;
			Phototube_brake1=1;//刹车
		}
		if(Phototube_Right_Flag == 1)  
		{
			Dir_Change_Flag=1; 
			Chassis_Freedom_i=1;
			CM1Encoder.round_cnt = 0;        //将CM1码盘的圈数清零
			CM1Encoder.raw_value = 0;        //将CM1码盘的码盘现有值清零
			Phototube_Right_Flag = 0;       //标志清零
			Destination_Flag = 1;
		}
		if(PI6_Level==0)
		{
			Phototube_Time_Count_L++;
			if(Phototube_Time_Count_L>500)
			{
			 Chassis_Freedom_i=1;
			 Dir_Change_Flag=1;
			}
		}
		lastPI6_Level = PI6_Level; //上次PI6电平值
		
}





