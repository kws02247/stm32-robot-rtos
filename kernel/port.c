#include "port.h"


#define SCB_ICSR \
    (*(volatile uint32_t *)0xE000ED04UL)

#define SCB_SHPR3 \
    (*(volatile uint32_t *)0xE000ED20UL)


#define PENDSVSET \
    (1UL << 28)


/*
 * Configure PendSV and SysTick
 * to the lowest priority.
 *
 * SysTick pends PendSV.
 * PendSV performs the actual switch.
 */
void port_init(void)
{
    uint32_t value =
        SCB_SHPR3;


    /*
     * Clear PendSV and SysTick
     * priority bytes.
     */
    value &=
        ~((0xFFUL << 16) |
          (0xFFUL << 24));


    /*
     * Both run at lowest priority.
     *
     * SysTick is already active when
     * it requests PendSV, so PendSV
     * executes after SysTick finishes.
     */
    value |=
        (0xFFUL << 16) |
        (0xFFUL << 24);


    SCB_SHPR3 = value;
}


/*
 * Pend the PendSV exception.
 */
void port_trigger_pendsv(void)
{
    SCB_ICSR =
        PENDSVSET;


    __asm volatile (
        "dsb"
        ::: "memory"
    );


    __asm volatile (
        "isb"
        ::: "memory"
    );
}


/*
 * Save interrupt state and disable IRQs.
 */
uint32_t port_irq_save(void)
{
    uint32_t primask;


    __asm volatile (
        "mrs %0, primask"
        : "=r"(primask)
        :
        : "memory"
    );


    __asm volatile (
        "cpsid i"
        ::: "memory"
    );


    return primask;
}


/*
 * Restore previous interrupt state.
 */
void port_irq_restore(
    uint32_t primask)
{
    __asm volatile (
        "msr primask, %0"
        :
        : "r"(primask)
        : "memory"
    );
}