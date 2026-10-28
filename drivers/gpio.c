#include "gpio.h"
#include <stdint.h>

#define RCC_BASE        0x40023800UL
#define GPIOA_BASE      0x40020000UL

#define RCC_AHB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x30))
#define GPIOA_MODER     (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x14))

void gpio_init(void)
{
    RCC_AHB1ENR |= (1U << 0);   // GPIOA clock

    // PA5 output: Nucleo onboard LED
    GPIOA_MODER &= ~(3U << 10);
    GPIOA_MODER |=  (1U << 10);
}

void gpio_led_toggle(void)
{
    GPIOA_ODR ^= (1U << 5);
}