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
#include <linux/types.h>
#include <spi.h>
#include <asm/arch/stm32_gpio.h>

extern const struct stm32_gpio_dsc lcd_wrx_gpio;

#define LCD_SLEEP_OUT            0x11   /* Sleep out register */
#define LCD_GAMMA                0x26   /* Gamma register */
#define LCD_DISPLAY_OFF          0x28   /* Display off register */
#define LCD_DISPLAY_ON           0x29   /* Display on register */
#define LCD_COLUMN_ADDR          0x2A   /* Colomn address register */
#define LCD_PAGE_ADDR            0x2B   /* Page address register */
#define LCD_GRAM                 0x2C   /* GRAM register */
#define LCD_MAC                  0x36   /* Memory Access Control register*/
#define LCD_PIXEL_FORMAT         0x3A   /* Pixel Format register */
#define LCD_WDB                  0x51   /* Write Brightness Display register */
#define LCD_WCD                  0x53   /* Write Control Display register*/
#define LCD_RGB_INTERFACE        0xB0   /* RGB Interface Signal Control */
#define LCD_FRC                  0xB1   /* Frame Rate Control register */
#define LCD_BPC                  0xB5   /* Blanking Porch Control register*/
#define LCD_DFC                  0xB6   /* Display Function Control register*/
#define LCD_POWER1               0xC0   /* Power Control 1 register */
#define LCD_POWER2               0xC1   /* Power Control 2 register */
#define LCD_VCOM1                0xC5   /* VCOM Control 1 register */
#define LCD_VCOM2                0xC7   /* VCOM Control 2 register */
#define LCD_POWERA               0xCB   /* Power control A register */
#define LCD_POWERB               0xCF   /* Power control B register */
#define LCD_PGAMMA               0xE0   /* Positive Gamma Correction register*/
#define LCD_NGAMMA               0xE1   /* Negative Gamma Correction register*/
#define LCD_DTCA                 0xE8   /* Driver timing control A */
#define LCD_DTCB                 0xEA   /* Driver timing control B */
#define LCD_POWER_SEQ            0xED   /* Power on sequence register */
#define LCD_3GAMMA_EN            0xF2   /* 3 Gamma enable register */
#define LCD_INTERFACE            0xF6   /* Interface control register */
#define LCD_PRC                  0xF7   /* Pump ratio control register */

static struct spi_slave *ili9341_spi;

static void ili9341_write_cmd(uint8_t cmd)
{
	/* Reset WRX to send command */
	stm32_gpout_set(&lcd_wrx_gpio, 0);
	/* Reset LCD control line(/CS) and Send command */
	spi_cs_activate(ili9341_spi);
	spi_xfer(ili9341_spi, 8, &cmd, NULL, SPI_XFER_BEGIN | SPI_XFER_END);
	spi_cs_deactivate(ili9341_spi);
}

static void ili9341_write_data(uint8_t data)
{
	/* Set WRX to send data */
	stm32_gpout_set(&lcd_wrx_gpio, 1);
	/* Reset LCD control line(/CS) and Send data */
	spi_cs_activate(ili9341_spi);
	spi_xfer(ili9341_spi, 8, &data, NULL, SPI_XFER_BEGIN | SPI_XFER_END);
	spi_cs_deactivate(ili9341_spi);
}

