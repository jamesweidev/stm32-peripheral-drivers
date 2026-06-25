#include "stm32f446xx_i2c_driver.h"

#define APB_CLK_FREQ 16000000

static void I2C_SendStopSignal(I2C_Reg_t* pI2Cx)
{
	pI2Cx->CR1 |= (1 << I2C_CR1_STOP);;
}

static void I2C_ACKControl(I2C_Reg_t* pI2Cx, uint8_t enable)
{
	if (enable)
	{
		pI2Cx->CR1 |= (1 << I2C_CR1_ACK);
	} else
	{
		pI2Cx->CR1 &= ~(1 << I2C_CR1_ACK);
	}
}
static void I2C_CloseTx(I2C_Handle_t* pHandle)
{
	I2C_Reg_t* pI2Cx = pHandle->pI2Cx;

	// Disable all interrupts
	pI2Cx->CR2 &= ~(1 << I2C_CR2_ITERREN);
	pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);
	pI2Cx->CR2 &= ~(1 << I2C_CR2_ITEVTEN);

	// send stop signal (if applicable)
	if (pHandle->RepeatedStart == I2C_REPEATED_START_OFF)
	{
		I2C_SendStopSignal(pI2Cx);
	}

	// clear handle parameters
	pHandle->TxLen = 0;
	pHandle->pTxBuf = NULL;
	pHandle->State = I2C_STATE_FREE;
	pHandle->Addr = 0;

	I2C_ApplicationEventCallback(pHandle, I2C_EVENT_TX_COMPLETE);
}

static void I2C_CloseRx(I2C_Handle_t* pHandle)
{
	I2C_Reg_t* pI2Cx = pHandle->pI2Cx;

	// Disable all interrupts
	pI2Cx->CR2 &= ~(1 << I2C_CR2_ITERREN);
	pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);
	pI2Cx->CR2 &= ~(1 << I2C_CR2_ITEVTEN);

	// clear handle parameters
	pHandle->RxLen = 0;
	pHandle->pRxBuf = NULL;
	pHandle->State = I2C_STATE_FREE;

	pI2Cx->CR1 &= ~(1 << I2C_CR1_POS); // Clear POS
	I2C_ACKControl(pI2Cx, ENABLE); // Re-enable ACK

	I2C_ApplicationEventCallback(pHandle, I2C_EVENT_RX_COMPLETE);
}

static void I2C_ClearADDR(I2C_Handle_t* pHandle)
{
	I2C_Reg_t* pI2Cx = pHandle->pI2Cx;

	if (pHandle->State == I2C_STATE_TX_BUSY)
	{
		// Clear ADDR bit via dummy read to SR2. SR1 has already been read
		uint32_t dummy = pI2Cx->SR2;
		(void) dummy;

		return;
	}

	if (pHandle->RxSize == 2)
	{
		I2C_ACKControl(pI2Cx, DISABLE);

		pI2Cx->CR1 |= (1 << I2C_CR1_POS); // Enable POS, NACK the second byte
	}

	// Clear ADDR bit via dummy read to SR2. SR1 has already been read
	uint32_t dummy = pI2Cx->SR2;
	(void) dummy;

	if (pHandle->RxSize == 1)
	{
		// ACK already disabled in sendIT
		I2C_SendStopSignal(pI2Cx);
	}
}

static void I2C_RxReadByte(uint8_t** buffer, uint32_t* len, I2C_Reg_t* pI2Cx)
{
	**buffer = (uint8_t) pI2Cx->DR;

	(*buffer)++;
	(*len)--;
}

static void I2C_CloseOnError(I2C_Handle_t* pHandle)
{
	if (pHandle->State == I2C_STATE_TX_BUSY)
	{
		I2C_SendStopSignal(pHandle->pI2Cx);
		pHandle->TxLen = 0;
		pHandle->pTxBuf = NULL;
		pHandle->State = I2C_STATE_FREE;
		pHandle->Addr = 0;
	}
}

void I2C_ClkControl(I2C_Reg_t* pI2Cx, uint8_t enable)
{
	uint8_t offset;
	if (pI2Cx == I2C1)
	{
		offset = 21;
	} else if (pI2Cx == I2C2)
	{
		offset = 22;
	} else if (pI2Cx == I2C3)
	{
		offset = 23;
	}

	if (enable)
	{
		RCC->APB1ENR |= (1 << offset);
	} else
	{
		RCC->APB1ENR &= ~(1 << offset);
	}
}

