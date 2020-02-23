#ifdef	stm32f4
#include "stm32f4xx_hal.h"
#endif

#ifdef	stm32f1
#include "stm32f1xx_hal.h"
#endif

#ifndef __BLib_TSL2561_H__
#define __BLib_TSL2561_H__

typedef struct
{
	uint16_t IntegrationTime;
	uint8_t Gain;
	uint16_t ChScale;
	uint32_t HighThreshold;
	uint32_t LowThreshold;
}TSL2561_Init_t;

#ifndef	TSL2561_Addr
#define TSL2561_Addr	0x49 // Address selection pin is connected to VDD
//#define TSL2561_ADDR  0x39 // Address selection pin is left float
//#define TSL2561_Addr	0x29 // Address selection pin is connected to GND
#endif

/***********TSL2561 Command Register**********************************************************/
#define TSL2561_CmdReg_Cmd_BP     	0x80    // Must be 1
#define TSL2561_CmdReg_Clr_BP       0x40    // Clears any pending interrupt (write 1 to clear)
#define TSL2561_CmdReg_Wrd_BP       0x20    // 1 = read/write word (rather than byte)
#define TSL2561_CmdReg_Blk_BP       0x10    // 1 = using block read/write
/***********TSL2561 Address Register**********************************************************/
#define TSL2561_AddrReg_Ctrl_BP			 0x00 //To access Control register
#define TSL2561_AddReg_Tim_BP			 0x01 //Integration time/gain control
#define TSL2561_AddReg_ThreshLowLow_BP	 0x02 //Low byte of low interrupt threshold
#define TSL2561_AddReg_ThreshLowHigh_BP	 0x03 //High byte of low interrupt threshold
#define TSL2561_AddReg_ThreshHighLow_BP	 0x04 //Low byte of high interrupt threshold
#define TSL2561_AddReg_ThreshHighHigh_BP 0x05 //High byte of high interrupt threshold
#define TSL2561_AddReg_Intrp_BP			 0x06 //Interrupt control
#define TSL2561_AddReg_CRC_BP			 0x08 //Factory test — not a user register
#define TSL2561_AddReg_ID_BP			 0x0A //Part number/ Rev ID
//TSL2561 Data Channel Register
#define TSL2561_AddReg_DATA0Low_BP		 0x0C //Low byte of ADC channel 0
#define TSL2561_AddReg_DATA0High_BP		 0x0D //High byte of ADC channel 0
#define TSL2561_AddReg_DATA1Low_BP		 0x0E //Low byte of ADC channel 1
#define TSL2561_AddReg_DATA1High_BP		 0x0F //High byte of ADC channel 1

/***********TSL2561 Timing Register***********************************************************/
//TSL2561 Integration Time
#define TSL2561_TIMReg_IntegTim_Slow_BP		0x10 //402ms
#define TSL2561_TIMReg_IntegTim_Medium_BP	0x01 //101ms
#define TSL2561_TIMReg_IntegTim_Fast_BP		0x00 //13.7ms
//TSL2561 Gain
#define TSL2561_TIMReg_Manual_BP		  		0x08 	// Manual setting gain
#define TSL2561_GAIN_1X                   0x00  // No gain
#define TSL2561_GAIN_16X                  0x10  // 16x gain
#define TSL2561_GAIN_Auto									0x11	// New mode, it's not defined in datasheet.

/***********TSL2561 Interrupt Control Register************************************************/
#define TSL2561_IntrReg_IntrDisable		0x00 //Interrupt output disabled
#define TSL2561_IntrReg_LvlIntrMode 	0x10 //Level Interrupt
#define TSL2561_IntrReg_SMBMode			0x20 //SMBAlert compliant
#define TSL2561_IntrReg_TstMode			0x30//Test Mode: Sets interrupt and functions as mode 10
//TSL2561 Interrupt Persistence Select
#define TSL2561_IntrReg_Attempt_Intr	0x00//Every ADC cycle generates interrupt
#define TSL2561_IntrReg_Attempt_1 		0x01//Any value outside of threshold range
#define TSL2561_IntrReg_Attempt_2		0x02//2  integration time periods out of range
#define TSL2561_IntrReg_Attempt_3		0x03//3  integration time periods out of range
#define TSL2561_IntrReg_Attempt_4		0x04//4  integration time periods out of range
#define TSL2561_IntrReg_Attempt_5		0x05//5  integration time periods out of range
#define TSL2561_IntrReg_Attempt_6		0x06//6  integration time periods out of range
#define TSL2561_IntrReg_Attempt_7		0x07//7  integration time periods out of range
#define TSL2561_IntrReg_Attempt_8		0x08//8  integration time periods out of range
#define TSL2561_IntrReg_Attempt_9		0x09//9  integration time periods out of range
#define TSL2561_IntrReg_Attempt_10		0x0A//10 integration time periods out of range
#define TSL2561_IntrReg_Attempt_11		0x0B//11 integration time periods out of range
#define TSL2561_IntrReg_Attempt_12		0x0C//12 integration time periods out of range
#define TSL2561_IntrReg_Attempt_13		0x0D//13 integration time periods out of range
#define TSL2561_IntrReg_Attempt_14		0x0E//14 integration time periods out of range
#define TSL2561_IntrReg_Attempt_15		0x0F//15 integration time periods out of range

