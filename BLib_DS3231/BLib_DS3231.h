#ifndef __BLib_RTC_H__
#define __BLib_RTC_H__

#ifndef	DS3231_Addr
#define DS3231_Addr	0x68
#endif

/***********DS3231 Time Data***********************************************************/
#define DS3231_TimeDat_Sec		0x00
#define DS3231_TimeDat_Min		0x01
#define DS3231_TimeDat_Hr			0x02
#define DS3231_TimeDat_Day		0x03
#define DS3231_TimeDat_Date		0x04
#define DS3231_TimeDat_Mon		0x05
#define DS3231_TimeDat_Yr			0x06


//It cans be used to convert BCD number to Decimal number.
uint8_t BLib_BCD2DEC(uint8_t BCD);

//It cans be used to convert Decimal number to BCD number.
uint8_t BLib_DEC2BCD(uint8_t DEC);

uint8_t BLib_DS3231_TimeGet(uint8_t DS3231_TimeDat);

void BLib_DS3231_TimeSet(uint8_t DS3231_TimeDat , uint8_t TimeDat_Value);

uint8_t BLib_DS3231_RealTimeAlrm(uint8_t DS3231_TimeDat, uint8_t TimeDat_AlarmVal, uint8_t duration);

#endif
