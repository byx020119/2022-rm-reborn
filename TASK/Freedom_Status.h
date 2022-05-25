//对号

#ifndef FREEDOM_STATUS_H
#define FREEDOM_STATUS_H
#include "stm32f4xx.h"

extern float YawCurrentPositionSave; 
extern float PitchCurrentPositionSave;
extern uint8_t DetectTargetFinish_Flag;
extern uint8_t RotationDir_Flag;

extern int32_t YAW_Round_Cnt;
extern int16_t YAW_Angle_YuShu;
extern int32_t Attacked_YAW_Pos_Ref;
extern uint8_t Attacked_YAW_Rotation_Dir;
extern uint8_t Chassis_Change_Dir_Flag;
extern uint8_t Chassis_Speed_UP_Flag;
extern uint8_t Chassis_Speed_DOWN_Flag;
extern uint8_t Chassis_Freedom_i;
extern int Yaw_rotate_flag;
extern int pitch_rotate_flag;
extern int Speed_change;
extern int Recognition_YAW_Rotation_Dir;
extern int32_t Recognition_YAW_Pos_Ref;
extern int32_t Recognition_Delay_count;
extern int Freedom_Rotation_flag;
extern float YawInitPositionSave; 
extern int chassis_speed; 

void YawFreeRoation(void);
void Chassis_Motion_Switch(void);
void Chassis_DiffSpeed_Task(void);	
extern double yaw_ecd_angle_flag;           //2022加  自由模式y轴一侧限位值
extern double yaw_ecd_angle_flag1 ;          //2022加  自由模式y轴另一侧限位值



#endif

