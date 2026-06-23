/*
 * stm32f446xx_systick_driver.h
 *
 *  Created on: May 14, 2026
 *      Author: james
 */

#ifndef INC_STM32F446XX_SYSTICK_DRIVER_H_
#define INC_STM32F446XX_SYSTICK_DRIVER_H_

#include "stm32f446xx.h"

void systick_enable(uint32_t clk_speed);
void delay(uint32_t millis);
uint32_t millis(void);
void timer_init(void);
uint32_t micros();
void delay_us(uint32_t us);

#endif /* INC_STM32F446XX_SYSTICK_DRIVER_H_ */
