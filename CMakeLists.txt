cmake_minimum_required(VERSION 3.22)

project(stm32_peripheral_drivers C ASM)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(system)
add_subdirectory(peripheral_drivers)
add_subdirectory(examples/i2c_master)
add_subdirectory(examples/spi_master)
