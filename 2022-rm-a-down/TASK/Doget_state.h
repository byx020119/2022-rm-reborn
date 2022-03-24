//Î´¸Ä

#ifndef DOGETSTATE_H
#define DOGETSTATE_H
#include <stm32f4xx.h>


extern int Doget_Change_Flag;
extern int Destination_Flag;
extern int16_t Random_MotionRange;
extern int16_t last_Random_MotionRange;
extern int Doget_twice;
extern int Random_change_flag;
extern int Random_change__flag_2;

void YawFreeRoation_Doget(void);
void Chassis_Motion_Switch_Doget(void);
void getRandom_MotionRange(void);


#endif

