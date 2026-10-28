#include "uart.h"
#include <stdint.h>

#define RCC_BASE        0x40023800UL
#define GPIOA_BASE      0x40020000UL
#define USART2_BASE     0x40004400UL

#define RCC_AHB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x30))
#define RCC_APB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x40))

#define GPIOA_MODER     (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_AFRL      (*(volatile uint32_t *)(GPIOA_BASE + 0x20))

#define USART2_SR       (*(volatile uint32_t *)(USART2_BASE + 0x00))
#define USART2_DR       (*(volatile uint32_t *)(USART2_BASE + 0x04))
#define USART2_BRR      (*(volatile uint32_t *)(USART2_BASE + 0x08))
#define USART2_CR1      (*(volatile uint32_t *)(USART2_BASE + 0x0C))

void uart_init(void)
{
    RCC_AHB1ENR |= (1U << 0);   // GPIOA clock
    RCC_APB1ENR |= (1U << 17);  // USART2 clock

    // PA2 = USART2_TX, PA3 = USART2_RX, alternate function mode
    GPIOA_MODER &= ~((3U << 4) | (3U << 6));
    GPIOA_MODER |=  ((2U << 4) | (2U << 6));

    // AF7 for PA2, PA3
    GPIOA_AFRL &= ~((0xFU << 8) | (0xFU << 12));
    GPIOA_AFRL |=  ((7U << 8) | (7U << 12));

    // 16 MHz clock, 115200 baud: 16000000 / 115200 = 138.888 ≈ 0x008B
    USART2_BRR = 0x008B;

    // UE = 1, TE = 1, RE = 1
    USART2_CR1 = (1U << 13) | (1U << 3) | (1U << 2);
}

void uart_putc(char c)
{
    while (!(USART2_SR & (1U << 7))) {
    }
    USART2_DR = (uint32_t)c;
}

void uart_puts(const char *s)  
{
    while (*s) {
        if (*s == '\n') {
            uart_putc('\r');
        }
        uart_putc(*s++);
    }
}