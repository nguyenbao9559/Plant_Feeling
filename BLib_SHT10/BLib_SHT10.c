#include "stm32f1xx_hal.h"
#include "BLib_SHT10.h"

/*
	This function is used to set the type of Hardware pin (input/output)
*/
//#define	SHT10_Power_Supply		3
//#define SHT10_Resolution			0

static float TempVal; // real temperature value of environment
SHT10_Init_t SHT10_Init;

void BLib_SHT10_Init(GPIO_TypeDef* GPIOx, uint16_t Data_Pin , uint16_t Clk_Pin , uint8_t SHT10_resolution , uint8_t SHT10_Power)
{
	SHT10_Init.SHT10_Resolution = SHT10_resolution;
	SHT10_Init.SHT10_Power_Supply = SHT10_Power;
	SHT10_Init.SHT10_DATA_PIN = Data_Pin;
	SHT10_Init.SHT10_CLK_PIN = Clk_Pin;
	SHT10_Init.SHT10_CLK_PORT = GPIOx;
	SHT10_Init.SHT10_DATA_PORT = GPIOx;
	if(SHT10_Init.SHT10_Power_Supply == 3)
	{
		SHT10_Init.d1 = -39.6;
	}
	if(SHT10_Init.SHT10_Power_Supply == 5)
	{
		SHT10_Init.d1 = -40.1;
	}	
	if(SHT10_Init.SHT10_Resolution == 0)
	{
		BLib_SHT10_StRegWrite(BLib_GlbSrv_CLr_Bit(BLib_SHT10_StRegRead() , SHT10_MeasReso_BP));
		SHT10_Init.d2 = 0.01;
		SHT10_Init.t1 = 0.01;
		SHT10_Init.t2 = 0.00008;
		SHT10_Init.c1 = -2.0468;
		SHT10_Init.c2 = 0.0367;
		SHT10_Init.c3 = -0.0000015955;
	}
	else
	{
		BLib_SHT10_StRegWrite(BLib_GlbSrv_Set_Bit(BLib_SHT10_StRegRead() , SHT10_MeasReso_BP));
		SHT10_Init.d2 = 0.04;
		SHT10_Init.t1 = 0.01;
		SHT10_Init.t2 = 0.00128;
		SHT10_Init.c1 = -2.0468;
		SHT10_Init.c2 = 0.5872;
		SHT10_Init.c3 = -0.00040845;
	}
}


void BLib_MODER_Update(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t Pin_Typ)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	if(Pin_Typ == Pin_Typ_Out)
	{
		GPIO_InitStruct.Pin   = GPIO_Pin;
		GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP; // Push-pull output
		GPIO_InitStruct.Pull  = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);	
	}
	else
	{
		GPIO_InitStruct.Pin   = GPIO_Pin;
		GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull  = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);	
	}
}

