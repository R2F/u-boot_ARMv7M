/*
 * (C) Copyright 2014
 * Kamil Lulko, <rev13@wp.pl>
 *
 * (C) Copyright 2014
 * STMicroelectronics
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <config.h>
#include <common.h>
#include <version.h>
#include <stdarg.h>
#include <linux/types.h>
#include <stdio_dev.h>
#include <lcd.h>
#include <asm/arch/stm32.h>

#define STM32_LTDC_BASE	0x40016800 /* APB2 */

struct stm32_ltdc {
	u32 reserved0[2];
	u32 sscr;
	u32 bpcr;
	u32 awcr;
	u32 twcr;
	u32 gcr;
	u32 reserved1[2];
	u32 srcr;
	u32 reserved2[1];
	u32 bccr;
	u32 reserved3[1];
	u32 ier;
	u32 isr;
	u32 icr;
	u32 lipcr;
	u32 cpsr;
	u32 cdsr;
	u32 reserved4[14];
	u32 l1cr;
	u32 l1whpcr;
	u32 l1wvpcr;
	u32 l1ckcr;
	u32 l1pfcr;
	u32 l1cacr;
	u32 l1dccr;
	u32 l1bfcr;
	u32 reserved5[2];
	u32 l1cfbar;
	u32 l1cfblr;
	u32 l1cfblnr;
	u32 reserved6[3];
	u32 l1clutwr;
	u32 reserved7[15];
	u32 l2cr;
	u32 l2whpcr;
	u32 l2wvpcr;
	u32 l2ckcr;
	u32 l2pfcr;
	u32 l2cacr;
	u32 l2dccr;
	u32 l2bfcr;
	u32 reserved8[2];
	u32 l2cfbar;
	u32 l2cfblr;
	u32 l2cfblnr;
	u32 reserved9[3];
	u32 l2clutwr;
};

vidinfo_t panel_info = {
		.vl_col = 240,
		.vl_row = 320,
		.vl_bpix = LCD_BPP
};

DECLARE_GLOBAL_DATA_PTR;

void lcd_ctrl_init(void *lcdbase)
{
	volatile struct stm32_ltdc *ltdc = (struct stm32_ltdc*)STM32_LTDC_BASE;

	STM32_RCC->apb2enr |= (1 << 26); /* ltdc en */

	STM32_RCC->pllsaicfgr = 0x40003000;
	STM32_RCC->dckcfgr |= (1 << 17);
	STM32_RCC->cr |= (1 << 28);
	while((STM32_RCC->cr & (1 << 29)) == 0);

	ltdc->sscr = (9 << 16) | 1;
	ltdc->bpcr = (29 << 16) | 3;
	ltdc->awcr = (269 << 16) | 323;
	ltdc->twcr = (279 << 16) | 327;

	ltdc->bccr = 0;

	ltdc->l1whpcr = 30 | ((30 + 240 - 1) << 16);
	ltdc->l1wvpcr = 4 | ((4 + 320 - 1) << 16);
	ltdc->l1pfcr = 2;
	ltdc->l1cacr = 255;
	ltdc->l1dccr = 0x0;
	ltdc->l1bfcr = 0x405;
	ltdc->l1cfbar = gd->fb_base;
	ltdc->l1cfblr = ((320 * 2) + 3) | ((320 * 2) << 16);
	ltdc->l1cfblnr = 240;
	ltdc->l1cr |= 1;

	ltdc->srcr |= 1;

	ltdc->gcr |= (1 << 0);
}

void lcd_enable(void)
{

}

void lcd_setcolreg(ushort regno, ushort red, ushort green, ushort blue)
{

}
