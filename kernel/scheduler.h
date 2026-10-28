#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>


void scheduler_init(void);


/*
 * Set Task 0 as initial running task.
 */
void scheduler_start_first_task(void);


/*
 * Save current task PSP.
 */
void scheduler_save_current_sp(
    uint32_t *sp
);


/*
 * Select next READY task.
 *
 * Returns that task's saved SP.
 */
uint32_t *scheduler_switch_next(void);


/*
 * Block current task for ticks.
 */
void scheduler_delay_current(
    uint32_t ticks
);


/*
 * Called every 1ms.
 */
void scheduler_tick(void);


#endif