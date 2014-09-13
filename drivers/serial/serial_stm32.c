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
#include <asm/arch/stm32.h>
#include <asm/arch/stm32_gpio.h>

#define STM32_USART1_BASE	0x40011000 /* APB2 */
#define STM32_USART2_BASE	0x40004400 /* APB1 */

#if defined(CONFIG_STM32_USART1)
#define USART_BASE	STM32_USART1_BASE
#define USART_SR_FLAG_RXNE	0x20
#define USART_SR_FLAG_TXE	0x80
#define RCC_USART_ENABLE	0x10
static const struct stm32_gpio_dsc usart_gpio[] = {
		{STM32_GPIO_PORT_A, STM32_GPIO_PIN_9},
		{STM32_GPIO_PORT_A, STM32_GPIO_PIN_10},
};
#elif defined(CONFIG_STM32_USART2)
#define USART_BASE	STM32_USART2_BASE
#endif

struct stm32_serial {
	uint32_t USART_SR;
	uint32_t USART_DR;
	uint32_t USART_BRR;
	uint32_t USART_CR1;
	uint32_t USART_CR2;
	uint32_t USART_CR3;
	uint32_t USART_GTPR;
};

static void stm32_serial_setbrg(void)
{
	serial_init();
}

static int stm32_serial_init(void)
{
	int i, rv;
	volatile struct stm32_serial* base = (struct stm32_serial *)USART_BASE;
	volatile struct stm32_rcc_regs* rcc = STM32_RCC_BASE;

	/* Enable clocks to peripherals (GPIO, USART) */
	rcc->apb2enr |= RCC_USART_ENABLE; /* USART2 enable */

	/* Enable and mux GPIOs */
	for (i = 0; i < ARRAY_SIZE(usart_gpio); i++) {
		rv = stm32_gpio_config(&usart_gpio[i], STM32_GPIO_ROLE_USART1);
		if(rv) {
			return rv;
		}
	}

	/* USART configuration */
	/* for 115.200k program 22.8125 if fpclk = 42MHz */
	base->USART_BRR = 0x2D9;
	base->USART_CR1 = 0x0000200C;

	return 0;
}

static int stm32_serial_getc(void)
{
	volatile struct stm32_serial* base = (struct stm32_serial *)USART_BASE;
	while((base->USART_SR & USART_SR_FLAG_RXNE) == 0);
	return base->USART_DR;
}

static void stm32_serial_putc(const char c)
{
	volatile struct stm32_serial* base = (struct stm32_serial *)USART_BASE;
	while((base->USART_SR & USART_SR_FLAG_TXE) == 0);
	base->USART_DR = c;
}

static int stm32_serial_tstc(void)
{
	volatile struct stm32_serial* base = (struct stm32_serial *)USART_BASE;
	return (base->USART_SR & USART_SR_FLAG_RXNE);
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
