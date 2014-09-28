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

#define STM32_SPI4_BASE	0x40013400 /* APB2 */
#define RCC_SPI4_ENABLE	0x2000

struct stm32_spi {
	uint32_t CR1;
	uint32_t CR2;
	uint32_t SR;
	uint32_t DR;
	uint32_t CRCPR;
	uint32_t TXCRCR;
	uint32_t I2SCFGR;
	uint32_t I2SPR;
};

struct stm32_spi_slave {
	struct spi_slave slave;
	unsigned int bus;
	unsigned int cs;
	u8 op_mode_rx;
	u8 op_mode_tx;
	unsigned int wordlen;
	unsigned int max_write_size;
	void *memory_map;
	u8 option;
	u8 flags;
	u8 ss_pol;
};

static const struct stm32_gpio_dsc spi_gpio[] = {
		{STM32_GPIO_PORT_E, STM32_GPIO_PIN_2}, /* SCK */
		{STM32_GPIO_PORT_E, STM32_GPIO_PIN_5}, /* MISO */
		{STM32_GPIO_PORT_E, STM32_GPIO_PIN_6} /* MOSI */
};
static const struct stm32_gpio_dsc spi_ss_gpio = {
		STM32_GPIO_PORT_E, STM32_GPIO_PIN_4
};

void spi_init(void)
{

}

static int spi_cfg_stm32(struct stm32_spi_slave *stm32s,
		unsigned int cs, unsigned int max_hz, unsigned int mode)
{
	volatile struct stm32_spi* spi = (struct stm32_spi*)STM32_SPI4_BASE;

	if(mode & SPI_CPHA) {
		spi->CR1 |= 0x01;
	} else {
		spi->CR1 &= (uint16_t)(~0x01);
	}

	if(mode & SPI_CPOL) {
		spi->CR1 |= 0x02;
	} else {
		spi->CR1 &= (uint16_t)(~0x02);
	}

	if(mode & SPI_LSB_FIRST) {
		spi->CR1 |= 0x80;
	} else {
		spi->CR1 &= (uint16_t)(~0x80);
	}

	return 0;
}

int  spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return 1;
}

static inline struct stm32_spi_slave *to_stm32_spi_slave(struct spi_slave *slave)
{
	return container_of(slave, struct stm32_spi_slave, slave);
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
		unsigned int max_hz, unsigned int mode)
{
	struct stm32_spi_slave *stm32s;
	int ret;

	if(bus != 0)
		return NULL;

	stm32s = spi_alloc_slave(struct stm32_spi_slave, bus, cs);
	if (!stm32s) {
		puts("stm32_spi: SPI Slave not allocated !\n");
		return NULL;
	}

	stm32s->ss_pol = (mode & SPI_CS_HIGH) ? 1 : 0;

	ret = spi_cfg_stm32(stm32s, cs, max_hz, mode);
	if (ret) {
		printf("stm32_spi: cannot setup SPI controller\n");
		free(stm32s);
		return NULL;
	}
	return &stm32s->slave;
}

void spi_free_slave(struct spi_slave *slave)
{
	struct stm32_spi_slave *stm32s = to_stm32_spi_slave(slave);
	free(stm32s);
}

int spi_claim_bus(struct spi_slave *slave)
{
	volatile struct stm32_spi* spi = (struct stm32_spi*)STM32_SPI4_BASE;
	int i, rv;

	STM32_RCC->apb2enr |= RCC_SPI4_ENABLE;

	/* Enable and mux GPIOs */
	for (i = 0; i < ARRAY_SIZE(spi_gpio); i++) {
		rv = stm32_gpio_config(&spi_gpio[i], STM32_GPIO_ROLE_SPI4);
		if(rv) {
			return rv;
		}
	}

	rv = stm32_gpio_config(&spi_ss_gpio, STM32_GPIO_ROLE_GPOUT);
	if(rv) {
		return rv;
	}

	spi->CR1 = 0x30C;
	spi->CR1 |= 0x40;

	spi->I2SCFGR &= (uint16_t)(~0x0800);

	stm32_gpout_set(&spi_ss_gpio, 1);

	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
	/* TODO: Shut the controller down */
}

int  spi_xfer(struct spi_slave *slave, unsigned int bitlen, const void *dout,
		void *din, unsigned long flags)
{
	volatile struct stm32_spi* spi = (struct stm32_spi*)STM32_SPI4_BASE;
	struct stm32_spi_slave *as = to_stm32_spi_slave(slave);
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
		while((spi->SR & 0x02) == 0);
		spi->DR = value;
		while((spi->SR & 0x01) == 0);
		value = spi->DR;
		if (rxp)
			*rxp++ = value;
	}

	return 0;
}

void spi_cs_activate(struct spi_slave *slave)
{
	struct stm32_spi_slave *stm32s = to_stm32_spi_slave(slave);
	stm32_gpout_set(&spi_ss_gpio, 0);
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	struct stm32_spi_slave *stm32s = to_stm32_spi_slave(slave);
	stm32_gpout_set(&spi_ss_gpio, 1);
}

void spi_set_speed(struct spi_slave *slave, uint hz)
{
	volatile struct stm32_spi* spi = (struct stm32_spi*)STM32_SPI4_BASE;
	if(hz > 400000) {
		spi->CR1 &= (~0x38);
		spi->CR1 |= (0x1 << 3);
	} else {
		spi->CR1 &= (~0x38);
		spi->CR1 |= (0x7 << 3);
	}
}
