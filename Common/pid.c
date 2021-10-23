//对号

#include "pid.h"
#include "common.h"

//component:要素，零件
void PID_Calc(struct PID_Regulator *pid)
{
	pid->err[0] = pid->ref - pid->fdb;//err[0]存放的是ref和fdb的差值，误差
	
	if((pid->err[0] >= pid->jump_out) || (pid->err[0] < -pid->jump_out))
	{
		pid->componentKp = pid->kp * pid->err[0];
		VAL_LIMIT(pid->componentKp,-pid->componentKpMax,pid->componentKpMax)

		pid->componentKi+= pid->ki * pid->err[0];
		VAL_LIMIT(pid->componentKi,-pid->componentKiMax,pid->componentKiMax)
		if(pid->Reset_i == 1&& (pid->err[0] * pid->err[1] < 0))
		{
			pid->componentKi = 0;
		}
		
		pid->componentKd = pid->kd * ( pid->err[0] - pid->err[1] );
		VAL_LIMIT(pid->componentKd,-pid->componentKdMax,pid->componentKdMax)
		pid->componentoutput =pid->componentKp + pid->componentKi + pid->componentKd;
	}
	else
	{
	}
	pid->output = pid->componentoutput;
	VAL_LIMIT(pid->output,-pid->outputMax,pid->outputMax)
	
	pid->err[1] = pid->err[0];
}

void PID_Reset(PID_Regulator *pid)
{
	pid->ref 		= 0;
	pid->fdb		= 0;
	
	pid->err[0] = 0;
	pid->err[1]	= 0;

	pid->componentKp = 0;
	pid->componentKi = 0;
	pid->componentKd = 0;
	pid->componentoutput=0;
	pid->output			 = 0;
}

double yawKdPart_Array[5] = {0,0,0,0,0};
u8 yawKdPart_Index = 0;

//云台专用PID
void GimbalPID_Calc(PID_Regulator *pid)
{
	pid->err[0] = pid->ref - pid->fdb;
	
	if((pid->err[0] >= pid->jump_out) || (pid->err[0] < -pid->jump_out))
	{
		pid->componentKp = pid->kp * pid->err[0];
		VAL_LIMIT(pid->componentKp,-pid->componentKpMax,pid->componentKpMax)

		pid->componentKi+= pid->ki * pid->err[0];
		VAL_LIMIT(pid->componentKi,-pid->componentKiMax,pid->componentKiMax)
		if(pid->Reset_i == 1&& (pid->err[0] * pid->err[1] < 0))
		{
			pid->componentKi = 0;
		}
		
		yawKdPart_Array[yawKdPart_Index++] = pid->err[0] - pid->err[1];
		if(yawKdPart_Index>=5) yawKdPart_Index = 0;
		
		pid->componentKd = pid->kd * (yawKdPart_Array[0]+yawKdPart_Array[1]+yawKdPart_Array[2]+yawKdPart_Array[3]+yawKdPart_Array[4])/5;
		VAL_LIMIT(pid->componentKd,-pid->componentKdMax,pid->componentKdMax)
		pid->componentoutput =pid->componentKp + pid->componentKi + pid->componentKd;
	}
	else
	{
	}
	pid->output = pid->componentoutput;
	VAL_LIMIT(pid->output,-pid->outputMax,pid->outputMax)
	
	pid->err[1] = pid->err[0];
}	

void GimbalPID_Reset(PID_Regulator *pid)
{
	pid->ref 		= 0;
	pid->fdb		= 0;
	
	pid->err[0] = 0;
	pid->err[1]	= 0;

	pid->componentKp = 0;
	pid->componentKi = 0;
	pid->componentKd = 0;
	pid->componentoutput=0;
	pid->output			 = 0;
}
