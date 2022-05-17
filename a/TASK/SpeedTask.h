#ifndef SpeedTask_H
#define SpeedTask_H
#include <stm32f4xx.h>
#include "pid.h"
#include "Ramp.h"

//pitchλ�û��ṹ���ʼ��
#define GIMBAL_MOTOR_PITCH_POSITION_PID_DEFAULT \
{\
	0,\
	0,\
	{0,0},\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	4900,\
	1000,\
	1500,\
	0,\
	4900,\
	&PID_Calc,\
	&PID_Reset,\
	0,\
	0,\
	0,\
}\

//pitch�ٶȻ��ṹ���ʼ��
#define GIMBAL_MOTOR_PITCH_SPEED_PID_DEFAULT \
{\
	0,\
	0,\
	{0,0},\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	25000,\
	3500,\
	1500,\
	0,\
	25000,\
	&PID_Calc,\
	&PID_Reset,\
	0,\
	5,\
	0,\
}\

//yawλ�û��ṹ���ʼ��
#define GIMBAL_MOTOR_YAW_POSITION_PID_DEFAULT \
{\
	0,\
	0,\
	{0,0},\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	20000,\
	1000,\
	1500,\
	0,\
	20000,\
	&PID_Calc,\
	&PID_Reset,\
	0,\
	0,\
	1,\
}\

//yaw�ٶȻ��ṹ���ʼ��
#define GIMBAL_MOTOR_YAW_SPEED_PID_DEFAULT \
{\
	0,\
	0,\
	{0,0},\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	20000,\
	3500,\
	1500,\
	0,\
	20000,\
	&PID_Calc,\
	&PID_Reset,\
	0,\
	5,\
	0,\
}\

//���̵��λ�û��ṹ���ʼ��
#define CHASSIS_MOTOR_POSITION_PID_DEFAULT \
{\
	0,\
	0,\
	{0,0},\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	4000,\
	300,\
	300,\
	0,\
	6000,\
	&PID_Calc,\
	&PID_Reset,\
	0,\
	0,\
	1,\
}\

//���̵���ٶȻ�PID��ʼ��
#define CHASSIS_MOTOR_SPEED_PID_DEFAULT \
{\
	0,\
	0,\
	{0,0},\
	0.0f,\
	0.0f,\
	0.0f,\
	0,\
	0,\
	0,\
	8000,\
	3000,\
	1000,\
	0,\
	8000,\
	&PID_Calc,\
	&PID_Reset,\
	0,\
	0,\
	0,\
}\
//�ٶȵ���PID��ʼ��
#define CHASSIS_SPEED_OFFSET_DEFAULT \
{\
	0,\
	0,\
	{0,0},\
	0.0f,\
	0.0f,\
	0.0f,\
	0,\
	0,\
	0,\
	400,\
	0,\
	100,\
	0,\
	400,\
	&PID_Calc,\
	&PID_Reset,\
	0,\
	0,\
	0,\
}\

//CM6λ�û��ṹ���ʼ�� 2022��
#define CM6_POSITION_PID_DEFAULT \
{\
	0,\
	0,\
	{0,0},\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	4900,\
	1000,\
	1500,\
	0,\
	4900,\
	&PID_Calc,\
	&PID_Reset,\
	0,\
	0,\
	0,\
}\
//CM6�ٶȻ�PID��ʼ�� 2022��
#define CM6_SPEED_PID_DEFAULT \
{\
	0,\
	0,\
	{0,0},\
	0.0f,\
	0.0f,\
	0.0f,\
	0,\
	0,\
	0,\
	5000,\
	3500,\
	1500,\
	0,\
	5000,\
	&PID_Calc,\
	&PID_Reset,\
	0,\
	0,\
	0,\
}\

//CM7λ�û��ṹ���ʼ��
#define CM7_POSITION_PID_DEFAULT \
{\
	0,\
	0,\
	{0,0},\
	0,\
	0,\
	0,\
	0,\
	0,\
	0,\
	4900,\
	1000,\
	1500,\
	0,\
	4900,\
	&PID_Calc,\
	&PID_Reset,\
	0,\
	0,\
	0,\
}\

//CM7�ٶȻ�PID��ʼ��
#define CM7_SPEED_PID_DEFAULT \
{\
	0,\
	0,\
	{0,0},\
	0.0f,\
	0.0f,\
	0.0f,\
	0,\
	0,\
	0,\
	5000,\
	3500,\
	1500,\
	0,\
	5000,\
	&PID_Calc,\
	&PID_Reset,\
	0,\
	0,\
	0,\
}\


extern PID_Regulator CMPositionPID;
extern PID_Regulator CM1SpeedPID;
extern PID_Regulator CM2SpeedPID;
extern PID_Regulator CM3SpeedPID;
extern PID_Regulator CM4SpeedPID;
extern PID_Regulator Speed_Offset;

extern PID_Regulator GMPPositionPID;  
extern PID_Regulator GMPPositionPID1;  
extern PID_Regulator GMPSpeedPID;
extern PID_Regulator GMYPositionPID;			
extern PID_Regulator GMYSpeedPID;

extern PID_Regulator CM6PositionPID;
extern PID_Regulator CM6SpeedPID;

extern PID_Regulator CM7PositionPID;
extern PID_Regulator CM7SpeedPID;

extern RampGen_t GMPitchRamp;
extern RampGen_t GMYawRamp;
extern RampGen_t CMRamp;

#define PITCH_MAX 60.0f   //�޸�ʱf��βʱ������Ӧ��С��
#define PITCH_MIN -60.0f
#define YAW_MAX 90.0f

typedef struct
{
  float pitch_angle_dynamic_ref;
  float yaw_angle_dynamic_ref;
}Gimbal_Ref_t;

typedef struct
{
  float CM2_angle_dynamic_ref;
}CM2_Ref_t;


extern Gimbal_Ref_t GimbalRef;
extern CM2_Ref_t CM2Ref;
extern int16_t count_temp;
extern float CMSpeedRate;

extern float Chassis_Position_Ref;
extern float last_Chassis_Temp_Speed;
extern float Chassis_Temp_Speed;
extern float last_Chassis_Temp_Speed_Save;
extern float Chassis_Temp_Speed_Save;
extern float Chassis_Speed_Ref;
extern float Yaw_FeedForward;
extern int   Dodeg_STATE_Change;
extern int   Last_Dodeg_STATE_Change;
extern int   Brake_flag; 


void GMPitchControlLoop(void);
void GMYawControlLoop(void);
void SetGimbalMotorOutput(void);
void CMControlLoop(void);
void ShooterMControlLoop(void);
void GraduallyChangeCMSpeed(void);
void GMBrakeControlLoop(void);

#endif
