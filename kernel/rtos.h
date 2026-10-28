#ifndef RTOS_H
#define RTOS_H

#include <stdint.h>


void rtos_init(void);


int32_t rtos_create_task(
    void (*entry)(void),
    uint32_t *stack_mem,
    uint32_t stack_words
);


void rtos_start(void);


void rtos_yield(void);


void rtos_delay_ms(
    uint32_t ms
);


uint32_t rtos_get_tick(void);


uint32_t rtos_get_task_count(void);


void rtos_enter_critical(void);


void rtos_exit_critical(void);


/*
 * Internal kernel functions.
 *
 * Used by exception handlers.
 */
uint32_t *rtos_get_first_task_sp(void);

void rtos_systick_handler(void);


#endif