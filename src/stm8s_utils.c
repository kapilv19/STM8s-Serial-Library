/**
  *******************************************************************************
  * @file    stm8s_utils.c
  * @author  Kapil Verma 
  * @version V0.1.0
  * @date    31-October-2022
  * @brief   This file contains all functions prototype and macros for the STM8s Serial library.
  *******************************************************************************
  * @attention
  *
	********************** Copyright (c) 2022 Kapil Verma *************************
  	* 
	* 	Licensed under MIT License
	*
	*	Permission is hereby granted, free of charge, to any person obtaining a copy
	*	of this software and associated documentation files (the "Software"), to deal
	*	in the Software without restriction, including without limitation the rights
	*	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	*	copies of the Software, and to permit persons to whom the Software is
	*	furnished to do so, subject to the following conditions:
	*	
	*	The above copyright notice and this permission notice shall be included in all
	*	copies or substantial portions of the Software.
	*
	*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	*	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	*	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	*	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	*	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	*	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	*	SOFTWARE.
	* 
	*	Note - Parts of code are copyrighted by STMicroelectronics, licensed under 
	*	MCD-ST Liberty SW License Agreement V2. You may obtain a copy that at:  
	*
	*        http://www.st.com/software_license_agreement_liberty_v2
  * 
  *******************************************************************************
	*/

	
/* Includes -------------------------------------------------------------------*/
#include "stm8s_utils.h"


/* State Variables ------------------------------------------------------------*/
static bool isRunning = FALSE;

static volatile uint32_t ovfs = 0;


/* Internal Methods -----------------------------------------------------------*/

//TIM4 Overflow interrupt handler
//Increments overflow count every 250us 
@far @interrupt void TIM4_OVF_IRQHandler(void)
{
	ovfs++;

	TIM4_ClearFlag(TIM4_FLAG_UPDATE);
}


/* Exposed Methods ------------------------------------------------------------*/

/** @brief Initializes the Utilities library for usage.
 *
 *  @param None
 *  @retval None
 */
void Utils_init(void) 
{
	if (isRunning) return;
	
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, ENABLE);
	
	disableInterrupts();
	
	TIM4_DeInit();
	TIM4_TimeBaseInit(TIM4_PRESCALER_16, 249);      
	TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
	TIM4_Cmd(ENABLE);
	
	isRunning = TRUE;
	
	enableInterrupts();
}


/** @brief Stops the Utilities library.
 *
 *  @param None
 *  @retval None
 */
void Utils_deInit(void) 
{
	if (!isRunning) return;
	
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, DISABLE);
	
	disableInterrupts();
	
	TIM4_Cmd(DISABLE);
	TIM4_DeInit();
	
	ovfs = 0;
	isRunning = FALSE;
	
	enableInterrupts();
}


/** @brief Returns the time elapsed in microseconds since 
 *				 the initialisation of the library. The value 
 *				 overflows after 4,294,967,295 us.
 *
 *  @param None
 *  @return uint32_t Time elapsed in microseconds
 */
uint32_t micros(void)
{	
	uint32_t m;
	
	if (!isRunning) return 0;
	
	disableInterrupts();
	
	m = ((uint32_t) 250ul*ovfs) + ((uint32_t) TIM4_GetCounter());
	
	enableInterrupts();
	
	return m;
}


/** @brief Returns the time elapsed in milliseconds since 
 *				 the initialisation of the library. The value 
 *				 overflows after 1,073,741,823 ms.
 *
 *  @param None
 *  @return uint32_t Time elapsed in milliseconds
 */
uint32_t millis(void)
{
	if (!isRunning) return 0;
	
	return (uint32_t) ovfs/4ul;
}


/** @brief Stucks the program for specified number of 
 * 				 milliseconds.
 *
 *  @param milliseconds Time in milliseconds.
 *  @retval None
 */
void delay_ms(const uint32_t milliseconds)
{	
	uint32_t start = millis();
	
	if (!isRunning) return;
	
	while(millis() - start < milliseconds);
}


/** @brief Stucks the program for specified number of 
 * 				 microseconds.
 *
 *  @param microseconds Time in microseconds.
 *  @retval None
 */
void delay_us(const uint32_t microseconds)
{	
	uint32_t start = micros();
	
	if (!isRunning) return;
	
	while(micros() - start < microseconds);
}