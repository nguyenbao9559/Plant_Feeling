//#include "BLib_RTC.h"
#include "stm32f4xx_hal.h"
#include "BLib_DS3231.h"
#include "BLib_GlbSrv.h"

extern I2C_HandleTypeDef hi2c2;
extern TIM_HandleTypeDef htim2;
static uint8_t FlipFlop_RealTIme_St;

uint8_t BLib_BCD2DEC(uint8_t BCD)
{
	uint8_t Dec_Loc;
	Dec_Loc = (BCD>>4)*10 + (BCD & 0x0F);
	return Dec_Loc;
}

uint8_t BLib_DEC2BCD(uint8_t DEC)
{
	uint8_t BCD_Loc;
	BCD_Loc = ((DEC / 10)<<4) | (DEC % 10);
	return BCD_Loc;
}

uint8_t BLib_DS3231_TimeGet(uint8_t DS3231_TimeDat)
{
	uint8_t Data_loc;
	HAL_I2C_Mem_Read(&hi2c2,DS3231_Addr<<1,DS3231_TimeDat,I2C_MEMADD_SIZE_8BIT,&Data_loc,1,HAL_MAX_DELAY);
	Data_loc = BLib_BCD2DEC(Data_loc);
	return Data_loc;
}

void BLib_DS3231_TimeSet(uint8_t DS3231_TimeDat , uint8_t TimeDat_Value)
{
	TimeDat_Value = BLib_DEC2BCD(TimeDat_Value);
	HAL_I2C_Mem_Write(&hi2c2,DS3231_Addr<<1,DS3231_TimeDat,I2C_MEMADD_SIZE_8BIT,&TimeDat_Value,1,HAL_MAX_DELAY);
}

uint8_t BLib_DS3231_RealTimeAlrm(uint8_t DS3231_TimeDat, uint8_t TimeDat_AlarmVal, uint8_t duration)
{
	uint8_t Val_loc , FFSet , FFReset , FFstate;
	Val_loc = BLib_DS3231_TimeGet(DS3231_TimeDat_Sec);
	FFSet = (Val_loc == TimeDat_AlarmVal) ? 1:0;
	if(FFSet)
		BLib_GlbSrv_Timer_Start(&htim2);
	FFReset = (BLib_GlbSrv_Timer_CurrentVal(&htim2) == duration)? 1:0;
	if(FFReset)
		BLib_GlbSrv_Timer_Stop(&htim2);
	FFstate = BLib_GlbSrv_RSFlipFlop(FFReset , FFSet , &FlipFlop_RealTIme_St);
	return FFstate;
}

