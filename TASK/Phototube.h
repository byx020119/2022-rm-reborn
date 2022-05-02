#ifndef Phototube_H
#define Phototube_H
#include "main.h"

void Phototube_Configuration(void);
void Phototube_Handle(void);

extern u8 lastPD13_Level;
extern u8 PD13_Level;
extern u8 lastPD14_Level;
extern u8 PD14_Level;
extern u8 lastPD12_Level;
extern u8 PD12_Level;
extern  int Phototube_Flag;
extern u8 Phototube_Left_Flag;
extern u8 Phototube_Middle_Flag;
extern u8 Phototube_Right_Flag;


#endif
