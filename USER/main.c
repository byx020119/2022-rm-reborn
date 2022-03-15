#include "main.h"

//试试git/////////
int main()
{	

 	delay_ms(2000);//延时等待上点完成
	BSP_Init();//各种初始化
  engineerpower_Init();//GPIO初始化
	ControtLoopTaskInit();
	TIM6_Start();//时钟
	while(1)
	{
//	Sendtosightway(12);	//视觉
	delay_us(200);
	}
}
