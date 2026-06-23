/*
 * stm32f446xx_usart_driver.c
 *
 *  Created on: May 21, 2026
 *      Author: james
 */


#include "stm32f446xx_usart_driver.h"

static void usart_handle_txeie(USART_Handle_t* p_handle);
static void usart_handle_tcie(USART_Handle_t* p_handle);
static void usart_handle_rxneie(USART_Handle_t* p_handle);

void usart_clk_control(USART_Reg_t* pUSARTx, uint8_t enable)
{
	if (enable)
	{
		if (pUSARTx == USART1) USART1_CLK_EN();
		if (pUSARTx == USART2) USART2_CLK_EN();
		if (pUSARTx == USART3) USART3_CLK_EN();
		if (pUSARTx == USART4) USART4_CLK_EN();
		if (pUSARTx == USART5) USART5_CLK_EN();
		if (pUSARTx == USART6) USART6_CLK_EN();
	} else
	{
		if (pUSARTx == USART1) USART1_CLK_STOP();
		if (pUSARTx == USART2) USART2_CLK_STOP();
		if (pUSARTx == USART3) USART3_CLK_STOP();
		if (pUSARTx == USART4) USART4_CLK_STOP();
		if (pUSARTx == USART5) USART5_CLK_STOP();
		if (pUSARTx == USART6) USART6_CLK_STOP();
	}
}

void usart_init(USART_Handle_t* pHandle)
{
	USART_Config_t config = pHandle->config;
	USART_Reg_t* pUSARTx = pHandle->pUSARTx;

	uint32_t tempreg = 0;

	// enable clock
	usart_clk_control(pUSARTx, 1);

	// CR2
	tempreg |= (config.stop_bits << 12); // configure stop bits

	pUSARTx->CR2 = tempreg;

	// CR3
	tempreg = 0;
	if (config.flow_ctrl == 1 || config.flow_ctrl == 3)
	{
		tempreg |= (1 << 9);
	}
	if (config.flow_ctrl == 2 || config.flow_ctrl == 3)
	{
		tempreg |= (1 << 8);
	}
	pUSARTx->CR3 = tempreg;

	// CR1
	tempreg = 0;
	if (config.mode == 0 || config.mode == 2) // enable transmit
	{
		tempreg |= (1 << 3);
	}
	if (config.mode == 1 || config.mode == 2) // enable receive
	{
		tempreg |= (1 << 2);
	}

	tempreg |= (config.word_len << 12); // configure word length

	tempreg |= (1 << 13); // enable USART

	if (config.parity == 1)
	{
		tempreg |= (1 << 10);
		tempreg |= (1 << 9);
	} else if (config.parity == 2)
	{
		tempreg |= (1 << 10);
	}

	pUSARTx->CR1 = tempreg;



	// Configure BRR
	tempreg = 0;

	uint32_t baud = (config.baud == 0) ? 1200 :
					(config.baud == 1) ? 2400 :
					(config.baud == 2) ? 9600 :
					(config.baud == 3) ? 19200 :
					(config.baud == 4) ? 115200 : 0;

	uint32_t tempvalue = (16000000 + baud / 2) / baud;

	if (config.over8)
	{
		uint32_t mantissa = tempvalue / 8;
		uint32_t fraction = tempvalue % 8;

		tempreg |= ((mantissa << 4) | fraction);
	} else
	{
		tempreg = tempvalue;
	}

	pUSARTx->BRR = tempreg;
}



void usart_send_data(USART_Handle_t* p_handle, uint8_t* p_tx_buffer, uint32_t len)
{
	USART_Reg_t* pUSARTx = p_handle->pUSARTx;

	while (len > 0)
	{
		while (! (pUSARTx->SR & (1 << 7)) ); 	// wait for TXE

		pUSARTx->DR = *(p_tx_buffer);

		len--;
		p_tx_buffer++;
	}

	while (! (pUSARTx->SR & (1 << 6))); // wait til transmission is complete
	pUSARTx->SR &= ~(1 << 6);
}


void usart_receive_data(USART_Handle_t* p_handle, uint8_t* p_rx_buffer, uint32_t len)
{
	USART_Reg_t* pUSARTx = p_handle->pUSARTx;

	 uint8_t parity_mask = 0xFF;

	// if parity is enabled
	if (pUSARTx->CR1 & (1 << 10))
	{
		parity_mask = 0x7F;
	}

	while (len > 0)
	{
		while (! (pUSARTx->SR & (1 << 5)) ); 	// wait for RXNE

		*p_rx_buffer = (uint8_t) (pUSARTx->DR & parity_mask);

		len--;
		p_rx_buffer++;
	}

}


uint8_t usart_send_data_it(USART_Handle_t* p_handle, uint8_t* p_tx_buffer, uint32_t len)
{
	if (p_handle->txstate) return 1;

	USART_Reg_t* pUSARTx = p_handle->pUSARTx;

	p_handle->p_tx_buffer = p_tx_buffer;
	p_handle->len = len;
	p_handle->txstate = 1;

	pUSARTx->CR1 |= (1 << 7); // enable TXEIE
	pUSARTx->CR1 |= (1 << 6); // enable TCIE

	return 0;
}



uint8_t usart_receive_data_it(USART_Handle_t* p_handle, uint8_t* p_tx_buffer, uint32_t len)
{
	if (p_handle->rxstate) return 1;

	USART_Reg_t* pUSARTx = p_handle->pUSARTx;

	p_handle->p_rx_buffer = p_tx_buffer;
	p_handle->len = len;
	p_handle->rxstate = 1;

	pUSARTx->CR1 |= (1 << 5); // enable RXNE

	return 0;
}


