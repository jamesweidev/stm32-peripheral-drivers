# STM32 Peripheral Drivers

Bare metal peripheral drivers for GPIO, UART, SPI, I2C, interrupts, and SysTick for the stm32f446xx family nucleo boards. It's written in C and is meant for learning bare metal embedded.

Example usage at peripheral_drivers/Src/example_usage.c

Driver include and source code at peripheral_drivers/drivers/

## Build
From root:
1. cmake -S . -B build -G Ninja \\
  -DCMAKE_TOOLCHAIN_FILE=system/cmake/gcc-arm-none-eabi.cmake

2. cmake --build build

## Flash
From root: 

openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
  -c "program build/examples/i2c_master/i2c_master.elf verify reset exit"