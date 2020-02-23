#ifdef	stm32f4
#include "stm32f4xx_hal.h"
#endif

#ifdef	stm32f1
#include "stm32f1xx_hal.h"
#endif

#ifndef __BLib_GLBSRV_H__
#define __BLib_GLBSRV_H__


uint32_t BLib_GlbSrv_Set_Bit(uint32_t Data_Reg , uint8_t BP);

uint32_t BLib_GlbSrv_Clr_Bit(uint32_t Data_Reg , uint8_t BP);

uint8_t BLib_GlbSrv_Get_Bit(uint32_t Data_Reg , uint8_t BP);

#endif