#include "rtos.h"

#include "task.h"
#include "scheduler.h"
#include "port.h"
#include "systick.h"


#define IDLE_STACK_WORDS 128U


static uint32_t
    g_idle_stack[IDLE_STACK_WORDS];


static uint32_t
    g_rtos_started;


static uint32_t
    g_idle_created;


/*
 * Critical-section nesting.
 */
static uint32_t
    g_critical_nesting;


static uint32_t
    g_saved_primask;


/*
 * Kernel idle task.
 *
 * It guarantees that the scheduler
 * always has a READY task.
 */
static void idle_task(void)
{
    while (1)
    {
        /*
         * Sleep until next interrupt.
         */
        __asm volatile ("wfi");
    }
}


/*
 * Initialize kernel.
 */
void rtos_init(void)
{
    g_rtos_started = 0U;

    g_idle_created = 0U;

    g_critical_nesting = 0U;

    g_saved_primask = 0U;


    task_init();

    scheduler_init();

    port_init();
}


/*
 * Public task creation API.
 */
int32_t rtos_create_task(
    void (*entry)(void),
    uint32_t *stack_mem,
    uint32_t stack_words)
{
    return task_create(
        entry,
        stack_mem,
        stack_words
    );
}


/*
 * Start RTOS.
 */
void rtos_start(void)
{
    if (task_get_count() == 0U)
    {
        return;
    }


    /*
     * Add the kernel idle task.
     */
    if (g_idle_created == 0U)
    {
        int32_t idle_id =
            task_create(
                idle_task,
                g_idle_stack,
                IDLE_STACK_WORDS
            );


        if (idle_id < 0)
        {
            return;
        }


        g_idle_created = 1U;
    }


    scheduler_start_first_task();


    /*
     * SVC starts Task 0.
     */
    __asm volatile ("svc 0");


    /*
     * Should never execute.
     */
    while (1)
    {
    }
}


/*
 * Voluntary context switch.
 */
void rtos_yield(void)
{
    port_trigger_pendsv();
}


/*
 * Block current task.
 *
 * 1 tick = 1 ms.
 */
void rtos_delay_ms(uint32_t ms)
    
{
    if (ms == 0U)
    {
        rtos_yield();

        return;
    }


    scheduler_delay_current(ms);


    /*
     * Current task is BLOCKED,
     * so immediately switch.
     */
    port_trigger_pendsv();
}


/*
 * Return system time in milliseconds.
 */
uint32_t rtos_get_tick(void)
{
    return systick_get_tick();
}


/*
 * Number of tasks.
 */
uint32_t rtos_get_task_count(void)
{
    return task_get_count();
}


/*
 * SysTick kernel hook.
 *
 * Called every 1 ms.
 */
void rtos_systick_handler(void)
{
    /*
     * SysTick also runs before
     * rtos_start(), so do nothing
     * until SVC has started Task 0.
     */
    if (g_rtos_started == 0U)
    {
        return;
    }


    /*
     * Update blocked task timers.
     */
    scheduler_tick();


    /*
     * Preempt current task.
     */
    port_trigger_pendsv();
}


/*
 * Called from SVC_Handler.
 */
uint32_t *rtos_get_first_task_sp(void)
{
    tcb_t *task =
        task_get(0);


    if (task == 0)
    {
        return 0;
    }


    /*
     * We are currently inside SVC.
     *
     * Marking started here avoids
     * PendSV running before Task 0
     * has actually started.
     */
    g_rtos_started = 1U;


    return task->sp;
}


/*
 * Disable interrupts.
 *
 * Nesting is supported.
 */
void rtos_enter_critical(void)
{
    uint32_t primask =
        port_irq_save();


    if (g_critical_nesting == 0U)
    {
        g_saved_primask =
            primask;
    }


    g_critical_nesting++;
}


/*
 * Leave critical section.
 */
void rtos_exit_critical(void)
{
    if (g_critical_nesting == 0U)
    {
        return;
    }


    g_critical_nesting--;


    if (g_critical_nesting == 0U)
    {
        port_irq_restore(
            g_saved_primask
        );
    }
}