/***********TSL2561 Control Register**********************************************************/
#define TSL2561_CtrlReg_PwrUp		0x03 // Power up
#define TSL2561_CtrlReg_PwrDwn	0x00 // Power down

// T, FN and CL package values
#define TSL2561_LUX_LUXSCALE      (14)      ///< Scale by 2^14
#define TSL2561_LUX_RATIOSCALE    (9)       ///< Scale ratio by 2^9
#define TSL2561_LUX_CHSCALE       (10)      ///< Scale channel values by 2^10
#define TSL2561_LUX_CHSCALE_TINT0 (0x7517)  ///< 322/11 * 2^TSL2561_LUX_CHSCALE
#define TSL2561_LUX_CHSCALE_TINT1 (0x0FE7)  ///< 322/81 * 2^TSL2561_LUX_CHSCALE
/////////////////////////////////////////////////////////////////////////////////
#define TSL2561_LUX_K1T           (0x0040)  ///< 0.125 * 2^RATIO_SCALE
#define TSL2561_LUX_B1T           (0x01f2)  ///< 0.0304 * 2^LUX_SCALE
#define TSL2561_LUX_M1T           (0x01be)  ///< 0.0272 * 2^LUX_SCALE
#define TSL2561_LUX_K2T           (0x0080)  ///< 0.250 * 2^RATIO_SCALE
#define TSL2561_LUX_B2T           (0x0214)  ///< 0.0325 * 2^LUX_SCALE
#define TSL2561_LUX_M2T           (0x02d1)  ///< 0.0440 * 2^LUX_SCALE
#define TSL2561_LUX_K3T           (0x00c0)  ///< 0.375 * 2^RATIO_SCALE
#define TSL2561_LUX_B3T           (0x023f)  ///< 0.0351 * 2^LUX_SCALE
#define TSL2561_LUX_M3T           (0x037b)  ///< 0.0544 * 2^LUX_SCALE
#define TSL2561_LUX_K4T           (0x0100)  ///< 0.50 * 2^RATIO_SCALE
#define TSL2561_LUX_B4T           (0x0270)  ///< 0.0381 * 2^LUX_SCALE
#define TSL2561_LUX_M4T           (0x03fe)  ///< 0.0624 * 2^LUX_SCALE
#define TSL2561_LUX_K5T           (0x0138)  ///< 0.61 * 2^RATIO_SCALE
#define TSL2561_LUX_B5T           (0x016f)  ///< 0.0224 * 2^LUX_SCALE
#define TSL2561_LUX_M5T           (0x01fc)  ///< 0.0310 * 2^LUX_SCALE
#define TSL2561_LUX_K6T           (0x019a)  ///< 0.80 * 2^RATIO_SCALE
#define TSL2561_LUX_B6T           (0x00d2)  ///< 0.0128 * 2^LUX_SCALE
#define TSL2561_LUX_M6T           (0x00fb)  ///< 0.0153 * 2^LUX_SCALE
#define TSL2561_LUX_K7T           (0x029a)  ///< 1.3 * 2^RATIO_SCALE
#define TSL2561_LUX_B7T           (0x0018)  ///< 0.00146 * 2^LUX_SCALE
#define TSL2561_LUX_M7T           (0x0012)  ///< 0.00112 * 2^LUX_SCALE
#define TSL2561_LUX_K8T           (0x029a)  ///< 1.3 * 2^RATIO_SCALE
#define TSL2561_LUX_B8T           (0x0000)  ///< 0.000 * 2^LUX_SCALE
#define TSL2561_LUX_M8T           (0x0000)  ///< 0.000 * 2^LUX_SCALE

/***********TSL2561 Status Register***********************************************************/
#define BLib_TSL2561_StReg_Gain_1x			0
#define BLib_TSL2561_StReg_Gain_16x			1
#define BLib_TSl2561_StReg_Autogain			2
#define BLib_TSL2561_StReg_IntTim_Slow	3
#define BLib_TSl2561_StReg_IntTim_Med		4
#define BLib_TSL2561_StReg_IntTim_Fast	5

/////////////////////////////////////////////////////////////////////////////////

uint16_t BLib_TSL2561_CmdRead(uint8_t cmd);
void BLib_TSL2561_PwrUp();
void BLib_TSL2561_PwrDwn();
void BLib_TSL2561_IntegrationTime_Set(uint16_t IntegrationTime);
void BLib_TSL2561_Gain_Set(uint8_t Gain);
void BLib_TSL2561_Init(uint16_t IntegrationTime , uint8_t Gain);
uint16_t BLib_TSL2561_Ch0RawRead();
uint16_t BLib_TSL2561_Ch1RawRead();
uint32_t BLib_TSL2561_Lux();

#endif