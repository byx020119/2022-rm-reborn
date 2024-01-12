#include "main.h"
#include "ControlTask.h"
#include "timer.h"
#include "SpeedTask.h"
//#include "WorkState.h"
//#include "ShootingTask.h"
#include "InputState.h"

extern InputMode_e inputmode = STOP;   //输入模式设定
//Shoot_State_e shootState = NOSHOOTING;
//FrictionWheelState_e friction_wheel_state = FRICTION_WHEEL_OFF;

void SetInputMode(Remote *rc)
{
	if(rc->s2 == 1)
	{
		inputmode = REMOTE_INPUT;
	}
	else if(rc->s2 == 3)
	{
		inputmode = KEY_MOUSE_INPUT;
	}
	else if(rc->s2 == 2)
	{
		inputmode = STOP;
	}	
}

InputMode_e GetInputMode(void)
{
	return inputmode;
}


