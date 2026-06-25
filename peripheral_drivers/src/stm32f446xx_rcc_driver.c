#include "stm32f446xx.h"


static const uint32_t AHB_PrescTable[] = {2, 4, 8, 16, 64, 128, 256, 512};
static const uint32_t APB_PrescTable[] = {2, 4, 8, 16};

uint32_t RCC_GetSysClk(void)
{
    uint32_t clk = (RCC->CFGR >> 2) & 0x3;

    if (clk == 0)
    {
        // clock is HSI
        clk = HSI_CLK_FREQ;
    } else if (clk == 1)
    {
        // Clock is HSE
        clk = HSE_CLK_FREQ;
    } else if (clk == 2)
    {
        // not using PLL clock
    }

    return clk;
}

uint32_t RCC_GetHClk(void)
{
    uint8_t AHB_presc = ((RCC->CFGR >> 4) & 0x7);

    uint32_t sysclk = RCC_GetSysClk();

    if ((RCC->CFGR >> 7 & 0x1))
    {
        // AHB Prescaler is not 1
        sysclk /= AHB_PrescTable[AHB_presc];
    }

    return sysclk;
}

uint32_t RCC_GetPClk1(void)
{
    uint8_t APB1_presc = ((RCC->CFGR >> 10) & 0x3);

    uint32_t hclk = RCC_GetHClk();

    if ((RCC->CFGR >> 12 & 0x1))
    {
        // APB1 presc is not 1
        hclk /= APB_PrescTable[APB1_presc];
    }

    return hclk;
}
uint32_t RCC_GetPClk2(void)
{
    uint8_t APB2_presc = ((RCC->CFGR >> 13) & 0x3);

    uint32_t hclk = RCC_GetHClk();

    if ((RCC->CFGR >> 15 & 0x1))
    {
        // APB1 presc is not 1
        hclk /= APB_PrescTable[APB2_presc];
    }

    return hclk;
}