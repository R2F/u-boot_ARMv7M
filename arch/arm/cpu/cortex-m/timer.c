/*
 * (C) Copyright 2014
 * Kamil Lulko, <rev13@wp.pl>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>

ulong get_timer_us(ulong base)
{
	return 0;
}

ulong get_timer(ulong base)
{
	return 0;
}

unsigned long long get_ticks(void)
{
	return get_timer(0);
}

ulong get_tbclk(void)
{
	return CONFIG_SYS_HZ;
}

void __udelay(unsigned long usec)
{
	ulong endtime;
	signed long diff;

	endtime = get_timer_us(0) + usec;

	do {
		ulong now = get_timer_us(0);
		diff = endtime - now;
	} while (diff >= 0);
}
