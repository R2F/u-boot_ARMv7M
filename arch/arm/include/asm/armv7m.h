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

#if !defined(__ASSEMBLY__)
/* SCB Base Address */
#define CM3_SCB_BASE			0xE000ED00
struct cm3_scb {
	uint32_t cpuid;			/* CPUID Base Register */
	uint32_t icsr;			/* Interrupt Control and State Register */
	uint32_t vtor;			/* Vector Table Offset Register */
	uint32_t aircr;			/* App Interrupt and Reset Control Register */
};
#define CM3_SCB_REGS		((volatile struct cm3_scb *)CM3_SCB_BASE)

#define CM3_AIRCR_VECTKEY		0x5fa
#define CM3_AIRCR_VECTKEY_SHIFT		16
#define CM3_AIRCR_ENDIAN		(1<<15)
#define CM3_AIRCR_PRIGROUP_MSK		0x7
#define CM3_AIRCR_PRIGROUP_SHIFT	8
#define CM3_AIRCR_SYSRESET		(1<<2)

#define CM3_ICSR_VECTACT_MSK		0xFF


/* MPU Base Address */
#define CM3_MPU_BASE			0xE000ED90
struct cm3_mpu {
	uint32_t type;			/* Type Register */
	uint32_t ctrl;			/* Control Register */
	uint32_t rnr;			/* Region Number Register */
	uint32_t rbar;			/* Region Base Address Register */
	uint32_t rasr;			/* Region Attribute and Size Register */
};
#define CM3_MPU_REGS		((volatile struct cm3_mpu *)CM3_MPU_BASE)

/*
 * MPU Region Base Address Register
 */
/*
 * MPU Region Attribute and Size Register
 */
/* Region enable bit */
#define CM3_MPU_RASR_EN			(1 << 0)
/*
 * Region size field
 */
#define CM3_MPU_RASR_SIZE_BITS		1
#define CM3_MPU_RASR_SIZE_4GB		(31 << CM3_MPU_RASR_SIZE_BITS)
/*
 * Access permission field
 */
/* Full access for both privileged and unprivileged software */
#define CM3_MPU_RASR_AP_RW_RW		(3 << 24)

/*
 * MPU Control register
 */
/*
 * Enables the operation of MPU during hard fault, NMI, and
 * FAULTMASK handlers
 */
#define CM3_MPU_CTRL_EN_MSK		(1 << 0)
#define CM3_MPU_CTRL_HFNMI_EN_MSK	(1 << 1)

/* SysTick Base Address */
#define CM3_SYSTICK_BASE		0xE000E010
struct cm3_systick {
	uint32_t ctrl;			/* Control and Status Register */
	uint32_t load;			/* Reload Value Register       */
	uint32_t val;			/* Current Value Register      */
	uint32_t cal;			/* Calibration Register        */
};

#define CM3_SYSTICK_LOAD_RELOAD_MSK	(0x00FFFFFF)
/* System Tick counter enable */
#define CM3_SYSTICK_CTRL_EN		(1 << 0)
/* System Tick clock source selection: 1=CPU, 0=STCLK (external clock pin) */
#define CM3_SYSTICK_CTRL_SYSTICK_CPU	(1 << 2)

u8 cortex_m3_irq_vec_get(void);

void cortex_m3_mpu_add_region(u32 region, u32 address, u32 attr);
void cortex_m3_mpu_enable(int enable);
void cortex_m3_mpu_full_access(void);
#endif

#endif /* ARMV7M_H */
