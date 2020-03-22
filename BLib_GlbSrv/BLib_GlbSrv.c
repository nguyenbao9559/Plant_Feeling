#define stm32f4
#include "BLib_GlbSrv.h"


extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
//Variable to store the counter of Timer2
static uint16_t InternalCounter2;

static uint8_t FlipFlop_TurnOnDelay_St;

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


void BLib_GlbSrv_Timer_Init(TIM_HandleTypeDef *htim , uint16_t PSC , uint16_t ARR)
{
	if(htim->Instance == htim2.Instance)
	{
		TIM2->PSC = PSC;
		TIM2->ARR = ARR;
	}
	if(htim->Instance == htim4.Instance)
	{
		TIM4->PSC = PSC;
		TIM4->ARR = ARR;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == htim2.Instance)
		InternalCounter2+=1;
}


void BLib_GlbSrv_Timer_Start(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == htim2.Instance)
	{
		HAL_TIM_Base_Start_IT(&htim2);
		TIM2->CR1 = BLib_GlbSrv_Set_Bit(TIM2->CR1 , BLib_Timer_Enable); 
	}
	if(htim->Instance == htim4.Instance)
	{
		TIM4->CR1 = BLib_GlbSrv_Set_Bit(TIM4->CR1 , BLib_Timer_Enable); 
	}
}

void BLib_GLbSrv_Timer_Reset(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == htim2.Instance)
	{
		TIM2->CNT = 0;
		InternalCounter2 = 0;
	}
	if(htim->Instance == htim4.Instance)
	{
		TIM4->CNT = 0;
	}
}

void BLib_GlbSrv_Timer_Stop(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == htim2.Instance)
	{
		HAL_TIM_Base_Stop_IT(&htim2);
		BLib_GlbSrv_Clr_Bit(TIM2->CR1 , BLib_Timer_Enable);
		BLib_GLbSrv_Timer_Reset(&htim2);
		InternalCounter2 = 0;
	}
	if(htim->Instance == htim4.Instance)
	{
		BLib_GlbSrv_Clr_Bit(TIM4->CR1 , BLib_Timer_Enable);
		BLib_GLbSrv_Timer_Reset(&htim4);
	}
}

uint16_t BLib_GlbSrv_Timer_CurrentVal(TIM_HandleTypeDef *htim)
{
	uint16_t CurrentVal;
	if(htim->Instance == htim2.Instance)
		CurrentVal = InternalCounter2;
	return CurrentVal;
}

uint16_t BLib_GlbSrv_Timer_CurrentCntr(TIM_HandleTypeDef *htim)
{
	uint16_t CurrentCnt;
	if(htim->Instance == htim2.Instance)
		CurrentCnt = TIM2->CNT;
	if(htim->Instance == htim4.Instance)
		CurrentCnt = TIM4->CNT;
	return CurrentCnt;	
}

uint8_t BLib_GlbSrv_Timer_State(TIM_HandleTypeDef *htim)
{
	uint8_t state;
	if(htim->Instance == htim2.Instance)
	{
		if(BLib_GlbSrv_Get_Bit(TIM2->CR1 , BLib_Timer_Enable) == 1)
			state = 1;
		else
			state = 0;
	}
	if(htim->Instance == htim4.Instance)
	{
		if(BLib_GlbSrv_Get_Bit(TIM4->CR1 , BLib_Timer_Enable) == 1)
			state = 1;
		else
			state = 0;
	}
	return state;
}


uint8_t BLib_GlbSrv_RSFlipFlop(uint8_t Reset , uint8_t Set , uint8_t* State)
{
	*State = (Reset != 0) ? 0 : (Set != 0) ? 1 : *State;
	return *State;
}

uint8_t BLib_GlbSrv_Timer_TurnOnDelay_Sec(uint8_t Trigger_Cond, uint16_t delay_duration , uint16_t duration)
{
	uint8_t FFSet = 0, FFReset = 0, FFState = 0;
	if(Trigger_Cond)
	{
		BLib_GlbSrv_Timer_Start(&htim2);	
	}		
	if(BLib_GlbSrv_Timer_CurrentVal(&htim2) == delay_duration)
	{
		if(Trigger_Cond)
			FFSet = 1;
		else
			BLib_GlbSrv_Timer_Stop(&htim2);
	}
	FFReset = BLib_GlbSrv_Timer_CurrentVal(&htim2) == (duration + delay_duration) ? 1 : 0;
	if(BLib_GlbSrv_Timer_CurrentVal(&htim2) == (duration + delay_duration))
	if(FFReset)
		BLib_GlbSrv_Timer_Stop(&htim2);
	FFState = BLib_GlbSrv_RSFlipFlop(FFReset , FFSet , &FlipFlop_TurnOnDelay_St);
	return FFState;
}

void BLib_GLbSrv_Timer_Delay_ms(uint16_t time)
{
	BLib_GlbSrv_Timer_Init(&htim4 , 48000 , time);
	BLib_GlbSrv_Timer_Start(&htim4);
	BLib_GLbSrv_Timer_Reset(&htim4);
	while(TIM4->CNT < time);
	BLib_GlbSrv_Timer_Stop(&htim4);
}


