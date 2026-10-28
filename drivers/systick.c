#include "systick.h"
#include "rtos.h"


#define SYST_CSR \
    (*(volatile uint32_t *)0xE000E010UL)

#define SYST_RVR \
    (*(volatile uint32_t *)0xE000E014UL)

#define SYST_CVR \
    (*(volatile uint32_t *)0xE000E018UL)


#define SYSTICK_RELOAD \
    (16000U - 1U)


volatile uint32_t
    system_ticks = 0U;


/*
 * Configure SysTick:
 *
 * processor clock
 * 16 MHz / 16000
 * = 1000 Hz
 *
 * therefore:
 *
 * 1 tick = 1 ms
 */
void systick_init(void)
{
    system_ticks = 0U;


    SYST_RVR =
        SYSTICK_RELOAD;


    SYST_CVR = 0U;


    /*
     * Bit 2 CLKSOURCE = CPU clock
     * Bit 1 TICKINT   = interrupt enable
     * Bit 0 ENABLE    = SysTick enable
     */
    SYST_CSR =
        (1U << 2) |
        (1U << 1) |
        (1U << 0);
}


/*
 * Return current time.
 */
uint32_t systick_get_tick(void)
{
    return system_ticks;
}


/*
 * 1ms system tick.
 */
void SysTick_Handler(void)
{
    system_ticks++;


    /*
     * Tell RTOS that one tick elapsed.
     */
    rtos_systick_handler();
}