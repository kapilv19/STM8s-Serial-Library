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
	
/* Includes -------------------------------------------------------------------*/
#include "stm8s_serial.h"


/* State Variables ------------------------------------------------------------*/
static bool serial_started = FALSE;

static uint8_t rx_buffer[RX_BUFFER_SIZE];
static uint8_t rx_buffer_head = 0;
static uint8_t rx_buffer_tail = 0;

static uint8_t rx_timeout = 1000;


/* Internal Methods -----------------------------------------------------------*/
 @far @interrupt void UART1_RX_IRQHandler(void)
{
	uint8_t i = (uint8_t)(rx_buffer_tail + 1) % RX_BUFFER_SIZE;

	if (i != rx_buffer_head) 
	{
		rx_buffer[rx_buffer_tail] = UART1_ReceiveData8();
		rx_buffer_tail = i;
	}
	
	UART1_ClearITPendingBit(UART1_IT_RXNE);
	UART1_ClearFlag(UART1_FLAG_RXNE);
} 


/** @brief Configures UART1 to work as our Serial 
 *         Implementation in asynchronus full-duplex mode.
 *
 *  @param baudRate The desired baud rate for communication.
 *  @retval None
 */
void Serial_begin(const uint32_t baudRate) 
{
	Serial_begin_custom(baudRate, 
						UART1_WORDLENGTH_8D, 
						UART1_STOPBITS_1, 
						UART1_PARITY_NO);
}


/** @brief Configures UART1 to work as our Serial 
 *         Implementation in asynchronus full-duplex mode.
 *
 *  @param baudRate The desired baud rate for communication.
 *  @param wordLength The Wordlength.
 *  @param stopBits The desired baud rate for communication.
 *  @param parity The desired baud rate for communication.
 *  @retval None
 */
void Serial_begin_custom(const uint32_t baudRate, 
									UART1_WordLength_TypeDef wordLength, 
									UART1_StopBits_TypeDef stopBits, 
									UART1_Parity_TypeDef parity) 
{
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, ENABLE);
		
	disableInterrupts();
	
	Utils_init();
	
	GPIO_Init(TX, GPIO_MODE_OUT_PP_HIGH_FAST);
	GPIO_Init(RX, GPIO_MODE_IN_PU_NO_IT);
	
	UART1_DeInit();
						
	UART1_Init(baudRate, 
						UART1_WORDLENGTH_8D, 
						UART1_STOPBITS_1, 
						UART1_PARITY_NO, 
						UART1_SYNCMODE_CLOCK_DISABLE, 
						UART1_MODE_TXRX_ENABLE);
						
	UART1_ITConfig(UART1_IT_RXNE, ENABLE);
	
	enableInterrupts();
						
	UART1_Cmd(ENABLE);
	
	serial_started = TRUE;
}


/** @brief 
 *
 *  @param baudRate The desired baud rate for communication.
 *  @retval None
 */
void Serial_end(void) 
{
	disableInterrupts();	
						
	UART1_Cmd(DISABLE);
	
	UART1_DeInit();
	
	GPIO_Init(TX, GPIO_MODE_IN_FL_NO_IT); //GPIO_MODE_IN_FL_NO_IT is equivalent to deinitializing a pin.
	GPIO_Init(RX, GPIO_MODE_IN_FL_NO_IT);
	
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, DISABLE);
	
	serial_started = FALSE;
	
	enableInterrupts();
}


/** @brief Sets maximum time to wait for the next byte to 
 *				 arrive.  
 *
 *  @param timeout Timeout in milliseconds.
 *  @retval None
 */
void Serial_setTimeout(uint16_t timeout) 
{
	rx_timeout = timeout;
}


/** @brief Return maximum time to wait for the next byte to 
 *				 arrive.  
 *
 *  @param None
 *  @return uint16_t Value of timeout in milliseconds.
 */
uint16_t Serial_getTimeout(void) 
{
	return rx_timeout;
}


/** @brief Gives the number of bytes available for reading
 *				 in the serial input buffer. 
 *
 *  @param Void.
 *  @return int Number of bytes available in the serial 
 *							input buffer or -1 if Serial not started.
 */
int Serial_available() 
{
	if (!serial_started) return -1;
	return ((uint8_t)(RX_BUFFER_SIZE + rx_buffer_tail - rx_buffer_head)) % RX_BUFFER_SIZE;
}

//Next Steps: Implementing Output buffer
/** @brief Write a single raw byte to the Serial output pin.
 *
 *  @param data Byte to be written to the Serial.
 *  @retval None
 */
void Serial_write(const uint8_t data) 
{
	if (!serial_started) return;
	
	UART1_SendData8(data);
	while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
}


/** @brief Writes a string of characters to the Serial 
 * 				 output pin.
 *
 *  @param data Bytes to be written to the Serial.
 *  @param len Number of bytes to be written.
 *  @retval None
 */
void Serial_print(const uint8_t* data, const uint8_t len)
{
	int i = 0;
	
	if (!serial_started) return;
	
	for (; i<len; i++) 
	{
		Serial_write(data[i]);
	}
}


