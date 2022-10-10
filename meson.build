project('den', 'c',
    version : '1.0.0')

# Includes and sources
inc = [
      include_directories('lib/tinystd')
    , include_directories('lib/ILI9341')
    , include_directories('lib/encoder')
    , include_directories('lib/usart')
    , include_directories('lib/periph/')
    , include_directories('lib/periph/CMSIS/Device/ST/STM32F4xx/')
    , include_directories('lib/periph/CMSIS/Device/ST/STM32F4xx/Include/')
    , include_directories('lib/periph/CMSIS/Include/')
    , include_directories('lib/periph/STM32F4xx_StdPeriph_Driver/inc/')
    , include_directories('lib/periph/CMSIS/Device/ST/STM32F4xx/Source/Templates/arm')
]

src = [
      'main.c'
    , 'lib/periph/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c'
    , 'lib/periph/CMSIS/Device/ST/STM32F4xx/Source/Templates/iar/startup_stm32f40xx.s'
    # , 'startup/startup_stm32f407xx.s'
    , 'lib/periph/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c'
    , 'lib/periph/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c'
    , 'lib/periph/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c'
    , 'lib/periph/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c'
    , 'lib/periph/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c'
    , 'lib/tinystd/itoa.c'
    , 'lib/ILI9341/delay.c'
    , 'lib/ILI9341/core.c'
    , 'lib/ILI9341/dma.c'
    , 'lib/ILI9341/graph.c'
    , 'lib/ILI9341/text.c'
    , 'lib/encoder/encoder.c'
]

# Dependencies
# deps = [
#     dependency('libsystemd')
# ]

# Create executable
main = executable(
    'main',
    sources : src,
    include_directories : inc,
    name_suffix : 'elf',
    c_args: ['-DSTM32F40_41xxx', '-DUSE_STDPERIPH_DRIVER', '-DHSE_VALUE=8000000'],
    link_args : ['-Wl,--gc-sections', '-TSTM32F407VG_FLASH_NoBL.ld']
    # dependencies : deps
)

objcopy = '@0@'.format(find_program('objcopy').path())

mainbin = custom_target(
                        'main.bin',
    output           : ['main.bin'],
    # name_suffix      : 'bin',
    build_by_default : true,
    command          : [objcopy, '-O', 'binary', 'main.elf', 'main.bin'],
    depends          : [main]
)

mainhex = custom_target(
                        'main.hex',
    output           : ['main.hex'],
    build_by_default : true,
    command          : [objcopy, '-O', 'ihex', 'main.elf', 'main.hex'],
    depends          : [main]
)