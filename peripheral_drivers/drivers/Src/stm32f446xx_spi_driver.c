#include "stm32f446xx_spi_driver.h"
#include "stm32f446xx_gpio_driver.h"
#include <stdbool.h>

void SPI_ClockControl(SPI_Reg_t* pSPIx, bool enable)
{
	if (enable)
	{
		if (pSPIx == SPI1)
			{
				RCC->APB2ENR |= (0x1 << 12);
			} else if (pSPIx == SPI2)
			{
				RCC->APB1ENR |= (0x1 << 14);
			} else if (pSPIx == SPI3)
			{
				RCC->APB1ENR |= (0x1 << 15);
			} else if (pSPIx == SPI4)
			{
				RCC->APB2ENR |= (0x1 << 13);
			}
	} else
	{
		if (pSPIx == SPI1)
			{
				RCC->APB2ENR &= ~(0x1 << 12);
			} else if (pSPIx == SPI2)
			{
				RCC->APB1ENR &= ~(0x1 << 14);
			} else if (pSPIx == SPI3)
			{
				RCC->APB1ENR &= ~(0x1 << 15);
			} else if (pSPIx == SPI4)
			{
				RCC->APB2ENR &= ~(0x1 << 13);
			}
	}

}


void SPI_Init(SPI_Handle_t* pHandle)
{
	SPI_Reg_t* SPIx = pHandle->pSPIx;
	SPI_Config_t SPIConfig = pHandle->SPIConfig;

	SPI_ClockControl(SPIx, ENABLE);

	// Master selection
	SPIx->CR1 |= (SPIConfig.SPI_DeviceMode << 2);

	// Duplex
	if (SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD)
	{
		SPIx->CR1 &= ~(0x1 << 15);
	} else if (SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
	{
		SPIx->CR1 |= (0x1 << 15);
	} else if (SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SPRX)
	{
		SPIx->CR1 &= ~(0x1 << 15);
		SPIx->CR1 |= (0x1 << 10);
	}

	// Baud rate
	SPIx->CR1 &= ~(0x7 << 3);
	SPIx->CR1 |= (SPIConfig.SPI_SclkSpeed << 3);

	// DFF
	SPIx->CR1 |= (SPIConfig.SPI_DFF << 11);

	SPIx->CR1 |= (SPIConfig.SPI_CPOL << 1);

	SPIx->CR1 |= (SPIConfig.SPI_CPHA << 0);

	SPIx->CR1 |= (SPIConfig.SPI_SSM << 9);


	if (SPIConfig.SPI_SSM == SPI_SSM_SW)
	{
	    SPIx->CR1 |= (1 << 8);   // SSI = 1

	}

}


void SPI_Uninit(SPI_Reg_t* pSPIx)
{
	// RCC Reset registers
	if (pSPIx == SPI1)
		{
			RCC->APB2RSTR |= (0x1 << 12);
			RCC->APB2RSTR &= ~(0x1 << 12);
		} else if (pSPIx == SPI2)
		{
			RCC->APB1RSTR |= (0x1 << 14);
			RCC->APB1RSTR &= ~(0x1 << 14);
		} else if (pSPIx == SPI3)
		{
			RCC->APB1RSTR |= (0x1 << 15);
			RCC->APB1RSTR &= ~(0x1 << 15);
		} else if (pSPIx == SPI4)
		{
			RCC->APB2RSTR |= (0x1 << 13);
			RCC->APB2RSTR &= ~(0x1 << 13);
		}
}


void SPI_Toggle(SPI_Reg_t* pSPIx, bool enable)
{
	if (enable)
	{
		pSPIx->CR1 |= (0x1 << 6);
	} else {
		pSPIx->CR1 &= ~(0x1 << 6);
	}
}

// Blocking
void SPI_SendData(SPI_Reg_t* pSPIx, uint8_t* pTXBuf, uint32_t len)
{
	GPIO_WriteToOutputPin(GPIOB, 12, 0);

	while ((pSPIx->SR & 0x2) == 0);

	pSPIx->DR = *((volatile uint8_t*)&pSPIx->DR) = (uint8_t)len;;

	while (len > 0)
	{
		while ((pSPIx->SR & 0x2) == 0);


		if (!(pSPIx->CR1 & (0x1 << 11)))
		{
			pSPIx->DR = *pTXBuf;

			pTXBuf++;
			len--;
		} else {
			if (len < 2) break;

			*((volatile uint8_t*)&pSPIx->DR) = *pTXBuf;

			pTXBuf += 2;
			len -= 2;
		}
	}
	while (!(pSPIx->SR & (1 << 1)));
	for (volatile uint32_t i = 0; i < 5000; i++);
	GPIO_WriteToOutputPin(GPIOB, 12, 1);

}
void SPI_ReceiveData(SPI_Reg_t* pSPIx, uint8_t* pRXBuf, uint32_t len)
{
	while (len > 0)
	{
		while ((pSPIx->SR & 0x1) == 0);


		if (!(pSPIx->CR1 & (0x1 << 11)))
		{
			// 8 bit DFF
			*pRXBuf = *(uint8_t*)(&pSPIx->DR);

			pRXBuf++;
			len--;
		} else {
			// 16 bit DFF
			if (len < 2) break;

			*pRXBuf = *(uint16_t*)(&pSPIx->DR);

			pRXBuf += 2;
			len -= 2;
		}
	}

}

// Non blocking
uint8_t SPI_SendDataIT(SPI_Handle_t* pHandle, uint8_t* pTXBuf, uint32_t len)
{
	uint8_t state = pHandle->TxState;

	if (state == SPI_STATE_BUSY) return state;

	pHandle->pTxBuf = pTXBuf;
	pHandle->TxLen = len;
	pHandle->TxState = SPI_STATE_BUSY;
	pHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE); // Enable TXEIE Interrupt

	return state;
}


