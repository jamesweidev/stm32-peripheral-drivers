/*
 * stm32f446xx_gpio_driver.h
 *
 *  Created on: May 11, 2026
 *      Author: james
 */

#ifndef INC_STM32F446XX_GPIO_DRIVER_H_
#define INC_STM32F446XX_GPIO_DRIVER_H_


#include "stm32f446xx.h"
#include <stdbool.h>

typedef struct
{
	uint8_t GPIO_PinNumber;
	uint8_t GPIO_PinMode;
	uint8_t GPIO_PinSpeed;
	uint8_t GPIO_PinPuPdControl;
	uint8_t GPIO_PinOPType;
 	uint8_t GPIO_PinAltFuncMode;
} GPIO_Config_t;

typedef struct
{
	GPIO_Reg_t* pGPIOx;
	GPIO_Config_t GPIOConfig;

} GPIO_Handle_t;


/**
 * Enable or Disable peripheral clock for a given GPIO Base addr
 */
void GPIO_ClockControl(GPIO_Reg_t* pGPIOx, bool enable);

void GPIO_Init(GPIO_Handle_t* pGPIOHandle);
void GPIO_UnInit(GPIO_Reg_t* pGPIOx);


uint8_t GPIO_ReadFromInputPin(GPIO_Reg_t* pGPIOx, uint8_t PinNumber);
uint16_t GPIO_ReadFromInputPort(GPIO_Reg_t* pGPIOx);
void GPIO_WriteToOutputPin(GPIO_Reg_t* pGPIOx, uint8_t PinNumber, uint8_t Value);
void GPIO_WriteToOutputPort(GPIO_Reg_t* pGPIOx, uint16_t Value);
void GPIO_TogglePin(GPIO_Reg_t* pGPIOx, uint8_t PinNumber);

void GPIO_IRQConfig(uint8_t IRQNumber, uint8_t enable);
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority);
void GPIO_IRQHandling(uint8_t PinNumber);









#endif /* INC_STM32F446XX_GPIO_DRIVER_H_ */
