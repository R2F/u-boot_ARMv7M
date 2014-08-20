/*
 * (C) Copyright 2014
 * Kamil Lulko, <rev13@wp.pl>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

void lowlevel_init(void)
{
	reset_rcc();
	configure_clocks();
}
