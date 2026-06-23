/*
 * stm32f446xx_gpio_driver.c
 *
 *  Created on: May 11, 2026
 *      Author: James
 */


#include "stm32f446xx_gpio_driver.h"

/**
 * @brief			Enable or disable a peripheral clock for a give GPIO port
 *
 * @param[in]		base address of a GPIO peripheral
 * @param[in] 		1 for enable, 0 for disable
 */
void GPIO_ClockControl(GPIO_Reg_t* pGPIOx, bool enable)
{
    uint32_t bit_position;

    if (pGPIOx == GPIOA)
    {
        bit_position = 0;
    }
    else if (pGPIOx == GPIOB)
    {
        bit_position = 1;
    }
    else if (pGPIOx == GPIOC)
    {
        bit_position = 2;
    }
    else if (pGPIOx == GPIOD)
    {
        bit_position = 3;
    }
    else if (pGPIOx == GPIOE)
    {
        bit_position = 4;
    }
    else if (pGPIOx == GPIOF)
    {
        bit_position = 5;
    }
    else if (pGPIOx == GPIOG)
    {
        bit_position = 6;
    }
    else if (pGPIOx == GPIOH)
    {
        bit_position = 7;
    }
    else
    {
        return;
    }

    if (enable)
    {
        RCC->AHB1ENR |= (1U << bit_position);
    }
    else
    {
        RCC->AHB1ENR &= ~(1U << bit_position);
    }
}



void GPIO_Init(GPIO_Handle_t* pGPIOHandle)
{
	GPIO_Reg_t* GPIO_Reg = pGPIOHandle->pGPIOx;
	GPIO_Config_t* GPIO_Config = &pGPIOHandle->GPIOConfig;

	GPIO_ClockControl(GPIO_Reg, ENABLE);

	uint8_t pin_number = GPIO_Config->GPIO_PinNumber;

	uint8_t bit_offset_1 = pin_number;
	uint8_t bit_offset_2 = pin_number * 2;
	uint8_t bit_offset_4 = (pin_number % 8) * 4;

	// Initialize to output mode
	uint32_t mode = GPIO_Config->GPIO_PinMode >= GPIO_PIN_MODE_IT_RT ? GPIO_PIN_MODE_IN : GPIO_Config->GPIO_PinMode;
	GPIO_Reg->MODER &= ~(0x3 << bit_offset_2);
	GPIO_Reg->MODER |= (((uint32_t) mode) << bit_offset_2);

	// Initialize output to Push Pull
	GPIO_Reg->OTYPER &= ~(0x1 << bit_offset_1);
	GPIO_Reg->OTYPER |= (((uint32_t) GPIO_Config->GPIO_PinOPType) << bit_offset_1);

	// Initialize output speed to low speed
	GPIO_Reg->OSPEEDR &= ~(0x3 << bit_offset_2); 	// 00 for low speed
	GPIO_Reg->OSPEEDR |= (((uint32_t) GPIO_Config->GPIO_PinSpeed) << bit_offset_2);

	// Initialize PUPD to pull down
	GPIO_Reg->PUPDR &= ~(0x3 << bit_offset_2);
	GPIO_Reg->PUPDR |= (((uint32_t) GPIO_Config->GPIO_PinPuPdControl) << bit_offset_2);

	// Initialize Alt function
	if (GPIO_Config->GPIO_PinMode == 2)   // alternate function mode
	{
	    if (pin_number < 8)
	    {
	        GPIO_Reg->AFRL &= ~(0xFU << bit_offset_4);
	        GPIO_Reg->AFRL |=  ((uint32_t)GPIO_Config->GPIO_PinAltFuncMode << bit_offset_4);
	    }
	    else
	    {
	        GPIO_Reg->AFRH &= ~(0xFU << bit_offset_4);
	        GPIO_Reg->AFRH |=  ((uint32_t)GPIO_Config->GPIO_PinAltFuncMode << bit_offset_4);
	    }

	}

	if (GPIO_Config->GPIO_PinMode >= 4)  // Interrupt
	{

		if (GPIO_Config->GPIO_PinMode == 4) // Rising edge interrupt
		{
			EXTI->RTSR |= (0x1 << pin_number);
			EXTI->FTSR &= ~(0x1 << pin_number);
		} else if (GPIO_Config->GPIO_PinMode == 5) // Falling edge interrupt
		{
			EXTI->FTSR |= (0x1 << pin_number);
			EXTI->RTSR &= ~(0x1 << pin_number);
		} else if (GPIO_Config->GPIO_PinMode == 6) // Falling and rising edge
		{
			EXTI->RTSR |= (0x1 << pin_number);
			EXTI->FTSR |= (0x1 << pin_number);
		}

		EXTI->IMR |= (0x1 << pin_number);


		// SYSCFG

		// Enable SYSCFG clock
		RCC->APB2ENR |= (0x1 << 14);

		uint8_t EXTICR_value;
		if (GPIO_Reg == GPIOA)
		{
			EXTICR_value = 0;
		} else if (GPIO_Reg == GPIOB)
		{
			EXTICR_value = 1;
		} else if (GPIO_Reg == GPIOC)
		{
			EXTICR_value = 2;
		} else if (GPIO_Reg == GPIOD)
		{
			EXTICR_value = 3;
		} else if (GPIO_Reg == GPIOE)
		{
			EXTICR_value = 4;
		} else if (GPIO_Reg == GPIOF)
		{
			EXTICR_value = 5;
		} else if (GPIO_Reg == GPIOG)
		{
			EXTICR_value = 6;
		}

		uint8_t EXTICR_num = pin_number / 4;
		uint8_t EXTICR_offset = (pin_number % 4) * 4;

		if (EXTICR_num == 0)
		{
			SYSCFG->EXTICR1 &= ~(0xF << EXTICR_offset);
			SYSCFG->EXTICR1 |= (EXTICR_value << EXTICR_offset);
		} else if (EXTICR_num == 1)
		{
			SYSCFG->EXTICR2 &= ~(0xF << EXTICR_offset);
			SYSCFG->EXTICR2 |= (EXTICR_value << EXTICR_offset);
		} else if (EXTICR_num == 2)
		{
			SYSCFG->EXTICR3 &= ~(0xF << EXTICR_offset);
			SYSCFG->EXTICR3 |= (EXTICR_value << EXTICR_offset);
		} else if (EXTICR_num == 3)
		{
			SYSCFG->EXTICR4 &= ~(0xF << EXTICR_offset);
			SYSCFG->EXTICR4 |= (EXTICR_value << EXTICR_offset);
		}

	}

}




