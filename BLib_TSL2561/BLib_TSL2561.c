#include "main.h"
#include "BLib_TSL2561.h"
#include "BLib_GlbSrv.h"

extern I2C_HandleTypeDef hi2c1;
static TSL2561_Init_t TSL2561_Init;
static uint16_t BLib_TSL2561_StMon_Reg = 0x00;

uint16_t BLib_TSL2561_Status()
{
	return BLib_TSL2561_StMon_Reg;
}

void BLib_TSL2561_CmdWrite(uint8_t Reg , uint8_t cmd) {
	HAL_I2C_Mem_Write(&hi2c1,TSL2561_Addr<<1,Reg,I2C_MEMADD_SIZE_8BIT,&cmd,1,HAL_MAX_DELAY);
}

uint16_t BLib_TSL2561_CmdRead(uint8_t cmd) {
	uint8_t data[2];
	uint16_t Data_loc;
	HAL_I2C_Mem_Read(&hi2c1,TSL2561_Addr<<1,cmd,I2C_MEMADD_SIZE_8BIT,data,2,HAL_MAX_DELAY);
	Data_loc = (data[1]<<8) | data[0];
	return Data_loc;
}

void BLib_TSL2561_PwrUp()
{
	BLib_TSL2561_CmdWrite(TSL2561_CmdReg_Cmd_BP | TSL2561_AddrReg_Ctrl_BP , TSL2561_CtrlReg_PwrUp);
}

void BLib_TSL2561_PwrDwn()
{
	BLib_TSL2561_CmdWrite(TSL2561_CmdReg_Cmd_BP | TSL2561_AddrReg_Ctrl_BP , TSL2561_CtrlReg_PwrDwn);
}

void BLib_TSL2561_IntegrationTime_Set(uint16_t IntegrationTime)
{
	BLib_TSL2561_PwrUp();
	BLib_TSL2561_CmdWrite(TSL2561_CmdReg_Cmd_BP | TSL2561_AddReg_Tim_BP , IntegrationTime);
	BLib_TSL2561_PwrDwn();
	switch(IntegrationTime)
	{
		case TSL2561_TIMReg_IntegTim_Slow_BP:
			BLib_TSL2561_StMon_Reg = BLib_GlbSrv_Set_Bit(BLib_TSL2561_StMon_Reg,BLib_TSL2561_StReg_IntTim_Slow);
			BLib_TSL2561_StMon_Reg = BLib_GlbSrv_Clr_Bit(BLib_TSL2561_StMon_Reg,BLib_TSl2561_StReg_IntTim_Med);
			BLib_TSL2561_StMon_Reg = BLib_GlbSrv_Clr_Bit(BLib_TSL2561_StMon_Reg,BLib_TSL2561_StReg_IntTim_Fast);
			break;
		case TSL2561_TIMReg_IntegTim_Medium_BP:
			BLib_TSL2561_StMon_Reg = BLib_GlbSrv_Set_Bit(BLib_TSL2561_StMon_Reg,BLib_TSl2561_StReg_IntTim_Med);
			BLib_TSL2561_StMon_Reg = BLib_GlbSrv_Clr_Bit(BLib_TSL2561_StMon_Reg,BLib_TSL2561_StReg_IntTim_Slow);
			BLib_TSL2561_StMon_Reg = BLib_GlbSrv_Clr_Bit(BLib_TSL2561_StMon_Reg,BLib_TSL2561_StReg_IntTim_Fast);
			break;
		case TSL2561_TIMReg_IntegTim_Fast_BP:
			BLib_TSL2561_StMon_Reg = BLib_GlbSrv_Set_Bit(BLib_TSL2561_StMon_Reg,BLib_TSL2561_StReg_IntTim_Fast);
			BLib_TSL2561_StMon_Reg = BLib_GlbSrv_Clr_Bit(BLib_TSL2561_StMon_Reg,BLib_TSL2561_StReg_IntTim_Slow);
			BLib_TSL2561_StMon_Reg = BLib_GlbSrv_Clr_Bit(BLib_TSL2561_StMon_Reg,BLib_TSl2561_StReg_IntTim_Med);		
			break;		
	}
}

