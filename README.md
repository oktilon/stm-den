# STM32 ILI9341 spi
TOC:
 - [Title1](#tit-1)
  - [Title11](#tit-1-1)
  - [Title12](#tit-1-2)
 - [Title2](#tit-2)
  - [Title21](#tit-2-1)

## 1. Title one {#tit-1}

stm-ILI9341-spi-master

### 1.1. Subtitle one - one {#tit-1-1}
Subtitle text 1-1

### 1.2. Subtitle one - two {#tit-1-2}
Subtitle text 1-2

## 2. Title two {#tit-2}
Title text
### 2.1. Subtitle one - one {#tit-2-1}
Subtitle text 2-1

 openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg -c "program build/$(TARGET).elf verify exit reset"