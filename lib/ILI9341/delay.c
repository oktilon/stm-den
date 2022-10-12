#include "delay.h"

volatile u32 tick = 0;

void SysTick_Handler(void) {
    tick++;
}

static inline void delay(uint32_t time) {
    tick = 0;

    do {
        while (!(SysTick->CTRL && SysTick_CTRL_COUNTFLAG_Msk));
    } while (tick < time);

    SysTick->CTRL = 0;
}

inline void delay_ms(uint32_t time) {
    delay(time);
}

