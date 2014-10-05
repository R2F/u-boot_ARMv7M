/*
 * (C) Copyright 2010,2011
 * Vladimir Khusainov, Emcraft Systems, vlad@emcraft.com
 *
 * (C) Copyright 2014
 * Kamil Lulko, <rev13@wp.pl>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <asm/armv7m.h>

DECLARE_GLOBAL_DATA_PTR;

/*
 * This is called right before passing control to
 * the Linux kernel point.
 */
int cleanup_before_linux(void)
{
	return 0;
}

/*
 * H/w WDT strobe routine
 */
#if defined (CONFIG_HW_WATCHDOG)
void hw_watchdog_reset(void)
{
	/*
	 * Call the h/w-specific WDT strobe.
	 */
	wdt_strobe();
}
#endif

/*
 * Perform the low-level reset.
 */
void reset_cpu(ulong addr)
{
	/*
	 * Perform reset but keep priority group unchanged.
	 */
	V7M_SCB->aircr = (V7M_AIRCR_VECTKEY << V7M_AIRCR_VECTKEY_SHIFT) |
			  (V7M_SCB->aircr &
			  (V7M_AIRCR_PRIGROUP_MSK << V7M_AIRCR_PRIGROUP_SHIFT))
			  | V7M_AIRCR_SYSRESET;
}
