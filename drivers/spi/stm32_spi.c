/*
 * (C) Copyright 2014
 * Kamil Lulko, <rev13@wp.pl>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <malloc.h>
#include <asm/arch/stm32_gpio.h>
#include <asm/arch/stm32.h>
#include <spi.h>

#define STM32_SPI1_BASE	0x40013000 /* APB2 */
#define STM32_SPI2_BASE	0x40003800 /* APB1 */
#define STM32_SPI3_BASE	0x40003C00 /* APB1 */
#define STM32_SPI4_BASE	0x40013400 /* APB2 */
#define STM32_SPI5_BASE	0x40015000 /* APB2 */
#define STM32_SPI6_BASE	0x40015400 /* APB2 */

static const uint32_t spi_bases[] = {
	STM32_SPI1_BASE, STM32_SPI2_BASE, STM32_SPI3_BASE,
	STM32_SPI4_BASE, STM32_SPI5_BASE, STM32_SPI6_BASE
};

#define RCC_SPI1_ENABLE	(1 << 12)
#define RCC_SPI2_ENABLE	(1 << 14)
#define RCC_SPI3_ENABLE	(1 << 15)
#define RCC_SPI4_ENABLE	(1 << 13)
#define RCC_SPI5_ENABLE	(1 << 20)
#define RCC_SPI6_ENABLE	(1 << 21)

static const uint32_t spi_rcc_en[] = {
	RCC_SPI1_ENABLE, RCC_SPI2_ENABLE, RCC_SPI3_ENABLE,
	RCC_SPI4_ENABLE, RCC_SPI5_ENABLE, RCC_SPI6_ENABLE
};

/* To have board specific CS pin list without having to touch this driver
 * the CS arrays are declared as weak symbols. Define your own arrays
 * in your board.c
 */
struct stm32_gpio_dsc spi1_cs_gpio[] __attribute__((weak)) = {{-1, -1}};
struct stm32_gpio_dsc spi2_cs_gpio[] __attribute__((weak)) = {{-1, -1}};
struct stm32_gpio_dsc spi3_cs_gpio[] __attribute__((weak)) = {{-1, -1}};
struct stm32_gpio_dsc spi4_cs_gpio[] __attribute__((weak)) = {{-1, -1}};
struct stm32_gpio_dsc spi5_cs_gpio[] __attribute__((weak)) = {{-1, -1}};
struct stm32_gpio_dsc spi6_cs_gpio[] __attribute__((weak)) = {{-1, -1}};

static const struct stm32_gpio_dsc *spi_cs_array[] = {
		spi1_cs_gpio, spi2_cs_gpio, spi3_cs_gpio,
		spi4_cs_gpio, spi5_cs_gpio, spi6_cs_gpio
};

struct stm32_spi {
	uint32_t cr1;
	uint32_t cr2;
	uint32_t sr;
	uint32_t dr;
	uint32_t crcpr;
	uint32_t txcrcr;
	uint32_t i2scfgr;
	uint32_t i2spr;
};

struct stm32_spi_slave {
	struct spi_slave slave;
	u8 cs_pol;
};

#define SPI_CR1_CPHA	(1 << 0)
#define SPI_CR1_CPOL	(1 << 1)
#define SPI_CR1_MSTR	(1 << 2)
#define SPI_CR1_BR0	(1 << 3)
#define SPI_CR1_BR1	(1 << 4)
#define SPI_CR1_BR2	(1 << 5)
#define SPI_CR1_BR_SHIFT	3
#define SPI_CR1_BR_MASK	(7 << SPI_CR1_BR_SHIFT)
#define SPI_CR1_SPE	(1 << 6)
#define SPI_CR1_LSBFIRST	(1 << 7)
#define SPI_CR1_SSI	(1 << 8)
#define SPI_CR1_SSM	(1 << 9)
#define SPI_CR1_RXONLY	(1 << 10)
#define SPI_CR1_DFF	(1 << 11)
#define SPI_CR1_CRCNEXT	(1 << 12)
#define SPI_CR1_CRCEN	(1 << 13)
#define SPI_CR1_BIDIOE	(1 << 14)
#define SPI_CR1_BIDIMODE	(1 << 15)

#define SPI_SR_RXNE	(1 << 0)
#define SPI_SR_TXE	(1 << 1)

#define SPI_I2SCFGR_I2SMOD	(1 << 11)

void spi_init(void)
{

}

