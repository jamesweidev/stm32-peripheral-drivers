/*
 * stm32f446xx_systick_driver.c
 *
 *  Created on: May 14, 2026
 *      Author: james
 */

#include "stm32f446xx_systick_driver.h"


volatile uint32_t g_ms_ticks = 0;

void systick_enable(uint32_t clk_speed)
{
	// 1. set reload value
	uint32_t reload = clk_speed / 1000;
	SYSTICK->RVR = reload;

	// 2. clear current value
	SYSTICK->CVR = 0;

	// 3. configure control and status
	SYSTICK->CSR |= 0b111;

	g_ms_ticks = 0;

}

void SysTick_Handler(void)
{
	g_ms_ticks++;
}


void delay(uint32_t millis)
{
	uint32_t start_ms = g_ms_ticks;
	while ((g_ms_ticks - start_ms) < millis);
}


uint32_t millis(void)
{
	return g_ms_ticks;
}


void timer_init(void)
{
	TIM2_CLK_EN();

	TIM2->CR1 &= ~(1 << 0);

	TIM2->ARR = 0xFFFFFFFF;
	TIM2->PSC = 15;
	TIM2->CNT = 0;

	TIM2->EGR |= (1 << 0);

	TIM2->CR1 |= (1 << 0);
}

uint32_t micros()
{
	return TIM2->CNT;
}

void delay_us(uint32_t us)
{
	uint32_t start_us = micros();

	while ((micros() - start_us) < us);
}
