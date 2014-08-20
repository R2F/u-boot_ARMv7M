/*
 * (C) Copyright 2014
 * Kamil Lulko, <rev13@wp.pl>
 *
 * (C) Copyright 2014
 * STMicroelectronics
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>

/* Basic working clock/flash configuration. Should make this more pretty
 * at some point
 */

#define STM32_RCC_BASE		0x40023800 /* AHB1 */

struct stm32_rcc {
	volatile uint32_t CR;
	volatile uint32_t PLLCFGR;
	volatile uint32_t CFGR;
	volatile uint32_t CIR;
	volatile uint32_t AHB1RSTR;
	volatile uint32_t AHB2RSTR;
	volatile uint32_t AHB3RSTR;
	volatile uint32_t RESERVED1;
	volatile uint32_t APB1RSTR;
	volatile uint32_t APB2RSTR;
	volatile uint32_t RESERVED2;
	volatile uint32_t RESERVED3;
	volatile uint32_t AHB1ENR;
	volatile uint32_t AHB2ENR;
	volatile uint32_t AHB3ENR;
	volatile uint32_t RESERVED4;
	volatile uint32_t APB1ENR;
	volatile uint32_t APB2ENR;
	volatile uint32_t RESERVED5;
	volatile uint32_t RESERVED6;
	volatile uint32_t AHB1LPENR;
	volatile uint32_t AHB2LPENR;
	volatile uint32_t AHB3LPENR;
	volatile uint32_t RESERVED7;
	volatile uint32_t APB1LPENR;
	volatile uint32_t APB2LPENR;
	volatile uint32_t RESERVED8;
	volatile uint32_t RESERVED9;
	volatile uint32_t BDCR;
	volatile uint32_t CSR;
	volatile uint32_t RESERVED10;
	volatile uint32_t RESERVED11;
	volatile uint32_t SSCGR;
	volatile uint32_t PLLI2SCFGR;
};

#define STM32_PWR_BASE		0x40007000 /* APB1 */

struct stm32_pwr {
	volatile uint32_t CR;
	volatile uint32_t CSR;
};

#define STM32_FLASH_BASE	0x40023C00 /* AHB1 */

struct stm32_flash {
	volatile uint32_t ACR;
	volatile uint32_t KEY;
	volatile uint32_t OPTKEYR;
	volatile uint32_t SR;
	volatile uint32_t CR;
	volatile uint32_t OPTCR;
	volatile uint32_t OPTCR1;
};

#define PLL_M      8
#define PLL_N      336
#define PLL_P      2
#define PLL_Q      7

void reset_rcc(void)
{
	volatile struct stm32_rcc *rcc = (struct stm32_rcc*)STM32_RCC_BASE;

	/* Set HSION */
	rcc->CR |= (uint32_t)0x00000001;
	/* Reset CFGR */
	rcc->CFGR = 0x00000000;
	/* Reset HSEON, CSSON and PLLON */
	rcc->CR &= (uint32_t)0xFEF6FFFF;
	/* Reset PLLCFGR */
	rcc->PLLCFGR = 0x24003010;
	/* Reset HSEBYP */
	rcc->CR &= (uint32_t)0xFFFBFFFF;
	/* Disable all interrupts */
	rcc->CIR = 0x00000000;
}

void configure_clocks(void)
{
	volatile struct stm32_rcc *rcc = (struct stm32_rcc*)STM32_RCC_BASE;
	volatile struct stm32_pwr *pwr = (struct stm32_pwr*)STM32_PWR_BASE;
	volatile struct stm32_flash *flash = (struct stm32_flash*)STM32_FLASH_BASE;

	/* Enable HSE */
	rcc->CR |= 0x00010000;
	/* Wait till HSE is ready */
	while(!(rcc->CR & 0x00020000));

	/* Enable high performance mode, System frequency up to 168 MHz */
	rcc->APB1ENR |= 0x10000000;
	pwr->CR |= 0x00004000;

	/* HCLK  = SYSCLK / 1 */
	/* PCLK2 = HCLK   / 2 */
	/* PCLK1 = HCLK   / 4 */
	rcc->CFGR |= 0x00009400;

	/* Configure the main PLL */
	rcc->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) - 1) << 16) |
			(0x400000) | (PLL_Q << 24);

	/* Enable the main PLL */
	rcc->CR |= 0x01000000;

	/* Wait till the main PLL is ready */
	while(!(rcc->CR & 0x02000000));

	/* Configure Flash prefetch, Instruction cache, Data cache and wait state */
	flash->ACR = 0x00000605;

	/* Select the main PLL as system clock source */
	rcc->CFGR &= (uint32_t)~(0x00000003);
	rcc->CFGR |= 0x00000002;

	/* Wait till the main PLL is used as system clock source */
	while((rcc->CFGR & (uint32_t)0x0000000C) != 0x00000008);
}
