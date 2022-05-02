#include "main.h"

void RampSetScale(struct RampGen_t *ramp, int32_t scale)
{
		ramp->XSCALE = scale;
}

void RampResetCounter(struct RampGen_t *ramp)
{
		ramp->count = 0;
}

float RampCalc(RampGen_t *ramp)
{
		if( ramp->count < ramp->XSCALE )
		{
			ramp->count++;
			ramp->out = (float)ramp->count/(float)ramp->XSCALE;
			return ramp->out;
		}
		return 1;
}

uint8_t RampIsOverflow(struct RampGen_t *ramp)
{
	if((float)ramp->count/(float)ramp->XSCALE >= 1)
	return 1;
	else return 0;
}
void RampInit(RampGen_t *ramp, int32_t XSCALE)
{
	;
}

void RampSetCounter(struct RampGen_t *ramp, int32_t count)
{
	ramp->count = count;
}