void BLib_TSL2561_Gain_Set(uint8_t Gain)
{
	BLib_TSL2561_PwrUp();
	BLib_TSL2561_CmdWrite(TSL2561_CmdReg_Cmd_BP | TSL2561_AddReg_Tim_BP , Gain);
	BLib_TSL2561_PwrDwn();
	switch(Gain)
	{
		case TSL2561_GAIN_1X:
			BLib_TSL2561_StMon_Reg = BLib_GlbSrv_Set_Bit(BLib_TSL2561_StMon_Reg , BLib_TSL2561_StReg_Gain_1x);
			BLib_TSL2561_StMon_Reg = BLib_GlbSrv_Clr_Bit(BLib_TSL2561_StMon_Reg , BLib_TSL2561_StReg_Gain_16x);
			break;
		case TSL2561_GAIN_16X:
			BLib_TSL2561_StMon_Reg = BLib_GlbSrv_Set_Bit(BLib_TSL2561_StMon_Reg , BLib_TSL2561_StReg_Gain_16x);
			BLib_TSL2561_StMon_Reg = BLib_GlbSrv_Clr_Bit(BLib_TSL2561_StMon_Reg , BLib_TSL2561_StReg_Gain_1x);	
			break;		
	}
}


void BLib_TSL2561_Init(uint16_t TSL2561_TIMReg , uint8_t TSL2561_GAIN)
{
	/* Get the correct scale depending on the intergration time */
	/* Update the delay time basde on configuration */
	switch(TSL2561_TIMReg)
	{
		case (0x00): // 13.7 msec
			TSL2561_Init.IntegrationTime = 14;
			TSL2561_Init.ChScale = TSL2561_LUX_CHSCALE_TINT0;
			TSL2561_Init.LowThreshold = 100;
			TSL2561_Init.HighThreshold = 4850;
			break;
		case (0x01): // 101 msec
			TSL2561_Init.IntegrationTime = 102;
			TSL2561_Init.ChScale = TSL2561_LUX_CHSCALE_TINT1;
			TSL2561_Init.LowThreshold = 200;
			TSL2561_Init.HighThreshold = 36000;			
			break;
		default: // assume no scaling
			TSL2561_Init.IntegrationTime = 405;
			TSL2561_Init.ChScale = 1 << TSL2561_LUX_CHSCALE;
			TSL2561_Init.LowThreshold = 500;
			TSL2561_Init.HighThreshold = 63000;		
			break;
	}
	if(TSL2561_GAIN == TSL2561_GAIN_Auto)
	{
		TSL2561_GAIN = TSL2561_GAIN_16X;
		BLib_TSL2561_StMon_Reg = BLib_GlbSrv_Set_Bit(BLib_TSL2561_StMon_Reg , BLib_TSl2561_StReg_Autogain);
	}
	else
	{
		if(TSL2561_GAIN == TSL2561_GAIN_1X)	
		{
			TSL2561_Init.ChScale = TSL2561_Init.ChScale << 4; // scale 1X to 16X
		}	
	}
	BLib_TSL2561_IntegrationTime_Set(TSL2561_TIMReg);
	BLib_TSL2561_Gain_Set(TSL2561_GAIN);
}

uint16_t BLib_TSL2561_Ch0RawRead()
{
	uint16_t RawVal_loc;
	BLib_TSL2561_PwrUp();
	HAL_Delay(TSL2561_Init.IntegrationTime);
	RawVal_loc = BLib_TSL2561_CmdRead(TSL2561_CmdReg_Cmd_BP | TSL2561_CmdReg_Wrd_BP | TSL2561_AddReg_DATA0Low_BP);
	BLib_TSL2561_PwrDwn();
	return RawVal_loc;
}

uint16_t BLib_TSL2561_Ch1RawRead()
{
	uint16_t RawVal_loc;
	BLib_TSL2561_PwrUp();
	HAL_Delay(TSL2561_Init.IntegrationTime);
	RawVal_loc = BLib_TSL2561_CmdRead(TSL2561_CmdReg_Cmd_BP | TSL2561_CmdReg_Wrd_BP | TSL2561_AddReg_DATA1Low_BP);
	BLib_TSL2561_PwrDwn();
	return RawVal_loc;
}