void I2C_Init(I2C_Handle_t* pHandle)
{
	I2C_Config_t config = pHandle->I2CConfig;
	I2C_Reg_t* pI2Cx = pHandle->pI2Cx;

	// Enable clock
	I2C_ClkControl(pI2Cx, ENABLE);


	// Configure ACK
	pI2Cx->CR1 = ((uint16_t)config.AckConfig << I2C_CR1_ACK);

	// Configure bus frequency (HSI)
	pI2Cx->CR2 = 16;

	// Configure address
	pI2Cx->OAR1 = (1 << 14) | ((uint8_t)config.DeviceAddr << 1);

	// Configure Mode
	uint32_t mode = 1; // default to FM, set to 0 if SM
	if (config.SCLSpeed == I2C_SCL_SPEED_SM)
	{
		mode = 0;
	}
	pI2Cx->CCR = (mode << 15);

	// Configure CCR
	// Retrieve correct duty cycle
	uint8_t duty_sum = 2;
	if (mode && config.FMDutyCycle)
	{
		duty_sum = 16 + 9;
	} else if (mode && !config.FMDutyCycle)
	{
		duty_sum = 3;
	}

	if (config.SCLSpeed == I2C_SCL_SPEED_SM)
	{
		pI2Cx->CCR |= (APB_CLK_FREQ / (I2C_SCL_SPEED_SM * duty_sum));
	} else if (config.SCLSpeed == I2C_SCL_SPEED_FM2K)
	{
		pI2Cx->CCR |= (APB_CLK_FREQ / (I2C_SCL_SPEED_FM2K * duty_sum));
	}

	// Configure TRise
	if (!mode)
	{
		// Standard Mode
		pI2Cx->TRISE = ((uint16_t) (1000 / 62.5) + 1) & 0x1F;
	} else
	{
		// Fast Mode
		pI2Cx->TRISE = ((uint16_t) (300 / 62.5) + 1) & 0x1F;
	}

	// Enable peripheral
	pI2Cx->CR1 |= (1 << I2C_CR1_PE);
}
void I2C_MasterSendData(uint8_t* data, uint32_t len, uint8_t target_addr, I2C_Reg_t* pI2Cx)
{
	// Set start bit to 1, triggers start signal generation
	pI2Cx->CR1 |= (1 << 8);

	// wait until start condition is generated
 	while (!(pI2Cx->SR1 & 1));


	uint16_t temp = (target_addr << 1);
	pI2Cx->DR = temp;

	// wait until address is matched
	while (!(pI2Cx->SR1 & (1 << 1)));
	// read sr2 to clear ADDR
	uint16_t dummy_read = pI2Cx->SR2;
	(void) dummy_read;

	while (len > 0)
	{
		// Only send if Tx is ready
		while (!(pI2Cx->SR1 & (1 << 7)));

		pI2Cx->DR = (uint16_t) *data;

		data++;
		len--;
	}
	// wait until send is complete
	while (!(pI2Cx->SR1 & (1 << 2)));

	// Trigger stop signal
	pI2Cx->CR1 |= (1 << 9);
}
void I2C_MasterReceiveData(uint8_t* data, uint32_t len, uint8_t target_addr, I2C_Reg_t* pI2Cx)
{
	I2C_ACKControl(pI2Cx, ENABLE); // Ensure ACK starts enabled

	pI2Cx->CR1 |= (1 << 8); // generate start

	while (!(pI2Cx->SR1 & 1)); // wait until start generates

	uint16_t temp = (target_addr << 1) + 1;
	pI2Cx->DR = temp;

	while (!(pI2Cx->SR1 & (1 << 1))); // wait until address is matched


	if (len == 1)
	{
		I2C_ACKControl(pI2Cx, DISABLE);

		// clear ADDR bit. starts clock, slave sends byte
		uint16_t dummy_read = pI2Cx->SR2;
		(void) dummy_read;

		pI2Cx->CR1 |= (1 << 9); // trigger stop

		while (! (pI2Cx->SR1 & (1 << 6))); // wait until RxNE is 1, thus ready for read

		I2C_RxReadByte(&data, &len, pI2Cx);
	}

	if (len > 1)
	{
		// clear ADDR bit. starts clock, slave sends byte
		uint16_t dummy_read = pI2Cx->SR2;
		(void) dummy_read;

		while (len > 0)
		{
			// wait until RxNE is 1, thus ready for read
			while (! (pI2Cx->SR1 & (1 << I2C_SR1_RXNE)));

			if (len == 3)
			{
				while (! (pI2Cx->SR1 & (1 << I2C_SR1_BTF)));
				I2C_ACKControl(pI2Cx, DISABLE);

				I2C_RxReadByte(&data, &len, pI2Cx);

				while (! (pI2Cx->SR1 & (1 << I2C_SR1_BTF)));

				pI2Cx->CR1 |= (1 << 9); // trigger stop

				I2C_RxReadByte(&data, &len, pI2Cx);
				I2C_RxReadByte(&data, &len, pI2Cx);

				return;
			}

			I2C_RxReadByte(&data, &len, pI2Cx);
		}
	}
}

