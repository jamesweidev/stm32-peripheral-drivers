#include <stdio.h>
#include <string.h>
#include "stm32f446xx.h"

#define I2C_SCL_PIN		8
#define I2C_SDA_PIN		9

#define TARGET_ADDR		0x68

uint8_t len_cmd = 0x66;
uint8_t tx_cmd = 0x67;
uint8_t rx_cmd = 0x69;

uint8_t rx_complete = 0;

uint8_t inc_data_len;


char* data = "This is the STM32";
char recv_data[128];

I2C_Handle_t pI2CHandle;

void I2C_Inits(void)
{
	I2C_Config_t config;

	config.AckConfig = I2C_ACKCONFIG_ACK;
	config.FMDutyCycle = I2C_FMDUTYCYCLE_2;
	config.SCLSpeed = I2C_SCL_SPEED_SM;
	config.DeviceAddr = 0x61;

	pI2CHandle.I2CConfig = config;
	pI2CHandle.pI2Cx = I2C1;

	I2C_Init(&pI2CHandle);
}

void I2C_GPIO_Inits(void)
{
	GPIO_Handle_t pHandle;
	GPIO_Config_t config;

	pHandle.pGPIOx = GPIOB;

	config.GPIO_PinAltFuncMode = 4;
	config.GPIO_PinMode = GPIO_PIN_MODE_AF;
	config.GPIO_PinOPType = GPIO_OT_OD;
	config.GPIO_PinPuPdControl = GPIO_PUPD_PU;
	config.GPIO_PinSpeed = GPIO_OSPEED_FAST;

	config.GPIO_PinNumber = I2C_SCL_PIN;
	pHandle.GPIOConfig = config;
	GPIO_Init(&pHandle);

	config.GPIO_PinNumber = I2C_SDA_PIN;
	pHandle.GPIOConfig = config;
	GPIO_Init(&pHandle);
}

int main(void)
{
	I2C_IRQConfig(31, ENABLE);
	I2C_IRQConfig(32, ENABLE);

	printf_init();

	I2C_GPIO_Inits();
	I2C_Inits();

	systick_enable(16000000);

	while (1)
	{
		while (I2C_MasterSendDataIT(&pI2CHandle, &tx_cmd, 1, TARGET_ADDR, I2C_REPEATED_START_ON) != I2C_STATE_FREE);
		while (I2C_MasterSendDataIT(&pI2CHandle, (uint8_t*)data, strlen(data), TARGET_ADDR, I2C_REPEATED_START_OFF) != I2C_STATE_FREE);
		delay(2000);
		// Get the number of bytes in the message
		I2C_MasterSendData(&len_cmd, 1, TARGET_ADDR, I2C1);
		I2C_MasterReceiveData(&inc_data_len, 1, TARGET_ADDR, I2C1);
		// Get the data itself
		while (I2C_MasterSendDataIT(&pI2CHandle, &rx_cmd, 1, TARGET_ADDR, I2C_REPEATED_START_ON) != I2C_STATE_FREE);
		while (I2C_MasterReceiveDataIT(&pI2CHandle, (uint8_t*)recv_data, (uint32_t) inc_data_len, TARGET_ADDR, I2C_REPEATED_START_OFF) != I2C_STATE_FREE);

		rx_complete = 0;

		while (!rx_complete);

		recv_data[inc_data_len] = '\0';
		printf("RECEIVED: %s \r\n", recv_data);
		delay(2000);
	}

	return 0;
}

void I2C1_EV_IRQHandler(void)
{
	I2C_IRQHandling(&pI2CHandle);
}

void I2C1_ER_IRQHandler(void)
{
	I2C_ER_IRQHandling(&pI2CHandle);
}


void I2C_ApplicationEventCallback(I2C_Handle_t* pHandle, uint8_t event_type)
{
	if (event_type == I2C_ERROR_AF)
	{
		printf("ACK FAILURE, NOOOO\r\n");
	} else if (event_type == I2C_EVENT_RX_COMPLETE)
	{
		rx_complete = 1;
	}
}