static int spi_cfg_stm32(struct spi_slave *slave,
		unsigned int cs, unsigned int max_hz, unsigned int mode)
{
	volatile struct stm32_spi* spi =
			(struct stm32_spi*)spi_bases[slave->bus];

	/* SPI2 and SPI3 are on the APB1 */
	if(slave->bus == 1 || slave->bus == 2) {
		STM32_RCC->apb1enr |= spi_rcc_en[slave->bus];
	} else {
		STM32_RCC->apb2enr |= spi_rcc_en[slave->bus];
	}

	spi->cr1 = SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM;
	spi->i2scfgr &= (uint16_t)(~SPI_I2SCFGR_I2SMOD);

	if(mode & SPI_CPHA) {
		spi->cr1 |= SPI_CR1_CPHA;
	} else {
		spi->cr1 &= (uint16_t)(~SPI_CR1_CPHA);
	}

	if(mode & SPI_CPOL) {
		spi->cr1 |= SPI_CR1_CPOL;
	} else {
		spi->cr1 &= (uint16_t)(~SPI_CR1_CPOL);
	}

	if(mode & SPI_LSB_FIRST) {
		spi->cr1 |= SPI_CR1_LSBFIRST;
	} else {
		spi->cr1 &= (uint16_t)(~SPI_CR1_LSBFIRST);
	}

	spi_set_speed(slave, max_hz);

	return 0;
}

int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	unsigned int i = 0;

	while(spi_cs_array[bus][i].pin != -1 && spi_cs_array[bus][i].port != -1) {
		i++;
	}

	if((cs + 1) > i) {
		return 0;
	} else {
		return 1;
	}
}

static inline struct stm32_spi_slave *to_stm32_spi_slave(struct spi_slave *slave)
{
	return container_of(slave, struct stm32_spi_slave, slave);
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
		unsigned int max_hz, unsigned int mode)
{
	struct stm32_spi_slave *stm32_slave;
	int ret;

	if(bus > 5)
		return NULL;

	if(!spi_cs_is_valid(bus, cs)) {
		return NULL;
	}

	stm32_slave = spi_alloc_slave(struct stm32_spi_slave, bus, cs);
	if (!stm32_slave) {
		puts("stm32_spi: SPI Slave not allocated !\n");
		return NULL;
	}

	stm32_slave->cs_pol = (mode & SPI_CS_HIGH) ? 1 : 0;

	ret = spi_cfg_stm32(&stm32_slave->slave, cs, max_hz, mode);
	if (ret) {
		printf("stm32_spi: cannot setup SPI controller\n");
		free(stm32_slave);
		return NULL;
	}

	return &stm32_slave->slave;
}

void spi_free_slave(struct spi_slave *slave)
{
	struct stm32_spi_slave *stm32_slave = to_stm32_spi_slave(slave);

	free(stm32_slave);
}

int spi_claim_bus(struct spi_slave *slave)
{
	volatile struct stm32_spi* spi =
			(struct stm32_spi*)spi_bases[slave->bus];

	spi->cr1 |= SPI_CR1_SPE;

	spi_cs_deactivate(slave);

	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
	/* TODO: Shut the controller down */
}

int  spi_xfer(struct spi_slave *slave, unsigned int bitlen, const void *dout,
		void *din, unsigned long flags)
{
	volatile struct stm32_spi* spi =
			(struct stm32_spi*)spi_bases[slave->bus];
	unsigned int	len;
	const u8	*txp = dout;
	u8 *rxp = din;
	u8 value;

	if (bitlen == 0) {
		return 0;
	}

	for (len = bitlen / 8; len > 0; len--) {
		if (txp)
			value = *txp++;
		else
			value = 0xFF;
		while((spi->sr & SPI_SR_TXE) == 0);
		spi->dr = value;
		while((spi->sr & SPI_SR_RXNE) == 0);
		value = spi->dr;
		if (rxp)
			*rxp++ = value;
	}

	return 0;
}

void spi_cs_activate(struct spi_slave *slave)
{
	struct stm32_spi_slave *stm32_slave = to_stm32_spi_slave(slave);

	if(stm32_slave->cs_pol == 0) {
		stm32_gpout_set(&spi_cs_array[slave->bus][slave->cs], 0);
	} else {
		stm32_gpout_set(&spi_cs_array[slave->bus][slave->cs], 1);
	}
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	struct stm32_spi_slave *stm32_slave = to_stm32_spi_slave(slave);

	if(stm32_slave->cs_pol == 0) {
		stm32_gpout_set(&spi_cs_array[slave->bus][slave->cs], 1);
	} else {
		stm32_gpout_set(&spi_cs_array[slave->bus][slave->cs], 0);
	}
}

void spi_set_speed(struct spi_slave *slave, uint hz)
{
	volatile struct stm32_spi* spi =
			(struct stm32_spi*)spi_bases[slave->bus];
	int apb_clk, i;

	if(slave->bus == 1 || slave->bus == 2) {
		apb_clk = clock_get(CLOCK_APB1);
	} else {
		apb_clk = clock_get(CLOCK_APB2);
	}

	spi->cr1 &= (~SPI_CR1_BR_MASK);

	for(i = 0; i < 8; i++) {
		int spi_clk = apb_clk / (1 << (i + 1));
		if(spi_clk <= hz) {
			spi->cr1 |= (i << SPI_CR1_BR_SHIFT);
			printf("stm32_spi: spi %d clock set to %d\n", slave->bus, spi_clk);
			break;
		}
	}
}
