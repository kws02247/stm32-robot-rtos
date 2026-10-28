#include <stdint.h>


#include "uart.h"
#include "gpio.h"
#include "systick.h"

#include "rtos.h"

#include "robot_task_stub.h"


#define TASK_STACK_WORDS 256U


static uint32_t
    imu_stack[TASK_STACK_WORDS];

static uint32_t
    filter_stack[TASK_STACK_WORDS];

static uint32_t
    pid_stack[TASK_STACK_WORDS];

static uint32_t
    motor_stack[TASK_STACK_WORDS];

static uint32_t
    safety_stack[TASK_STACK_WORDS];

static uint32_t
    logger_stack[TASK_STACK_WORDS];

static uint32_t
    preempt_stack[TASK_STACK_WORDS];


int main(void)
{
    /*
     * Bare-metal drivers.
     */
    uart_init();      /*debug messages*/

    gpio_init();      /*LED*/


    uart_puts(
        "TEST:STM32_BOOT_OK\n"
    );


    /*
     * Initialize RTOS.
     */
    rtos_init();


    uart_puts(
        "TEST:RTOS_INIT_OK\n"
    );


    /*
     * Initialize robot validation data.
     */
    robot_validation_init();


    /*
     * Create 6 robot tasks.
     */

    if (rtos_create_task(
            task_imu,
            imu_stack,
            TASK_STACK_WORDS) < 0)
    {
        uart_puts(
            "FAIL:CREATE_IMU\n"
        );

        while (1)
        {
        }
    }


    if (rtos_create_task(
            task_filter,
            filter_stack,
            TASK_STACK_WORDS) < 0)
    {
        uart_puts(
            "FAIL:CREATE_FILTER\n"
        );

        while (1)
        {
        }
    }


    if (rtos_create_task(
            task_pid,
            pid_stack,
            TASK_STACK_WORDS) < 0)
    {
        uart_puts(
            "FAIL:CREATE_PID\n"
        );

        while (1)
        {
        }
    }


    if (rtos_create_task(
            task_motor,
            motor_stack,
            TASK_STACK_WORDS) < 0)
    {
        uart_puts(
            "FAIL:CREATE_MOTOR\n"
        );

        while (1)
        {
        }
    }


    if (rtos_create_task(
            task_safety,
            safety_stack,
            TASK_STACK_WORDS) < 0)
    {
        uart_puts(
            "FAIL:CREATE_SAFETY\n"
        );

        while (1)
        {
        }
    }


    if (rtos_create_task(
            task_logger,
            logger_stack,
            TASK_STACK_WORDS) < 0)
    {
        uart_puts(
            "FAIL:CREATE_LOGGER\n"
        );

        while (1)
        {
        }
    }


    /*
     * Special CPU-hog task used only
     * to prove preemption.
     */
    if (rtos_create_task(
            task_preempt_probe,
            preempt_stack,
            TASK_STACK_WORDS) < 0)
    {
        uart_puts(
            "FAIL:CREATE_PREEMPT_PROBE\n"
        );

        while (1)
        {
        }
    }


    uart_puts(
        "TEST:TASK_CREATE_OK\n"
    );


    /*
     * Start 1ms system tick.
     */
    systick_init();


    uart_puts(
        "TEST:SYSTICK_OK\n"
    );


    uart_puts(
        "TEST:ROBOT_RTOS_START\n"
    );


    /*
     * SVC starts Task 0.
     *
     * rtos_start() also creates
     * internal idle task.
     */
    rtos_start();


    /*
     * Should never happen.
     */
    uart_puts(
        "TEST:ERROR_RTOS_RETURNED\n"
    );


    while (1)
    {
    }
}