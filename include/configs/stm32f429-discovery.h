/*
 * (C) Copyright 2014
 * Kamil Lulko, <rev13@wp.pl>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_STM32F4DISCOVERY
#define CONFIG_SYS_GENERIC_BOARD

#define DEBUG

#define CONFIG_SYS_INIT_SP_ADDR	0x10010000
#define CONFIG_SYS_TEXT_BASE		0x08000000
#define CONFIG_SYS_SDRAM_BASE		0x20000000
#define CONFIG_SYS_LOAD_ADDR		0x20000000

#define CONFIG_SYS_MAX_FLASH_SECT	1024
#define CONFIG_SYS_MAX_FLASH_BANKS	1
#define CONFIG_ENV_SIZE			1024
#define CONFIG_NR_DRAM_BANKS		1

#define CONFIG_STM32_GPIO
#define CONFIG_STM32_SERIAL

#define CONFIG_STM32_USART1

#define CONFIG_SYS_ICACHE_OFF
#define CONFIG_SYS_DCACHE_OFF

#define CONFIG_ENV_IS_NOWHERE

#define CONFIG_SYS_CBSIZE		256	/* Console I/O Buffer Size	*/
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16) /* Print Buffer Size */

#define CONFIG_SYS_MAXARGS		8

#define CONFIG_SYS_MALLOC_LEN		512

#define CONFIG_STACKSIZE		(1024)

#define CONFIG_BAUDRATE			115200
#define CONFIG_BOOTARGS							\
	"console=ttyS0 root=/dev/mtdblock1 rootfstype=jffs2"
#define CONFIG_BOOTCOMMAND						\
	"fsload; bootm"

/*
 * Only interrupt autoboot if <space> is pressed. Otherwise, garbage
 * data on the serial line may interrupt the boot sequence.
 */
#define CONFIG_BOOTDELAY		1
#define CONFIG_AUTOBOOT
#define CONFIG_AUTOBOOT_KEYED
#define CONFIG_AUTOBOOT_PROMPT		\
	"Press SPACE to abort autoboot in %d seconds\n", bootdelay
#define CONFIG_AUTOBOOT_DELAY_STR	"d"
#define CONFIG_AUTOBOOT_STOP_STR	" "

/*
 * Command line configuration.
 */
#include <config_cmd_default.h>


#endif /* __CONFIG_H */
