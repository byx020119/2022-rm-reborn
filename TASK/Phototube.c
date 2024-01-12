//δ��

#include "Phototube.h"
#include "encoder.h"
#include "ControlTask.h"
#include "stdio.h"
#include "FreeDom_Status.h"
#include "Doget_state.h"
#include "stdlib.h"
#include "common.h"
#include "usart3.h"

/***
���ܵ����
����ʵ���ǹ���GPIO�˿��ϵģ����԰��ն˿ڴ���Ϳɡ�
�ȳ�ʼ���˿ڣ��ٵõ����ڶ˿ڵĸߵ͵�ƽֵ��
������ܵ��������޸ĵ��̻����־��
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
uint8_t Phototube_Middle_Flag     = 0;  //�м���ܼ�⵽���ӱ�־
uint8_t Phototube_Right_Flag    = 0;    //�Ҳ���ܼ�⵽���ӱ�־
int Phototube_Time_Count_R=0;      //���ܼ�ʱ ��
int Phototube_Time_Count_L=0;      //���ܼ�ʱ ��
int Phototube_brake=0;             //��ഫ��ɲ���ı�־���˱�־�ĸ�λ��BrakeTask��
int Phototube_brake1=0;  




/***
������Phototube_Configuration()
���ܣ�����IO��ʼ��
��ע��
 C��
		 PC6���Ҳ����
		 PA1���м����//�Ѳ�ж
		 PI7��������
 A��
 		 PI6���Ҳ����
		 PA1���м����//�Ѳ�ж
		 PI7��������
***/

void Phototube_Configuration(void)
{

	GPIO_InitTypeDef GPIO_PI7_InitStructure;
	GPIO_InitTypeDef GPIO_PI6_InitStructure;
	GPIO_InitTypeDef GPIO_PI5_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI,ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	/***---�Ҳ����***/
	// F
	GPIO_PI5_InitStructure.GPIO_Pin=GPIO_Pin_5;
	GPIO_PI5_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_PI5_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_PI5_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_PI5_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;	
	GPIO_Init(GPIOC,&GPIO_PI5_InitStructure);
	
	/***---������**/
	//G
	GPIO_PI7_InitStructure.GPIO_Pin=GPIO_Pin_7;
	GPIO_PI7_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_PI7_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_PI7_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_PI7_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;	
	GPIO_Init(GPIOI,&GPIO_PI7_InitStructure);
	
	//����

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
���ܣ����ܸ�ֵ��������״̬���޸ĵ��̻����־
��ע���ȼ��PI7��ƽֵ������Ǹߵ�ƽ���͵�ƽ������Ϊ�Ǽ�⵽���ӣ��޸ı�־����ʼ����ʱ��
***/
/***�Ҳ����**/
	//PI7������˿ӡ

	  PI7_Level = PIin(7);
		if(lastPI7_Level==1 && PI7_Level==0)
		{
			Phototube_Left_Flag = 1;//�����ܼ�⵽���ӱ�־
			Phototube_Time_Count_R=0;
			senddata_control(0);//������ͣ����
		}
		if(lastPI7_Level==1 && PI7_Level==1){
			
		}
		
		if(Phototube_Left_Flag == 1)  
		{
			Dir_Change_Flag=1;//���̻����־
			Chassis_Freedom_i=0;//��������״̬�����־
			CM1Encoder.round_cnt = 0;        //��CM1���̵�Ȧ������
			CM1Encoder.raw_value = 0;        //��CM1���̵���������ֵ����
			Phototube_Left_Flag = 0;       //��־����
			Destination_Flag = 1;
	//scc		Aerocraft_Phototube_flag=0;
		}
		if(PI7_Level==0)
		{
			Phototube_Time_Count_R++;
			if(Phototube_Time_Count_R>500)
			{
			 Chassis_Freedom_i=0;//0����1����
	     Dir_Change_Flag=1;
			}
		}
		lastPI7_Level = PI7_Level; //�ϴ�PI7�ĵ�ƽֵ
	/***************/

/***
���ܣ����ܸ�ֵ
��ע����
***/
/**������**/
		//PI6������˿ӡ
		PI6_Level = PIin(6);  //����PI6��ƽֵ ;  2022A���PI5�����⣬��ΪPI6
		if(lastPI6_Level==1 && PI6_Level==0)
		{
			Phototube_Right_Flag = 1;//�Ҳ���ܼ�⵽���ӱ�־1
			Phototube_Time_Count_L=0;
			senddata_control(0);//������ͣ����
		}
		if(lastPI6_Level==1 && PI6_Level==1){

		}
		if(Phototube_Right_Flag == 1)  
		{
			Dir_Change_Flag=1; 
			Chassis_Freedom_i=1;
			CM1Encoder.round_cnt = 0;        //��CM1���̵�Ȧ������
			CM1Encoder.raw_value = 0;        //��CM1���̵���������ֵ����
			Phototube_Right_Flag = 0;       //��־����
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
		lastPI6_Level = PI6_Level; //�ϴ�PI6��ƽֵ
		
}





