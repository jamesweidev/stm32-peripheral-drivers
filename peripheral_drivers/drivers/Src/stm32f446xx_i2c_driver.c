/*
 * stm32f446xx_i2c_driver.c
 *
 *  Created on: May 18, 2026
 *      Author: james
 */

#include "stm32f446xx_i2c_driver.h"


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


void I2C_Init(I2C_Handle_t* Handle)
{
	I2C_Config_t config = Handle->I2CConfig;
	I2C_Reg_t* pI2Cx = Handle->pI2Cx;

	// Enable clock
	I2C_ClkControl(pI2Cx, ENABLE);


	// Configure ACK
	pI2Cx->CR1 = (uint16_t) (config.ACKCONFIG << 10);

	// Enable peripheral
	pI2Cx->CR1 |= (1 << 0);

	// Configure bus frequency (HSI)
	pI2Cx->CR2 = 16;

	// Configure address
	pI2Cx->OAR1 = (uint16_t) (config.DEVICEADDR << 1);

	// Configure Mode
	pI2Cx->CCR = (uint16_t) (config.MODE << 15);

	// Set CCR to 14
	pI2Cx->CCR |= 0xE;

	// Configure TRise
	if (!config.MODE) // standard mode
	{
		pI2Cx->TRISE = (uint16_t) (1000 / 62.5) & 0x1F;
		// Trise / Tclk

	} else
	{
		pI2Cx->TRISE = (uint16_t) (300 / 62.5) & 0x1F;
	}


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
	pI2Cx->CR1 |= (1 << 8); // generate start

	while (!(pI2Cx->SR1 & 1)); // wait until start generates

	uint16_t temp = (target_addr << 1) + 1;
	pI2Cx->DR = temp;

	while (!(pI2Cx->SR1 & (1 << 1))); // wait until address is matched


	if (len == 1)
	{
		pI2Cx->CR1 &= ~(1 << 10); // disable ACK

		// clear ADDR bit. starts clock, slave sends byte
		uint16_t dummy_read = pI2Cx->SR2;
		(void) dummy_read;

		while (! (pI2Cx->SR1 & (1 << 6))); // wait until RxNE is 1, thus ready for read

		*data = (uint8_t) pI2Cx->DR;

		data++;
		len--;

		pI2Cx->CR1 |= (1 << 9); // trigger stop
	}

	if (len > 1)
	{
		// clear ADDR bit. starts clock, slave sends byte
		uint16_t dummy_read = pI2Cx->SR2;
		(void) dummy_read;

		while (len > 0)
		{
			while (! (pI2Cx->SR1 & (1 << 6))); // wait until RxNE is 1, thus ready for read

			if (len == 2)
			{
				pI2Cx->CR1 &= ~(1 << 10); // disable ACK

				pI2Cx->CR1 |= (1 << 9); // trigger stop
			}

			*data = (uint8_t) pI2Cx->DR;

			data++;
			len--;
		}


	}

}









