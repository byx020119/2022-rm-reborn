/*
 * @Descripttion: 
 * @version: 1.0
 * @Author: ShaHao
 * @Date: 2021-06-13 15:07:45
 * @LastEditors: ShaHao
 * @LastEditTime: 2021-06-13 16:16:33
 */


//#include "Headfile.h"
#include "ADRC_sha.h"
#include "math.h"
#include "common.h"
#include "main.h"

Fhan_Data ADRC_SPEED_Controller;

float adrc_kp = 0;
float adrc_kd = 0;
const float woy = 50 ;
const float wcy = 30;//15

const float woy_x = 30;
const float wcy_x = 30;//150.

const float wop = 45;
const float wcp = 25;//15
float last_u = 0;

const float ADRC_Unit[4][16]=
{
  /*TD跟踪微分器   改进最速TD,h0=N*h      扩张状态观测器ESO           扰动补偿     非线性组合*/
  /*  r     h      N                 beta_01   beta_02    beta_03       b0     beta_0  beta_1     beta_2     N1     C    alpha1  alpha2  zeta     b*/
  {4000000 ,0.002 , 8,               3*woy,    3*woy*woy, woy*woy*woy,  1,    0.85,     600 ,       500,    2,     5,    0.9,   1.2,    0.005,    0.03,   },//{1000000 ,0.002 , 5,               3*wo,      3*wo*wo,    wo*wo*wo,  1,    0.85,   wc*wc,      2*wc,       2,    5,    0.9,   1.2,    0.03,    0.1}
  {1000000 ,0.002 , 5,               3*wop,    3*wop*wop, wop*wop*wop,  0.75  ,   0  ,   1800,       550,     2,     5,    0.9,   1.2,    0.005,    0.05,   }, 
  {300000  ,0.005 , 3,               300,      4000,      10000,        100,   0.2,    1.2,       0.0010,    5,     5,    0.8,   1.5,    0.03,    0.05},
};

const float ADRC_Unit_SPEED[1][16]=
{
  /*TD跟踪微分器   改进最速TD,h0=N*h      扩张状态观测器ESO           扰动补偿     非线性组合*/
  /*  r     h      N                 beta_01   beta_02    beta_03       b0     beta_0  beta_1     beta_2     N1     C    alpha1  alpha2  zeta     b*/
  {3000 ,0.002 , 8,       3*woy_x, 3*woy_x*woy_x,woy_x*woy_x*woy_x,  0.5,   0.85,   3000,        3000,    2,     5,    0.9,   1.2,    1,    0.05,   },//{1000000 ,0.002 , 5,               3*wo,      3*wo*wo,    wo*wo*wo,  1,    0.85,   wc*wc,      2*wc,       2,    5,    0.9,   1.2,    0.03,    0.1}
};

float Constrain_Float(float amt, float low, float high)
{
  return ((amt)<(low)?(low):((amt)>(high)?(high):(amt)));
}
 