static void ili9341_poweron(void)
{
	ili9341_write_cmd(0xCA);
	ili9341_write_data(0xC3);
	ili9341_write_data(0x08);
	ili9341_write_data(0x50);
	ili9341_write_cmd(LCD_POWERB);
	ili9341_write_data(0x00);
	ili9341_write_data(0xC1);
	ili9341_write_data(0x30);
	ili9341_write_cmd(LCD_POWER_SEQ);
	ili9341_write_data(0x64);
	ili9341_write_data(0x03);
	ili9341_write_data(0x12);
	ili9341_write_data(0x81);
	ili9341_write_cmd(LCD_DTCA);
	ili9341_write_data(0x85);
	ili9341_write_data(0x00);
	ili9341_write_data(0x78);
	ili9341_write_cmd(LCD_POWERA);
	ili9341_write_data(0x39);
	ili9341_write_data(0x2C);
	ili9341_write_data(0x00);
	ili9341_write_data(0x34);
	ili9341_write_data(0x02);
	ili9341_write_cmd(LCD_PRC);
	ili9341_write_data(0x20);
	ili9341_write_cmd(LCD_DTCB);
	ili9341_write_data(0x00);
	ili9341_write_data(0x00);
	ili9341_write_cmd(LCD_FRC);
	ili9341_write_data(0x00);
	ili9341_write_data(0x1B);
	ili9341_write_cmd(LCD_DFC);
	ili9341_write_data(0x0A);
	ili9341_write_data(0xA2);
	ili9341_write_cmd(LCD_POWER1);
	ili9341_write_data(0x10);
	ili9341_write_cmd(LCD_POWER2);
	ili9341_write_data(0x10);
	ili9341_write_cmd(LCD_VCOM1);
	ili9341_write_data(0x45);
	ili9341_write_data(0x15);
	ili9341_write_cmd(LCD_VCOM2);
	ili9341_write_data(0x90);
	ili9341_write_cmd(LCD_MAC);
	ili9341_write_data(0xC8);
	ili9341_write_cmd(LCD_3GAMMA_EN);
	ili9341_write_data(0x00);
	ili9341_write_cmd(LCD_RGB_INTERFACE);
	ili9341_write_data(0xC2);
	ili9341_write_cmd(LCD_DFC);
	ili9341_write_data(0x0A);
	ili9341_write_data(0xA7);
	ili9341_write_data(0x27);
	ili9341_write_data(0x04);

	/* column address set */
	ili9341_write_cmd(LCD_COLUMN_ADDR);
	ili9341_write_data(0x00);
	ili9341_write_data(0x00);
	ili9341_write_data(0x00);
	ili9341_write_data(0xEF);
	/* Page Address Set */
	ili9341_write_cmd(LCD_PAGE_ADDR);
	ili9341_write_data(0x00);
	ili9341_write_data(0x00);
	ili9341_write_data(0x01);
	ili9341_write_data(0x3F);
	ili9341_write_cmd(LCD_INTERFACE);
	ili9341_write_data(0x01);
	ili9341_write_data(0x00);
	ili9341_write_data(0x06);

	ili9341_write_cmd(LCD_GRAM);
	mdelay(200);

	ili9341_write_cmd(LCD_GAMMA);
	ili9341_write_data(0x01);

	ili9341_write_cmd(LCD_PGAMMA);
	ili9341_write_data(0x0F);
	ili9341_write_data(0x29);
	ili9341_write_data(0x24);
	ili9341_write_data(0x0C);
	ili9341_write_data(0x0E);
	ili9341_write_data(0x09);
	ili9341_write_data(0x4E);
	ili9341_write_data(0x78);
	ili9341_write_data(0x3C);
	ili9341_write_data(0x09);
	ili9341_write_data(0x13);
	ili9341_write_data(0x05);
	ili9341_write_data(0x17);
	ili9341_write_data(0x11);
	ili9341_write_data(0x00);
	ili9341_write_cmd(LCD_NGAMMA);
	ili9341_write_data(0x00);
	ili9341_write_data(0x16);
	ili9341_write_data(0x1B);
	ili9341_write_data(0x04);
	ili9341_write_data(0x11);
	ili9341_write_data(0x07);
	ili9341_write_data(0x31);
	ili9341_write_data(0x33);
	ili9341_write_data(0x42);
	ili9341_write_data(0x05);
	ili9341_write_data(0x0C);
	ili9341_write_data(0x0A);
	ili9341_write_data(0x28);
	ili9341_write_data(0x2F);
	ili9341_write_data(0x0F);

	ili9341_write_cmd(LCD_SLEEP_OUT);
	mdelay(200);
	ili9341_write_cmd(LCD_DISPLAY_ON);
	/* GRAM start writing */
	ili9341_write_cmd(LCD_GRAM);
}

int ili9341_init(void)
{
	ili9341_spi = spi_setup_slave(4, 0, 5600000, SPI_MODE_0);
	if (!ili9341_spi) {
		return 1;
	}
	spi_claim_bus(ili9341_spi);
	ili9341_poweron();

	return 0;
}
