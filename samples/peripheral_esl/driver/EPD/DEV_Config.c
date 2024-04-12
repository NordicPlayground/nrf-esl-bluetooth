/*****************************************************************************
 * |File		:	DEV_Config.c
 * |Author		:	Waveshare team
 *				Nordic Semiconductor
 * |Function		:	Hardware underlying interface
 * |Info		:
 * ------------------------------------------------------------------------------
 * |This version	:	V3.1
 * |Date		:	2024-03-25
 * |Info		:	Adapted for Nordic Semiconductor nRF52
 *
 *#
 *# Permission is hereby granted, free of charge, to any person obtaining a copy
 *# of this software and associated documnetation files (the "Software"), to deal
 *# in the Software without restriction, including without limitation the rights
 *# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *# copies of theex Software, and to permit persons to  whom the Software is
 *# furished to do so, subject to the following conditions:
 *#
 *# The above copyright notice and this permission notice shall be included in
 *# all copies or substantial portions of the Software.
 *#
 *# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *# THE SOFTWARE.
 *#
 ****************************************************************************/

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>

#include "DEV_Config.h"
#define LOG_LEVEL LOG_LEVEL_DBG
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(DEV_Config);

#define DT_DRV_COMPAT generic_epd

/* SPI */
static const struct device *epaper_spi;

/* GPIO PIN */
/* TODO: Get gpio dt from dts */

const EPD_Pin CS_PIN = {.gpio = GPIO_DT_SPEC_GET(DT_PARENT(DT_DRV_INST(0)), cs_gpios)};
const EPD_Pin RST_PIN = {.gpio = GPIO_DT_SPEC_INST_GET(0, reset_gpios)};
const EPD_Pin DC_PIN = {.gpio = GPIO_DT_SPEC_INST_GET(0, dc_gpios)};
const EPD_Pin BUSY_PIN = {.gpio = GPIO_DT_SPEC_INST_GET(0, busy_gpios)};
const EPD_Pin PWR_PIN = {.gpio = GPIO_DT_SPEC_INST_GET_OR(0, vcc_gpios, {})};
EPD_Pin pins[5];
struct spi_cs_control epd_spi_cs_pin = {
	.gpio = GPIO_DT_SPEC_GET(DT_PARENT(DT_DRV_INST(0)), cs_gpios),
	.delay = 0,
};

struct spi_config epaper_spi_config;

/**
 * GPIO read and write
 **/
void DEV_Digital_Write(UWORD Pin, UBYTE Value)
{
	if (Value == HIGH) {
		gpio_pin_set_dt(&pins[Pin].gpio, 1);
	} else {
		gpio_pin_set_dt(&pins[Pin].gpio, 0);
	}
}

UBYTE DEV_Digital_Read(UWORD Pin)
{
	int value;

	value = gpio_pin_get_dt(&pins[Pin].gpio);
	if (value < 0) {
		LOG_ERR("Read gpio %d %d", pins[Pin].gpio.pin, value);
		return value;
	}

	if (value) {
		return HIGH;
	} else {
		return LOW;
	}
}

/**
 * SPI
 **/
void DEV_SPI_WriteByte(uint8_t Value)
{
	struct spi_buf tx_buf = {.buf = &Value, .len = 1};
	struct spi_buf_set tx_bufs = {.buffers = &tx_buf, .count = 1};
	int spi_status = spi_write(epaper_spi, &epaper_spi_config, &tx_bufs);

	if (spi_status < 0) {
		LOG_ERR("SPI transceive error: %d\r\n", spi_status);
	}
}

void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len)
{
	/* TODO: IMPLEMENT IT */
}

/**
 * delay x ms
 **/
void DEV_Delay_ms(UDOUBLE xms)
{
	k_msleep(xms);
}

static void dump_pin(uint8_t pin_id)
{
	switch (pin_id) {
	case EPD_CS_PIN:
		printk("EPD_CS_PIN\n");
		break;
	case EPD_RST_PIN:
		printk("EPD_RST_PIN\n");
		break;
	case EPD_DC_PIN:
		printk("EPD_DC_PIN\n");
		break;
	case EPD_BUSY_PIN:
		printk("EPD_BUSY_PIN\n");
		break;
	case EPD_PWR_PIN:
		printk("EPD_PWR_PIN\n");
		break;
	default:
		printk("Unknown pin %d\n", pin_id);
		break;
	}

	printk("pin %d: %p %d\n", pin_id, pins[pin_id].gpio.port, pins[pin_id].gpio.pin);
}
void DEV_GPIO_Init(void)
{
	pins[EPD_CS_PIN] = CS_PIN;
	dump_pin(EPD_CS_PIN);
	pins[EPD_RST_PIN] = RST_PIN;
	dump_pin(EPD_RST_PIN);
	pins[EPD_DC_PIN] = DC_PIN;
	dump_pin(EPD_DC_PIN);
	pins[EPD_BUSY_PIN] = BUSY_PIN;
	dump_pin(EPD_BUSY_PIN);
	pins[EPD_PWR_PIN] = PWR_PIN;
	dump_pin(EPD_PWR_PIN);

	gpio_pin_configure_dt(&pins[EPD_RST_PIN].gpio, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&pins[EPD_DC_PIN].gpio, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&pins[EPD_BUSY_PIN].gpio, GPIO_INPUT);

	if (PWR_PIN.gpio.port) {
		gpio_pin_configure_dt(&pins[EPD_PWR_PIN].gpio, GPIO_OUTPUT);
		DEV_Digital_Write(EPD_PWR_PIN, HIGH);
	}

	DEV_Digital_Write(EPD_DC_PIN, LOW);
	DEV_Digital_Write(EPD_RST_PIN, LOW);
	DEV_Delay_ms(10);
	DEV_Digital_Write(EPD_RST_PIN, HIGH);
}
/******************************************************************************
 * function:	Module Initialize, the library and initialize the pins, SPI protocol
 * parameter:
 * Info:
 *****************************************************************************/
UBYTE DEV_Module_Init(void)
{
	printk("/***********************************/init \r\n");

	/* GPIO Config  */
	DEV_GPIO_Init();
	/* Hardcoded for now, make sure 4in2 v1 working then get pin from devicetree */

	epaper_spi = DEVICE_DT_GET(DT_PARENT(DT_DRV_INST(0)));
	printk("epaper_spi %p \r\n", epaper_spi);

	if (!epaper_spi) {
		LOG_ERR("no epaper_spi");
	}

	epaper_spi_config.frequency = MIN(DT_PROP(DT_PARENT(DT_DRV_INST(0)), max_frequency),
					  DT_PROP(DT_DRV_INST(0), spi_max_frequency));
	epaper_spi_config.operation =
		SPI_WORD_SET(8) | SPI_MODE_CPOL | SPI_MODE_CPHA | SPI_OP_MODE_MASTER;
	epaper_spi_config.cs = epd_spi_cs_pin;

	printk("epaper_spi_config.frequency %d \r\n", epaper_spi_config.frequency);
	printk("/***********************************/ end\r\n");
	return 0;
}

/******************************************************************************
 * function:	Module exits, closes SPI and BCM2835 library
 * parameter:
 * Info:
 *****************************************************************************/
void DEV_Module_Exit(void)
{
	DEV_Digital_Write(EPD_CS_PIN, LOW);
	if (PWR_PIN.gpio.port) {
		DEV_Digital_Write(EPD_PWR_PIN, LOW);
	}

	DEV_Digital_Write(EPD_DC_PIN, LOW);
	DEV_Digital_Write(EPD_RST_PIN, LOW);

	/* SPI end and MCU close. */
}