// Butter_Parameter ADRC_Div_LPF_Parameter={
//   //200---20hz
//   1,    -1.14298050254,   0.4128015980962,
//   0.06745527388907,   0.1349105477781,  0.06745527388907
// };
// float ADRC_LPF(float curr_inputer,Butter_BufferData *Buffer,Butter_Parameter *Parameter)
// {
//   /* 加速度计Butterworth滤波 */
//   /* 获取最新x(n) */
//   Buffer->Input_Butter[2]=curr_inputer;
//   /* Butterworth滤波 */
//   Buffer->Output_Butter[2]=
//     Parameter->b[0] * Buffer->Input_Butter[2]
//       +Parameter->b[1] * Buffer->Input_Butter[1]
//     +Parameter->b[2] * Buffer->Input_Butter[0]
//           -Parameter->a[1] * Buffer->Output_Butter[1]
//             -Parameter->a[2] * Buffer->Output_Butter[0];
//   /* x(n) 序列保存 */
//   Buffer->Input_Butter[0]=Buffer->Input_Butter[1];
//   Buffer->Input_Butter[1]=Buffer->Input_Butter[2];
//   /* y(n) 序列保存 */
//   Buffer->Output_Butter[0]=Buffer->Output_Butter[1];
//   Buffer->Output_Butter[1]=Buffer->Output_Butter[2];
//   return (Buffer->Output_Butter[2]);
// }
 
 
 
 
int Sign_ADRC(float Input)
{
  int output=0;
  if(Input>1E-6) output=1;
  else if(Input<-1E-6) output=-1;
  else output=0;
  return output;
}
int Fsg_ADRC(float x,float d)
{
  int output=0;
  output=(Sign_ADRC(x+d)-Sign_ADRC(x-d))/2;
  return output;
}
//void ADRC_Init(Fhan_Data *fhan_Input1,Fhan_Data *fhan_Input2)
//{
//	if(GetWorkState() == XTL_STATE )
//	{
//		fhan_Input1->r=ADRC_Unit_XTL[0][0];
//		fhan_Input1->h=ADRC_Unit_XTL[0][1];
//		fhan_Input1->N0=(int)(ADRC_Unit_XTL[0][2]);
//		fhan_Input1->beta_01=ADRC_Unit_XTL[0][3];
//		fhan_Input1->beta_02=ADRC_Unit_XTL[0][4];
//		fhan_Input1->beta_03=ADRC_Unit_XTL[0][5];
//		fhan_Input1->b0=ADRC_Unit_XTL[0][6];
//		fhan_Input1->beta_0=ADRC_Unit_XTL[0][7];
//		fhan_Input1->beta_1=ADRC_Unit_XTL[0][8];
//		fhan_Input1->beta_2=ADRC_Unit_XTL[0][9];
//		fhan_Input1->N1=(int)(ADRC_Unit_XTL[0][10]);
//		fhan_Input1->c=ADRC_Unit_XTL[0][11];
//		
//		fhan_Input1->alpha1=ADRC_Unit_XTL[0][12];
//		fhan_Input1->alpha2=ADRC_Unit_XTL[0][13];
//		fhan_Input1->zeta=ADRC_Unit_XTL[0][14];
//	}
//	else
//	{
//		fhan_Input1->r=ADRC_Unit[0][0];
//		fhan_Input1->h=ADRC_Unit[0][1];
//		fhan_Input1->N0=(int)(ADRC_Unit[0][2]);
//		fhan_Input1->beta_01=ADRC_Unit[0][3];
//		fhan_Input1->beta_02=ADRC_Unit[0][4];
//		fhan_Input1->beta_03=ADRC_Unit[0][5];
//		fhan_Input1->b0=ADRC_Unit[0][6];
//		fhan_Input1->beta_0=ADRC_Unit[0][7];
//		fhan_Input1->beta_1=ADRC_Unit[0][8];
//		fhan_Input1->beta_2=ADRC_Unit[0][9];
//		fhan_Input1->N1=(int)(ADRC_Unit[0][10]);
//		fhan_Input1->c=ADRC_Unit[0][11];
//		
//		fhan_Input1->alpha1=ADRC_Unit[0][12];
//		fhan_Input1->alpha2=ADRC_Unit[0][13];
//		fhan_Input1->zeta=ADRC_Unit[0][14];
//	}

//  //fhan_Input1->b=ADRC_Unit[0][15];
////		///////////////////////////////
////	fhan_Input1->Last_TD_Input=0;
////	fhan_Input1->TD_Input=0;
////	fhan_Input1->TD_Input_Div=0;
////	fhan_Input1->z1 = 0;
////	fhan_Input1->z2 = 0;
////	fhan_Input1->z3 = 0;
////	fhan_Input1->x1 = 0;
////	fhan_Input1->x2 = 0;
////	fhan_Input1->y = 0;
////	fhan_Input1->e = 0;

