#define stm32f4
#include "BLib_GlbSrv.h"



uint32_t BLib_GlbSrv_Set_Bit(uint32_t Data_Reg , uint8_t BP)
{
	return (Data_Reg | (1 << BP));
}

uint32_t BLib_GlbSrv_Clr_Bit(uint32_t Data_Reg , uint8_t BP)
{
	return (Data_Reg & (~(1 << BP)));
}

uint8_t BLib_GlbSrv_Get_Bit(uint32_t Data_Reg , uint8_t BP)
{
	uint8_t status;
	if(Data_Reg &  (1 << BP))
		status = 1;
	else
		status = 0;
	return status;
}
