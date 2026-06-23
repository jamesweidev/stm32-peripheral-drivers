
// Example LED blinker

#include "stm32f446xx.h"

#define LED_PIN_NUM 5

int main(void)
{

	GPIO_Handle_t GPIO_led;
	GPIO_Config_t led_config;

	// Configure PA5 as a low speed, push pull output with no internal pull up/pull down
	led_config.GPIO_PinSpeed = GPIO_OSPEED_LOW;
	led_config.GPIO_PinNumber = LED_PIN_NUM;
	led_config.GPIO_PinMode = GPIO_PIN_MODE_OUT;
	led_config.GPIO_PinOPType = GPIO_OT_PP;
	led_config.GPIO_PinPuPdControl = GPIO_PUPD_NONE;
	led_config.GPIO_PinAltFuncMode = 0;

	GPIO_led.pGPIOx = GPIOA;
	GPIO_led.GPIOConfig = led_config;

	GPIO_ClockControl(GPIO_led.pGPIOx, ENABLE);
	GPIO_Init(&GPIO_led);

	// Enable systick with 16MHz system clock
	systick_enable(16000000);
	while (1)
	{
		// Drive PA5 high, wait 1 second, then drive PA5 low and wait 1 second
		GPIO_WriteToOutputPin(GPIO_led.pGPIOx, LED_PIN_NUM, ENABLE);
		delay(1000);
		GPIO_WriteToOutputPin(GPIO_led.pGPIOx, LED_PIN_NUM, DISABLE);
		delay(1000);
	}

	return 0;
}
