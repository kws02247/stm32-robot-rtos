#include "task.h"


#define INITIAL_XPSR       0x01000000UL
#define STACK_GUARD_VALUE  0xDEADBEEFUL


static tcb_t g_tasks[RTOS_MAX_TASKS];

static uint32_t g_task_count;


/*
 * Called if a task accidentally returns.
 */
static void task_exit_handler(void)
{
    while (1)
    {
    }
}


/*
 * Build the initial Cortex-M task stack.
 *
 * Hardware frame:
 *
 * R0
 * R1
 * R2
 * R3
 * R12
 * LR
 * PC
 * xPSR
 *
 * Software frame:
 *
 * R4-R11
 */
static uint32_t *task_stack_init(
    void (*entry)(void),                 /* which stack to run */
    uint32_t *stack_mem,                 /* memory of stack to init */
    uint32_t stack_words)                /* how big the stack is */
{
    uint32_t *sp = stack_mem + stack_words;


    /*
     * Cortex-M stack must be 8-byte aligned.
     */
    sp = (uint32_t *)
        ((uintptr_t)sp & ~(uintptr_t)0x7U);


    /*
     * Hardware exception frame.
     */

    *(--sp) = INITIAL_XPSR;

    *(--sp) =
        (uint32_t)(uintptr_t)entry;

    *(--sp) =
        (uint32_t)(uintptr_t)task_exit_handler;

    *(--sp) = 0U;     /* R12 */
    *(--sp) = 0U;     /* R3  */
    *(--sp) = 0U;     /* R2  */
    *(--sp) = 0U;     /* R1  */
    *(--sp) = 0U;     /* R0  */


    /*
     * Software-saved registers.
     */

    *(--sp) = 0U;     /* R11 */
    *(--sp) = 0U;     /* R10 */
    *(--sp) = 0U;     /* R9  */
    *(--sp) = 0U;     /* R8  */
    *(--sp) = 0U;     /* R7  */
    *(--sp) = 0U;     /* R6  */
    *(--sp) = 0U;     /* R5  */
    *(--sp) = 0U;     /* R4  */


    return sp;
}


/*
 * Initialize task table.
 */
void task_init(void)
{
    g_task_count = 0U;


    for (uint32_t i = 0;
         i < RTOS_MAX_TASKS;
         i++)
    {
        g_tasks[i].sp = 0;

        g_tasks[i].stack_base = 0;

        g_tasks[i].stack_size = 0;

        g_tasks[i].delay_ticks = 0U;

        g_tasks[i].state = TASK_BLOCKED;

        g_tasks[i].id = -1;

        g_tasks[i].entry = 0;
    }
}


/*
 * Create a static task.
 */
int32_t task_create(
    void (*entry)(void),
    uint32_t *stack_mem,
    uint32_t stack_words)
{
    if (entry == 0)
    {
        return -1;
    }


    if (stack_mem == 0)
    {
        return -1;
    }


    if (stack_words < 32U)
    {
        return -1;
    }


    if (g_task_count >= RTOS_MAX_TASKS)
    {
        return -1;
    }


    /*
     * Stack overflow guard.
     *
     * Stack grows downward.
     * If the task reaches the bottom,
     * this value will be overwritten.
     */
    stack_mem[0] = STACK_GUARD_VALUE;


    tcb_t *task =
        &g_tasks[g_task_count];


    task->entry = entry;

    task->stack_base = stack_mem;

    task->stack_size = stack_words;

    task->delay_ticks = 0U;

    task->state = TASK_READY;

    task->id =
        (int32_t)g_task_count;


    task->sp =
        task_stack_init(
            entry,
            stack_mem,
            stack_words
        );


    g_task_count++;


    return task->id;
}


/*
 * Return TCB by ID.
 */
tcb_t *task_get(int32_t id)
{
    if (id < 0)
    {
        return 0;
    }


    if ((uint32_t)id >=
        g_task_count)
    {
        return 0;
    }


    return &g_tasks[id];
}


/*
 * Number of tasks.
 */
uint32_t task_get_count(void)
{
    return g_task_count;
}


/*
 * Verify that no task has overwritten
 * its stack guard.
 */
int task_check_all_stacks(void)
{
    for (uint32_t i = 0;
         i < g_task_count;
         i++)
    {
        if (g_tasks[i].stack_base == 0)
        {
            return 0;
        }


        if (g_tasks[i].stack_base[0]
            != STACK_GUARD_VALUE)
        {
            return 0;
        }
    }


    return 1;
}