#include "main.h"
#include "BLib_GlbSrv.h"

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
//Variable to store the counter of Timer2
static uint16_t InternalCounter2;
static uint8_t FlipFlop_TurnOnDelay_St;
//////////////////////////////////////
static uint32_t InternalADC1;
static uint32_t ADC_Array[2];
	


/*********************HAL function Start**********************/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance == hadc1.Instance)
		InternalADC1 = HAL_ADC_GetValue(hadc);
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == htim2.Instance)
		InternalCounter2+=1;
}
/*********************HAL function Stop**********************/

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

uint8_t BLib_GlbSrv_RSFlipFlop(uint8_t Reset , uint8_t Set , uint8_t* State)
{
	*State = (Reset != 0) ? 0 : (Set != 0) ? 1 : *State;
	return *State;
}

uint8_t BLib_GlbSrv_RngCheck_State(uint32_t Val_In , uint32_t Val_LowThresh , uint32_t Val_HighThresh)
{
	uint8_t RngCheck_St;
	if((Val_In >= Val_LowThresh) && (Val_In <= Val_HighThresh))
		RngCheck_St = 1;
	else
		RngCheck_St = 0;
	return RngCheck_St;
}

uint32_t BLib_GlbSrv_RngCheck(int32_t Val_In , int32_t Val_LowThresh , int32_t Val_HighThresh)
{
	int32_t Val_Out;
	if((Val_In >= Val_LowThresh) && (Val_In <= Val_HighThresh))
		Val_Out = Val_In;
	else
	{
		if(Val_In < Val_LowThresh)
			Val_Out = Val_LowThresh;
		if(Val_In > Val_HighThresh)
			Val_Out = Val_HighThresh;
	}
	return Val_Out;
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
	BLib_GlbSrv_Timer_Init(&htim4 , 24000 , time);
	BLib_GlbSrv_Timer_Start(&htim4);
	BLib_GLbSrv_Timer_Reset(&htim4);
	while(TIM4->CNT < time);
	BLib_GlbSrv_Timer_Stop(&htim4);
}

/*
	Channel : 0-> 18
*/
//void BLib_GlbSrv_ADC_Init(ADC_HandleTypeDef *hadc,uint8_t channel_Order, uint8_t channel_Numb ,uint8_t BLib_ADC_Accuracy)
//{
//	HAL_ADC_Start_IT(hadc);
//	uint32_t Sample_Loc;
//	/**Number of data items to be transferred (0 up to 65535 based on 16 bit DMA_SxNDT register*/
//	//HAL_ADC_Start_DMA(hadc , (uint32_t*)ADC_Array , BLib_ADC_Accuracy);
//	//To convert to naming convention standard
//	if(channel_Order > 9)
//		channel_Order = channel_Order - 9; 
//	channel_Order = channel_Order * 3;
//	//////////////////////////////////////////
//	switch(BLib_ADC_Accuracy)
//	{
//		case BLib_ADC_Accuracy_Low:
//			Sample_Loc = BLib_ADC_SMPR_BP_1 >> channel_Order;
//			break;
//		case BLib_ADC_Accuracy_Med:
//			Sample_Loc = (BLib_ADC_SMPR_BP_1 | BLib_ADC_SMPR_BP_2) >> channel_Order;
//			break;
//		case BLib_ADC_Accuracy_High:
//			Sample_Loc = (BLib_ADC_SMPR_BP_0 | BLib_ADC_SMPR_BP_1 | BLib_ADC_SMPR_BP_2) >> channel_Order;
//			break;
//		default:
//			Sample_Loc = (BLib_ADC_SMPR_BP_0 | BLib_ADC_SMPR_BP_1 | BLib_ADC_SMPR_BP_2) >> channel_Order;
//			break;
//	}
//	if(hadc->Instance == hadc1.Instance)
//	{
//		if(channel_Order > 9)
//			ADC1->SMPR1 = 0;
//		else
//			ADC1->SMPR2 = 0;
//	}

//}
uint16_t BLib_GlbSrv_ADC_Read(ADC_HandleTypeDef *hadc)
{
	if(!(hadc->Instance->SR & ADC_SR_EOC))
		HAL_ADC_Start_IT(hadc);
	uint16_t ADCVal_Loc;
	if(hadc->Instance == hadc1.Instance)
		ADCVal_Loc = InternalADC1;
	return ADCVal_Loc;
}
