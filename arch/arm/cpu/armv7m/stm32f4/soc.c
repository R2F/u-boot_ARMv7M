/*
 * (C) Copyright 2014
 * Kamil Lulko, <rev13@wp.pl>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/armv7m.h>
#include <asm/arch/stm32.h>

u32 get_cpu_rev(void)
{
	return 0;
}

int arch_cpu_init(void)
{
	configure_clocks();

	/*
	 * Configure the memory protection unit (MPU) to allow full access to
	 * the whole 4GB address space.
	 */
	V7M_MPU->rnr = 0;
	V7M_MPU->rbar = 0;
	V7M_MPU->rasr = (V7M_MPU_RASR_AP_RW_RW | V7M_MPU_RASR_SIZE_4GB
			| V7M_MPU_RASR_EN);
	V7M_MPU->ctrl = (V7M_MPU_CTRL_ENABLE | V7M_MPU_CTRL_HFNMIENA);

	return 0;
}

void s_init(void)
{

}
