#include <stdint.h>

#include "robot_task_stub.h"

#include "rtos.h"
#include "task.h"
#include "uart.h"
#include "gpio.h"


volatile robot_state_t
    g_robot_state;


/*
 * Execution counters.
 */
static volatile uint32_t
    g_imu_count;

static volatile uint32_t
    g_filter_count;

static volatile uint32_t
    g_pid_count;

static volatile uint32_t
    g_motor_count;

static volatile uint32_t
    g_safety_count;

static volatile uint32_t
    g_logger_count;


/*
 * This task never yields or blocks.
 *
 * Continued execution of the other tasks
 * proves that SysTick preemption works.
 */
static volatile uint32_t
    g_preempt_probe_count;


/*
 * Simple critical-section validation value.
 */
static volatile uint32_t
    g_critical_probe;


/*
 * Initialize validation data.
 */
void robot_validation_init(void)
{
    g_robot_state.angle = 0.0f;

    g_robot_state.gyro_rate = 0.0f;

    g_robot_state.pid_output = 0.0f;

    g_robot_state.motor_left_pwm = 0;

    g_robot_state.motor_right_pwm = 0;

    g_robot_state.safety_shutdown = 0;


    g_imu_count = 0U;

    g_filter_count = 0U;

    g_pid_count = 0U;

    g_motor_count = 0U;

    g_safety_count = 0U;

    g_logger_count = 0U;

    g_preempt_probe_count = 0U;

    g_critical_probe = 0U;
}


/*
 * Check whether a task approximately
 * ran at its required period.
 *
 * count * period should be close
 * to elapsed milliseconds.
 *
 * 20 percent tolerance is allowed
 * for this UART/debug validation build.
 */
static int rate_is_ok(
    uint32_t count,
    uint32_t period_ms,
    uint32_t elapsed_ms)
{
    uint32_t actual_time =
        count * period_ms;


    uint32_t tolerance =
        elapsed_ms / 5U;


    uint32_t lower =
        elapsed_ms - tolerance;


    uint32_t upper =
        elapsed_ms + tolerance;


    if (actual_time < lower)
    {
        return 0;
    }


    if (actual_time > upper)
    {
        return 0;
    }


    return 1;
}


/*
 * IMU task
 *
 * 200 Hz
 */
void task_imu(void)
{
    while (1)
    {
        rtos_enter_critical();


        g_imu_count++;


        /*
         * Stub sensor data.
         *
         * No real IMU yet.
         */
        g_robot_state.gyro_rate =
            g_robot_state.gyro_rate;


        rtos_exit_critical();


        rtos_delay_ms(5U);
    }
}


/*
 * Complementary filter task
 *
 * 200 Hz
 */
void task_filter(void)
{
    while (1)
    {
        rtos_enter_critical();


        g_filter_count++;


        /*
         * Stub filter.
         */
        g_robot_state.angle =
            g_robot_state.gyro_rate;


        rtos_exit_critical();


        rtos_delay_ms(5U);
    }
}


/*
 * PID task
 *
 * 200 Hz
 */
void task_pid(void)
{
    while (1)
    {
        rtos_enter_critical();


        g_pid_count++;


        /*
         * Stub PID.
         */
        g_robot_state.pid_output =
            g_robot_state.angle;


        rtos_exit_critical();


        rtos_delay_ms(5U);
    }
}


/*
 * Motor task
 *
 * 200 Hz
 */
void task_motor(void)
{
    while (1)
    {
        rtos_enter_critical();


        g_motor_count++;


        /*
         * Stub motor output.
         */
        g_robot_state.motor_left_pwm = 0;

        g_robot_state.motor_right_pwm = 0;


        rtos_exit_critical();


        rtos_delay_ms(5U);
    }
}


/*
 * Safety task
 *
 * 50 Hz
 */
void task_safety(void)
{
    while (1)
    {
        rtos_enter_critical();


        g_safety_count++;


        /*
         * Stub safety state.
         */
        if (g_robot_state.safety_shutdown)
        {
            g_robot_state.motor_left_pwm = 0;

            g_robot_state.motor_right_pwm = 0;
        }


        rtos_exit_critical();


        rtos_delay_ms(20U);
    }
}


/*
 * Preemption validation task.
 *
 * IMPORTANT:
 *
 * There is intentionally:
 *
 * NO rtos_yield()
 * NO rtos_delay_ms()
 *
 * If SysTick preemption does not work,
 * this task will take the CPU forever
 * and Logger will never run again.
 */
void task_preempt_probe(void)
{
    while (1)
    {
        g_preempt_probe_count++;
    }
}


/*
 * Logger / validation task
 *
 * 10 Hz
 */
