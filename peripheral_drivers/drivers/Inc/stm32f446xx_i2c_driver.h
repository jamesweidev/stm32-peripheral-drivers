/*
 * stm32f446xx_i2c_driver.h
 *
 *  Created on: May 18, 2026
 *      Author: james
 */

#ifndef INC_STM32F446XX_I2C_DRIVER_H_
#define INC_STM32F446XX_I2C_DRIVER_H_


#include "stm32f446xx.h"

typedef struct
{
	uint8_t MODE;			// SM: 0, FM: 1
	uint8_t ACKCONFIG;		// ACK: 1, NOACK: 0
	uint8_t DEVICEADDR;

} I2C_Config_t;

#define I2C_MODE_SM 		0
#define I2C_MODE_FM 		1
#define I2C_ACKCONFIG_NOACK	0
#define I2C_ACKCONFIG_ACK	1


// 1. CR1 PE, ENGC, NOSTRETCH, ACK, OAR1,
// to send: move data into DR, loop until TxE (Tx Empty) is 1, send another byte.
// Have a NACK interrupt? or maybe each loop check ACK value and quit if NACK?
 typedef struct
 {
	 I2C_Reg_t* pI2Cx;
	 I2C_Config_t I2CConfig;
 } I2C_Handle_t;


 void I2C_ClkControl(I2C_Reg_t* pI2Cx, uint8_t enable);

void I2C_Init(I2C_Handle_t* Handle);

void I2C_MasterSendData(uint8_t* data, uint32_t len, uint8_t target_addr, I2C_Reg_t* pI2Cx);
void I2C_MasterReceiveData(uint8_t* data, uint32_t len, uint8_t target_addr, I2C_Reg_t* pI2Cx);

void I2C_MasterSendDataIT(uint8_t* data, uint32_t len, uint8_t target_addr, I2C_Reg_t* pI2Cx);
void I2C_MasterReceiveDataIT(uint8_t* data, uint32_t len, uint8_t target_addr, I2C_Reg_t* pI2Cx);










#endif /* INC_STM32F446XX_I2C_DRIVER_H_ */
