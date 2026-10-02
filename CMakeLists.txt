cmake_minimum_required(VERSION 3.16)

project(TempAlarm LANGUAGES C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -x assembler-with-cpp")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(MCU "STM32F103xB")
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/STM32F103XX_FLASH.ld")
set(STARTUP_FILE "${CMAKE_SOURCE_DIR}/Core/Startup/startup_stm32f103xb.s")

include_directories(
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
    ${CMAKE_SOURCE_DIR}/Drivers/STM32F1xx_HAL_Driver/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/FreeRTOS/include
    ${CMAKE_SOURCE_DIR}/Drivers/FreeRTOS/portable/GCC/ARM_CM3
    ${CMAKE_SOURCE_DIR}/Drivers/BH1750
    ${CMAKE_SOURCE_DIR}/Drivers/DHT11
    ${CMAKE_SOURCE_DIR}/Drivers/ST7735S
)

file(GLOB CORE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/Core/Src/*.c
)

file(GLOB DRIVER_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/Drivers/BH1750/*.c
    ${CMAKE_SOURCE_DIR}/Drivers/DHT11/*.c
    ${CMAKE_SOURCE_DIR}/Drivers/ST7735S/*.c
    ${CMAKE_SOURCE_DIR}/Drivers/FreeRTOS/src/*.c
    ${CMAKE_SOURCE_DIR}/Drivers/FreeRTOS/portable/MemMang/*.c
    ${CMAKE_SOURCE_DIR}/Drivers/FreeRTOS/portable/GCC/ARM_CM3/*.c
    ${CMAKE_SOURCE_DIR}/Drivers/STM32F1xx_HAL_Driver/Src/*.c
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Source/*.c
)

set(PROJECT_SOURCES
    ${CORE_SOURCES}
    ${DRIVER_SOURCES}
    ${STARTUP_FILE}
)

add_executable(TempAlarm ${PROJECT_SOURCES})

target_compile_options(TempAlarm PRIVATE
    -mcpu=cortex-m3
    -mthumb
    -ffunction-sections
    -fdata-sections
    -Wall
    -Wextra
    -Wpedantic
)

target_compile_definitions(TempAlarm PRIVATE
    STM32F103xB
    USE_HAL_DRIVER
)

target_link_options(TempAlarm PRIVATE
    -T${LINKER_SCRIPT}
    -Wl,-Map=${CMAKE_BINARY_DIR}/TempAlarm.map
    -Wl,--gc-sections
    -nostartfiles
)

set_target_properties(TempAlarm PROPERTIES
    OUTPUT_NAME "TempAlarm"
    SUFFIX ".elf"
)

add_custom_command(TARGET TempAlarm POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:TempAlarm> ${CMAKE_BINARY_DIR}/TempAlarm.bin
    COMMENT "Generating binary image"
)
