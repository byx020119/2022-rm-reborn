#ifndef GunHeartAndBleed_H
#define GunHeartAndBleed_H
#include "main.h"

void GunHeart_CalcProcess(uint16_t BoomSpeed);
void GunHeart_CoolingProcess(void);
void Bleed_Monitor(void);
void Attacked_Monitor(void);
void BulletSpeed_Monitor(void);


#define Bomb_17_Heart_Toplimit  360
#define Cooling_Value_PerSec    160

extern int16_t GunHeart_Value;
extern int Doget_bleed_time;
extern uint32_t Dodeg_Delay_Count;
extern uint32_t Dodeg_Delay__Count_2;
extern int HP_error;
extern int16_t HP_error__time[60];
extern int Big_armor_recognition_flag;
extern int Aerocraft_attack_flag;
extern uint32_t Aerocraft_doget_count_delay;
extern int Aerocraft_Phototube_flag;

#endif
