#ifndef ROBOT_TASK_STUB_H
#define ROBOT_TASK_STUB_H


typedef struct
{
    float angle;

    float gyro_rate;

    float pid_output;

    int motor_left_pwm;

    int motor_right_pwm;

    int safety_shutdown;

} robot_state_t;


/*
 * Initialize validation state.
 */
void robot_validation_init(void);


/*
 * Robot tasks.
 */
void task_imu(void);

void task_filter(void);

void task_pid(void);

void task_motor(void);

void task_safety(void);

void task_logger(void);


/*
 * Special validation task.
 *
 * This task NEVER calls yield or delay.
 *
 * If the RTOS is really preemptive,
 * SysTick must still take the CPU away.
 */
void task_preempt_probe(void);


#endif