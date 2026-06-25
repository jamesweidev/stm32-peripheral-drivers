#ifndef INC_STM32F446XX_H_
#define INC_STM32F446XX_H_

#include <stdint.h>
#include <stddef.h>

#define FLASH_BASE_ADDR				0x08000000UL
#define SRAM1_BASE_ADDR				0x20000000UL
#define SRAM2_BASE_ADDR				0x2001C000UL
#define ROM_BASE_ADDR				0x1FFF0000UL

#define PERIPH_BASE_ADDR			0x40000000UL
#define APB1_BASE_ADDR				PERIPH_BASE_ADDR
#define APB2_BASE_ADDR				0x40010000UL
#define AHB1_BASE_ADDR				0x40020000UL
#define AHB2_BASE_ADDR				0x50000000UL

#define RCC_BASE_ADDR				(uint32_t*) (AHB1_BASE_ADDR + 0x3800)
#define RCC							((RCC_Reg_t*) RCC_BASE_ADDR)

#define EXTI_BASE_ADDR				((uint32_t*) (APB2_BASE_ADDR + 0x3C00))
#define EXTI						((EXTI_Reg_t*) EXTI_BASE_ADDR)

#define SYSCFG_BASE_ADDR			((uint32_t*) (APB2_BASE_ADDR + 0x3800))
#define SYSCFG						((SYSCFG_Reg_t*) SYSCFG_BASE_ADDR)

#define GPIOA_BASE_ADDR				(AHB1_BASE_ADDR + 0x0000)
#define GPIOB_BASE_ADDR				(AHB1_BASE_ADDR + 0x0400)
#define GPIOC_BASE_ADDR				(AHB1_BASE_ADDR + 0x0800)
#define GPIOD_BASE_ADDR				(AHB1_BASE_ADDR + 0x0C00)
#define GPIOE_BASE_ADDR				(AHB1_BASE_ADDR + 0x1000)
#define GPIOF_BASE_ADDR				(AHB1_BASE_ADDR + 0x1400)
#define GPIOG_BASE_ADDR				(AHB1_BASE_ADDR + 0x1800)
#define GPIOH_BASE_ADDR				(AHB1_BASE_ADDR + 0x1C00)

#define GPIOA						(GPIO_Reg_t*) GPIOA_BASE_ADDR
#define GPIOB						(GPIO_Reg_t*) GPIOB_BASE_ADDR
#define GPIOC						(GPIO_Reg_t*) GPIOC_BASE_ADDR
#define GPIOD						(GPIO_Reg_t*) GPIOD_BASE_ADDR
#define GPIOE						(GPIO_Reg_t*) GPIOE_BASE_ADDR
#define GPIOF						(GPIO_Reg_t*) GPIOF_BASE_ADDR
#define GPIOG						(GPIO_Reg_t*) GPIOG_BASE_ADDR
#define GPIOH						(GPIO_Reg_t*) GPIOH_BASE_ADDR

#define SPI1_BASE_ADDR				(APB2_BASE_ADDR + 0x3000)
#define SPI4_BASE_ADDR				(APB2_BASE_ADDR + 0x3400)
#define SPI2_BASE_ADDR				(APB1_BASE_ADDR + 0x3800)
#define SPI3_BASE_ADDR				(APB1_BASE_ADDR + 0x3C00)

#define SPI1						((SPI_Reg_t*) SPI1_BASE_ADDR)
#define SPI2						((SPI_Reg_t*) SPI2_BASE_ADDR)
#define SPI3						((SPI_Reg_t*) SPI3_BASE_ADDR)
#define SPI4						((SPI_Reg_t*) SPI4_BASE_ADDR)




typedef struct
{
	volatile uint32_t MODER;			//
	volatile uint32_t OTYPER;
	volatile uint32_t OSPEEDR;
	volatile uint32_t PUPDR;
	volatile uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
	volatile uint32_t LCKR;
	volatile uint32_t AFRL;
	volatile uint32_t AFRH;
} GPIO_Reg_t;

