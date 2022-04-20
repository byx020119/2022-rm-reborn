#ifndef SpeedTask_H
#define SpeedTask_H
#include <stm32f4xx.h>
#include "pid.h"
#include "Ramp.h"

//pitch位置环结构体初始化
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
	8000,\
	1000,\
	1500,\
	0,\
	8000,\
	&PID_Calc,\
	&PID_Reset,\
	0,\
	0,\
	0,\
}\

//pitch速度环结构体初始化
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

//yaw位置环结构体初始化20000
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

//yaw速度环结构体初始化20000
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

//底盘电机位置环结构体初始化
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

//底盘电机速度环PID初始化
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
//速度抵消PID初始化
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

//CM6位置环结构体初始化 2022加
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
//CM6速度环PID初始化 2022加
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

//CM7位置环结构体初始化
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

//CM7速度环PID初始化
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

#define PITCH_MAX 60.0f   //修改时f结尾时，数字应是小数
#define PITCH_MIN -60.0f
#define YAW_MAX 90.0f

typedef struct
{
  float pitch_angle_dynamic_ref;
  float yaw_angle_dynamic_ref;
	//float brake_angle_dynamic_ref;
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
