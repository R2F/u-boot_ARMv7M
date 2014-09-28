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
#include <asm/arch/stm32.h>

#define RCC_CR_HSION	0x00000001
#define RCC_CR_HSEON	0x00010000
#define RCC_CR_HSERDY	0x00020000
#define RCC_CR_HSEBYP	0x00040000
#define RCC_CR_CSSON	0x00080000
#define RCC_CR_PLLON	0x01000000
#define RCC_CR_PLLRDY	0x02000000

#define RCC_PLLCFGR_PLLM_MASK	0x3F
#define RCC_PLLCFGR_PLLN_MASK	0x7FC0
#define RCC_PLLCFGR_PLLP_MASK	0x30000
#define RCC_PLLCFGR_PLLQ_MASK	0xF000000
#define RCC_PLLCFGR_PLLSRC	0x00400000

#define RCC_CFGR_AHB_PSC_MASK	0xF0
#define RCC_CFGR_APB1_PSC_MASK	0x1C00
#define RCC_CFGR_APB2_PSC_MASK	0xE000
#define RCC_CFGR_SW0	0x00000001
#define RCC_CFGR_SW1	0x00000002
#define RCC_CFGR_SW_MASK	0x3
#define RCC_CFGR_SW_HSI	0
#define RCC_CFGR_SW_HSE	RCC_CFGR_SW0
#define RCC_CFGR_SW_PLL	RCC_CFGR_SW1
#define RCC_CFGR_SWS0	0x00000004
#define RCC_CFGR_SWS1	0x00000008
#define RCC_CFGR_SWS_MASK	0xC
#define RCC_CFGR_SWS_HSI	0
#define RCC_CFGR_SWS_HSE	RCC_CFGR_SWS0
#define RCC_CFGR_SWS_PLL	RCC_CFGR_SWS1

#define RCC_APB1_PWREN	0x10000000

#define PWR_CR_VOS0	0x00004000
#define PWR_CR_VOS1	0x00008000
#define PWR_CR_VOS_MASK	0xC000
#define PWR_CR_VOS_SCALE_MODE_1	(PWR_CR_VOS0 | PWR_CR_VOS1)
#define PWR_CR_VOS_SCALE_MODE_2	(PWR_CR_VOS1)
#define PWR_CR_VOS_SCALE_MODE_3	(PWR_CR_VOS0)

struct pll_psc {
	u8 pll_m;
	u16 pll_n;
	u8 pll_p;
	u8 pll_q;
	u8 ahb_psc;
	u8 apb1_psc;
	u8 apb2_psc;
};

#define AHB_PSC_1	0
#define AHB_PSC_2	0x8
#define AHB_PSC_4	0x9
#define AHB_PSC_8	0xA
#define AHB_PSC_16	0xB
#define AHB_PSC_64	0xC
#define AHB_PSC_128	0xD
#define AHB_PSC_256	0xE
#define AHB_PSC_512	0xF

#define APB_PSC_1	0
#define APB_PSC_2	0x4
#define APB_PSC_4	0x5
#define APB_PSC_8	0x6
#define APB_PSC_16	0x7

#if !defined(CONFIG_STM32_HSE_HZ)
#error "CONFIG_STM32_HSE_HZ not defined!"
#else
#if (CONFIG_STM32_HSE_HZ == 8000000)
struct pll_psc pll_psc_168 = {
	.pll_m = 8,
	.pll_n = 336,
	.pll_p = 2,
	.pll_q = 7,
	.ahb_psc = AHB_PSC_1,
	.apb1_psc = APB_PSC_4,
	.apb2_psc = APB_PSC_2
};
#else
#error "No PLL/Prescaler configuration for given CONFIG_STM32_HSE_HZ exists"
#endif
#endif

int configure_clocks(void)
{
	/* Reset RCC configuration */
	STM32_RCC->cr |= RCC_CR_HSION;
	STM32_RCC->cfgr = 0; /* Reset CFGR */
	STM32_RCC->cr &= ~(RCC_CR_HSEON | RCC_CR_CSSON | RCC_CR_PLLON);
	STM32_RCC->pllcfgr = 0x24003010; /* Reset PLLCFGR, value from RM */
	STM32_RCC->cr &= ~RCC_CR_HSEBYP;
	STM32_RCC->cir = 0; /* Disable all interrupts */

	/* Configure for HSE+PLL operation */
	STM32_RCC->cr |= RCC_CR_HSEON;
	while(!(STM32_RCC->cr & RCC_CR_HSERDY));

	/* Enable high performance mode, System frequency up to 168 MHz */
	STM32_RCC->apb1enr |= RCC_APB1_PWREN;
	STM32_PWR->cr = PWR_CR_VOS_SCALE_MODE_1;

	STM32_RCC->cfgr |= ((pll_psc_168.ahb_psc << 4)
			| (pll_psc_168.apb1_psc << 10)
			| (pll_psc_168.apb2_psc << 13));

	STM32_RCC->pllcfgr = pll_psc_168.pll_m
			| (pll_psc_168.pll_n << 6)
			| (((pll_psc_168.pll_p >> 1) - 1) << 16)
			| (pll_psc_168.pll_q << 24);
	STM32_RCC->pllcfgr |= RCC_PLLCFGR_PLLSRC;

	STM32_RCC->cr |= RCC_CR_PLLON;

	while(!(STM32_RCC->cr & RCC_CR_PLLRDY));

	/* 5 wait states, D-Cache enabled, I-Cache enabled */
	STM32_FLASH->acr = 0x00000605;

	STM32_RCC->cfgr &= ~(RCC_CFGR_SW0 | RCC_CFGR_SW1);
	STM32_RCC->cfgr |= RCC_CFGR_SW_PLL;

	while((STM32_RCC->cfgr & RCC_CFGR_SWS_MASK) != RCC_CFGR_SWS_PLL);

	return 0;
}

unsigned long clock_get(enum clock clck)
{
	u32 sysclk = 0;
	u32 shift = 0;
	/* Prescaler table lookups for clock computation */
	u8 ahb_psc_table[16] =
	{0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
	u8 apb_psc_table[8] =
	{0, 0, 0, 0, 1, 2, 3, 4};

	if((STM32_RCC->cfgr & RCC_CFGR_SWS_MASK) == RCC_CFGR_SWS_PLL) {
		u16 pllm, plln, pllp;
		pllm = (STM32_RCC->pllcfgr & RCC_PLLCFGR_PLLM_MASK);
		plln = ((STM32_RCC->pllcfgr & RCC_PLLCFGR_PLLN_MASK) >> 6);
		pllp = ((((STM32_RCC->pllcfgr & RCC_PLLCFGR_PLLP_MASK) >> 16) + 1) << 1);
		sysclk = ((CONFIG_STM32_HSE_HZ / pllm) * plln) / pllp;
	}

	switch(clck) {
	case CLOCK_CORE:
		return sysclk;
		break;
	case CLOCK_AHB:
		shift = ahb_psc_table[((STM32_RCC->cfgr & RCC_CFGR_AHB_PSC_MASK) >> 4)];
		return sysclk >>= shift;
		break;
	case CLOCK_APB1:
		shift = apb_psc_table[((STM32_RCC->cfgr & RCC_CFGR_APB1_PSC_MASK) >> 10)];
		return sysclk >>= shift;
		break;
	case CLOCK_APB2:
		shift = apb_psc_table[((STM32_RCC->cfgr & RCC_CFGR_APB2_PSC_MASK) >> 13)];
		return sysclk >>= shift ;
		break;
	case CLOCK_SYSTICK:
		return sysclk / 8;
		break;
	default:
		return 0;
		break;
	}
}
