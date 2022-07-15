#ifndef WorkState_H
#define WorkState_H
#include <stm32f4xx.h>

typedef enum
{
    PREPARE_STATE,     		      //上电后初始化状态 4s钟左右
    Freedom_STATE,					    //自由运动状态，粗巡逻状态
	  Waiting_STATE,              //等待状态
	  Patrol_STATE,               //精巡逻状态
		ChariotRecognition_STATE,   //战车识别状态
	  Dodeg_STATE,                //躲避状态
	  Test_STATE,                 //遥控器测试状态
	  STOP_STATE,        		      //停止状态
	  Attacked_STATE,             //被攻击状态
	  Other_ChariotRecognition_STATE,//其他枪识别状态
}WorkState_e;


void WorkStateFSM(void);
void WorkStateSwitchProcess(void);
void SetWorkState(WorkState_e state);
WorkState_e GetWorkState(void);
WorkState_e GetlastWorkState(void);

extern u8 CameraDetectTarget_Flag;
extern u8 DodgeTarget_Flag;
extern u8 RemoteTest_Flag;
extern u8 Waiting_Flag;
extern u8 Chassis_Power_On_Flag;
extern u8 Attacked_Flag;
extern u8 Freedom_Flag;
extern uint32_t Dodge_time_count;
extern WorkState_e workState;

#endif
