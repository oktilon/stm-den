#ifndef __TASK_H
#define __TASK_H

#include "uart.h"
#include "sys.h"
#include "bme280.h"
#include "itoa.h"

extern s32 temp;
extern u32 press;
extern u32 hum;

#define IS_VALID_COMMAND(cmd)   ((cmd & 0xFFFF00FF) == 0xAA5500AA)
#define IS_VALID_MANUAL(cmd)    ((cmd & 0xFFFF00FF) == 0x636d0064)

void checkUartCommand(void);

#endif // __TASK_H