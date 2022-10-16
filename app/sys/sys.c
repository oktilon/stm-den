#include "sys.h"

volatile u32 tick = 0;
uint32_t upTime = 0;


void SysTick_Handler(void) {
    tick++;
    upTime++;
}

static inline void delay(uint32_t time) {
    tick = 0;

    do {
        __asm("NOP");
    } while (tick < time);

}

inline void delay_ms(uint32_t time) {
    delay(time);
}