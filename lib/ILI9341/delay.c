#include "delay.h"

volatile u32 tick = 0;


void SysTick_Handler(void) {
    tick++;
}

static inline void delay(uint32_t time) {
    tick = 0;
    int8_t tmp = 1;

    do {
        tmp = -tmp;
    } while (tick < time);

}

inline void delay_ms(uint32_t time) {
    delay(time);
}

