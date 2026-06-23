/*
 * lcd.h
 *
 *  Created on: May 25, 2026
 *      Author: james
 */

#ifndef LCD_H_
#define LCD_H_

#include "stm32f446xx.h"

#define LCD_GPIO_PORT		GPIOC

// PINS
#define LCD_PIN_SCL			0
#define LCD_PIN_SDA			1

#define LCD_DATA_WRITE		1
#define LCD_INST_WRITE		0

#define LCD_I2C_ADDR		0x27

void lcd_init(void);

void lcd_clear(void);
uint8_t lcd_write(uint8_t* data, uint8_t len);







#endif /* LCD_H_ */
