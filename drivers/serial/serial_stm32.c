/*
 * (C) Copyright 2014
 * Kamil Lulko, <rev13@wp.pl>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <serial.h>
#include <linux/compiler.h>

#define STM32_RCC_BASE		0x40023800 /* AHB1 */
#define STM32_GPIOA_BASE	0x40020000 /* AHB1 */

#define CONFIG_STM32_USART1

#define STM32_USART1_BASE	0x40011000 /* APB2 */
#define STM32_USART2_BASE	0x40004400 /* APB1 */

#if defined(CONFIG_STM32_USART1)
#define USART_BASE	STM32_USART1_BASE
#elif defined(CONFIG_STM32_USART2)
#define USART_BASE	STM32_USART2_BASE
#endif

struct stm32_serial {
	volatile uint32_t USART_SR;
	volatile uint32_t USART_DR;
	volatile uint32_t USART_BRR;
	volatile uint32_t USART_CR1;
	volatile uint32_t USART_CR2;
	volatile uint32_t USART_CR3;
	volatile uint32_t USART_GTPR;
};

static void stm32_serial_setbrg(void)
{
	serial_init();
}

static int stm32_serial_init(void)
{
	volatile struct stm32_serial* base = (struct stm32_serial *)USART_BASE;

	/* Enable clocks to peripherals (GPIO, USART) */
	*(uint32_t*)(STM32_RCC_BASE + 0x44) |= 0x10; /* USART2 enable */
	*(uint32_t*)(STM32_RCC_BASE + 0x30) |= 0x00000001; /* GPIOA enable */

	/* Enable and mux GPIOs */
	*(uint32_t*)(STM32_GPIOA_BASE + 0x00) |= 0x280000; /* alternate function mode GPIOA pin 9,10 */
	*(uint32_t*)(STM32_GPIOA_BASE + 0x08) |= 0x280000; /* 50MHz speed */
	*(uint32_t*)(STM32_GPIOA_BASE + 0x24) |= 0x770; /* AF7 on pin 9,10 */

	/* USART configuration */
	/* for 115.200k program 22.8125 if fpclk = 42MHz */
	base->USART_BRR = 0x2D9;
	base->USART_CR1 = 0x0000200C;

	return 0;
}

static int stm32_serial_getc(void)
{
	volatile struct stm32_serial* base = (struct stm32_serial *)USART_BASE;
	while((base->USART_SR & 0x20) == 0);
	return base->USART_DR;
}

static void stm32_serial_putc(const char c)
{
	volatile struct stm32_serial* base = (struct stm32_serial *)USART_BASE;
	while((base->USART_SR & 0x80) == 0);
	base->USART_DR = c;
}

static int stm32_serial_tstc(void)
{
	return 0;
}

static struct serial_device stm32_serial_drv = {
	.name	= "stm32_serial",
	.start	= stm32_serial_init,
	.stop	= NULL,
	.setbrg	= stm32_serial_setbrg,
	.putc	= stm32_serial_putc,
	.puts	= default_serial_puts,
	.getc	= stm32_serial_getc,
	.tstc	= stm32_serial_tstc,
};

void stm32_serial_initialize(void)
{
	serial_register(&stm32_serial_drv);
}

__weak struct serial_device *default_serial_console(void)
{
	return &stm32_serial_drv;
}
