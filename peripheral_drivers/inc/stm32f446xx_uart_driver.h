#ifndef INC_STM32F446XX_USART_DRIVER_H_
#define INC_STM32F446XX_USART_DRIVER_H_

#include "stm32f446xx.h"

typedef struct
{
	uint8_t mode;
	uint32_t baud;
	uint8_t word_len;
	uint8_t stop_bits;
	uint8_t flow_ctrl;
	uint8_t parity;
	uint8_t over8; // samples per usart tick
} USART_Config_t;

typedef struct
{
	USART_Reg_t* pUSARTx;
	USART_Config_t config;
	uint8_t* pTxBuf;
	uint8_t* pRxBuf;
	uint32_t len;
	uint8_t TxState;
	uint8_t RxState;
} USART_Handle_t;

#define USART_MODE_TX			0
#define USART_MODE_RX			1
#define USART_MODE_TX_RX		2

#define USART_WORD_LEN_8		0
#define USART_WORD_LEN_9		1

#define USART_STOP_BITS_1		0
#define USART_STOP_BITS_0_5		1
#define USART_STOP_BITS_2		2
#define USART_STOP_BITS_1_5		3

#define USART_FLOW_CTRL_NONE	0
#define USART_FLOW_CTRL_CTS		1
#define USART_FLOW_CTRL_RTS		2
#define USART_FLOW_CTRL_CTS_RTS	3

#define USART_PARITY_OFF		0
#define USART_PARITY_ODD		1
#define USART_PARITY_EVEN		2

#define USART_OVER8_8			1
#define USART_OVER8_16			0

#define USART_STATE_FREE		0
#define USART_STATE_RX_BUSY		1
#define USART_STATE_TX_BUSY		2

void USART_ClkControl(USART_Reg_t* pUSARTx, uint8_t enable);

void USART_Init(USART_Handle_t* pHandle);
void USART_Uninit(USART_Handle_t* pHandle);


void USART_SendData(USART_Handle_t* pHandle, uint8_t* data, uint32_t len);
void USART_ReceiveData(USART_Handle_t* pHandle, uint8_t* data, uint32_t len);

uint8_t USART_SendDataIT(USART_Handle_t* pHandle, uint8_t* data, uint32_t len);
uint8_t USART_ReceiveDataIT(USART_Handle_t* pHandle, uint8_t* data, uint32_t len);

void USART_IRQHandling(USART_Handle_t* pHandle);

void printf_init(void);



#endif /* INC_STM32F446XX_USART_DRIVER_H_ */
