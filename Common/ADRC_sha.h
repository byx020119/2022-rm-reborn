/*
 * @Descripttion: 
 * @version: 1.0
 * @Author: ShaHao
 * @Date: 2021-06-13 15:08:37
 * @LastEditors: ShaHao
 * @LastEditTime: 2021-06-13 15:22:15
 */

#ifndef _ADRC_H_
#define _ADRC_H_


typedef struct
{
/*****安排过度过程*******/
float x1;//跟踪微分期状态量
float x2;//跟踪微分期状态量微分项
float r;//时间尺度
float h;//ADRC系统积分时间
int N0;//跟踪微分器解决速度超调h0=N*h
 
float h0;
float fh;//最速微分加速度跟踪量
/*****扩张状态观测器*******/
/******已系统输出y和输入u来跟踪估计系统状态和扰动*****/
float z1;
float z2;
float z3;//根据控制对象输入与输出，提取的扰动信息
float e;//系统状态误差
float y;//系统输出量
float fe;
float fe1;
float beta_01;//L1,观测器误差反馈增益矩阵的  反馈系数
float beta_02;//L2,
float beta_03;//L3,
 
/**********系统状态误差反馈率*********/
float e0;//状态误差积分项
float e1;//状态偏差
float e2;//状态量微分项
float u0;//非线性组合系统输出
float u;//带扰动补偿后的输出
 
 
/*********第一种组合形式*********/
float beta_0;//线性
float beta_1;//非线性组合参数
float beta_2;//u0=beta_1*e1+beta_2*e2+(beta_0*e0);
/*********第二种组合形式*********/
float alpha1;//u0=beta_1*fal(e1,alpha1,zeta)+beta_2*fal(e2,alpha2,zeta)
float alpha2;//0<alpha1<1<alpha2
float zeta;//线性段的区间长度
/*********第三种组合形式*********/
float h1;//u0=-fhan(e1,e2,r,h1);
int N1;//跟踪微分器解决速度超调h0=N*h
/*********第四种组合形式*********/
float c;//u0=-fhan(e1,c*e2*e2,r,h1);
float b0;//扰动补偿
//////////////////////////////////////////////////
float Last_TD_Input;
float TD_Input;
float TD_Input_Div;

/////////////////////////////
float Last_ESO_Input;
float ESO_Input;
float ESO_Input_Div;

}Fhan_Data;
 
 
 
void ADRC_Init(Fhan_Data *fhan_Input1,Fhan_Data *fhan_Input2);
void ADRC_Init_SPEED(Fhan_Data *fhan_Input1);
void ADRC_Init_SPEED_XTL(Fhan_Data *fhan_Input1);
void Fhan_ADRC(Fhan_Data *fhan_Input,float expect_ADRC);
void ADRC_Control(Fhan_Data *fhan_Input,float expect_ADRC,float feedback);
void ADRC_Control_SPEED(Fhan_Data *fhan_Input,float expect_ADRC);
void ADRC_Control_SPEED_XTL(Fhan_Data *fhan_Input,float expect_ADRC);
 
extern Fhan_Data ADRC_Pitch_Controller,ADRC_Yaw_Controller, ADRC_SPEED_Controller,ADRC_SPEED_Controller_XTL;
extern float last_u;
#endif