/** @brief Writes a string of characters to the Serial 
 * 				 output pin followed by a carriage return and 
 *				 newline character.
 *
 *  @param data Bytes to be written to the Serial.
 *  @param len Number of bytes to be written.
 *  @retval None
 */
void Serial_println(const uint8_t* data, const uint8_t len)
{
	Serial_print(data, len);
	Serial_print("\n", 2);
}


/** @brief Writes an integer/long to the Serial.
 *
 *  @param data Integer/long to be written to Serial.
 *  @retval None
 */
void Serial_print_int(long data) 
{
	int n = 0;
	char buffer[10];
	
	if (data == 0) Serial_write('0');
	else if (data < 0) Serial_write('-');
	
	while(data > 0) {
		buffer[n] = data%10 + '0';
		data /= 10;
		n++;
	}
	
	while(n > 0) {
		Serial_write(buffer[n-1]);
		n--;
	}
	
	Serial_write('\0');
}


/** @brief Writes an unsigned integer/long to the Serial.
 *
 *  @param data unsigned Integer/long to be written to Serial.
 *  @retval None
 */
void Serial_print_uint(uint32_t data) 
{
	int n = 0;
	char buffer[10];
	
	if (data == 0) Serial_write('0');
	
	while(data > 0) {
		buffer[n] = data%10 + '0';
		data /= 10;
		n++;
	}
	
	while(n > 0) {
		Serial_write(buffer[n-1]);
		n--;
	}
	
	Serial_write('\0');
}


/** @brief Writes an float/double to the Serial.
 *
 *  @param data float/double to be written to Serial.
 *  @retval None
 */
void Serial_print_float(const double data)
{
	long intg = (long) data;
	double frac = (data - intg);
	
	int n = SERIAL_DOUBLE_PECISION;
	
	while(n > 0) {
		frac *= 10;
		n--;
	}
	
	Serial_print_int(intg);
	
	Serial_write('.');
	
	Serial_print_int(frac);
}


/** @brief Reads a single byte from the Serial receive 
 *				 buffer. 
 *
 *  @param None
 *  @return Int Byte read from the serial or -1 if Serial 
 * 							not started or buffer is empty.
 */
int Serial_read(void) 
{
	uint32_t start;
	
	if (!serial_started) return -1;
	
	start = millis();
	
	while(millis() - start < rx_timeout) 
	{
		if (rx_buffer_head != rx_buffer_tail)  
		{
			uint8_t c = rx_buffer[rx_buffer_head];
			rx_buffer_head = (uint8_t)(rx_buffer_head + 1) % RX_BUFFER_SIZE;
			return c;
		}
	}
	
	return -1;
}


/** @brief Reads a specific number of bytes from the Serial 
 *				 receive buffer.
 *
 *  @param data The buffer to store bytes in.
 *  @param len Size of the data buffer.
 *  @return int Numer of bytes read from the serial or -1 
 * 							if Serial not started or no bytes received 
 *							yet.
 */
int Serial_readBytes(uint8_t* data, const uint8_t len)
{
	int i = 0;
	
	if (!serial_started) return -1;
	
	for (; i<len; i++) {
		data[i] = Serial_read();
		if (data[i] == -1) return i;
	}
	
	return len;
}


/** @brief Reads a specific number of bytes from the Serial 
 *				 or until a specific character arrives or until 
 *				 function call times out. 
 *				 Note - This will block code execution until 
 *				 timeout.
 *
 *  @param data The buffer to store bytes in.
 *  @param len Size of the data buffer.
 *  @param character The character to search for. 
 *  @return int Numer of bytes read from the serial or -1 
 * 							if Serial not started or no bytes received 
 *							yet.
 */
int Serial_readBytesUntil(uint8_t* data, const uint8_t len, const char character) 
{
	int i = 0;
	
	if (!serial_started) return -1;
	
	for (; i<len; i++) 
	{
		data[i] = Serial_read();
		if (data[i] == -1) return i;
		if (data[i] == (uint8_t) character) return i+1;
	}
	
	return len;
}


/** @brief Reads a string from the Serial until null('\0')
 *				 or until function call times out. 
 *				 Note - This will block code execution until 
 *				 timeout.
 *
 *  @param str The char array to store input string.
 *  @param len Length of string buffer.
 *  @return Int Length of string read from the serial or -1 
 * 							if Serial not started or no bytes received 
 *							yet.
 */
int Serial_readString(const char* str, const uint8_t len)
{
	return Serial_readBytesUntil((uint8_t*) str, len, '\0');
}


/** @brief Reads a string from the Serial until null('\0')
 *				 or until given character or until function call
 *				 times out. 
 *				 Note - This will block code execution until 
 *				 timeout.
 *
 *  @param str The char array to store input string.
 *  @param len Length of string buffer.
 *  @return Int Length of string read from the serial or -1 
 * 							if Serial not started or no bytes received 
 *							yet.
 */
int Serial_readStringUntil(char* str, const uint8_t len, const char character)
{
	int i = 0;
	
	if (!serial_started) return -1;
	
	for (; i<len; i++) 
	{
		str[i] = (char) Serial_read();
		if (str[i] == -1) return i;
		if (str[i] == '\0' || str[i] == character) return i+1;
	}
	
	return len;
}