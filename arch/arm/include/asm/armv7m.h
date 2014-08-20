/*
 * (C) Copyright 2014
 * Kamil Lulko, <rev13@wp.pl>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef ARMV7M_H
#define ARMV7M_H

#if defined(__ASSEMBLY__)
	.syntax unified
	.thumb
#endif

#define BASEADDR_V7M_SCB 	0xe000ed00
#define V7M_SCB_VTOR		0x08

#endif /* ARMV7M_H */