void BLib_TSL2561_AutoGain(uint16_t* Ch0Raw)
{
	if((*Ch0Raw < TSL2561_Init.LowThreshold) && BLib_GlbSrv_Get_Bit(BLib_TSL2561_StMon_Reg , BLib_TSL2561_StReg_Gain_1x))
	{
		TSL2561_Init.ChScale = TSL2561_Init.ChScale >> 4; // scale 16X to 1X
		BLib_TSL2561_Gain_Set(TSL2561_GAIN_16X);
		BLib_TSL2561_StMon_Reg = BLib_GlbSrv_Set_Bit(BLib_TSL2561_StMon_Reg , BLib_TSL2561_StReg_Gain_16x);
		BLib_TSL2561_StMon_Reg = BLib_GlbSrv_Clr_Bit(BLib_TSL2561_StMon_Reg , BLib_TSL2561_StReg_Gain_1x);		
	}
	if((*Ch0Raw > TSL2561_Init.HighThreshold) && BLib_GlbSrv_Get_Bit(BLib_TSL2561_StMon_Reg , BLib_TSL2561_StReg_Gain_16x))
	{
		TSL2561_Init.ChScale = TSL2561_Init.ChScale << 4; // scale 16X to 1X
		BLib_TSL2561_Gain_Set(TSL2561_GAIN_1X);
		BLib_TSL2561_StMon_Reg = BLib_GlbSrv_Set_Bit(BLib_TSL2561_StMon_Reg , BLib_TSL2561_StReg_Gain_1x);
		BLib_TSL2561_StMon_Reg = BLib_GlbSrv_Clr_Bit(BLib_TSL2561_StMon_Reg , BLib_TSL2561_StReg_Gain_16x);		
	}	
}

uint32_t BLib_TSL2561_Lux()
{
	uint16_t Ch0RawValue_Loc , Ch1RawValue_Loc; 
	uint32_t ratio , lux_loc ,b ,m;
	Ch0RawValue_Loc = BLib_TSL2561_Ch0RawRead();
	if(BLib_GlbSrv_Get_Bit(BLib_TSL2561_StMon_Reg , BLib_TSl2561_StReg_Autogain))
		BLib_TSL2561_AutoGain(&Ch0RawValue_Loc);	
	Ch1RawValue_Loc = BLib_TSL2561_Ch1RawRead();
	// scale the channel values
	Ch0RawValue_Loc = (Ch0RawValue_Loc * TSL2561_Init.ChScale) >> TSL2561_LUX_CHSCALE;
	Ch1RawValue_Loc = (Ch1RawValue_Loc * TSL2561_Init.ChScale) >> TSL2561_LUX_CHSCALE;
	if (Ch0RawValue_Loc != 0) 
	{
		ratio = (Ch1RawValue_Loc << (TSL2561_LUX_RATIOSCALE+1)) / Ch0RawValue_Loc;
	}
	// round the ratio value
	ratio = (ratio + 1) >> 1;	
  if (ratio <= TSL2561_LUX_K1T)
    {b=TSL2561_LUX_B1T; m=TSL2561_LUX_M1T;}
  else if (ratio <= TSL2561_LUX_K2T)
    {b=TSL2561_LUX_B2T; m=TSL2561_LUX_M2T;}
  else if (ratio <= TSL2561_LUX_K3T)
    {b=TSL2561_LUX_B3T; m=TSL2561_LUX_M3T;}
  else if (ratio <= TSL2561_LUX_K4T)
    {b=TSL2561_LUX_B4T; m=TSL2561_LUX_M4T;}
  else if (ratio <= TSL2561_LUX_K5T)
    {b=TSL2561_LUX_B5T; m=TSL2561_LUX_M5T;}
  else if (ratio <= TSL2561_LUX_K6T)
    {b=TSL2561_LUX_B6T; m=TSL2561_LUX_M6T;}
  else if (ratio <= TSL2561_LUX_K7T)
    {b=TSL2561_LUX_B7T; m=TSL2561_LUX_M7T;}
  else if (ratio > TSL2561_LUX_K8T)
    {b=TSL2561_LUX_B8T; m=TSL2561_LUX_M8T;}	
		
	lux_loc = ((Ch0RawValue_Loc * b) - (Ch1RawValue_Loc * m));
	// do not allow negative lux value
	// round lsb (2^(LUX_SCALE-1))
	lux_loc += (1 << (TSL2561_LUX_CHSCALE-1));
	// strip off fractional portion
	lux_loc = lux_loc >> TSL2561_LUX_CHSCALE;
	return(lux_loc);
}