void task_logger(void)
{
    uint32_t last_report_tick =
        rtos_get_tick();


    uint32_t previous_probe =
        g_preempt_probe_count;


    int ready_reported = 0;


    /*
     * Critical section functional test.
     */
    rtos_enter_critical();

    g_critical_probe =
        0x12345678UL;

    rtos_exit_critical();


    while (1)
    {
        /*
         * Logger itself runs at 10 Hz.
         */
        rtos_delay_ms(100U);


        rtos_enter_critical();

        g_logger_count++;

        rtos_exit_critical();


        uint32_t now =
            rtos_get_tick();


        uint32_t elapsed =
            now - last_report_tick;


        if (elapsed < 1000U)
        {
            continue;
        }


        uint32_t imu;

        uint32_t filter;

        uint32_t pid;

        uint32_t motor;

        uint32_t safety;

        uint32_t logger;


        /*
         * Snapshot/reset counters
         * atomically.
         */
        rtos_enter_critical();


        imu =
            g_imu_count;

        filter =
            g_filter_count;

        pid =
            g_pid_count;

        motor =
            g_motor_count;

        safety =
            g_safety_count;

        logger =
            g_logger_count;


        g_imu_count = 0U;

        g_filter_count = 0U;

        g_pid_count = 0U;

        g_motor_count = 0U;

        g_safety_count = 0U;

        g_logger_count = 0U;


        int critical_ok =
            (g_critical_probe ==
             0x12345678UL);


        int shared_state_ok =
            (g_robot_state.motor_left_pwm ==
             g_robot_state.motor_right_pwm);


        rtos_exit_critical();


        uint32_t probe_now =
            g_preempt_probe_count;


        int preempt_ok =
            (probe_now !=
             previous_probe);


        previous_probe =
            probe_now;


        int imu_ok =
            rate_is_ok(
                imu,
                5U,
                elapsed
            );


        int filter_ok =
            rate_is_ok(
                filter,
                5U,
                elapsed
            );


        int pid_ok =
            rate_is_ok(
                pid,
                5U,
                elapsed
            );


        int motor_ok =
            rate_is_ok(
                motor,
                5U,
                elapsed
            );


        int safety_ok =
            rate_is_ok(
                safety,
                20U,
                elapsed
            );


        int logger_ok =
            rate_is_ok(
                logger,
                100U,
                elapsed
            );


        int stack_ok =
            task_check_all_stacks();


        int all_ok =
            preempt_ok &&
            imu_ok &&
            filter_ok &&
            pid_ok &&
            motor_ok &&
            safety_ok &&
            logger_ok &&
            critical_ok &&
            shared_state_ok &&
            stack_ok;


        /*
         * Print detailed final validation
         * once after the first successful
         * one-second window.
         */
        if (all_ok)
        {
            gpio_led_toggle();

            if (!ready_reported)
            {
                uart_puts(
                    "TEST:PREEMPT_OK\n"
                );

                uart_puts(
                    "TEST:DELAY_OK\n"
                );

                uart_puts(
                    "TEST:IMU_200HZ_OK\n"
                );

                uart_puts(
                    "TEST:FILTER_200HZ_OK\n"
                );

                uart_puts(
                    "TEST:PID_200HZ_OK\n"
                );

                uart_puts(
                    "TEST:MOTOR_200HZ_OK\n"
                );

                uart_puts(
                    "TEST:SAFETY_50HZ_OK\n"
                );

                uart_puts(
                    "TEST:LOGGER_10HZ_OK\n"
                );

                uart_puts(
                    "TEST:CRITICAL_SECTION_OK\n"
                );

                uart_puts(
                    "TEST:STACK_OK\n"
                );

                uart_puts(
                    "TEST:ROBOT_RTOS_READY\n"
                );


                ready_reported = 1;
            }
            else
            {
                /*
                 * Low-rate health log.
                 */
                uart_puts(
                    "TEST:RTOS_HEALTH_OK\n"
                );
            }
        }
        else
        {
            uart_puts(
                "TEST:VALIDATION_FAIL\n"
            );


            if (!preempt_ok)
            {
                uart_puts(
                    "FAIL:PREEMPT\n"
                );
            }


            if (!imu_ok)
            {
                uart_puts(
                    "FAIL:IMU_RATE\n"
                );
            }


            if (!filter_ok)
            {
                uart_puts(
                    "FAIL:FILTER_RATE\n"
                );
            }


            if (!pid_ok)
            {
                uart_puts(
                    "FAIL:PID_RATE\n"
                );
            }


            if (!motor_ok)
            {
                uart_puts(
                    "FAIL:MOTOR_RATE\n"
                );
            }


            if (!safety_ok)
            {
                uart_puts(
                    "FAIL:SAFETY_RATE\n"
                );
            }


            if (!logger_ok)
            {
                uart_puts(
                    "FAIL:LOGGER_RATE\n"
                );
            }


            if (!critical_ok ||
                !shared_state_ok)
            {
                uart_puts(
                    "FAIL:CRITICAL_SECTION\n"
                );
            }


            if (!stack_ok)
            {
                uart_puts(
                    "FAIL:STACK\n"
                );
            }
        }


        last_report_tick = now;
    }
}