typedef struct
{
    volatile uint32_t CR;          	// 0x00
    volatile uint32_t PLLCFGR;     	// 0x04
    volatile uint32_t CFGR;       	// 0x08
    volatile uint32_t CIR;        	// 0x0C
    volatile uint32_t AHB1RSTR;    	// 0x10
    volatile uint32_t AHB2RSTR;    	// 0x14
    volatile uint32_t AHB3RSTR;    	// 0x18
    uint32_t RESERVED0;            	// 0x1C
    volatile uint32_t APB1RSTR;    	// 0x20
    volatile uint32_t APB2RSTR;    	// 0x24
    uint32_t RESERVED1[2];         	// 0x28, 0x2C
    volatile uint32_t AHB1ENR;     	// 0x30
    volatile uint32_t AHB2ENR;     	// 0x34
    volatile uint32_t AHB3ENR;     	// 0x38
    uint32_t RESERVED2;		    	// 0x3C
    volatile uint32_t APB1ENR;		// 0x40
    volatile uint32_t APB2ENR;		// 0x44
    uint32_t RESERVED3[2];			// 0x48, 0x4C
    volatile uint32_t AHB1LPENR;	// 0x50
    volatile uint32_t AHB2LPENR;	// 0x54
    volatile uint32_t AHB3LPENR;	// 0x58
    uint32_t RESERVED4;				// 0x5C
    volatile uint32_t APB1LPENR;	// 0x60
    volatile uint32_t APB2LPENR;	// 0x64
    uint32_t RESERVED5[2];			// 0x68, 0x6C
    volatile uint32_t BDCR;			// 0x70
    volatile uint32_t CSR;			// 0x74
    uint32_t RESERVED6[2];			// 0x78, 0x7C


} RCC_Reg_t;


typedef struct
{
	volatile uint32_t IMR;			// 0x00
	volatile uint32_t EMR;			// 0x04
	volatile uint32_t RTSR;			// 0x08
	volatile uint32_t FTSR;			// 0x0C
	volatile uint32_t SWIER;		// 0x10
	volatile uint32_t PR;			// 0x14
} EXTI_Reg_t;


typedef struct
{
	volatile uint32_t MEMRMP;		// 0x00
	volatile uint32_t PMC;			// 0x04
	volatile uint32_t EXTICR1;		// 0x08
	volatile uint32_t EXTICR2; 		// 0x0C
	volatile uint32_t EXTICR3;		// 0x10
	volatile uint32_t EXTICR4; 		// 0x14
	uint32_t SPACER_1[2];			// 0x18, 0x1C
	volatile uint32_t CMPCR;		// 0x20
	uint32_t SPACER_2[2];			// 0x24, 0x28
	volatile uint32_t CFGR; 		// 0x2C
} SYSCFG_Reg_t;



#define NVIC_BASE_ADDR 				0xE000E100
#define NVIC						((NVIC_Reg_t*) NVIC_BASE_ADDR)


typedef struct
{
	volatile uint32_t ISER[8]; 		// 0xE000E100 - 0xE000E11C
	uint32_t RESERVED_0[24];
	volatile uint32_t ICER[8];		// 0XE000E180 - 0xE000E19C
	uint32_t RESERVED_1[24];
	volatile uint32_t ISPR[8];		// 0XE000E200 - 0xE000E21C
	uint32_t RESERVED_2[24];
	volatile uint32_t ICPR[8];		// 0XE000E280 - 0xE000E29C
	uint32_t RESERVED_3[24];
	volatile uint32_t IABR[8];		// 0XE000E300 - 0xE000E31C
	uint32_t RESERVED_4[56];
	volatile uint32_t IPR[60];		// 0XE000E400 - 0xE000E4EF
} NVIC_Reg_t;


#define SYSTICK_BASE_ADDR			0xE000E010
#define SYSTICK						((SYSTICK_Reg_t*) SYSTICK_BASE_ADDR)

typedef struct
{
	volatile uint32_t CSR;			// 0xE000E010 - Control and Status
	volatile uint32_t RVR;			// 0xE000E014 - Reload Value
	volatile uint32_t CVR;			// 0xE000E018 - Current Value
	volatile uint32_t CALIB;		// 0xE000E01C - Calibration Value
} SYSTICK_Reg_t;

typedef struct
{
	volatile uint32_t CR1;			//
	volatile uint32_t CR2;			//
	volatile uint32_t SR;			//
	volatile uint32_t DR;			//
	volatile uint32_t CRCPR;		//
	volatile uint32_t RXCRCR;		//
	volatile uint32_t TXCRCR;		//
	volatile uint32_t I2SCFGR; 		//
	volatile uint32_t I2SPR;		//
} SPI_Reg_t;

