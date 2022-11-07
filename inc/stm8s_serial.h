/**
  *******************************************************************************
  * @file    stm8s_serial.h
  * @author  Kapil Verma
  * @version V0.1.0
  * @date    27-October-2022
  * @brief   This file contains all functions prototype and macros for the STM8s Serial library.
   ******************************************************************************
  * @attention
  *
	********************** Copyright (c) 2022 Kapil Verma *************************
  * 
	* Licensed under MIT License
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
	* Note - Parts of code are copyrighted by STMicroelectronics, licensed under 
	*	MCD-ST Liberty SW License Agreement V2. You may obtain a copy that at:  
	*
	*        http://www.st.com/software_license_agreement_liberty_v2
  * 
  *******************************************************************************
	*/

/* Define to prevent recursive inclusion --------------------------------------*/
#ifndef __STM8S_SERIAL_H
#define __STM8S_SERIAL_H


/* Includes -------------------------------------------------------------------*/
#include "stm8s.h"
#include "stm8s_uart1.h"
#include "stm8s_utils.h"
#include "stm8s_private.h"


/* Macros ---------------------------------------------------------------------*/
#define TX GPIOD, GPIO_PIN_5
#define RX GPIOD, GPIO_PIN_6


/** @brief Declare RX_BUFFER_SIZE to modify RX buffer size.
 *
 */
#ifndef RX_BUFFER_SIZE
#define RX_BUFFER_SIZE 128
#endif


/** @brief Declare SERIAL_DOUBLE_PECISION to modify number 
 *				 of digits printed after decimal.
 *
 */
#ifndef SERIAL_DOUBLE_PECISION 
#define SERIAL_DOUBLE_PECISION 4
#endif


/* Exposed Methods Prototypes -------------------------------------------------*/


void Serial_begin(uint32_t baudRate);
void Serial_begin_custom(uint32_t baudRate, 
									UART1_WordLength_TypeDef wordLength, 
									UART1_StopBits_TypeDef stopBits, 
									UART1_Parity_TypeDef parity);
void Serial_write(uint8_t data);						
void Serial_print(uint8_t* data, uint8_t len);
void Serial_print_int(long data);
void Serial_print_float(double data);
void Serial_println(uint8_t* data, uint8_t len);
int Serial_available(void);
int Serial_read(void);
int Serial_readBytes(uint8_t* data, uint8_t len);
int Serial_readBytesUntil(uint8_t* data, uint8_t len, char character);
int Serial_readString(char* str, uint8_t len);
int Serial_readStringUntil(char* str, uint8_t len, char character);


#endif /* __STM8S_SERIAL_H */

/************************ Copyright (c) 2022 Kapil Verma ******END OF FILE******/