void GPIO_UnInit(GPIO_Reg_t* pGPIOx)
{
    uint32_t bit_position;

    if (pGPIOx == GPIOA)
    {
        bit_position = 0;
    }
    else if (pGPIOx == GPIOB)
    {
        bit_position = 1;
    }
    else if (pGPIOx == GPIOC)
    {
        bit_position = 2;
    }
    else if (pGPIOx == GPIOD)
    {
        bit_position = 3;
    }
    else if (pGPIOx == GPIOE)
    {
        bit_position = 4;
    }
    else if (pGPIOx == GPIOF)
    {
        bit_position = 5;
    }
    else if (pGPIOx == GPIOG)
    {
        bit_position = 6;
    }
    else if (pGPIOx == GPIOH)
    {
        bit_position = 7;
    }
    else
    {
        return;
    }

    RCC->AHB1RSTR |=  (1U << bit_position);
    RCC->AHB1RSTR &= ~(1U << bit_position);
}



uint8_t GPIO_ReadFromInputPin(GPIO_Reg_t* pGPIOx, uint8_t PinNumber)
{
	uint8_t value = (uint8_t) (pGPIOx->IDR >> PinNumber) & 0x1;

	return value;
}


uint16_t GPIO_ReadFromInputPort(GPIO_Reg_t* pGPIOx)
{
	uint16_t value = (uint16_t) (pGPIOx->IDR);
	return value;
}

void GPIO_WriteToOutputPin(GPIO_Reg_t* pGPIOx, uint8_t PinNumber, uint8_t Value)
{
	if (Value == 0)
	{
		pGPIOx->ODR &= ~(0x1 << PinNumber);
	}
	else
	{
		pGPIOx->ODR |= (0x1 << PinNumber);
	}
}





void GPIO_WriteToOutputPort(GPIO_Reg_t* pGPIOx, uint16_t Value)
{
	pGPIOx->ODR = Value;
}


void GPIO_TogglePin(GPIO_Reg_t* pGPIOx, uint8_t PinNumber)
{
	pGPIOx->ODR ^= (0x1 << PinNumber);
}



void GPIO_IRQConfig(uint8_t IRQNumber, uint8_t enable)
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


void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority)
{
	uint8_t IPR_num = IRQNumber / 4;
	uint8_t IPR_offset = (IRQNumber % 4) * 8;

	NVIC->IPR[IPR_num] &= ~(0xFF << IPR_offset);
	NVIC->IPR[IPR_num] |= (IRQPriority << (IPR_offset + 4));
}


void GPIO_IRQHandling(uint8_t PinNumber)
{
	if (EXTI->PR & (0x1 << PinNumber))
	{
		EXTI->PR = (0x1 << PinNumber);
	}
}












