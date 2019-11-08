//#include "BLib_RTC.h"
#include "stm32f4xx_hal.h"
#include "BLib_RTC.h"



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

void BLib_TimeInit(I2C_HandleTypeDef *hi2c , uint8_t* Time_Dat , RTC_typedef* DS3231)
{
	DS3231->Hr = 4;
	DS3231->Min = 9;
	DS3231->Sec = 0;
	DS3231->Day = 6;
	DS3231->Mon = 11;
	DS3231->Yr = 19;
	
	Time_Dat[0] = BLib_DEC2BCD(DS3231->Sec);	// Second
	Time_Dat[1] = BLib_DEC2BCD(DS3231->Min);	// Minutes
	Time_Dat[2] = BLib_DEC2BCD(DS3231->Hr);	  // Hour
	Time_Dat[3] = BLib_DEC2BCD(DS3231->Day);	// Date
	Time_Dat[4] = BLib_DEC2BCD(DS3231->Date);	// Day
	Time_Dat[5] = BLib_DEC2BCD(DS3231->Mon);	// Month
	Time_Dat[6] = BLib_DEC2BCD(DS3231->Yr);	  // Year
	HAL_I2C_Mem_Write(hi2c,DS3231_Addr<<1,0,I2C_MEMADD_SIZE_8BIT,Time_Dat,sizeof(Time_Dat)/sizeof(uint8_t),HAL_MAX_DELAY);
}

void BLib_TimeGet(I2C_HandleTypeDef *hi2c , uint8_t* Time_Dat , RTC_typedef* DS3231)
{
	HAL_I2C_Mem_Read(hi2c,DS3231_Addr<<1,0x00,I2C_MEMADD_SIZE_8BIT,Time_Dat,7,HAL_MAX_DELAY);
	DS3231->Sec  = BLib_BCD2DEC(Time_Dat[0]); //Second
	DS3231->Min  = BLib_BCD2DEC(Time_Dat[1]); // Minutes
	DS3231->Hr   = BLib_BCD2DEC(Time_Dat[2]); // Hour
	DS3231->Day  = BLib_BCD2DEC(Time_Dat[3]); // Datae
	DS3231->Date = BLib_BCD2DEC(Time_Dat[4]); // Day
	DS3231->Mon  = BLib_BCD2DEC(Time_Dat[5]); // Month
	DS3231->Yr   = BLib_BCD2DEC(Time_Dat[6]);	// Year
}

