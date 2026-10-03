# Copyright (c) 2026 何少华 (LIMUHEshua). All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# 说明：本文件骨架由 STM32CubeMX 生成，业务逻辑与修改部分
#       版权归何少华所有，采用 Apache License 2.0 授权。

cmake_minimum_required(VERSION 3.22)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_PROJECT_NAME TempAlarm)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

enable_language(C ASM)

set(FREERTOS_DIR ${CMAKE_SOURCE_DIR}/Drivers/FreeRTOS)

add_executable(${PROJECT_NAME}.elf
    # ---- 启动文件（必须）----
    ${CMAKE_SOURCE_DIR}/Core/Startup/startup_stm32f103xb.s

    # ---- 用户代码 ----
    ${CMAKE_SOURCE_DIR}/Core/Src/main.c
    ${CMAKE_SOURCE_DIR}/Core/Src/gpio.c
    ${CMAKE_SOURCE_DIR}/Core/Src/stm32f1xx_it.c
    ${CMAKE_SOURCE_DIR}/Core/Src/stm32f1xx_hal_msp.c
    ${CMAKE_SOURCE_DIR}/Core/Src/syscalls.c
    ${CMAKE_SOURCE_DIR}/Core/Src/sysmem.c

    # ---- 自写驱动 ----
    ${CMAKE_SOURCE_DIR}/Drivers/DHT11/dht11.c
    ${CMAKE_SOURCE_DIR}/Drivers/ST7735S/st7735s.c
    ${CMAKE_SOURCE_DIR}/Drivers/BH1750/bh1750.c

    # ---- FreeRTOS 内核 ----
    ${FREERTOS_DIR}/src/croutine.c
    ${FREERTOS_DIR}/src/event_groups.c
    ${FREERTOS_DIR}/src/list.c
    ${FREERTOS_DIR}/src/queue.c
    ${FREERTOS_DIR}/src/stream_buffer.c
    ${FREERTOS_DIR}/src/tasks.c
    ${FREERTOS_DIR}/src/timers.c
    ${FREERTOS_DIR}/portable/MemMang/heap_4.c
    ${FREERTOS_DIR}/portable/GCC/ARM_CM3/port.c
)

add_subdirectory(cmake/stm32cubemx)

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/DHT11
    ${CMAKE_SOURCE_DIR}/Drivers/ST7735S
    ${CMAKE_SOURCE_DIR}/Drivers/BH1750
    ${FREERTOS_DIR}/include
    ${FREERTOS_DIR}/portable/GCC/ARM_CM3
    ${FREERTOS_DIR}
    ${CMAKE_SOURCE_DIR}/Drivers/STM32F1xx_HAL_Driver/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
)

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE
    USE_HAL_DRIVER
    STM32F103xB
)

# ---- 链接脚本 + 链接选项 ----
target_link_options(${PROJECT_NAME}.elf PRIVATE
    -T${CMAKE_SOURCE_DIR}/STM32F103XX_FLASH.ld
    -Wl,-Map=${PROJECT_NAME}.map
    --specs=nano.specs
    -Wl,--print-memory-usage
)

list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

target_link_libraries(${PROJECT_NAME}.elf
    stm32cubemx
)