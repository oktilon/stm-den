cmake_minimum_required(VERSION 3.4)
project(Test2)

set(PERIPH_LIB lib/periph/)
include_directories(
        ${PERIPH_LIB}/CMSIS/Device/ST/STM32F4xx/
        ${PERIPH_LIB}/CMSIS/Device/ST/STM32F4xx/Include/
        ${PERIPH_LIB}/STM32F4xx_StdPeriph_Driver/inc/
        ${PERIPH_LIB}/CMSIS/Device/ST/STM32F4xx/Source/Templates/arm
)
set(PERIPH_LIB_SRC ${PERIPH_LIB}/STM32F4xx_StdPeriph_Driver/src/)

set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_LINKER "arm-none-eabi-ld")
set(CMAKE_OBJCOPY "arm-none-eabi-objcopy")

set(LINK_SCRIPT STM32F407VG_FLASH_BL.ld)

set(STARTUP system_stm32f4xx.c startup_stm32f40xx.s)
set(STARTUP ${STARTUP}
        ${PERIPH_LIB_SRC}/stm32f4xx_rcc.c
        ${PERIPH_LIB_SRC}/stm32f4xx_tim.c
        ${PERIPH_LIB_SRC}/stm32f4xx_gpio.c
#       ${PERIPH_LIB_SRC}/stm32f4xx_exti.c
        ${PERIPH_LIB_SRC}/stm32f4xx_spi.c
        ${PERIPH_LIB_SRC}/stm32f4xx_dma.c
#       ${PERIPH_LIB_SRC}/stm32f4xx_usart.c
        lib/tinystd/itoa.c
        lib/ILI9341/delay.c
        lib/ILI9341/core.c
        lib/ILI9341/dma.c
        lib/ILI9341/graph.c
        lib/ILI9341/text.c
        lib/encoder/encoder.c
#                lib/usart/usart.c
#                test_suite/fps_counter.c
#                test_suite/fill.c
#                test_suite/primitives.c
        )

set(DEBUG_FLAGS "-O9")
#set(DEBUG_FLAGS "-O0 -g")

set(MCU_FLAGS "-march=armv7-m -mcpu=cortex-m3 -mthumb")
set(CMAKE_C_FLAGS "${MCU_FLAGS} -fno-common -fno-builtin -nostdlib -ffreestanding -pedantic -std=c99 -Wall ${DEBUG_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
add_definitions(-DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000)
set(CMAKE_EXE_LINKER_FLAGS "${DEBUG_FLAGS} -T${LINK_SCRIPT}")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

add_executable(${PROJECT_NAME}.elf main.c ${STARTUP})

add_custom_command(DEPENDS ${PROJECT_NAME}.elf OUTPUT ${PROJECT_NAME}.hex COMMAND ${CMAKE_OBJCOPY} -Oihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex)
add_custom_command(DEPENDS ${PROJECT_NAME}.elf OUTPUT ${PROJECT_NAME}.bin COMMAND ${CMAKE_OBJCOPY} -Obinary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin)

add_custom_target(generate_bin ALL DEPENDS ${PROJECT_NAME}.bin)
add_custom_target(generate_hex ALL DEPENDS ${PROJECT_NAME}.hex)

add_custom_target(write0 stm32flash -w ${PROJECT_NAME}.bin -g 0x0 /dev/ttyUSB0 DEPENDS ${PROJECT_NAME}.bin)
add_custom_target(write1 stm32flash -w ${PROJECT_NAME}.bin -g 0x0 /dev/ttyUSB1 DEPENDS ${PROJECT_NAME}.bin)
add_custom_target(write2 stm32flash -w ${PROJECT_NAME}.bin -g 0x0 /dev/ttyUSB2 DEPENDS ${PROJECT_NAME}.bin)
