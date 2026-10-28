#include "scheduler.h"
#include "task.h"


static int32_t g_current_task_id = -1;


/*
 * Initialize scheduler.
 */
void scheduler_init(void)
{
    g_current_task_id = -1;
}


/*
 * First created task is Task 0.
 */
void scheduler_start_first_task(void)
{
    tcb_t *task = task_get(0);


    if (task == 0)
    {
        return;
    }


    g_current_task_id = 0;

    task->state = TASK_RUNNING;
}


/*
 * Save current task's PSP.
 *
 * Called from PendSV_Handler after
 * R4-R11 have been pushed.
 */
void scheduler_save_current_sp(uint32_t *sp)
   
{
    if (g_current_task_id < 0)
    {
        return;
    }


    tcb_t *current =
        task_get(g_current_task_id);


    if (current == 0)
    {
        return;
    }


    current->sp = sp;


    /*
     * Normal preemption:
     *
     * RUNNING -> READY
     *
     * A task blocked by rtos_delay_ms()
     * remains BLOCKED.
     */
    if (current->state == TASK_RUNNING)
        
    {
        current->state =  TASK_READY;
    }
}


/*
 * Round-robin selection.
 */
uint32_t *scheduler_switch_next(void)
{
    uint32_t task_count =
        task_get_count();


    if (task_count == 0U)
    {
        return 0;
    }


    for (uint32_t i = 0;
         i < task_count;
         i++)
    {
        int32_t next_id =
            (g_current_task_id
             + 1
             + (int32_t)i)
            % (int32_t)task_count;


        tcb_t *next =
            task_get(next_id);


        if ((next != 0) &&
            (next->state ==
             TASK_READY))
        {
            g_current_task_id =
                next_id;


            next->state =
                TASK_RUNNING;


            return next->sp;
        }
    }


    /*
     * Normally the kernel idle task
     * guarantees that at least one
     * READY task exists.
     */
    tcb_t *current =
        task_get(g_current_task_id);


    if (current != 0)
    {
        current->state =
            TASK_RUNNING;


        return current->sp;
    }


    return 0;
}


/*
 * Put the current task to sleep.
 */
void scheduler_delay_current(uint32_t ticks)

{
    if (g_current_task_id < 0)
    {
        return;
    }


    tcb_t *current =
        task_get(g_current_task_id);


    if (current == 0)
    {
        return;
    }


    current->delay_ticks = ticks;

    current->state =
        TASK_BLOCKED;
}


/*
 * Called once per SysTick.
 *
 * 1 tick = 1 ms
 */
void scheduler_tick(void)
{
    uint32_t task_count =
        task_get_count();


    for (uint32_t i = 0;
         i < task_count;
         i++)
    {
        tcb_t *task =
            task_get((int32_t)i);


        if (task == 0)
        {
            continue;
        }


        if (task->state ==
            TASK_BLOCKED)
        {
            if (task->delay_ticks > 0U)
            {
                task->delay_ticks--;
            }


            if (task->delay_ticks == 0U)
            {
                task->state =
                    TASK_READY;
            }
        }
    }
}