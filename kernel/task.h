#ifndef TASK_H
#define TASK_H

#include <stdint.h>


#define RTOS_MAX_TASKS 8U


typedef enum
{
    TASK_READY = 0,
    TASK_RUNNING,
    TASK_BLOCKED

} task_state_t;


typedef struct
{
    uint32_t *sp;
    uint32_t *stack_base;
    uint32_t stack_size;

    uint32_t delay_ticks;

    task_state_t state;

    int32_t id;

    void (*entry)(void);

} tcb_t;


/* Initialize task manager */
void task_init(void);


/* Create a task */
int32_t task_create(
    void (*entry)(void),
    uint32_t *stack_mem,
    uint32_t stack_words
);


/* Get task by ID */
tcb_t *task_get(int32_t id);


/* Number of created tasks */
uint32_t task_get_count(void);


/* Check stack guard words */
int task_check_all_stacks(void);


#endif