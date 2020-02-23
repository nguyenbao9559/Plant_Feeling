#ifdef	stm32f4
#include "stm32f4xx_hal.h"
#endif

#ifdef	stm32f1
#include "stm32f1xx_hal.h"
#endif

#ifndef __BLib_RTC_H__
#define __BLib_RTC_H__

#ifndef	DS3231_Addr
#define DS3231_Addr	0x68
#endif


typedef struct {
	uint8_t Sec;
	uint8_t Min;
	uint8_t Hr;
	uint8_t Day;
	uint8_t Date;
	uint8_t Mon;
	uint8_t Yr;
}RTC_typedef;

//It cans be used to convert BCD number to Decimal number.
uint8_t BLib_BCD2DEC(uint8_t BCD);

//It cans be used to convert Decimal number to BCD number.
uint8_t BLib_DEC2BCD(uint8_t DEC);

//This function is used for getting time data from RTC chip
void BLib_TimeGet(I2C_HandleTypeDef *hi2c , uint8_t* Time_Dat , RTC_typedef* DS3231);

//This function is used to set the value for Time.
void BLib_TimeInit(I2C_HandleTypeDef *hi2c , uint8_t* Time_Dat , RTC_typedef* DS3231);

#endif