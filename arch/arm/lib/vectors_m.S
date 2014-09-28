/*
 * (C) Copyright 2014
 * Kamil Lulko, <rev13@wp.pl>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <config.h>
#include <asm/armv7m.h>
#include <asm-offsets.h>
#include <linux/linkage.h>

__invalid_entry:
	b	__invalid_entry

vector_swi:
	b	vector_swi

__pendsv_entry:
	b	__pendsv_entry

__irq_entry:
	b	__irq_entry

   .section  .vectors

ENTRY(vector_table)
	.long	CONFIG_SYS_INIT_SP_ADDR		@ 0 - Reset stack pointer
	.long	_start				@ 1 - Reset
	.long	__invalid_entry			@ 2 - NMI
	.long	__invalid_entry			@ 3 - HardFault
	.long	__invalid_entry			@ 4 - MemManage
	.long	__invalid_entry			@ 5 - BusFault
	.long	__invalid_entry			@ 6 - UsageFault
	.long	__invalid_entry			@ 7 - Reserved
	.long	__invalid_entry			@ 8 - Reserved
	.long	__invalid_entry			@ 9 - Reserved
	.long	__invalid_entry			@ 10 - Reserved
	.long	vector_swi			@ 11 - SVCall
	.long	__invalid_entry			@ 12 - Debug Monitor
	.long	__invalid_entry			@ 13 - Reserved
	.long	__pendsv_entry			@ 14 - PendSV
	.long	__invalid_entry			@ 15 - SysTick
	.rept	107 - 16
	.long	__irq_entry			@ 16..107 - External Interrupts
	.endr