//	/////////////////////////////
////	fhan_Input1->Last_ESO_Input=0;
////	fhan_Input1->ESO_Input=0;
////	fhan_Input1->ESO_Input_Div=0;
//  
//  fhan_Input2->r=ADRC_Unit[1][0];
//  fhan_Input2->h=ADRC_Unit[1][1];
//  fhan_Input2->N0=(int)(ADRC_Unit[1][2]);
//  fhan_Input2->beta_01=ADRC_Unit[1][3];
//  fhan_Input2->beta_02=ADRC_Unit[1][4];
//  fhan_Input2->beta_03=ADRC_Unit[1][5];
//  fhan_Input2->b0=ADRC_Unit[1][6];
//  fhan_Input2->beta_0=ADRC_Unit[1][7];
//  fhan_Input2->beta_1=ADRC_Unit[1][8];
//  fhan_Input2->beta_2=ADRC_Unit[1][9];
//  fhan_Input2->N1=(int)(ADRC_Unit[1][10]);
//  fhan_Input2->c=ADRC_Unit[1][11];
//  
//  fhan_Input2->alpha1=ADRC_Unit[1][12];
//  fhan_Input2->alpha2=ADRC_Unit[1][13];
//  fhan_Input2->zeta=ADRC_Unit[1][14];
//	///////////////////////////////
////	fhan_Input2->Last_TD_Input=0;
////	fhan_Input2->TD_Input=0;
////	fhan_Input2->TD_Input_Div=0;

////	/////////////////////////////
////	fhan_Input2->Last_ESO_Input=0;
////	fhan_Input2->ESO_Input=0;
////	fhan_Input2->ESO_Input_Div=0;


//}
 
 
 
