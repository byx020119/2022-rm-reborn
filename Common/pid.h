//¶ÔºÅ

#ifndef __PID_H__
#define __PID_H__
#include "stm32f4xx.h"

typedef struct PID_Regulator
{
	double ref;
	double fdb;
	double err[2];
	double kp;
	double ki;
	double kd;
	double componentKp;
	double componentKi;
	double componentKd;
	double componentKpMax;
	double componentKiMax;
	double componentKdMax;
	double output;
	double outputMax;
	void (*Calc)(struct PID_Regulator *pid);//º¯ÊýÖ¸Õë
	void (*Reset)(struct PID_Regulator *pid);
	double componentoutput;
	double jump_out;
	double Reset_i;
}PID_Regulator;

void PID_Calc(PID_Regulator *pid);
void PID_Reset(PID_Regulator *pid);

void GimbalPID_Calc(PID_Regulator *pid);
void GimbalPID_Reset(PID_Regulator *pid);

#endif
