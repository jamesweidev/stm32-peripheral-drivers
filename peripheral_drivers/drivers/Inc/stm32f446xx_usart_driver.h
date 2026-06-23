/*
 * stm32f446xx_usart_driver.h
 *
 *  Created on: May 21, 2026
 *      Author: james
 */

#ifndef INC_STM32F446XX_USART_DRIVER_H_
#define INC_STM32F446XX_USART_DRIVER_H_

#include "stm32f446xx.h"

typedef struct
{
	uint8_t mode; // 0: tx, 1: rx, 2: both
	uint8_t baud; // 0: 1,200, 1: 2,400, 2: 9,600, 3: 19,200, 4: 115200
	uint8_t word_len; // 0: 1 start, 8 data. 1: 1 start, 9 data
	uint8_t stop_bits; // 0: 1 bit, 1: 0.5 bits, 2: 2 bits, 3: 1.5 bits
	uint8_t flow_ctrl; // 0: none, 1: cts, 2: rts, 3: both
	uint8_t parity; // 0: disable, 1: odd, 2: even
	uint8_t over8; // 0: over 16, 1: over 8
} USART_Config_t;

typedef struct
{
	USART_Reg_t* pUSARTx;
	USART_Config_t config;
	uint8_t* p_tx_buffer;
	uint8_t* p_rx_buffer;
	uint32_t len;
	uint8_t txstate;
	uint8_t rxstate;
} USART_Handle_t;

void usart_clk_control(USART_Reg_t* pUSARTx, uint8_t enable);

void usart_init(USART_Handle_t* pHandle);
void usart_uninit(USART_Handle_t* pHandle);


void usart_send_data(USART_Handle_t* p_handle, uint8_t* p_data_buffer, uint32_t len);
void usart_receive_data(USART_Handle_t* p_handle, uint8_t* p_data_buffer, uint32_t len);

uint8_t usart_send_data_it(USART_Handle_t* p_handle, uint8_t* p_data_buffer, uint32_t len);
uint8_t usart_receive_data_it(USART_Handle_t* p_handle, uint8_t* p_data_buffer, uint32_t len);



void printf_init(void);



#endif /* INC_STM32F446XX_USART_DRIVER_H_ */