//ADRC最速跟踪微分器TD，改进的算法fhan
void Fhan_ADRC(Fhan_Data *fhan_Input,float expect_ADRC)//安排ADRC过度过程
{
  float d=0,a0=0,y=0,a1=0,a2=0,a=0;
  float x1_delta=0;//ADRC状态跟踪误差项
  x1_delta=fhan_Input->x1-expect_ADRC;//用x1-v(k)替代x1得到离散更新公式
  fhan_Input->h0=fhan_Input->N0*fhan_Input->h;//用h0替代h，解决最速跟踪微分器速度超调问题
  d=fhan_Input->r*fhan_Input->h0*fhan_Input->h0;//d=rh^2;//////////////////
  a0=fhan_Input->h0*fhan_Input->x2;//a0=h*x2
  y=x1_delta+a0;//y=x1+a0
  a1=sqrt(d*(d+8*fabs(y)));//a1=sqrt(d*(d+8*ABS(y))])
  a2=a0+Sign_ADRC(y)*(a1-d)/2;//a2=a0+sign(y)*(a1-d)/2;
  a=(a0+y)*Fsg_ADRC(y,d)+a2*(1-Fsg_ADRC(y,d));
  fhan_Input->fh=-fhan_Input->r*(a/d)*Fsg_ADRC(a,d)
    -fhan_Input->r*Sign_ADRC(a)*(1-Fsg_ADRC(a,d));//得到最速微分加速度跟踪量
  fhan_Input->x1+=fhan_Input->h*fhan_Input->x2;//跟新最速跟踪状态量x1
  fhan_Input->x2+=fhan_Input->h*fhan_Input->fh;//跟新最速跟踪状态量微分x2
}

 
//原点附近有连线性段的连续幂次函数
float Fal_ADRC(float e,float alpha,float zeta)
{
  int s=0;
  float fal_output=0;
  s=(Sign_ADRC(e+zeta)-Sign_ADRC(e-zeta))/2;
  fal_output=e*s/(powf(zeta,1-alpha))+powf(fabs(e),alpha)*Sign_ADRC(e)*(1-s);
  return fal_output;
}
/************扩张状态观测器********************/
//状态观测器参数beta01=1/h  beta02=1/(3*h^2)  beta03=2/(8^2*h^3) ...
void ESO_ADRC(Fhan_Data *fhan_Input)
{
  fhan_Input->e=fhan_Input->z1-fhan_Input->y;//状态误差
  
  fhan_Input->fe=Fal_ADRC(fhan_Input->e,0.5,fhan_Input->N1*fhan_Input->h);//非线性函数，提取跟踪状态与当前状态误差
  fhan_Input->fe1=Fal_ADRC(fhan_Input->e,0.25,fhan_Input->N1*fhan_Input->h);
  
  /*************扩展状态量更新**********/
/////////////////////非线性版////////////////////
   fhan_Input->z1+=fhan_Input->h*(fhan_Input->z2-fhan_Input->beta_01*fhan_Input->e);
   fhan_Input->z2+=fhan_Input->h*(fhan_Input->z3
                                  -fhan_Input->beta_02*fhan_Input->fe
                                    +fhan_Input->b0*fhan_Input->u);
   //ESO估计状态加速度信号，进行扰动补偿，传统MEMS陀螺仪漂移较大，估计会产生漂移
   fhan_Input->z3+=fhan_Input->h*(-fhan_Input->beta_03*fhan_Input->fe1);

	VAL_LIMIT(fhan_Input->z3,-6000,6000);
/////////////////////线性版////////////////////
//  fhan_Input->z1+=fhan_Input->h*(fhan_Input->z2-fhan_Input->beta_01*fhan_Input->e);
//  fhan_Input->z2+=fhan_Input->h*(fhan_Input->z3-fhan_Input->beta_02*fhan_Input->e+fhan_Input->b0*fhan_Input->u);
//  fhan_Input->z3+=fhan_Input->h*(-fhan_Input->beta_03*fhan_Input->e);

}
/************非线性组合****************/
/*
void Nolinear_Conbination_ADRC(Fhan_Data *fhan_Input)
{
float d=0,a0=0,y=0,a1=0,a2=0,a=0;
float Sy=0,Sa=0;//ADRC状态跟踪误差项
fhan_Input->h1=fhan_Input->N1*fhan_Input->h;
d=fhan_Input->r*fhan_Input->h1*fhan_Input->h1;
a0=fhan_Input->h1*fhan_Input->c*fhan_Input->e2;
y=fhan_Input->e1+a0;
a1=sqrt(d*(d+8*ABS(y)));
a2=a0+Sign_ADRC(y)*(a1-d)/2;
Sy=Fsg_ADRC(y,d);
a=(a0+y-a2)*Sy+a2;
Sa=Fsg_ADRC(a,d);
fhan_Input->u0=-fhan_Input->r*((a/d)-Sign_ADRC(a))*Sa-fhan_Input->r*Sign_ADRC(a);
//a=(a0+y)*Fsg_ADRC(y,d)+a2*(1-Fsg_ADRC(y,d));
//fhan_Input->fh=-fhan_Input->r*(a/d)*Fsg_ADRC(a,d)
//                -fhan_Input->r*Sign_ADRC(a)*(1-Fsg_ADRC(a,d));//得到最速微分加速度跟踪量
}
*/
/////非线性组合
// void Nolinear_Conbination_ADRC(Fhan_Data *fhan_Input)
// {
//   float temp_e2=0;
//   //temp_e2=Constrain_Float(fhan_Input->e2,-3000,3000);
//   temp_e2=Constrain_Float(fhan_Input->e2_lpf,-3000,3000);
//   fhan_Input->u0=fhan_Input->beta_1*Fal_ADRC(fhan_Input->e1,fhan_Input->alpha1,fhan_Input->zeta)
//     +fhan_Input->beta_2*Fal_ADRC(temp_e2,fhan_Input->alpha2,fhan_Input->zeta);
  
// }

