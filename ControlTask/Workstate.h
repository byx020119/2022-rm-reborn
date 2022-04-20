//¶ÔºÅ

#ifndef WorkState_H
#define WorkState_H
#include <stm32f4xx.h>

typedef enum
{
    PREPARE_STATE,     		      //ÉÏµçºó³õÊ¼»¯×´Ì¬ 4sÖÓ×óÓÒ
    Freedom_STATE,					    //×ÔÓÉÔË¶¯×´Ì¬£¬´ÖÑ²Âß×´Ì¬
	  Waiting_STATE,              //µÈ´ý×´Ì¬
	  Patrol_STATE,               //¾«Ñ²Âß×´Ì¬
		ChariotRecognition_STATE,   //Õ½³µÊ¶±ð×´Ì¬
	  Dodeg_STATE,                //¶ã±Ü×´Ì¬
	  Test_STATE,                 //Ò£¿ØÆ÷²âÊÔ×´Ì¬
	  STOP_STATE,        		      //Í£Ö¹×´Ì¬
	  Attacked_STATE,             //±»¹¥»÷×´Ì¬
}WorkState_e;


void WorkStateFSM(void);
void WorkStateSwitchProcess(void);
void SetWorkState(WorkState_e state);
WorkState_e GetWorkState(void);
WorkState_e GetlastWorkState(void);

extern uint8_t CameraDetectTarget_Flag;
extern uint8_t DodgeTarget_Flag;
extern uint8_t RemoteTest_Flag;
extern uint8_t Waiting_Flag;
extern uint8_t Chassis_Power_On_Flag;
extern uint8_t Attacked_Flag;
extern uint8_t Freedom_Flag;
extern uint32_t Dodge_time_count;

#endif
