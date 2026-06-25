
add_executable(i2c_master 
    main.c
    ${PROJECT_ROOT}/system/startup_stm32f446xx.s
    $<TARGET_OBJECTS:stm32_system>
)

target_link_libraries(i2c_master PRIVATE 
    stm32_drivers
)

add_custom_command(TARGET i2c_master POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:i2c_master> i2c_master.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:i2c_master> i2c_master.hex
)