void ADRC_Control(Fhan_Data *fhan_Input,float expect_ADRC,float feedback_ADRC)
{
  fhan_Input->Last_TD_Input=fhan_Input->TD_Input;
  fhan_Input->TD_Input=expect_ADRC;
  fhan_Input->TD_Input_Div=(fhan_Input->TD_Input-fhan_Input->Last_TD_Input)/fhan_Input->h;
  
  fhan_Input->Last_ESO_Input=fhan_Input->ESO_Input;
  fhan_Input->ESO_Input=feedback_ADRC;
  fhan_Input->ESO_Input_Div=(fhan_Input->ESO_Input-fhan_Input->Last_ESO_Input)/fhan_Input->h;
  
  /*自抗扰控制器第1步*/
  /********
      **
      **
      **
      **
      **
  ********/
  /*****
  安排过度过程，输入为期望给定，
  由TD跟踪微分器得到：
  过度期望信号x1，过度期望微分信号x2
  ******/
	
  Fhan_ADRC(fhan_Input,expect_ADRC);
  
  /*自抗扰控制器第2步*/
  /********
          *
          *
  *  * *  *
  *
  *
  ********/
  /************系统输出值为反馈量，状态反馈，ESO扩张状态观测器的输入*********/
  fhan_Input->y=feedback_ADRC;
	
  /*****
  扩张状态观测器，得到反馈信号的扩张状态：
  1、状态信号z1；
  2、状态速度信号z2；
  3、状态加速度信号z3。
  其中z1、z2用于作为状态反馈与TD微分跟踪器得到的x1,x2做差后，
  经过非线性函数映射，乘以beta系数后，
  组合得到未加入状态加速度估计扰动补偿的原始控制量u
  *********/
  ESO_ADRC(fhan_Input);//低成本MEMS会产生漂移，扩展出来的z3此项会漂移，目前暂时未想到办法解决，未用到z3
  /*自抗扰控制器第3步*/
  /********
         **
      **
     **
      **
        **
  ********/
  /********状态误差反馈率***/
	//fhan_Input->x1 = expect_ADRC;
	//fhan_Input->x2 = 0;
	
  fhan_Input->e0+=fhan_Input->e1*fhan_Input->h;//状态积分项
  fhan_Input->e1=fhan_Input->x1-fhan_Input->z1;//状态偏差项
  fhan_Input->e2=-fhan_Input->z2;//fhan_Input->x2-fhan_Input->z2;//状态微分项
//   fhan_Input->e2_lpf=ADRC_LPF(fhan_Input->e2,
//                               &fhan_Input->ADRC_LPF_Buffer,
//                               &ADRC_Div_LPF_Parameter);//巴特沃斯低通后得到的微分项,20hz
  

  /********线性组合*******/
  
	
  fhan_Input->u0=fhan_Input->beta_0*fhan_Input->e1 + fhan_Input->beta_1*fhan_Input->e1+ fhan_Input->beta_2*fhan_Input->e2;//fhan_Input->beta_0*fhan_Input->e0
  //fhan_Input->u0=fhan_Input->beta_1*fhan_Input->e1;
	adrc_kp = fhan_Input->beta_1*fhan_Input->e1;
	adrc_kd = fhan_Input->beta_2*fhan_Input->e2;
	

  
  //Nolinear_Conbination_ADRC(fhan_Input);
  /**********扰动补偿*******/
  fhan_Input->u=fhan_Input->u0-fhan_Input->z3/fhan_Input->b0;//-fhan_Input->z3/fhan_Input->b;
	VAL_LIMIT(fhan_Input->u,-20000,20000);
    
  //fhan_Input->u+=Constrain_Float(fhan_Input->beta_0*fhan_Input->e0,-150,150);
  //由于MEMS传感器漂移比较严重，当beta_03取值比较大时，长时间z3漂移比较大，目前不加入扰动补偿控制量
  //fhan_Input->u=Constrain_Float(fhan_Input->u0,-450,450);//不加入扰动补偿
  //fhan_Input->u=Constrain_Float(fhan_Input->u,-450,450);//加入扰动补偿
}
void ADRC_Integrate_Reset(Fhan_Data *fhan_Input)  {fhan_Input->e0=0.0f;}


