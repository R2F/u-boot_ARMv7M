/*
 * (C) Copyright 2014
 * Kamil Lulko, <rev13@wp.pl>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>

/* This is filled with stubs for now */

DECLARE_GLOBAL_DATA_PTR;

int dram_init(void)
{
	gd->ram_size = 0x30000;
	return 0;
}

unsigned long flash_init (void)
{
	return 0x200000;
}

int flash_erase(flash_info_t * a, int b, int c)
{
	return 0;
}

void flash_print_info (flash_info_t *o)
{
	return;
}

int write_buff (flash_info_t *info, uchar *src, ulong addr, ulong cnt)
{
	return 0;
}

flash_info_t flash_info[CONFIG_SYS_MAX_FLASH_BANKS];

int board_init(void)
{
	return 0;
}

int board_late_init(void)
{
	return 0;
}
