

#include "stm32f1xx_hal.h"


>>>>>>> master
#ifndef __BLib_SHT10_H__
#define __BLib_SHT10_H__

typedef struct
{
	uint16_t SHT10_DATA_PIN;
	uint16_t SHT10_CLK_PIN;
	GPIO_TypeDef* SHT10_DATA_PORT;
	GPIO_TypeDef* SHT10_CLK_PORT;
	uint8_t SHT10_Resolution;
	uint8_t SHT10_Power_Supply;
	float d1;
	float d2;
	float t1;
	float t2;
	float c1;
	float c2;
	float c3;
}SHT10_Init_t;

/***********Command to communicate with SHT10*********************************************************/
#define TempRead		0x03 // 3 first bit is Address BP , last 5 BP is the command to read temperature
#define HumiRead 		0x05 // 3 first bit is Address BP , last 5 BP is the command to read Humidity
#define stRegRead		0x07 // Read from status register
#define stRegWrte		0x06 // Write to status register 
#define	sftRst			0x1E // Resets the interface, clears the status register to default values. Wait minimum 11 ms before next command



	

/***********SHT10 Communication configuration**********************************************************/
//#define SHT10_DATA_PORT		GPIOD
//#define SHT10_CLK_PORT		GPIOD
//#define SHT10_DATA_PIN		GPIO_PIN_10
//#define SHT10_CLK_PIN			GPIO_PIN_11

#define Pin_Typ_In				1
#define Pin_Typ_Out				0

#define DATA_RisiFall_Tim	1 // the needed delay time (ns) for DATA rising / falling edge
#define CLK_RisiFall_Tim	1 // the needed delay time (ns) for CLK rising / falling edge

#define SHT10_ACK					1	
#define SHT10_nonACK				0

/***********SHT10 Status Register Bit position*********************************************************/

#define SHT10_LowBatDect_BP			6 // will be activated when VDD < 2.47
#define SHT10_MeasReso_BP				0 //'1’ = 8bit RH / 12bit Temp. resolution
																	//’0’ = 12bit RH / 14bit Temp. resolution
#define SHT10_Heater_BP					2 // The heater may increase the temperature of the sensor by 5 – 10°c beyond ambient temperature
																	// o keep self heating below 0.1°C, SHT1x should not be active for more than 10% of the time – e.g. 
																	//maximum one measurement per second at 12bit accuracy shall be made
							  
							  
/***********SHT10 Function call************************************************************************/				

void BLib_SHT10_Init(GPIO_TypeDef* GPIOx, uint16_t Data_Pin , uint16_t Clk_Pin , uint8_t SHT10_resolution , uint8_t SHT10_Power);
							  
uint8_t BLib_SHT10_StRegRead();							

void BLib_SHT10_StRegWrite(uint8_t Data_Reg);

uint32_t BLib_GlbSrv_Set_Bit(uint32_t Data_Reg , uint8_t BP);

uint32_t BLib_GlbSrv_CLr_Bit(uint32_t Data_Reg , uint8_t BP);

void BLib_SHT10_SelfHeat();
								
void BLib_MODER_Update(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t Pin_Typ);							  

void BLib_SHT10_Com_Strt(void);

/*
	This function is used for soft reset when the communication gets error
*/
void BLib_SHT10_Com_Rst(void);

/*
	This function is used for select command to communicate with SHT10
	#define TempRead	0b00000011 // 3 first bit is Address BP , last 5 BP is the command to read temperature
	#define HumiRead 	0b00000101 // 3 first bit is Address BP , last 5 BP is the command to read Humidity
	#define stRead		0b00000111 // Read from status register
	#define stWrte		0b00000110 // Write to status register 
	#define	sftRst		0b00011110 // Resets the interface, clears the status register to default values. Wait minimum 11 ms before next command
*/
uint8_t BLib_SHT10_Com_CmdWrite(uint8_t command);
/*
	This function is used for Reading data from SHT10 sensor	
	Argument:
		#define SHT10_ACK			1
		#define SHT_nonACK			0
*/
uint16_t BLib_SHT10_Com_DatRead(uint8_t Ack);
/*
	This function is used to read the raw temperature from sensor
*/
uint16_t BLib_SHT10_TempRaw_DdRead();
/*
	This function is used to read the raw humidity from sensor
*/
uint16_t BLib_SHT10_HumiRaw_DdRead();
/*
	This function is used to read the humidity value
*/
float BLib_SHT10_HumiRead();
/*
	This function is used to read the temperature value
*/
float BLib_SHT10_TempRead();
#endif