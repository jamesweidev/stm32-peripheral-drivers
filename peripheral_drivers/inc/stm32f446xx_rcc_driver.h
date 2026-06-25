#ifndef INC_STM32F446XX_RCC_DRIVER_H_
#define INC_STM32F446XX_RCC_DRIVER_H_

#define HSI_CLK_FREQ            16000000
#define HSE_CLK_FREQ            8000000


uint32_t RCC_GetSysClk(void);
uint32_t RCC_GetHClk(void);
uint32_t RCC_GetPClk1(void);
uint32_t RCC_GetPClk2(void);


#endif