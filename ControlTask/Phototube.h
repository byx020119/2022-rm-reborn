//Î´¸Ä


#ifndef Phototube_H
#define Phototube_H
#include <stm32f4xx.h>

void Phototube_Configuration(void);
void Phototube_Handle(void);


extern uint8_t lastPI7_Level;
extern uint8_t PI7_Level;
extern uint8_t lastPC6_Level;
extern uint8_t PC6_Level;
extern uint8_t lastPI6_Level;
extern uint8_t PI6_Level;
extern  int Phototube_Flag;
extern uint8_t Phototube_Left_Flag;
extern uint8_t Phototube_Middle_Flag;
extern uint8_t Phototube_Right_Flag;
extern uint32_t Phototube_Time_Left; 
extern uint32_t Phototube_Time_Right;

typedef uint32_t datatype;


#endif