#define I2C1_BASE_ADDR				(APB1_BASE_ADDR + 0x5400)
#define I2C2_BASE_ADDR				(APB1_BASE_ADDR + 0x5800)
#define I2C3_BASE_ADDR				(APB1_BASE_ADDR + 0x5C00)

#define I2C1 						((I2C_Reg_t*) I2C1_BASE_ADDR)
#define I2C2 						((I2C_Reg_t*) I2C2_BASE_ADDR)
#define I2C3 						((I2C_Reg_t*) I2C3_BASE_ADDR)

typedef struct
{
	volatile uint32_t CR1;			// Control reg 1
	volatile uint32_t CR2; 			// Control reg 2
	volatile uint32_t OAR1; 		// Own address reg 1
	volatile uint32_t OAR2; 		// Own address reg 2
	volatile uint32_t DR; 			// Data reg
	volatile uint32_t SR1; 			// Status reg 1
	volatile uint32_t SR2; 			// Status reg 2
	volatile uint32_t CCR; 			// Clock control reg
	volatile uint32_t TRISE; 		// Trise
	volatile uint32_t FLTR; 		// FLTR
} I2C_Reg_t;

#define I2C1_CLK_EN() 				((RCC->APB1ENR) |= (1 << 21))
#define I2C2_CLK_EN() 				((RCC->APB1ENR) |= (1 << 22))
#define I2C3_CLK_EN() 				((RCC->APB1ENR) |= (1 << 23))


#define USART1_BASE_ADDR			(APB2_BASE_ADDR + 0x1000)
#define USART2_BASE_ADDR			(APB1_BASE_ADDR + 0x4400)
#define USART3_BASE_ADDR			(APB1_BASE_ADDR + 0x4800)
#define USART4_BASE_ADDR			(APB1_BASE_ADDR + 0x4C00)
#define USART5_BASE_ADDR			(APB1_BASE_ADDR + 0x5000)
#define USART6_BASE_ADDR			(APB2_BASE_ADDR + 0x1400)

#define USART1						((USART_Reg_t*) USART1_BASE_ADDR)
#define USART2						((USART_Reg_t*) USART2_BASE_ADDR)
#define USART3						((USART_Reg_t*) USART3_BASE_ADDR)
#define USART4						((USART_Reg_t*) USART4_BASE_ADDR)
#define USART5						((USART_Reg_t*) USART5_BASE_ADDR)
#define USART6						((USART_Reg_t*) USART6_BASE_ADDR)

typedef struct
{
	volatile uint32_t SR;
	volatile uint32_t DR;
	volatile uint32_t BRR;
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t CR3;
	volatile uint32_t GTPR;
} USART_Reg_t;

#define USART1_CLK_EN()				((RCC->APB2ENR) |= (1 << 4))
#define USART2_CLK_EN()				((RCC->APB1ENR) |= (1 << 17))
#define USART3_CLK_EN()				((RCC->APB1ENR) |= (1 << 18))
#define USART4_CLK_EN()				((RCC->APB1ENR) |= (1 << 19))
#define USART5_CLK_EN()				((RCC->APB1ENR) |= (1 << 20))
#define USART6_CLK_EN()				((RCC->APB2ENR) |= (1 << 5))

#define USART1_CLK_STOP()			((RCC->APB2ENR) &= ~(1 << 4))
#define USART2_CLK_STOP()			((RCC->APB1ENR) &= ~(1 << 17))
#define USART3_CLK_STOP()			((RCC->APB1ENR) &= ~(1 << 18))
#define USART4_CLK_STOP()			((RCC->APB1ENR) &= ~(1 << 19))
#define USART5_CLK_STOP()			((RCC->APB1ENR) &= ~(1 << 20))
#define USART6_CLK_STOP()			((RCC->APB2ENR) &= ~(1 << 5))


