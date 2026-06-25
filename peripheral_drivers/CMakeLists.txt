
add_library(stm32_drivers STATIC
    src/stm32f446xx_gpio_driver.c
    src/stm32f446xx_uart_driver.c
    src/stm32f446xx_i2c_driver.c
    src/stm32f446xx_systick_driver.c
    src/stm32f446xx_spi_driver.c
    src/stm32f446xx_rcc_driver.c
)

target_include_directories(stm32_drivers PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