uint8_t I2C_MasterSendDataIT(I2C_Handle_t* pHandle, uint8_t* data, uint32_t len, uint8_t target_addr, uint8_t sr)
{
	// Check for state
	I2C_Reg_t* pI2Cx = pHandle->pI2Cx;
	uint8_t state = pHandle->State;
	if (state == I2C_STATE_TX_BUSY || state == I2C_STATE_RX_BUSY) return state;
	pHandle->State = I2C_STATE_TX_BUSY;

	// Load values into handle
	pHandle->TxLen = len;
	pHandle->pTxBuf = data;
	pHandle->Addr = target_addr;
	pHandle->RepeatedStart = sr;

	// Generate start condition
	pI2Cx->CR1 |= (1 << I2C_CR1_START);

	// Enable appropriate interrupts
	pI2Cx->CR2 |= (1 << I2C_CR2_ITERREN);
	pI2Cx->CR2 |= (1 << I2C_CR2_ITBUFEN);
	pI2Cx->CR2 |= (1 << I2C_CR2_ITEVTEN);

	return state;
}
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t* pHandle, uint8_t* data, uint32_t len, uint8_t target_addr, uint8_t sr)
{
	I2C_ACKControl(pHandle->pI2Cx, ENABLE); // Ensure ACK starts enabled

	// Check for state
	I2C_Reg_t* pI2Cx = pHandle->pI2Cx;
	uint8_t state = pHandle->State;
	if (state == I2C_STATE_TX_BUSY || state == I2C_STATE_RX_BUSY) return state;
	pHandle->State = I2C_STATE_RX_BUSY;

	// Load values into handle
	pHandle->RxLen = len;
	pHandle->RxSize = len;
	pHandle->pRxBuf = data;
	pHandle->Addr = target_addr;
	pHandle->RepeatedStart = sr;

	if (len == 1)
	{
		I2C_ACKControl(pI2Cx, DISABLE);
	}

	// Generate start condition
	pI2Cx->CR1 |= (1 << I2C_CR1_START);

	// Enable appropriate interrupts
	pI2Cx->CR2 |= (1 << I2C_CR2_ITERREN);
	pI2Cx->CR2 |= (1 << I2C_CR2_ITBUFEN);
	pI2Cx->CR2 |= (1 << I2C_CR2_ITEVTEN);

	return state;
}

void I2C_IRQConfig(uint8_t IRQNumber, uint8_t enable)
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

void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority)
{
	uint8_t IPR_num = IRQNumber / 4;
	uint8_t IPR_offset = (IRQNumber % 4) * 8;

	NVIC->IPR[IPR_num] &= ~(0xFF << IPR_offset);
	NVIC->IPR[IPR_num] |= (IRQPriority << (IPR_offset + 4));
}