// TIMERS
#define TIM1_BASE_ADDR				(APB2_BASE_ADDR + 0x0000)
#define TIM2_BASE_ADDR				(APB1_BASE_ADDR + 0x0000)
#define TIM3_BASE_ADDR				(APB1_BASE_ADDR + 0x0400)
#define TIM4_BASE_ADDR				(APB1_BASE_ADDR + 0x0800)
#define TIM5_BASE_ADDR				(APB1_BASE_ADDR + 0x0C00)
#define TIM6_BASE_ADDR				(APB1_BASE_ADDR + 0x1000)
#define TIM7_BASE_ADDR				(APB1_BASE_ADDR + 0x1400)
#define TIM8_BASE_ADDR				(APB2_BASE_ADDR + 0x0400)
#define TIM9_BASE_ADDR				(APB2_BASE_ADDR + 0x4000)
#define TIM10_BASE_ADDR				(APB2_BASE_ADDR + 0x4400)
#define TIM11_BASE_ADDR				(APB2_BASE_ADDR + 0x4800)
#define TIM12_BASE_ADDR				(APB1_BASE_ADDR + 0x1800)
#define TIM13_BASE_ADDR				(APB1_BASE_ADDR + 0x1C00)
#define TIM14_BASE_ADDR				(APB1_BASE_ADDR + 0x2000)

typedef struct
{
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t SMCR;
	volatile uint32_t DIER;
	volatile uint32_t SR;
	volatile uint32_t EGR;
	volatile uint32_t CCMR1;
	volatile uint32_t CCMR2;
	volatile uint32_t CCER;
	volatile uint32_t CNT;
	volatile uint32_t PSC;
	volatile uint32_t ARR;
	uint32_t RESERVED1;
	volatile uint32_t CCR1;
	volatile uint32_t CCR2;
	volatile uint32_t CCR3;
	volatile uint32_t CCR4;
	uint32_t RESERVED2;
	volatile uint32_t DCR;
	volatile uint32_t DMAR;
	volatile uint32_t TIM2_OR;
	volatile uint32_t TIM5_OR;
} TIM_Reg_t;

#define TIM2 						((TIM_Reg_t *) TIM2_BASE_ADDR)
#define TIM3						((TIM_Reg_t *) TIM3_BASE_ADDR)
#define TIM4						((TIM_Reg_t *) TIM4_BASE_ADDR)

#define TIM2_CLK_EN()				((RCC->APB1ENR) |= (1 << 0))
#define TIM3_CLK_EN()				((RCC->APB1ENR) |= (1 << 1))
#define TIM4_CLK_EN()				((RCC->APB1ENR) |= (1 << 2))



#define IRQ_NO_EXTI0		6
#define IRQ_NO_EXTI1		7
#define IRQ_NO_EXTI2		8
#define IRQ_NO_EXTI3		9
#define IRQ_NO_EXTI4		10
#define IRQ_NO_EXTI9_5		23
#define IRQ_NO_EXTI15_10	40

#define IRQ_NO_SPI1			35
#define IRQ_NO_SPI2			36



// GPIO Pin Modes
#define GPIO_PIN_MODE_IN 		0
#define GPIO_PIN_MODE_OUT 		1
#define GPIO_PIN_MODE_AF 		2
#define GPIO_PIN_MODE_ANALOG 	3
#define GPIO_PIN_MODE_IT_RT 	4
#define GPIO_PIN_MODE_IT_FT 	5
#define GPIO_PIN_MODE_IT_RFT 	6

// GPIO Ouput Types
#define GPIO_OT_PP			0
#define GPIO_OT_OD			1

// GPIO Output Speed
#define GPIO_OSPEED_LOW		0
#define GPIO_OSPEED_MED		1
#define GPIO_OSPEED_FAST	2
#define GPIO_OSPEED_HIGH	3

// GPIO PuPd
#define GPIO_PUPD_NONE		0
#define GPIO_PUPD_PU		1
#define GPIO_PUPD_PD		2


// General macros
#define ENABLE			1
#define DISABLE			0


/*
 * Bit position definitions SPI_CR2
 */
#define SPI_CR2_TXEIE		7
#define SPI_CR2_RXNEIE		6
#define SPI_CR2_ERRIE		5
#define SPI_CR2_FRF			4
#define SPI_CR2_SSOE		2
#define SPI_CR2_TXDMAEN		1
#define SPI_CR2_RXDMAEN		0

/*
 * Bit position definitions SPI_SR
 */
#define SPI_SR_FRE			8
#define SPI_SR_BSY			7
#define SPI_SR_OVR			6
#define SPI_SR_MODF			5
#define SPI_SR_CRCERR		4
#define SPI_SR_UDR			3
#define SPI_SR_CHSIDE		2
#define SPI_SR_TXE			1
#define SPI_SR_RXNE			0

/*
 * Bit position definitions I2C_CR1
 */
