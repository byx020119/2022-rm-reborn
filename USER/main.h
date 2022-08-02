#ifndef __MAIN_H__
#define __MAIN_H__

/*********SYSTEM********/
#include "stm32f4xx.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 
#include "stdlib.h"
#include "math.h"

/*********BSP********/
#include "bsp.h"
#include "can1.h"
#include "timer.h"
#include "uart1.h"
#include "usart3.h"
//#include "usart6.h"
#include "gun.h"
#include "usart2.h"
#include "adc.h"

/********TASK*********/
#include "CanBusTask.h"
#include "encoder.h"
#include "ControlTask.h"
#include "SpeedTask.h"
#include "WorkState.h"
#include "RemoteTask.h"
#include "Freedom_Status.h"
//#include "ShootingTask.h"
//#include "JudgingSystemTask.h"
//#include "GunHeartAndBleed.h"
#include "Phototube.h"
#include "DogetState.h"

/********COMMON*********/
#include "common.h"
#include "pid.h"
#include "Ramp.h"
#include "crc.h"
#include "kalman.h"
#include "ADRC_sha.h"

#endif
