/*
 * (C) Copyright 2010,2011
 * Vladimir Khusainov, Emcraft Systems, vlad@emcraft.com
 *
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

#define V7M_SCB_BASE		0xE000ED00
#define V7M_MPU_BASE		0xE000ED90
#define V7M_SYSTICK_BASE	0xE000E010

#if !defined(__ASSEMBLY__)
struct v7m_scb {
	uint32_t cpuid;			/* CPUID Base Register */
	uint32_t icsr;			/* Interrupt Control and State Register */
	uint32_t vtor;			/* Vector Table Offset Register */
	uint32_t aircr;			/* App Interrupt and Reset Control Register */
};
#define V7M_SCB		((volatile struct v7m_scb *)V7M_SCB_BASE)

#define V7M_AIRCR_VECTKEY		0x5fa
#define V7M_AIRCR_VECTKEY_SHIFT		16
#define V7M_AIRCR_ENDIAN		(1 << 15)
#define V7M_AIRCR_PRIGROUP_MSK		0x7
#define V7M_AIRCR_PRIGROUP_SHIFT		8
#define V7M_AIRCR_SYSRESET		(1 << 2)

#define V7M_ICSR_VECTACT_MSK		0xFF

struct v7m_mpu {
	uint32_t type;			/* Type Register */
	uint32_t ctrl;			/* Control Register */
	uint32_t rnr;			/* Region Number Register */
	uint32_t rbar;			/* Region Base Address Register */
	uint32_t rasr;			/* Region Attribute and Size Register */
};
#define V7M_MPU		((volatile struct v7m_mpu *)V7M_MPU_BASE)

#define V7M_MPU_CTRL_ENABLE		(1 << 0)

struct v7m_systick {
	uint32_t ctrl;			/* Control and Status Register */
	uint32_t load;			/* Reload Value Register */
	uint32_t val;			/* Current Value Register */
	uint32_t cal;			/* Calibration Register */
};
#define V7M_SYSTICK		((volatile struct v7m_sytick *)V7M_SYSTICK_BASE)

#define V7M_SYSTICK_LOAD_RELOAD_MSK		(0x00FFFFFF)
#define V7M_SYSTICK_CTRL_EN		(1 << 0)
#define V7M_SYSTICK_CTRL_SYSTICK_CPU		(1 << 2)

#endif

#endif /* ARMV7M_H */
