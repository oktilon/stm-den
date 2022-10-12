#/bin/bash

openocd -f board/stm32f4discovery.cfg -c init -c halt -c "flash write_image erase $1" -c reset -c shutdown