uint8_t SPI_ReceiveDataIT(SPI_Handle_t* pHandle, uint8_t* pRXBuf, uint32_t len)
{
	uint8_t state = pHandle->TxState;

	if (state == SPI_STATE_BUSY) return state;

	pHandle->pRxBuf = pRXBuf;
	pHandle->RxLen = len;
	pHandle->RxState = SPI_STATE_BUSY;
	pHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE); // Enable RXEIE Interrupt

	return state;
}


void SPI_IRQConfig(uint8_t IRQNumber, uint8_t enable)
{
	uint8_t reg_num = IRQNumber / 32;
	uint8_t IRQ_offset = IRQNumber % 32;

	if (enable)
	{
		NVIC->ISER[reg_num] |= (0x1 << IRQ_offset);
	} else
	{
		NVIC->ICER[reg_num] |= (0x1 << IRQ_offset);
	}
}

void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority)
{
	uint8_t IPR_num = IRQNumber / 4;
	uint8_t IPR_offset = (IRQNumber % 4) * 8;

	NVIC->IPR[IPR_num] &= ~(0xFF << IPR_offset);
	NVIC->IPR[IPR_num] |= (IRQPriority << (IPR_offset + 4));
}

void SPI_IRQHandling(SPI_Handle_t* pHandle)
{
	SPI_Config_t config = pHandle->SPIConfig;
	SPI_Reg_t* pSPIx = pHandle->pSPIx;
	uint8_t tempreg1;
	uint8_t tempreg2;

	// TxE Interrupt
	tempreg1 = pSPIx->SR & (1 << SPI_SR_TXE);
	tempreg2 = pSPIx->CR2 & (1 << SPI_CR2_TXEIE);
	if (tempreg1 && tempreg2)
	{
		if (config.SPI_DFF == SPI_DFF_8)
		{
			pSPIx->DR = *pHandle->pTxBuf;

			pHandle->TxLen--;
			pHandle->pTxBuf++;
		} else if (config.SPI_DFF == SPI_DFF_16)
		{
			pSPIx->DR = *((uint16_t*) pHandle->pTxBuf);

			pHandle->TxLen -= 2;
			pHandle->pTxBuf += 2;
		}

		// Terminate the Tx communication
		if (pHandle->TxLen <= 0)
		{
			pHandle->pTxBuf = NULL;
			pHandle->TxState = SPI_STATE_FREE;
			pHandle->TxLen = 0;

			// Disable Tx interrupt
			pSPIx->CR2 &= ~(1 << SPI_CR2_TXEIE);

			SPI_ApplicationEventCallback(pHandle, SPI_EVENT_TX_COMPLETE);
		}
	}

	// RxNE Interrupt
	tempreg1 = pSPIx->SR & (1 << SPI_SR_RXNE);
	tempreg2 = pSPIx->CR2 & (1 << SPI_CR2_RXNEIE);
	if (tempreg1 && tempreg2)
	{
		if (config.SPI_DFF == SPI_DFF_8)
		{
			*pHandle->pRxBuf = (uint8_t) pSPIx->DR;

			pHandle->RxLen--;
		} else if (config.SPI_DFF == SPI_DFF_16)
		{
			*((uint16_t*) pHandle->pTxBuf) = pSPIx->DR;

			pHandle->RxLen -= 2;
		}

		// Terminate the Rx communication
		if (pHandle->RxLen <= 0)
		{
			pHandle->pRxBuf = NULL;
			pHandle->RxState = SPI_STATE_FREE;
			pHandle->RxLen = 0;

			// Disable Rx interrupt
			pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE);

			SPI_ApplicationEventCallback(pHandle, SPI_EVENT_RX_COMPLETE);
		}
	}

	// Check for OVR
	tempreg1 = pSPIx->SR & (1 << SPI_SR_OVR);
	tempreg2 = pSPIx->CR2 & (1 << SPI_CR2_ERRIE);

	if (tempreg1 && tempreg2)
	{
		SPI_ApplicationEventCallback(pHandle, SPI_EVENT_OVR_ERR);
	}
}

__attribute__((weak)) void SPI_ApplicationEventCallback(SPI_Handle_t* pHandle, uint8_t event_type)
{

}
