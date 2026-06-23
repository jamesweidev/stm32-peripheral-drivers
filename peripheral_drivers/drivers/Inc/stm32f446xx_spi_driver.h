#ifndef INC_STM32F446XX_SPI_DRIVER_H_
#define INC_STM32F446XX_SPI_DRIVER_H_

#include "stm32f446xx.h"
#include <stdbool.h>

typedef struct
{
	uint8_t SPI_DeviceMode;		// Master or slave
	uint8_t SPI_BusConfig;		// Duplex
	uint8_t SPI_SclkSpeed;		// clock speed
	uint8_t SPI_DFF;			// 8 or 16 bit register
	uint8_t SPI_CPOL;			// clock polarity, resting high or low
	uint8_t SPI_CPHA;			// clock phase, read on leading or trailing edge
	uint8_t SPI_SSM;			// slave selection
} SPI_Config_t;

typedef struct
{
	SPI_Reg_t* pSPIx;
	SPI_Config_t SPIConfig;
	uint8_t* pTxBuf;
	uint8_t* pRxBuf;
	uint32_t TxLen;
	uint32_t RxLen;
	uint8_t TxState;
	uint8_t RxState;
} SPI_Handle_t;


/*
 * @SPI_DeviceMode
 */
#define SPI_DEVICE_MODE_SLAVE		0
#define SPI_DEVICE_MODE_MASTER		1

/*
 * @SPI_BusConfig
 */
#define SPI_BUS_CONFIG_FD			0
#define SPI_BUS_CONFIG_HD			1
#define SPI_BUS_CONFIG_SPRX			2

/*
 * @SPI_SclkSpeed
 */
#define SPI_CLK_SPD_DIV2			0
#define SPI_CLK_SPD_DIV4			1
#define SPI_CLK_SPD_DIV8			2
#define SPI_CLK_SPD_DIV16			3
#define SPI_CLK_SPD_DIV32			4
#define SPI_CLK_SPD_DIV64			5
#define SPI_CLK_SPD_DIV128			6
#define SPI_CLK_SPD_DIV256			7

/*
 * @SPI_DFF
 */
#define SPI_DFF_8					0
#define SPI_DFF_16					1

/*
 * @SPI_SSM
 */
#define SPI_SSM_HW 					0
#define SPI_SSM_SW 					1

/*
 * @SPI_CPOL
 */
#define SPI_CPOL_LOW				0
#define SPI_CPOL_HIGH				1

/*
 * @SPI_CPHA
 */
#define SPI_CPHA_LOW				0
#define SPI_CPHA_HIGH				1

/*
 * @SPI_State
 */
#define SPI_STATE_FREE				0
#define SPI_STATE_BUSY				1

/*
 * SPI Application Events
 */
#define SPI_EVENT_TX_COMPLETE		0
#define SPI_EVENT_RX_COMPLETE		1
#define SPI_EVENT_OVR_ERR			2

void SPI_ClockControl(SPI_Reg_t* pSPIx, bool enable);

void SPI_Init(SPI_Handle_t* pHandle);
void SPI_Uninit(SPI_Reg_t* pSPIx);

void SPI_Toggle(SPI_Reg_t* pSPIx, bool enable);

void SPI_SendData(SPI_Reg_t* pSPIx, uint8_t* pTXBuf, uint32_t len);
void SPI_ReceiveData(SPI_Reg_t* pSPIx, uint8_t* pRXBuf, uint32_t len);

uint8_t SPI_SendDataIT(SPI_Handle_t* pHandle, uint8_t* pTXBuf, uint32_t len);
uint8_t SPI_ReceiveDataIT(SPI_Handle_t* pHandle, uint8_t* pRXBuf, uint32_t len);

void SPI_IRQConfig(uint8_t IRQNumber, uint8_t enable);
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority);
void SPI_IRQHandling(SPI_Handle_t* pHandle);

void SPI_ApplicationEventCallback(SPI_Handle_t* pHandle, uint8_t event_type);

#endif /* INC_STM32F446XX_SPI_DRIVER_H_ */
