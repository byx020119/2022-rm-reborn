//ע���˲��ֺ�Ħ���֣��ޱ���

#include "RemoteTask.h"
#include "stdio.h"
#include "SpeedTask.h"
#include "WorkState.h"
#include "ShootingTask.h"

RC_Ctl_t RC_CtrlData;
double pitch_err = 0;//2022�ӣ�p����������,�����Ӿ���
int pitch_err_flag = 0;//2022�� p������������־λ��ֻ�б�־λΪ1��-1��ch0Ϊ0ʱ��pitch_errֵ�ӻ��һ����λ

/**
������RemoteDataPrcess(pData)
���ܣ���ң�������ݽ��д���
��ע��
        1                               1
   (s1) 3                          (s2) 3
	      2                               2
				
			(ch3)              (ch1)
       |                  |
       |                  |
	-----------(ch2)  -------------(ch0)     
			 |                  |
			 |                  |
		 ң��������ģʽʱs2���ϣ�
		 1.ch1���Ƶ���ǰ���ƶ�
		 2.ch2����yawˮƽ��ת(�ۼӡ���λ)
		 3.ch3����pitch��ֱ�ڶ�(�ۼӡ���λ)
		 4.s1�⵽�����棬ȫ��,ɲ������
		 5.s2�⵽�м䣬Ħ���ֿ������ֹ�
		 6.s2�⵽���ϣ�ȫ��
		 
		 //ң��������ģʽs2=1��
		 1.ch1���Ƶ���ǰ���ƶ�
		 2.ch2����yawˮƽ��ת(�ۼӡ���λ)
		 3.ch3����pitch��ֱ�ڶ�(�ۼӡ���λ)
		 4.s2=2,ֹͣ״̬ s2=3,����״̬ s2=1,׼��״̬��2s���������״̬
		 5.s1����ɲ����
		 
**/
void RemoteDataPrcess(uint8_t *pData)
{

	if(pData == NULL)
	{
			return;
	}

//	GetRemoteSwitchAction(switch1, s1);
//	RemoteTest_Flag = 1;//ң������ʱ��ң�������Ա�־��Ϊ1��
	Remote_microsecond.time_last = Get_Time_Micros();
	
	//ch0~ch3:max=1684,min=364,|error|=660
	RC_CtrlData.rc.ch0 = ((int16_t)pData[0] | ((int16_t)pData[1] << 8)) & 0x07FF; 
	RC_CtrlData.rc.ch1 = (((int16_t)pData[1] >> 3) | ((int16_t)pData[2] << 5)) & 0x07FF;
	RC_CtrlData.rc.ch2 = (((int16_t)pData[2] >> 6) | ((int16_t)pData[3] << 2) | ((int16_t)pData[4] << 10)) & 0x07FF;
	RC_CtrlData.rc.ch3 = (((int16_t)pData[4] >> 1) | ((int16_t)pData[5]<<7)) & 0x07FF;
	
	RC_CtrlData.rc.s1 = ((pData[5] >> 4) & 0x000C) >> 2;
	RC_CtrlData.rc.s2 = ((pData[5] >> 4) & 0x0003);
	
  RC_CtrlData.mouse.x = ((int16_t)pData[6]) | ((int16_t)pData[7] << 8);//�������
	RC_CtrlData.mouse.y = ((int16_t)pData[8]) | ((int16_t)pData[9] << 8);//�������
	RC_CtrlData.mouse.z = ((int16_t)pData[10]) | ((int16_t)pData[11] << 8);    //û�õ�

	RC_CtrlData.mouse.press_l = pData[12];//������
	RC_CtrlData.mouse.press_r = pData[13];//����Ҽ�

	RC_CtrlData.key.v = ((int16_t)pData[14]) | ((int16_t)pData[15] << 8);//ÿһλ��Ӧһ������
	
	Chassis_Speed_Ref = (RC_CtrlData.rc.ch1-1024);
	GimbalRef.pitch_angle_dynamic_ref +=(RC_CtrlData.rc.ch3-1024)*0.003;//*0.01   /*ԭ0.005*/
	GimbalRef.yaw_angle_dynamic_ref  +=(RC_CtrlData.rc.ch2-1024)*0.00285;//0.0085
	
	/****************�������ݺ���************************/
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
	
//	if((RC_CtrlData.rc.s1==2)&&(GetFrictionState()==FRICTION_WHEEL_OFF))//��Ħ����

	
	//�ⷢ���ӳ�ʹ��
//	if(RC_CtrlData.mouse.press_l==1 && RC_CtrlData.mouse.last_press_l ==0)
//	{
//		SetShootState(SHOOTING);
//		count_temp++;
//	}
//	RC_CtrlData.mouse.last_press_l = RC_CtrlData.mouse.press_l;
	
	if(RC_CtrlData.rc.s2==1)
	{
		RemoteTest_Flag = 2;//ң����S2���˲���1λ��ʱ��ң������־��Ϊ2���빤��״̬��Ӧ
	}
	
	if(RC_CtrlData.rc.s2==3)
	{
		RemoteTest_Flag = 1;//ң������ʱ��ң�������Ա�־��Ϊ1�����˲����м�λ��
	}
	
	if(RC_CtrlData.rc.s2==2)
	{
		RemoteTest_Flag = 0;//ң�����ر�ʱ��ң��������״̬��Ϊ0�����˲���������
		Brake_flag=0;//ɲ������
	}

	
	
 if(RC_CtrlData.rc.s2==1)//����״̬�У����˲���������
 {
	 
	 if(RC_CtrlData.rc.s1==3|| RC_CtrlData.rc.s1==1)//��Ħ���֣�1.3
	 {
			friction_wheel_state_flag = 1;
	 }
	
	
 	 if(RC_CtrlData.rc.s1==2)  //�ر�Ħ���� 2 ɲ��ģ������ƽ��
	 {
			friction_wheel_state_flag = 0;
		  
	 }  
	
	 if(RC_CtrlData.rc.s1==1)//�򿪲��� 1
	 {
			TempShootingFlag=1;
	 }
	

	 if(RC_CtrlData.rc.s1==2||RC_CtrlData.rc.s1==3)//�رղ��� 2.3
	 {
		  TempShootingFlag=0;
	 }
	 switch(RC_CtrlData.rc.s1)//ɲ��ģ��λ�ò��ԣ�����Ӱ��ӷ������ط��򿴣�
	 {
		 case 2://(�м�)
		 { 
			 Brake_flag=0;
			 break;
		 }
		 case 3://�����棩
		 {			
			 Brake_flag=-1;
			 break;
		 }
		 case 1://(������)
		 {
			 Brake_flag=1;
			 break;
		 }
	 }
	 
	
}	
 
 if(RC_CtrlData.rc.s2==3)//׼��->���ɣ�֮��ɽ���ģʽ��ת��  ���3������״̬
 {
 	 if(RC_CtrlData.rc.s1== 2)
	 {
//	 Attacked_Flag = 0;
//   CameraDetectTarget_Flag = 0;
//	 DodgeTarget_Flag = 0;
	 }
	 
 }

 }
 
