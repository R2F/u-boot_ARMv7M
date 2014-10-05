/*
 * (C) Copyright 2014
 * Kamil Lulko, <rev13@wp.pl>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/arch/stm32.h>

u32 get_cpu_rev(void)
{
	return 0;
}

int arch_cpu_init(void)
{
	configure_clocks();

	return 0;
}

void s_init(void)
{

}