void usart_irq_handling(USART_Handle_t* p_handle)
{
	USART_Reg_t* pUSARTx = p_handle->pUSARTx;
	uint32_t temp1, temp2;

	temp1 = pUSARTx->SR & (1 << 6); // tc
	temp2 = pUSARTx->CR1 & (1 << 6); // tcie

	// TCIE Handling
	if (temp1 && temp2)
	{
		usart_handle_tcie(p_handle);
	}

	temp1 = pUSARTx->SR & (1 << 7); // txe
	temp2 = pUSARTx->CR1 & (1 << 7); // txeie

	if (temp1 && temp2)
	{
		usart_handle_txeie(p_handle);
	}

	temp1 = pUSARTx->SR & (1 << 5); // rxne
	temp2 = pUSARTx->CR1 & (1 << 5); // rxneie

	if (temp1 && temp2)
	{
		usart_handle_rxneie(p_handle);
	}
}

// Interrupt Handlers

static void usart_handle_txeie(USART_Handle_t* p_handle)
{
	USART_Reg_t* pUSARTx = p_handle->pUSARTx;

	if (p_handle->txstate)
	{
		if (p_handle->len > 0)
		{
			if (pUSARTx->CR1 & (1 << 12)) // 9 bit word
			{
				pUSARTx->DR = *((uint16_t*) p_handle->p_tx_buffer) & 0x01FF;
				p_handle->p_tx_buffer++;
				p_handle->len--;
			} else // 8 bit word
			{
				pUSARTx->DR = *(p_handle->p_tx_buffer);
			}

			p_handle->len--;
			p_handle->p_tx_buffer++;
		} else
		{
			pUSARTx->CR1 &= ~(1 << 7); // Clear TXEIE
		}
	}
}

static void usart_handle_tcie(USART_Handle_t* p_handle)
{
	USART_Reg_t* pUSARTx = p_handle->pUSARTx;
	if (p_handle->txstate)
	{
		if (p_handle->len == 0)
		{
			pUSARTx->SR &= ~(1 << 6); // clear TC flag

			pUSARTx->CR1 &= ~(1 << 6); // Clear TCIE

			p_handle->txstate = 0;
			p_handle->len = 0;
			p_handle->p_tx_buffer = 0;
		}
	}
}

static void usart_handle_rxneie(USART_Handle_t* p_handle)
{
	if (!p_handle->rxstate) return;

	USART_Reg_t* pUSARTx = p_handle->pUSARTx;
	uint8_t parity_mask = 0xFF;

	(void) parity_mask;


	if (p_handle->len > 0)
	{
		if (pUSARTx->CR1 & (1 << 12)) // if 9 bit frame
		{
			if (pUSARTx->CR1 & (1 << 10)) // if parity is enabled
			{
				*(p_handle->p_rx_buffer) = (uint8_t) pUSARTx->DR;
				p_handle->len--;
				p_handle->p_rx_buffer++;
			} else
			{
				*(p_handle->p_rx_buffer) = (uint16_t) (pUSARTx->DR & 0x01FF);
				p_handle->len -= 2;
				p_handle->p_rx_buffer += 2;
			}
		} else
		{
			if (pUSARTx->CR1 & (1 << 10)) // parity enabled
			{
				*(p_handle->p_rx_buffer) = (uint8_t) (pUSARTx->DR & 0x7F);
			} else
			{
				*(p_handle->p_rx_buffer) = (uint8_t) pUSARTx->DR;
			}

			p_handle->len--;
			p_handle->p_rx_buffer++;
		}
	} else
	{
		pUSARTx->CR1 &= ~(1 << 5); // Clear rxneie
		p_handle->rxstate = 0;
	}
}


USART_Handle_t usart2_handle;

int _write(int file, char *ptr, int len)
{
	(void) file;

	usart_send_data(&usart2_handle, (uint8_t *) ptr, len);

	return len;
}


void USART2_GPIOInit(void)
{
    GPIO_Handle_t gpio;

    gpio.pGPIOx = GPIOA;
    gpio.GPIOConfig.GPIO_PinMode = GPIO_PIN_MODE_AF;
    gpio.GPIOConfig.GPIO_PinAltFuncMode = 7;
    gpio.GPIOConfig.GPIO_PinOPType = GPIO_OT_PP;
    gpio.GPIOConfig.GPIO_PinPuPdControl = GPIO_PUPD_NONE;
    gpio.GPIOConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;

    GPIO_ClockControl(GPIOA, ENABLE);

    // PA2 = USART2_TX
    gpio.GPIOConfig.GPIO_PinNumber = 2;
    GPIO_Init(&gpio);

    // PA3 = USART2_RX, optional if you only print
    gpio.GPIOConfig.GPIO_PinNumber = 3;
    GPIO_Init(&gpio);
}


void USART2_Init(void)
{


    usart2_handle.pUSARTx = USART2;
    usart2_handle.config.baud = 4;
    usart2_handle.config.flow_ctrl = 0;
    usart2_handle.config.mode = 0;
    usart2_handle.config.stop_bits = 0;
    usart2_handle.config.word_len = 0;
    usart2_handle.config.parity = 0;
    usart2_handle.config.over8 = 0;

    usart_clk_control(USART2, ENABLE);
    usart_init(&usart2_handle);
}

void printf_init(void)
{
	USART2_GPIOInit();
	USART2_Init();
}


__attribute__((weak)) void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle,uint8_t event)
{

}
