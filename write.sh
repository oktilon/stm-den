#/bin/bash

#openocd -f board/stm32f4discovery.cfg -c init -c "reset init" -c "flash write_image erase $1" -c reset -c shutdown
openocd -f board/stm32f4discovery.cfg -c "program build/main.elf verify exit reset"
