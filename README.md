# STM32 ILI9341 spi

stm-ILI9341-spi-master

 openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg -c "program build/$(TARGET).elf verify exit reset"