#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>


extern volatile uint32_t
    system_ticks;


/*
 * Configure 1ms SysTick.
 */
void systick_init(void);


/*
 * Current tick count.
 */
uint32_t systick_get_tick(void);


/*
 * Cortex-M exception handler.
 */
void SysTick_Handler(void);


#endif