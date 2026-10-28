#ifndef PORT_H
#define PORT_H

#include <stdint.h>


/*
 * Configure Cortex-M exception priorities.
 */
void port_init(void);


/*
 * Request PendSV.
 */
void port_trigger_pendsv(void);


/*
 * Disable interrupts and return
 * previous PRIMASK.
 */
uint32_t port_irq_save(void);


/*
 * Restore previous PRIMASK.
 */
void port_irq_restore(
    uint32_t primask
);


#endif