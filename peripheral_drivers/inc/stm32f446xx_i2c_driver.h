#ifndef INC_STM32F446XX_I2C_DRIVER_H_
#define INC_STM32F446XX_I2C_DRIVER_H_


#include "stm32f446xx.h"

typedef struct
{
	uint32_t SCLSpeed;
	uint8_t AckConfig;
	uint8_t DeviceAddr;
	uint8_t FMDutyCycle;
} I2C_Config_t;


 typedef struct
 {
	 I2C_Reg_t* pI2Cx;
	 I2C_Config_t I2CConfig;
	 uint8_t* pTxBuf;
	 uint8_t* pRxBuf;
	 uint32_t TxLen;
	 uint32_t RxLen;
	 uint32_t RxSize;
	 uint8_t State;
	 uint8_t Addr;
	 uint8_t RepeatedStart;
 } I2C_Handle_t;

 /*
  * @I2C_SCLSPEED
  */
#define I2C_SCL_SPEED_SM 		100000
#define I2C_SCL_SPEED_FM4K		400000
#define I2C_SCL_SPEED_FM2K		200000

 /*
  * @I2C_ACK
  */
#define I2C_ACKCONFIG_NOACK		0
#define I2C_ACKCONFIG_ACK		1


/*
 * @I2C_FMDutyCycle
 */
#define I2C_FMDUTYCYCLE_2		0
#define I2C_FMDUTYCYCLE_16_9	1

 /*
  * @I2C_States
  */
#define I2C_STATE_FREE			0
#define I2C_STATE_TX_BUSY		1
#define I2C_STATE_RX_BUSY		2

 /*
  * @I2C_REPEATED_START
  */
#define I2C_REPEATED_START_OFF 	0
#define I2C_REPEATED_START_ON 	1

/*
 * @I2C_EVENT_TYPE
 */
#define I2C_EVENT_TX_COMPLETE	0
#define I2C_EVENT_RX_COMPLETE	1
#define I2C_EVENT_SLAVE_STOP	2
#define I2C_EVENT_DATA_REQ		3
#define I2C_EVENT_DATA_RECEIVED	4
#define I2C_ERROR_BERR			5
#define I2C_ERROR_ARLO			6
#define I2C_ERROR_AF			7
#define I2C_ERROR_TIMEOUT		8
#define I2C_ERROR_OVR			9

void I2C_ClkControl(I2C_Reg_t* pI2Cx, uint8_t enable);

void I2C_Init(I2C_Handle_t* Handle);

void I2C_MasterSendData(uint8_t* data, uint32_t len, uint8_t target_addr, I2C_Reg_t* pI2Cx);
void I2C_MasterReceiveData(uint8_t* data, uint32_t len, uint8_t target_addr, I2C_Reg_t* pI2Cx);

uint8_t I2C_MasterSendDataIT(I2C_Handle_t* pHandle, uint8_t* data, uint32_t len, uint8_t target_addr, uint8_t sr);
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t* pHandle, uint8_t* data, uint32_t len, uint8_t target_addr, uint8_t sr);

void I2C_IRQConfig(uint8_t IRQNumber, uint8_t enable);
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority);
void I2C_IRQHandling(I2C_Handle_t* pHandle);
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle);

void I2C_ApplicationEventCallback(I2C_Handle_t* pHandle, uint8_t event_type);




#endif /* INC_STM32F446XX_I2C_DRIVER_H_ */
