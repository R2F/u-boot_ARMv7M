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

/*#define DEBUG*/

#define CONFIG_SYS_INIT_SP_ADDR	0x10010000
#define CONFIG_SYS_TEXT_BASE		0x08000000
/*
 * Configuration of the external SDRAM memory
 */
#define CONFIG_NR_DRAM_BANKS		1
#define CONFIG_SYS_RAM_SIZE		(8 * 1024 * 1024)
#define CONFIG_SYS_RAM_CS		1
#define CONFIG_SYS_RAM_FREQ_DIV		2
#define CONFIG_SYS_RAM_BASE		0xD0000000
#define CONFIG_SYS_SDRAM_BASE		CONFIG_SYS_RAM_BASE
#define CONFIG_SYS_LOAD_ADDR		0xD0000000
#define CONFIG_LOADADDR		0xD0000000

#define CONFIG_SYS_MAX_FLASH_SECT	1024
#define CONFIG_SYS_MAX_FLASH_BANKS	1
#define CONFIG_ENV_SIZE			1024

#define CONFIG_STM32_GPIO
#define CONFIG_STM32_SERIAL
#define CONFIG_STM32_SPI
#define CONFIG_MMC
#define CONFIG_MMC_SPI
#define CONFIG_GENERIC_MMC

#define CONFIG_DOS_PARTITION

#define CONFIG_STM32_USART1

#define CONFIG_SYS_ICACHE_OFF
#define CONFIG_SYS_DCACHE_OFF

#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_REVISION_TAG

#define CONFIG_STM32_HSE_HZ	8000000

/*
 * Number of clock ticks in 1 sec
 */
#define CONFIG_SYS_HZ			1000

#define CONFIG_ENV_IS_NOWHERE

#define CONFIG_SYS_CBSIZE		1024	/* Console I/O Buffer Size	*/
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16) /* Print Buffer Size */

#define CONFIG_SYS_MAXARGS		16

#define CONFIG_SYS_MALLOC_LEN		(4 * 1024 * 1024)

#define CONFIG_STACKSIZE		(64 * 1024)

#define CONFIG_BAUDRATE			115200
#define CONFIG_BOOTARGS							\
	"console=ttyS0 root=/dev/mtdblock1 rootfstype=jffs2"
#define CONFIG_BOOTCOMMAND						\
	"mmc_spi 0; ext4load mmc 0 ${loadaddr} /xipImage; go 0xD0000001"

/*
 * Only interrupt autoboot if <space> is pressed. Otherwise, garbage
 * data on the serial line may interrupt the boot sequence.
 */
#define CONFIG_BOOTDELAY		3
#define CONFIG_AUTOBOOT

/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT	       "U-Boot > "
#define CONFIG_AUTO_COMPLETE
#define CONFIG_CMDLINE_EDITING

#define CONFIG_CMD_MISC
#define CONFIG_CMD_TIMER
#define CONFIG_CMD_FAT
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_EXT4
#define CONFIG_CMD_SPI
#define CONFIG_CMD_MMC
#define CONFIG_CMD_MMC_SPI
#define HAVE_BLOCK_DEVICE


#endif /* __CONFIG_H */
