#ifdef	stm32f4
#include "stm32f4xx_hal.h"
#endif

#ifdef	stm32f1
#include "stm32f1xx_hal.h"
#endif

#ifndef __BLib_GLBSRV_H__
#define __BLib_GLBSRV_H__

<<<<<<< HEAD
=======
#include "stm32f4xx_hal_tim.h"

#define BLib_Timer_Enable		0x00
>>>>>>> plant_feeling

uint32_t BLib_GlbSrv_Set_Bit(uint32_t Data_Reg , uint8_t BP);

uint32_t BLib_GlbSrv_Clr_Bit(uint32_t Data_Reg , uint8_t BP);

uint8_t BLib_GlbSrv_Get_Bit(uint32_t Data_Reg , uint8_t BP);

<<<<<<< HEAD
=======
/*
	Name: BLib_GlbSrv_Timer_Init
	Desc: This function is used for initializing timer
	Argument: timer2 & timer4 
	PSC : Prescaler of timer.
	ARR: Auto reload register, to limit the range of timer counter.
	Usage: 
		+ Call this function to initialize the timer.
*/

void BLib_GlbSrv_Timer_Init(TIM_HandleTypeDef *htim , uint16_t PSC , uint16_t ARR);
/*
	Name: BLib_GlbSrv_Timer_Start
	Desc: This function is used for starting timer
	Argument: timer2 & timer4 
	Usage: 
		+ Call this function to start the timer.
*/
void BLib_GlbSrv_Timer_Start(TIM_HandleTypeDef *htim);
/*
	Name: BLib_GlbSrv_Timer_Stop
	Desc: This function is used for stopping timer
	Argument: timer2 & timer4 
	Usage: 
		+ Call this function to stop the timer.
*/
void BLib_GlbSrv_Timer_Stop(TIM_HandleTypeDef *htim);
/*
	Name: BLib_GlbSrv_Timer_Reset
	Desc: This function is used for Resetting timer
	Argument: timer2 & timer4 
	Usage: 
		+ Call this function to reset the value of the current timer.
*/
void BLib_GLbSrv_Timer_Reset(TIM_HandleTypeDef *htim);

/*
	Name: BLib_GLbSrv_Timer_Delay_ms
	Desc: This function is delay function.
	Argument: timer4 only
	Usage: 
		+ Call this function to reset the value of the current timer.
*/
void BLib_GLbSrv_Timer_Delay_ms(uint16_t time);

/*
	Name: BLib_GlbSrv_Timer_TurnOnDelay_Sec
	Desc: This function will generate the delay for output signal.
	Argument:
	+ Trigger_Cond: Condition to trigger delay function.
	+ delay_duration(s): The delay time before Setting signal output to 1(Output signal in this time will be 0)
	+ duration: The delay time after signal output set to 1(Output signal in this time will be 1).
	Usage: 
		When Trigger_Cond == 1, the output signal will be delay in delay_duration time before setting to HIGH level.
		Then the signal will be set to LOW level after duration time.
		After that check the Trigger_Cond:
			+Trigger_Cond == 1 : Restart the function.
			+Trigger_Cond == 0 : Stop the function.
*/
uint8_t BLib_GlbSrv_Timer_TurnOnDelay_Sec(uint8_t Trigger_Cond, uint16_t delay_duration , uint16_t duration);

/*
	Name: BLib_GlbSrv_Timer_CurrentVal
	Desc: This function is used for checking the current time value of timer
	Argument: timer2 only
	Usage: 
		+ Return current time value.
*/
uint16_t BLib_GlbSrv_Timer_CurrentVal(TIM_HandleTypeDef *htim);

/*
	Name: BLib_GlbSrv_Timer_CurrentCntr
	Desc: This function is used for checking the current counter of timer
	Argument: timer2 or timer4
	Usage: 
		+ Return current counter of timer.
*/
uint16_t BLib_GlbSrv_Timer_CurrentCntr(TIM_HandleTypeDef *htim);

/*
	Name: BLib_GlbSrv_Timer_State
	Desc: This function is used for checking the current state of timer
	Argument: timer
	Usage: 
		+ TRUE: Timer is running
		+ FALSE : Timer is stopped.
*/
uint8_t BLib_GlbSrv_Timer_State(TIM_HandleTypeDef *htim);

/*
	Name: BLib_GlbSrv_RSFlipFlop
	Desc: This function is used for Latching state of the signal output
	Argument:
		+ Reset: Condition to set the signal output to 0.
		+ Set: Condition to set the signal output to 1.
		+ State: the state of signal output.
	Usage.
*/
uint8_t BLib_GlbSrv_RSFlipFlop(uint8_t Reset , uint8_t Set , uint8_t* State);

>>>>>>> plant_feeling
#endif
