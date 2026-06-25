#include "stm32f446xx_uart_driver.h"

static void usart_handle_txeie(USART_Handle_t* pHandle);
static void usart_handle_tcie(USART_Handle_t* pHandle);
static void usart_handle_rxneie(USART_Handle_t* pHandle);

void usart_ClkControl(USART_Reg_t* pUSARTx, uint8_t enable)
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

void USART_Init(USART_Handle_t* pHandle)
{
	USART_Config_t config = pHandle->config;
	USART_Reg_t* pUSARTx = pHandle->pUSARTx;

	uint32_t tempreg = 0;

	// enable clock
	usart_clk_control(pUSARTx, 1);

	// CR2
	tempreg |= (config.stop_bits << USART_CR2_STOP); // configure stop bits

	pUSARTx->CR2 = tempreg;

	// CR3
	tempreg = 0;
	if (config.flow_ctrl == USART_FLOW_CTRL_CTS || config.flow_ctrl == USART_FLOW_CTRL_CTS_RTS)
	{
		tempreg |= (1 << USART_CR3_CTSE);
	}
	if (config.flow_ctrl == USART_FLOW_CTRL_RTS || config.flow_ctrl == USART_FLOW_CTRL_CTS_RTS)
	{
		tempreg |= (1 << USART_CR3_RTSE);
	}
	pUSARTx->CR3 = tempreg;

	// CR1
	tempreg = 0;
	if (config.mode == USART_MODE_TX || config.mode == USART_MODE_TX_RX) // enable transmit
	{
		tempreg |= (1 << USART_CR1_TE);
	}
	if (config.mode == USART_MODE_RX || config.mode == USART_MODE_TX_RX) // enable receive
	{
		tempreg |= (1 << USART_CR1_RE);
	}

	tempreg |= (config.word_len << USART_CR1_M); // configure word length

	tempreg |= (1 << USART_CR1_UE); // enable USART

	if (config.parity == USART_PARITY_ODD)
	{
		tempreg |= (1 << USART_CR1_PCE);
		tempreg |= (1 << USART_CR1_PS);
	} else if (config.parity == USART_PARITY_ODD)
	{
		tempreg |= (1 << USART_CR1_PCE);
	}

	pUSARTx->CR1 = tempreg;



	// Configure BRR
	tempreg = 0;
	uint32_t APB1_clk = RCC_GetPClk1();
	uint32_t tempvalue = (APB1_clk + config.baud / 2) / config.baud;

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


void USART_SendData(USART_Handle_t* pHandle, uint8_t* data, uint32_t len)
{
	USART_Reg_t* pUSARTx = pHandle->pUSARTx;

	while (len > 0)
	{
		while (! (pUSARTx->SR & (1 << USART_SR_TXE)) ); 	// wait for TXE

		pUSARTx->DR = *(data);

		len--;
		data++;
	}

	while (! (pUSARTx->SR & (1 << USART_SR_TC))); // wait til transmission is complete
	pUSARTx->SR &= ~(1 << USART_SR_TC);
}


void USART_ReceiveData(USART_Handle_t* pHandle, uint8_t* data, uint32_t len)
{
	USART_Reg_t* pUSARTx = pHandle->pUSARTx;

	 uint8_t parity_mask = 0xFF;

	// if parity is enabled
	if (pUSARTx->CR1 & (1 << USART_CR1_PCE))
	{
		parity_mask = 0x7F;
	}

	while (len > 0)
	{
		while (! (pUSARTx->SR & (1 << USART_SR_RXNE)) ); 	// wait for RXNE

		*data = (uint8_t) (pUSARTx->DR & parity_mask);

		len--;
		data++;
	}

}


uint8_t USART_SendDataIT(USART_Handle_t* pHandle, uint8_t* data, uint32_t len)
{
	uint8_t state = pHandle->RxState;
	if (state == USART_STATE_RX_BUSY || state == USART_STATE_TX_BUSY) return state;


	USART_Reg_t* pUSARTx = pHandle->pUSARTx;

	pHandle->pTxBuf = data;
	pHandle->len = len;
	pHandle->TxState = USART_STATE_TX_BUSY;

	pUSARTx->CR1 |= (1 << USART_CR1_TXEIE); // enable TXEIE
	pUSARTx->CR1 |= (1 << USART_CR1_TCIE); // enable TCIE

	return state;
}


uint8_t USART_ReceiveDataIT(USART_Handle_t* pHandle, uint8_t* data, uint32_t len)
{
	uint8_t state = pHandle->RxState;
	if (state == USART_STATE_RX_BUSY || state == USART_STATE_TX_BUSY) return state;

	USART_Reg_t* pUSARTx = pHandle->pUSARTx;

	pHandle->pRxBuf = data;
	pHandle->len = len;
	pHandle->RxState = USART_STATE_RX_BUSY;

	pUSARTx->CR1 |= (1 << USART_CR1_RXNEIE); // enable RXNE

	return state;
}


void USART_IRQHandler(USART_Handle_t* pHandle)
{
	USART_Reg_t* pUSARTx = pHandle->pUSARTx;
	uint32_t temp1, temp2;

	temp1 = pUSARTx->SR & (1 << USART_SR_TC);
	temp2 = pUSARTx->CR1 & (1 << USART_CR1_TCIE);

	// TCIE Handling
	if (temp1 && temp2)
	{
		usart_handle_tcie(pHandle);
	}

	temp1 = pUSARTx->SR & (1 << USART_SR_TXE);
	temp2 = pUSARTx->CR1 & (1 << USART_CR1_TXEIE);

	if (temp1 && temp2)
	{
		usart_handle_txeie(pHandle);
	}

	temp1 = pUSARTx->SR & (1 << USART_SR_RXNE);
	temp2 = pUSARTx->CR1 & (1 << USART_CR1_RXNEIE);

	if (temp1 && temp2)
	{
		usart_handle_rxneie(pHandle);
	}
}

// Interrupt Handlers

static void usart_handle_txeie(USART_Handle_t* pHandle)
{
	USART_Reg_t* pUSARTx = pHandle->pUSARTx;

	if (pHandle->TxState == USART_STATE_TX_BUSY)
	{
		if (pHandle->len > 0)
		{
			if (pUSARTx->CR1 & (1 << USART_CR1_M)) // 9 bit word
			{
				pUSARTx->DR = *((uint16_t*) pHandle->pTxBuf) & 0x01FF;
				pHandle->pTxBuf++;
				pHandle->len--;
			} else // 8 bit word
			{
				pUSARTx->DR = *(pHandle->pTxBuf);
			}

			pHandle->len--;
			pHandle->pTxBuf++;
		} else
		{
			pUSARTx->CR1 &= ~(1 << USART_CR1_TXEIE); // Clear TXEIE
		}
	}
}

static void usart_handle_tcie(USART_Handle_t* pHandle)
{
	USART_Reg_t* pUSARTx = pHandle->pUSARTx;
	if (pHandle->TxState == USART_STATE_TX_BUSYTE)
	{
		if (pHandle->len == 0)
		{
			pUSARTx->SR &= ~(1 << USART_SR_TC); // clear TC flag

			pUSARTx->CR1 &= ~(1 << USART_CR1_TCIE); // Clear TCIE

			pHandle->TxState = USART_STATE_FREE;
			pHandle->len = 0;
			pHandle->pTxBuf = 0;
		}
	}
}

static void usart_handle_rxneie(USART_Handle_t* pHandle)
{
	if (pHandle->RxState != USART_STATE_RX_BUSY) return;

	USART_Reg_t* pUSARTx = pHandle->pUSARTx;
	uint8_t parity_mask = 0xFF;

	(void) parity_mask;


	if (pHandle->len > 0)
	{
		if (pUSARTx->CR1 & (1 << USART_CR1_M)) // if 9 bit frame
		{
			if (pUSARTx->CR1 & (1 << USART_CR1_PCE)) // if parity is enabled
			{
				*(pHandle->pRxBuf) = (uint8_t) pUSARTx->DR;
				pHandle->len--;
				pHandle->pRxBuf++;
			} else
			{
				*(pHandle->pRxBuf) = (uint16_t) (pUSARTx->DR & 0x01FF);
				pHandle->len -= 2;
				pHandle->pRxBuf += 2;
			}
		} else
		{
			if (pUSARTx->CR1 & (1 << USART_CR1_PCE)) // parity enabled
			{
				*(pHandle->pRxBuf) = (uint8_t) (pUSARTx->DR & 0x7F);
			} else
			{
				*(pHandle->pRxBuf) = (uint8_t) pUSARTx->DR;
			}

			pHandle->len--;
			pHandle->pRxBuf++;
		}
	} else
	{
		pUSARTx->CR1 &= ~(1 << USART_CR1_RXNEIE); // Clear rxneie
		pHandle->RxState = USART_STATE_FREE;
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
