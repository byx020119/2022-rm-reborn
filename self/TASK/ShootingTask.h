#ifndef ShootingTask_H
#define ShootingTask_H
#include <main.h>

typedef enum
{
	FRICTION_WHEEL_OFF = 0,
	FRICTION_WHEEL_START=1,
	FRICTION_WHEEL_RISING_SPEED = 2,
	FRICTION_WHEEL_ON = 3,
}FrictionWheelState_e;

typedef enum
{
	NOSHOOTING = 0,
	SHOOTING = 1,
}Shoot_State_e;

typedef struct Friction_Speed_t
{
	uint16_t low_speed;
	uint16_t high_speed;
}Friction_Speed_t;

typedef struct Bullet_Speed_Ref_t
{
	uint16_t low_speed;
	uint16_t high_speed;
}Bullet_Speed_Ref_t;

typedef struct Bullet_Freq_Ref_t
{
	uint16_t low_speed;
	uint16_t high_speed;
}Bullet_Freq_Ref_t;

typedef struct Gravity_Add_Angle_t
{
	u8 low_speed;
	u8 high_speed;
}Gravity_Add_Angle_t;

extern FrictionWheelState_e friction_wheel_state;
extern FrictionWheelState_e friction_wheel_state_flag;
extern RampGen_t frictionRamp;   
extern uint16_t Friction_Output_Speed;
extern Shoot_State_e shootState;
extern u8 TempShootingFlag;
extern uint16_t Bullet_17_real_Speed;
extern Friction_Speed_t Friction_Speed;
extern Bullet_Speed_Ref_t Bullet_Speed_Ref;
extern Bullet_Freq_Ref_t Bullet_Freq_Ref;
extern Gravity_Add_Angle_t Gravity_Add_Angle;
extern u8 Bullet_Ouput_Freq;

void FrictionWheelControl(void);
void BoLunMotorControl(void);
void SetFrictionState(FrictionWheelState_e v);
FrictionWheelState_e GetFrictionState(void);
void SetShootState(Shoot_State_e v);
Shoot_State_e GetShootState(void);
void HeatControl_Task(void);

#endif
