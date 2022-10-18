project('den', 'c',
    version : '1.0.0')

# Includes and sources
inc = [
      include_directories('app')
    , include_directories('app/board')
    , include_directories('app/lcd')
    , include_directories('app/spi')
    , include_directories('app/i2c')
    , include_directories('app/sys')
    , include_directories('app/uart')
    , include_directories('app/ds1307')
    , include_directories('app/bme280')
    , include_directories('lib/tinystd')
    , include_directories('lib/STM32F4xx/')
    , include_directories('lib/STM32F4xx/CMSIS/Device/ST/STM32F4xx/')
    , include_directories('lib/STM32F4xx/CMSIS/Device/ST/STM32F4xx/Include/')
    , include_directories('lib/STM32F4xx/CMSIS/Include/')
    , include_directories('lib/STM32F4xx/STM32F4xx_StdPeriph_Driver/inc/')
    , include_directories('lib/STM32F4xx/CMSIS/Device/ST/STM32F4xx/Source/Templates/arm')
]

src = [
      'app/main.c'
    # , 'lib/tinystd/itoa.c'
    , 'app/ds1307/ds1307.c'
    , 'app/bme280/bme280.c'
    , 'app/sys/sys.c'
    , 'app/uart/uart.c'
    , 'app/board/board.c'
    , 'app/spi/spi.c'
    , 'app/lcd/lcd.c'
    , 'app/i2c/i2c.c'
    , 'startup_stm32f407xx.s'
    , 'lib/STM32F4xx/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c'
    , 'lib/STM32F4xx/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c'
    , 'lib/STM32F4xx/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c'
    , 'lib/STM32F4xx/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c'
    , 'lib/STM32F4xx/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c'
    , 'lib/STM32F4xx/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c'
    , 'lib/STM32F4xx/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_usart.c'
    , 'lib/STM32F4xx/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_i2c.c'
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
    c_args: ['-DSTM32F40_41xxx'
        , '-DSTM32F407xx'
        , '-DUSE_STDPERIPH_DRIVER'
        , '-DHSE_VALUE=8000000'
        , '-Wall'
        , '-fdata-sections'
        , '-ffunction-sections'
    ],
    # link_args removed : '-Wl,--gc-sections', , '--specs=nosys.specs'
    link_args : ['-T../STM32F407VGTx_FLASH.ld'
        , '-nodefaultlibs']
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