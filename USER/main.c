#include "main.h"

int main()
{	
  delay_ms(2000);//延时等待上点完成
	BSP_Init();
  engineerpower_Init();
	ControtLoopTaskInit();
	TIM3_Start();
	TIM6_Start();
	while(1)
	{
		delay_us(200);
	}
}