#define I2C_CR1_SWRST		15
#define I2C_CR1_ALERT		13
#define I2C_CR1_PEC			12
#define I2C_CR1_POS			11
#define I2C_CR1_ACK			10
#define I2C_CR1_STOP		9
#define I2C_CR1_START		8
#define I2C_CR1_NOSTRETCH	7
#define I2C_CR1_ENGC		6
#define I2C_CR1_ENPEC		5
#define I2C_CR1_ENARP		4
#define I2C_CR1_SMBTYPE		3
#define I2C_CR1_SMBUS		1
#define I2C_CR1_PE			0

/*
 * Bit position definition I2C_CR2
 */
#define I2C_CR2_LAST		12
#define I2C_CR2_DMAEN		11
#define I2C_CR2_ITBUFEN		10
#define I2C_CR2_ITEVTEN		9
#define I2C_CR2_ITERREN		8
#define I2C_CR2_FREQ		0

/*
 * Bit position definitions I2C_SR1
 */
#define I2C_SR1_SMBALERT	15
#define I2C_SR1_TIMEOUT		14
#define I2C_SR1_PECERR		12
#define I2C_SR1_OVR			11
#define I2C_SR1_AF			10
#define I2C_SR1_ARLO		9
#define I2C_SR1_BERR		8
#define I2C_SR1_TXE			7
#define I2C_SR1_RXNE		6
#define I2C_SR1_STOPF		4
#define I2C_SR1_ADD10		3
#define I2C_SR1_BTF			2
#define I2C_SR1_ADDR		1
#define I2C_SR1_SB			0

/*
 * Bit position definitions I2C_SR2
 */
#define I2C_SR2_PEC			8
#define I2C_SR2_DUALF		7
#define I2C_SR2_SMBHOST		6
#define I2C_SR2_SMBDEFAULT	5
#define I2C_SR2_GENCALL		4
#define I2C_SR2_TRA			2
#define I2C_SR2_BUSY		1
#define I2C_SR2_MSL			0

/*
 * Bit position definitions USART_SR
 */
#define USART_SR_CTS		9
#define USART_SR_LBD		8
#define USART_SR_TXE		7
#define USART_SR_TC			6
#define USART_SR_RXNE		5
#define USART_SR_IDLE		4
#define USART_SR_ORE		3
#define USART_SR_NF			2
#define USART_SR_FE			1
#define USART_SR_PE			0

/*
 * Bit position definitions USART_CR1
 */
#define USART_CR1_OVER8		15
#define USART_CR1_UE		13
#define USART_CR1_M			12
#define USART_CR1_WAKE		11
#define USART_CR1_PCE		10
#define USART_CR1_PS		9
#define USART_CR1_PEIE		8
#define USART_CR1_TXEIE		7
#define USART_CR1_TCIE		6
#define USART_CR1_RXNEIE	5
#define USART_CR1_IDLEIE	4
#define USART_CR1_TE		3
#define USART_CR1_RE		2
#define USART_CR1_RWU		1
#define USART_CR1_SBK		0

/*
 * Bit position definitions USART_CR2
 */
#define USART_CR2_LINEN		14
#define USART_CR2_STOP		12
#define USART_CR2_CLKEN		11
#define USART_CR2_CPOL		10
#define USART_CR2_CPHA		9
#define USART_CR2_LBCL		8
#define USART_CR2_LBDIE		6
#define USART_CR2_LBDL		5
#define USART_CR2_ADD		0

/*
 * Bit position definitions USART_CR2
 */
#define USART_CR3_ONEBIT	11
#define USART_CR3_CTSIE		10
#define USART_CR3_CTSE		9
#define USART_CR3_RTSE		8
#define USART_CR3_DMAT		7
#define USART_CR3_DMART		6
#define USART_CR3_SCEN		5
#define USART_CR3_NACK		4
#define USART_CR3_HDSEL		3
#define USART_CR3_IRLP		2
#define USART_CR3_IREN		1
#define USART_CR3_EIE		0

#include "stm32f446xx_uart_driver.h"
#include "stm32f446xx_gpio_driver.h"
#include "stm32f446xx_systick_driver.h"
#include "stm32f446xx_spi_driver.h"
#include "stm32f446xx_i2c_driver.h"
#include "stm32f446xx_rcc_driver.h"

#endif /* INC_STM32F446XX_H_ */