/*
	This function is used to start commnication with SHT10 , the process of SHT10 communication is that 
	Start communication ---> Sending command ---> wait for DATA ready ---> 00 MSB ---> ACK ---> LSB ---> ACK --->checksum ---> ACK	
	Therefore , this function is first used in commnication process.
*                           _____         ________  
*                   DATA:        |_______|  
*                               ___     ___  
*                   SCK :   ___|   |___|   |______  
*/
void BLib_SHT10_Com_Strt(void)
{
	BLib_MODER_Update(SHT10_Init.SHT10_DATA_PORT , SHT10_Init.SHT10_DATA_PIN , Pin_Typ_Out); // Set DATA Pin as Output 	
	HAL_GPIO_WritePin(SHT10_Init.SHT10_CLK_PORT ,SHT10_Init.SHT10_CLK_PIN , GPIO_PIN_RESET);	
	HAL_GPIO_WritePin(SHT10_Init.SHT10_DATA_PORT ,SHT10_Init.SHT10_DATA_PIN , GPIO_PIN_SET);
	HAL_Delay(1); 
	HAL_GPIO_WritePin(SHT10_Init.SHT10_CLK_PORT ,SHT10_Init.SHT10_CLK_PIN , GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(SHT10_Init.SHT10_DATA_PORT ,SHT10_Init.SHT10_DATA_PIN , GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(SHT10_Init.SHT10_CLK_PORT ,SHT10_Init.SHT10_CLK_PIN , GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(SHT10_Init.SHT10_CLK_PORT ,SHT10_Init.SHT10_CLK_PIN , GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(SHT10_Init.SHT10_DATA_PORT ,SHT10_Init.SHT10_DATA_PIN , GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(SHT10_Init.SHT10_CLK_PORT ,SHT10_Init.SHT10_CLK_PIN , GPIO_PIN_RESET);
	HAL_Delay(1);	
}

/*
	If communication with the device is lost the below signal sequence will reset the serial interface
	While leaving DATA high, toggle SCK nine or more times. This sequence resets the interface only:
*                   communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart  
*                     _____________________________________________________         ________  
*               DATA:                                                      |_______|  
*                        _    _    _    _    _    _    _    _    _        ___     ___  
*               SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______  	
*/

void BLib_SHT10_Com_Rst(void)
{
	BLib_MODER_Update(SHT10_Init.SHT10_DATA_PORT , SHT10_Init.SHT10_DATA_PIN , Pin_Typ_Out); // Set DATA Pin as Output
	HAL_GPIO_WritePin(SHT10_Init.SHT10_DATA_PORT ,SHT10_Init.SHT10_DATA_PIN , GPIO_PIN_SET);
	HAL_GPIO_WritePin(SHT10_Init.SHT10_CLK_PORT ,SHT10_Init.SHT10_CLK_PIN , GPIO_PIN_RESET);
	int i;
	for(i=0; i < 9;i++)
	{
		HAL_GPIO_WritePin(SHT10_Init.SHT10_CLK_PORT ,SHT10_Init.SHT10_CLK_PIN , GPIO_PIN_SET);
		HAL_Delay(1);
		HAL_GPIO_WritePin(SHT10_Init.SHT10_CLK_PORT ,SHT10_Init.SHT10_CLK_PIN , GPIO_PIN_RESET);
		HAL_Delay(1);
		
	}
	BLib_SHT10_Com_Strt();
	
}

uint8_t BLib_SHT10_Com_CmdWrite(uint8_t command)
{
	uint8_t i;
	uint8_t Ack_Loc;
	BLib_MODER_Update(SHT10_Init.SHT10_DATA_PORT , SHT10_Init.SHT10_DATA_PIN , Pin_Typ_Out); // Set DATA Pin as Output
	for(i=0x80; i > 0;i/=2)
	{
		if(command & i)
		{
			HAL_GPIO_WritePin(SHT10_Init.SHT10_DATA_PORT ,SHT10_Init.SHT10_DATA_PIN , GPIO_PIN_SET);
		}
		else
		{			
			HAL_GPIO_WritePin(SHT10_Init.SHT10_DATA_PORT ,SHT10_Init.SHT10_DATA_PIN , GPIO_PIN_RESET);			
		}
		HAL_GPIO_WritePin(SHT10_Init.SHT10_CLK_PORT ,SHT10_Init.SHT10_CLK_PIN , GPIO_PIN_SET);
		HAL_Delay(1);
		HAL_GPIO_WritePin(SHT10_Init.SHT10_CLK_PORT ,SHT10_Init.SHT10_CLK_PIN , GPIO_PIN_RESET);
		HAL_Delay(1);
	}
	BLib_MODER_Update(SHT10_Init.SHT10_DATA_PORT , SHT10_Init.SHT10_DATA_PIN , Pin_Typ_In); // Set DATA Pin as Input
	if(!HAL_GPIO_ReadPin(SHT10_Init.SHT10_DATA_PORT , SHT10_Init.SHT10_DATA_PIN))
	{
		Ack_Loc = SHT10_ACK;
	}
	else
	{
		Ack_Loc = SHT10_nonACK;
	}
	HAL_GPIO_WritePin(SHT10_Init.SHT10_CLK_PORT ,SHT10_Init.SHT10_CLK_PIN , GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(SHT10_Init.SHT10_CLK_PORT ,SHT10_Init.SHT10_CLK_PIN , GPIO_PIN_RESET);
	HAL_Delay(1);
	return Ack_Loc;
}

uint16_t BLib_SHT10_Com_DatRead(uint8_t Ack)
{
	uint8_t i;
	uint8_t Dat_Loc = 0;
	BLib_MODER_Update(SHT10_Init.SHT10_DATA_PORT , SHT10_Init.SHT10_DATA_PIN , Pin_Typ_In); // Set DATA Pin as Input
	HAL_GPIO_WritePin(SHT10_Init.SHT10_CLK_PORT ,SHT10_Init.SHT10_CLK_PIN , GPIO_PIN_RESET);	
	for(i=0x80; i > 0; i/=2)
	{
		if(HAL_GPIO_ReadPin(SHT10_Init.SHT10_DATA_PORT , SHT10_Init.SHT10_DATA_PIN))
		{	
			Dat_Loc += i;
		}
		HAL_GPIO_WritePin(SHT10_Init.SHT10_CLK_PORT ,SHT10_Init.SHT10_CLK_PIN , GPIO_PIN_SET);
		HAL_Delay(1);
		HAL_GPIO_WritePin(SHT10_Init.SHT10_CLK_PORT ,SHT10_Init.SHT10_CLK_PIN , GPIO_PIN_RESET);
		HAL_Delay(1);
	}
	if(Ack == SHT10_ACK)
	{
		BLib_MODER_Update(SHT10_Init.SHT10_DATA_PORT , SHT10_Init.SHT10_DATA_PIN , Pin_Typ_Out); // Set DATA Pin as Output
		HAL_GPIO_WritePin(SHT10_Init.SHT10_DATA_PORT ,SHT10_Init.SHT10_DATA_PIN , GPIO_PIN_RESET);		
		HAL_GPIO_WritePin(SHT10_Init.SHT10_CLK_PORT ,SHT10_Init.SHT10_CLK_PIN , GPIO_PIN_SET);
		HAL_Delay(1);		
		HAL_GPIO_WritePin(SHT10_Init.SHT10_CLK_PORT ,SHT10_Init.SHT10_CLK_PIN , GPIO_PIN_RESET);
		HAL_Delay(1);		
	}
		if(Ack == SHT10_nonACK)
	{
		BLib_MODER_Update(SHT10_Init.SHT10_DATA_PORT , SHT10_Init.SHT10_DATA_PIN , Pin_Typ_Out); // Set DATA Pin as Output
		HAL_GPIO_WritePin(SHT10_Init.SHT10_DATA_PORT ,SHT10_Init.SHT10_DATA_PIN , GPIO_PIN_SET);		
		HAL_GPIO_WritePin(SHT10_Init.SHT10_CLK_PORT ,SHT10_Init.SHT10_CLK_PIN , GPIO_PIN_SET);
		HAL_Delay(1);		
		HAL_GPIO_WritePin(SHT10_Init.SHT10_CLK_PORT ,SHT10_Init.SHT10_CLK_PIN , GPIO_PIN_RESET);
		HAL_Delay(1);		
	}
	return Dat_Loc;
}

uint16_t BLib_SHT10_TempRaw_DdRead()
{
	uint8_t MSB_loc , LSB_loc , CRC_Loc;
	uint16_t TempRaw_Loc = 0;
	BLib_SHT10_Com_Strt();
	BLib_SHT10_Com_CmdWrite(TempRead);
	while(HAL_GPIO_ReadPin(SHT10_Init.SHT10_DATA_PORT ,SHT10_Init.SHT10_DATA_PIN));
	MSB_loc = BLib_SHT10_Com_DatRead(SHT10_ACK);
	LSB_loc = BLib_SHT10_Com_DatRead(SHT10_ACK);
	CRC_Loc = BLib_SHT10_Com_DatRead(SHT10_nonACK);
	TempRaw_Loc = (MSB_loc << 8) | LSB_loc;
	return TempRaw_Loc;
}

uint16_t BLib_SHT10_HumiRaw_DdRead()
{
	uint8_t MSB_loc , LSB_loc , CRC_Loc;
	uint16_t HumiRaw_Loc = 0;
	BLib_SHT10_Com_Strt();
	BLib_SHT10_Com_CmdWrite(HumiRead);
	while(HAL_GPIO_ReadPin(SHT10_Init.SHT10_DATA_PORT ,SHT10_Init.SHT10_DATA_PIN));
	MSB_loc = BLib_SHT10_Com_DatRead(SHT10_ACK);
	LSB_loc = BLib_SHT10_Com_DatRead(SHT10_ACK);
	CRC_Loc = BLib_SHT10_Com_DatRead(SHT10_nonACK);
	HumiRaw_Loc = (MSB_loc << 8) | LSB_loc;
	return HumiRaw_Loc;
}

float BLib_SHT10_HumiRead()
{
	uint16_t HuminRaw_Loc;
	float Humi_Loc;
	HuminRaw_Loc = BLib_SHT10_HumiRaw_DdRead();
	Humi_Loc = SHT10_Init.c1 + SHT10_Init.c2*HuminRaw_Loc + SHT10_Init.c3*HuminRaw_Loc*HuminRaw_Loc; // Convert raw value of humidity to RH
	Humi_Loc = (TempVal - 25.0)*(SHT10_Init.t1 + SHT10_Init.t2*HuminRaw_Loc) + Humi_Loc; // Temperature compensation, when temp is high, Humidity will be low
	if(Humi_Loc > 90)
	return Humi_Loc;
}

float BLib_SHT10_TempRead()
{
	uint16_t TempRaw_Loc;
	float Temp_Loc;
	TempRaw_Loc = BLib_SHT10_TempRaw_DdRead();
	Temp_Loc = SHT10_Init.d1 + SHT10_Init.d2*TempRaw_Loc;
	TempVal = Temp_Loc;
	return Temp_Loc;
}

uint8_t BLib_SHT10_StRegRead()
{
	uint8_t CRC_Loc;
	uint8_t StReg_Loc;
	BLib_SHT10_Com_Strt();
	BLib_SHT10_Com_CmdWrite(stRegRead);
	StReg_Loc = BLib_SHT10_Com_DatRead(SHT10_ACK);
	CRC_Loc = BLib_SHT10_Com_DatRead(SHT10_nonACK);
	return StReg_Loc;	
}

void BLib_SHT10_StRegWrite(uint8_t Data_Reg)
{
	BLib_SHT10_Com_Strt();
	BLib_SHT10_Com_CmdWrite(stRegWrte);
	BLib_SHT10_Com_CmdWrite(Data_Reg);
}

void BLib_SHT10_SelfHeat()
{
	uint8_t Data_Reg;
	Data_Reg = BLib_GlbSrv_Set_Bit(BLib_SHT10_StRegRead() , SHT10_Heater_BP);
	BLib_SHT10_Com_Strt();
	BLib_SHT10_Com_CmdWrite(stRegWrte);
	BLib_SHT10_Com_CmdWrite(Data_Reg);
}

uint32_t BLib_GlbSrv_Set_Bit(uint32_t Data_Reg , uint8_t BP)
{
	return (Data_Reg | (1 << BP));
}

uint32_t BLib_GlbSrv_CLr_Bit(uint32_t Data_Reg , uint8_t BP)
{
	return (Data_Reg & (~(1 << BP)));
}
