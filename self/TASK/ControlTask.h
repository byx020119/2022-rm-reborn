#ifndef ControlTask_H
#define ControlTask_H
#include "stm32f4xx.h"

#define PREPARE_TIME_TICK_MS 	      2000
#define FRICTION_RAMP_TICK_COUNT		100

extern uint32_t time_tick_2ms;
extern uint16_t Dir_Change_Flag;

void Control_Task(void);
void ControtLoopTaskInit(void);
void engineerpower_Init(void);
#endif