/////////////////////////////////////////////////////////////////////
void ADRC_Init_SPEED(Fhan_Data *fhan_Input1)
{
		fhan_Input1->r=ADRC_Unit_SPEED[0][0];
		fhan_Input1->h=ADRC_Unit_SPEED[0][1];
		fhan_Input1->N0=(int)(ADRC_Unit_SPEED[0][2]);
		fhan_Input1->beta_01=ADRC_Unit_SPEED[0][3];
		fhan_Input1->beta_02=ADRC_Unit_SPEED[0][4];
		fhan_Input1->beta_03=ADRC_Unit_SPEED[0][5];
		fhan_Input1->b0=ADRC_Unit_SPEED[0][6];
		fhan_Input1->beta_0=ADRC_Unit_SPEED[0][7];
		fhan_Input1->beta_1=ADRC_Unit_SPEED[0][8];
		fhan_Input1->beta_2=ADRC_Unit_SPEED[0][9];
		fhan_Input1->N1=(int)(ADRC_Unit_SPEED[0][10]);
		fhan_Input1->c=ADRC_Unit_SPEED[0][11];
		
		fhan_Input1->alpha1=ADRC_Unit_SPEED[0][12];
		fhan_Input1->alpha2=ADRC_Unit_SPEED[0][13];
		fhan_Input1->zeta=ADRC_Unit_SPEED[0][14];
}

void ADRC_Control_SPEED(Fhan_Data *fhan_Input,float expect_ADRC)
{
  fhan_Input->Last_TD_Input=fhan_Input->TD_Input;
  fhan_Input->TD_Input=expect_ADRC;
  fhan_Input->TD_Input_Div=(fhan_Input->TD_Input-fhan_Input->Last_TD_Input)/fhan_Input->h;
  
  
  /*自抗扰控制器第1步*/
  /********
      **
      **
      **
      **
      **
  ********/
  /*****
  安排过度过程，输入为期望给定，
  由TD跟踪微分器得到：
  过度期望信号x1，过度期望微分信号x2
  ******/
	
  Fhan_ADRC(fhan_Input,expect_ADRC);
}


/////////////////////////////////////////////////////////////////////
//void ADRC_Init_SPEED_XTL(Fhan_Data *fhan_Input1)
//{
//		fhan_Input1->r=ADRC_Unit_SPEED_XTL[0][0];
//		fhan_Input1->h=ADRC_Unit_SPEED_XTL[0][1];
//		fhan_Input1->N0=(int)(ADRC_Unit_SPEED_XTL[0][2]);
//		fhan_Input1->beta_01=ADRC_Unit_SPEED_XTL[0][3];
//		fhan_Input1->beta_02=ADRC_Unit_SPEED_XTL[0][4];
//		fhan_Input1->beta_03=ADRC_Unit_SPEED_XTL[0][5];
//		fhan_Input1->b0=ADRC_Unit_SPEED_XTL[0][6];
//		fhan_Input1->beta_0=ADRC_Unit_SPEED_XTL[0][7];
//		fhan_Input1->beta_1=ADRC_Unit_SPEED_XTL[0][8];
//		fhan_Input1->beta_2=ADRC_Unit_SPEED_XTL[0][9];
//		fhan_Input1->N1=(int)(ADRC_Unit_SPEED_XTL[0][10]);
//		fhan_Input1->c=ADRC_Unit_SPEED_XTL[0][11];
//		
//		fhan_Input1->alpha1=ADRC_Unit_SPEED_XTL[0][12];
//		fhan_Input1->alpha2=ADRC_Unit_SPEED_XTL[0][13];
//		fhan_Input1->zeta=ADRC_Unit_SPEED_XTL[0][14];
//}

void ADRC_Control_SPEED_XTL(Fhan_Data *fhan_Input,float expect_ADRC)
{
  fhan_Input->Last_TD_Input=fhan_Input->TD_Input;
  fhan_Input->TD_Input=expect_ADRC;
  fhan_Input->TD_Input_Div=(fhan_Input->TD_Input-fhan_Input->Last_TD_Input)/fhan_Input->h;
  
  
  /*自抗扰控制器第1步*/
  /********
      **
      **
      **
      **
      **
  ********/
  /*****
  安排过度过程，输入为期望给定，
  由TD跟踪微分器得到：
  过度期望信号x1，过度期望微分信号x2
  ******/
	
  Fhan_ADRC(fhan_Input,expect_ADRC);
}