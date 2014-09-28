/*
 * (C) Copyright 2014
 * Kamil Lulko, <rev13@wp.pl>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/armv7m.h>
#include <asm/arch/stm32.h>

DECLARE_GLOBAL_DATA_PTR;

#define STM32_TIM2_BASE	0x40000000 /* APB1 */

struct stm32_tim2_5 {
	uint32_t cr1;
	uint32_t cr2;
	uint32_t smcr;
	uint32_t dier;
	uint32_t sr;
	uint32_t egr;
	uint32_t ccmr1;
	uint32_t ccmr2;
	uint32_t ccer;
	uint32_t cnt;
	uint32_t psc;
	uint32_t arr;
	uint32_t reserved1;
	uint32_t ccr1;
	uint32_t ccr2;
	uint32_t ccr3;
	uint32_t ccr4;
	uint32_t reserved2;
	uint32_t dcr;
	uint32_t dmar;
	uint32_t or;
};

#define CONFIG_SYS_HZ_CLOCK 1000000

int timer_init()
{
	volatile struct stm32_tim2_5 *tim =
			(struct stm32_tim2_5*)STM32_TIM2_BASE;

	STM32_RCC->apb1enr |= 0x01;

	if(clock_get(CLOCK_AHB) == clock_get(CLOCK_APB1)) {
		tim->psc = (clock_get(CLOCK_APB1) / CONFIG_SYS_HZ_CLOCK) - 1;
	} else {
		tim->psc = ((clock_get(CLOCK_APB1) * 2) / CONFIG_SYS_HZ_CLOCK) - 1;
	}
	tim->arr = 0xFFFFFFFF - 1;
	tim->cr1 = 0x01;
	tim->egr |= 0x01;

	gd->arch.tbl = 0;
	gd->arch.tbu = 0;
	gd->arch.lastinc = 0;

	return 0;
}

ulong get_timer(ulong base)
{
	return (get_ticks() / 1000) - base;
}

unsigned long long get_ticks(void)
{
	volatile struct stm32_tim2_5 *tim =
			(struct stm32_tim2_5*)STM32_TIM2_BASE;
	uint32_t now;

	now = tim->cnt;

	if (now >= gd->arch.lastinc) {
		gd->arch.tbl += (now - gd->arch.lastinc);
	} else {
		gd->arch.tbl += (0xFFFFFFFF - gd->arch.lastinc) + now;
	}
	gd->arch.lastinc = now;

	return gd->arch.tbl;
}

void reset_timer(void)
{
	volatile struct stm32_tim2_5 *tim =
			(struct stm32_tim2_5*)STM32_TIM2_BASE;
	gd->arch.lastinc = tim->cnt;
	gd->arch.tbl = 0;
}

/* delay x useconds */
void __udelay(ulong usec)
{
	unsigned long long start;

	start = get_ticks();		/* get current timestamp */
	while ((get_ticks() - start) < usec)
		;			/* loop till time has passed */
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk(void)
{
	return CONFIG_SYS_HZ_CLOCK;
}
