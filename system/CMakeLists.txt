
add_library(cmsis INTERFACE)

target_include_directories(cmsis INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS/Device/ST/STM32F4xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS/Include
)

target_compile_definitions(cmsis INTERFACE 
    STM32F446xx
)

add_library(stm32_system OBJECT 
    system_src/system_stm32f4xx.c
    system_src/syscalls.c 
    system_src/sysmem.c
)

target_link_libraries(stm32_system PRIVATE 
    cmsis
)