void I2C_IRQHandling(I2C_Handle_t* pHandle)
{
	I2C_Reg_t* pI2Cx = pHandle->pI2Cx;

	uint32_t tempreg1;

	uint32_t tempreg2 = pI2Cx->CR2 & (1 << I2C_CR2_ITEVTEN);
	uint32_t tempreg3 = pI2Cx->CR2 & (1<< I2C_CR2_ITBUFEN);

	// Case SB start bit generated
	tempreg1 = pI2Cx->SR1 & (1 << I2C_SR1_SB);

	if (tempreg1 && tempreg2)
	{
		// Send address + read/write bit
		uint8_t byte_to_write = (pHandle->Addr << 1);

		if (pHandle->State == I2C_STATE_RX_BUSY)
		{
			byte_to_write |= 1;
		}

		*((uint8_t*)(&pI2Cx->DR)) = byte_to_write;
	}

	// Case ADDR address matched
	tempreg1 = pI2Cx->SR1 & (1 << I2C_SR1_ADDR);

	if (tempreg1 && tempreg2)
	{
		I2C_ClearADDR(pHandle);
	}

	// Case BTF byte transfer complete
	tempreg1 = pI2Cx->SR1 & (1 << I2C_SR1_BTF);

	if (tempreg1 && tempreg2)
	{
		if (pHandle->State == I2C_STATE_TX_BUSY)
		{
			if (pHandle->TxLen == 0)
			{
				// When len reaches 0, close Tx
				I2C_CloseTx(pHandle);
			}
		} else if (pHandle->State == I2C_STATE_RX_BUSY)
		{
			if (pHandle->RxSize == 2)
			{
				if (pHandle->RepeatedStart == I2C_REPEATED_START_OFF)
				{
					I2C_SendStopSignal(pI2Cx);
				}

				// Data 1 in DR, 2 in Shift Reg
				I2C_RxReadByte(&pHandle->pRxBuf, &pHandle->RxLen, pI2Cx);
				I2C_RxReadByte(&pHandle->pRxBuf, &pHandle->RxLen, pI2Cx);

				I2C_CloseRx(pHandle);
			}

			if (pHandle->RxLen == 3)
			{
				// 3 bytes remaining. 1 in DR 1 in Shift Reg
				// ACK disabled for last byte
				I2C_ACKControl(pI2Cx, DISABLE);

				I2C_RxReadByte(&pHandle->pRxBuf, &pHandle->RxLen, pI2Cx);
			} else if (pHandle->RxLen == 2)
			{
				I2C_SendStopSignal(pI2Cx);

				I2C_RxReadByte(&pHandle->pRxBuf, &pHandle->RxLen, pI2Cx);
				I2C_RxReadByte(&pHandle->pRxBuf, &pHandle->RxLen, pI2Cx);

				I2C_CloseRx(pHandle);
			}
		}
	}

	// Case TXE, transfer ready for next byte
	tempreg1 = pI2Cx->SR1 & (1 << I2C_SR1_TXE);

	if (tempreg1 && tempreg2 && tempreg3)
	{
		// If master mode, load next byte into DR
		if (pI2Cx->SR2 & (1 << I2C_SR2_MSL))
		{
			if (pHandle->TxLen != 0)
			{
				*((uint8_t*)(&pI2Cx->DR)) = *pHandle->pTxBuf;

				pHandle->pTxBuf++;
				pHandle->TxLen--;
			}
		} else
		{
			// Slave mode, call user call back and request data
			if (pI2Cx->SR2 & (1 << I2C_SR2_TRA))
			{
				I2C_ApplicationEventCallback(pHandle, I2C_EVENT_DATA_REQ);
			}
		}
	}

	// Case RXNE, receive ready for read
	tempreg1 = pI2Cx->SR1 & (1 << I2C_SR1_RXNE);

	if (tempreg1 && tempreg2 && tempreg3)
	{
		// If master mode, read from DR
		if (pI2Cx->SR2 & (1 << I2C_SR2_MSL))
		{
			if (pHandle->RxSize == 1)
			{
				I2C_RxReadByte(&pHandle->pRxBuf, &pHandle->RxLen, pI2Cx);
				I2C_CloseRx(pHandle);
				return;
			}

			// Handle 2 byte receive in BTF
			if (pHandle->RxSize == 2 || pHandle->RxLen <= 3) return;

			I2C_RxReadByte(&pHandle->pRxBuf, &pHandle->RxLen, pI2Cx);
		} else
		{
			// Slave mode, call user call back and request data
			// Ensure slave is in receive mode
			if (!(pI2Cx->SR2 & (1 << I2C_SR2_TRA)))
			{
				I2C_ApplicationEventCallback(pHandle, I2C_EVENT_DATA_RECEIVED);
			}
		}
	}

	// Case Stop condition detected, only applicable to slave mode
	tempreg1 = pI2Cx->SR1 & (1 << I2C_SR1_STOPF);

	if (tempreg1 && tempreg2)
	{
		// Dummy write to clear STOPF
		pI2Cx->CR1 |= 0;

		I2C_ApplicationEventCallback(pHandle, I2C_EVENT_SLAVE_STOP);
	}

}


void I2C_ER_IRQHandling(I2C_Handle_t *pHandle)
{

	uint32_t temp1,temp2;

    //Know the status of  ITERREN control bit in the CR2
	temp2 = (pHandle->pI2Cx->CR2) & ( 1 << I2C_CR2_ITERREN);


	temp1 = (pHandle->pI2Cx->SR1) & ( 1<< I2C_SR1_BERR);
	if(temp1  && temp2 )
	{
		//This is Bus error
		pHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_BERR);

	   I2C_ApplicationEventCallback(pHandle,I2C_ERROR_BERR);
	}

	temp1 = (pHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_ARLO );
	if(temp1  && temp2)
	{
		//This is arbitration lost error
		pHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_ARLO);


		I2C_ApplicationEventCallback(pHandle,I2C_ERROR_ARLO);

	}

	temp1 = (pHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_AF);
	if(temp1  && temp2)
	{
		//This is ACK failure error
		pHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_AF);

		I2C_CloseOnError(pHandle);
		I2C_ApplicationEventCallback(pHandle,I2C_ERROR_AF);
	}

	temp1 = (pHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_OVR);
	if(temp1  && temp2)
	{
		//This is Overrun/underrun
		pHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_OVR);

		I2C_ApplicationEventCallback(pHandle,I2C_ERROR_OVR);
	}

	temp1 = (pHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_TIMEOUT);
	if(temp1  && temp2)
	{
		//This is Time out error
		pHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_TIMEOUT);

		I2C_ApplicationEventCallback(pHandle,I2C_ERROR_TIMEOUT);
	}

}

__attribute__((weak)) void I2C_ApplicationEventCallback(I2C_Handle_t* pHandle, uint8_t event_type)
{

}
