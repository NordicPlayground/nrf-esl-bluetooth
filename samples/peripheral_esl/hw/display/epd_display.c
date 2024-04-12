/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/pm/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/logging/log.h>

#include "esl.h"
#include "esl_hw_impl.h"
/* include EPD driver header */
#include "EPD_4in2.h"
#include "EPD_4in2_V2.h"
#include "EPD_1in54b_V2.h"
#include "EPD_5in83b_V2.h"
#include "EPD_5in83_V2.h"
#include "EPD_5in83_minew.h"
#include "EPD_2in9b_V3.h"
#include "EPD_2in13b_V3.h"

struct epd_display_fn_t {
	void (*epd_init)(void);
	void (*epd_clear)(void);
	void (*epd_display_full)(uint8_t *Image, ...);
	void (*epd_write_display)(uint16_t X_start, uint16_t Y_start, uint16_t width,
				  uint16_t height, uint8_t *Image);
	void (*epd_turn_on_display)(void);
	void (*epd_sleep)(void);
} epd_display_fn;

LOG_MODULE_DECLARE(peripheral_esl);
#define SSD16XX_FULL_UPDATE_TIME 3500

/* nRF52 family Devkit has arduino header as SPI interface */
#if IS_ENABLED(CONFIG_DT_HAS_ARDUINO_HEADER_R3_ENABLED)
#define SPI_NODE DT_NODELABEL(arduino_spi)
/* Minew ms18f& and stag85 use spi1*/
#elif IS_ENABLED(CONFIG_BOARD_MS138F7_NRF52833)
#define SPI_NODE DT_NODELABEL(spi1)
/* nRF54L Devkit uses SPI00 for now */
#elif IS_ENABLED(CONFIG_NRFX_SPIM00)
#define SPI_NODE DT_NODELABEL(spi00)
#else
#error "No SPI node found"
#endif /* CONFIG_DT_HAS_ARDUINO_HEADER_R3_ENABLED */

const struct device *spi = DEVICE_DT_GET(SPI_NODE);
PINCTRL_DT_DEFINE(SPI_NODE);
const struct pinctrl_dev_config *pcfg = PINCTRL_DT_DEV_CONFIG_GET(SPI_NODE);
static struct display_capabilities capabilities;
static struct display_buffer_descriptor buf_desc;

#if defined(CONFIG_ESL_POWER_PROFILE)
const struct gpio_dt_spec reset_gpio = GPIO_DT_SPEC_INST_GET(0, reset_gpios);
const struct gpio_dt_spec dc_gpio = GPIO_DT_SPEC_INST_GET(0, dc_gpios);
const struct gpio_dt_spec busy_gpio = GPIO_DT_SPEC_INST_GET(0, busy_gpios);

/** Turn off SPI and Wavashare gpio to save power */
int display_epd_onoff(uint8_t mode)
{
	int err;

	if (mode == EPD_POWER_ON) {
		err = gpio_pin_configure_dt(&reset_gpio, GPIO_OUTPUT_INACTIVE);
		if (err < 0) {
			LOG_ERR("Failed to configure reset GPIO");
			return err;
		}

		err = gpio_pin_configure_dt(&dc_gpio, GPIO_OUTPUT_INACTIVE);
		if (err < 0) {
			LOG_ERR("Failed to configure DC GPIO");
			return err;
		}

		(void)pm_device_action_run(spi, PM_DEVICE_ACTION_RESUME);
	} else if (mode == EPD_POWER_OFF || mode == EPD_POWER_OFF_IMMEDIATELY) {
		(void)pm_device_action_run(spi, PM_DEVICE_ACTION_SUSPEND);
		*(volatile uint32_t *)(DT_REG_ADDR(DT_NODELABEL(arduino_spi)) | 0xFFC) = 0;
		*(volatile uint32_t *)(DT_REG_ADDR(DT_NODELABEL(arduino_spi)) | 0xFFC);
		*(volatile uint32_t *)(DT_REG_ADDR(DT_NODELABEL(arduino_spi)) | 0xFFC) = 1;

		/* turn off EPD after full update otherwise immediately */
		if (mode == EPD_POWER_OFF) {
			k_msleep(SSD16XX_FULL_UPDATE_TIME);
		}

		err = gpio_pin_set_dt(&reset_gpio, 1);

		if (err < 0) {
			LOG_ERR("Failed to configure reset GPIO disconneted");
			return err;
		}

		err = gpio_pin_configure_dt(&dc_gpio, GPIO_DISCONNECTED);
		if (err < 0) {
			LOG_ERR("Failed to configure DC GPIO disconneted");
			return err;
		}

		err = gpio_pin_configure_dt(&busy_gpio, GPIO_DISCONNECTED);
		if (err < 0) {
			LOG_ERR("Failed to configure Busy GPIO disconneted");
			return err;
		}

		/* SPI related pin need to be disconnect after SPI module powered off */
		err = pinctrl_apply_state(pcfg, PINCTRL_STATE_SLEEP);
		if (err < 0) {
			return err;
		}
	}

	return 0;
}
#endif /* ESL_POWER_PROFILE */

int display_init(void)
{
	capabilities.x_resolution = CONFIG_ESL_DISPLAY_WIDTH;
	capabilities.y_resolution = CONFIG_ESL_DISPLAY_HEIGHT;

	DEV_Module_Init();
	if (IS_ENABLED(CONFIG_EPD_4IN2_V1)) {
		epd_display_fn.epd_init = EPD_4IN2_Init_Fast;
		epd_display_fn.epd_clear = EPD_4IN2_Clear;
		epd_display_fn.epd_display_full = EPD_4IN2_Display;
		epd_display_fn.epd_write_display = EPD_4IN2_PartialDisplay;
		epd_display_fn.epd_turn_on_display = EPD_4IN2_TurnOnDisplay;
		epd_display_fn.epd_sleep = EPD_4IN2_Sleep;
	} else if (IS_ENABLED(CONFIG_EPD_4IN2_V2)) {
		epd_display_fn.epd_init = EPD_4IN2_V2_Init_Fast;
		epd_display_fn.epd_clear = EPD_4IN2_V2_Clear;
		epd_display_fn.epd_display_full = EPD_4IN2_V2_Display_Fast;
		epd_display_fn.epd_write_display = EPD_4IN2_V2_WriteDisplay;
		epd_display_fn.epd_turn_on_display = EPD_4IN2_V2_TurnOnDisplay_Fast;
		epd_display_fn.epd_sleep = EPD_4IN2_V2_Sleep;
	} else if (IS_ENABLED(CONFIG_EPD_1IN54B_V2)) {
		epd_display_fn.epd_init = EPD_1IN54B_V2_Init;
		epd_display_fn.epd_clear = EPD_1IN54B_V2_Clear;
		epd_display_fn.epd_display_full = EPD_1IN54B_V2_Display;
		epd_display_fn.epd_sleep = EPD_1IN54B_V2_Sleep;
	} else if (IS_ENABLED(CONFIG_EPD_5IN83B_V2)) {
		epd_display_fn.epd_init = EPD_5IN83B_V2_Init;
		epd_display_fn.epd_clear = EPD_5IN83B_V2_Clear;
		epd_display_fn.epd_display_full = EPD_5IN83B_V2_Display;
		epd_display_fn.epd_sleep = EPD_5IN83B_V2_Sleep;
	} else if (IS_ENABLED(CONFIG_EPD_5IN83_V2)) {
		epd_display_fn.epd_init = EPD_5in83_V2_Init;
		epd_display_fn.epd_clear = EPD_5in83_V2_Clear;
		epd_display_fn.epd_display_full = EPD_5in83_V2_Display;
		epd_display_fn.epd_sleep = EPD_5in83_V2_Sleep;
	} else if (IS_ENABLED(CONFIG_EPD_5IN83M)) {
		epd_display_fn.epd_init = EPD_5IN83_MINEW_Init;
		epd_display_fn.epd_clear = EPD_5IN83_MINEW_Clear;
		epd_display_fn.epd_display_full = EPD_5IN83_MINEW_Display;
		epd_display_fn.epd_sleep = EPD_5IN83_MINEW_Sleep;
	} else if (IS_ENABLED(CONFIG_EPD_2IN9B_V3)) {
		epd_display_fn.epd_init = EPD_2IN9B_V3_Init;
		epd_display_fn.epd_clear = EPD_2IN9B_V3_Clear;
		epd_display_fn.epd_display_full = EPD_2IN9B_V3_Display;
		epd_display_fn.epd_sleep = EPD_2IN9B_V3_Sleep;
	} else if (IS_ENABLED(CONFIG_EPD_2IN13B_V3)) {
		epd_display_fn.epd_init = EPD_2IN13B_V3_Init;
		epd_display_fn.epd_clear = EPD_2IN13B_V3_Clear;
		epd_display_fn.epd_display_full = EPD_2IN13B_V3_Display;
		epd_display_fn.epd_sleep = EPD_2IN13B_V3_Sleep;
	} else {
		LOG_ERR("No EPD driver found");
		return -ENODEV;
	}

	epd_display_fn.epd_init();

	return 0;
}

int display_control(uint8_t disp_idx, uint8_t img_idx, bool enable)
{
	ARG_UNUSED(disp_idx);
	size_t img_size;
	struct waveshare_gray_head *img_head;
	struct bt_esls *esl_obj = esl_get_esl_obj();
	int err = 0;

#if defined(CONFIG_ESL_POWER_PROFILE)
	display_epd_onoff(EPD_POWER_ON);
	epd_display_fn.epd_init();
#endif /* ESL_POWER_PROFILE */

	LOG_DBG("display %d img %d on/off %d", disp_idx, img_idx, enable);

	memset(esl_obj->img_obj_buf, 0, CONFIG_ESL_IMAGE_BUFFER_SIZE);
	img_size = esl_obj->cb.read_img_size_from_storage(img_idx);
	/* Read header first */
	err = esl_obj->cb.read_img_from_storage(img_idx, esl_obj->img_obj_buf,
						sizeof(struct waveshare_gray_head), 0);
	if (err < 0) {
		LOG_ERR("read image idx %d failed (err %d)", img_idx, err);
	}

	/* Load image to buffer for all kind of E-paper*/

	img_head = (struct waveshare_gray_head *)esl_obj->img_obj_buf;
	buf_desc.width = img_head->w;
	buf_desc.height = img_head->h;
	buf_desc.pitch = buf_desc.width;
	buf_desc.buf_size = (img_head->w * img_head->h) / EPD_MONO_NUMOF_ROWS_PER_PAGE;
	/* Check image header rational */
	if (img_size < buf_desc.buf_size) {
		LOG_ERR("Invalid file size %d, image size %d", img_size, buf_desc.buf_size);
		return -EINVAL;
	}

	if (img_head->w == 0 || img_head->h == 0) {
		LOG_ERR("Invalid resolution width or height is zero");
		return -EINVAL;
	}

	if (img_head->w > capabilities.x_resolution || img_head->h > capabilities.y_resolution) {
		LOG_ERR("Invalid resolution, width or height is over display resolution");
		return -EINVAL;
	}

	if(epd_display_fn.epd_clear) {
		epd_display_fn.epd_clear();
	}

#if defined(CONFIG_ESL_OTS_NVS)
	err = esl_obj->cb.read_img_from_storage(img_idx, esl_obj->img_obj_buf, img_size, 0);
	if (err < 0) {
		LOG_ERR("read image idx %d failed (err %d)", img_idx, err);
	}

	if (epd_display_fn.epd_display_full) {
		epd_display_fn.epd_display_full(
			(esl_obj->img_obj_buf + sizeof(struct waveshare_gray_head)));
	}

#elif defined(CONFIG_ESL_OTS_LFS)
	/* Image size must align to 8 pixel*/
	size_t cur_pos, chunk_size;
	uint16_t cur_y;

	img_size -= sizeof(struct waveshare_gray_head);

	chunk_size = buf_desc.width;
	buf_desc.height = EPD_MONO_NUMOF_ROWS_PER_PAGE;
	cur_pos = 0;
	cur_y = 0;
	while (img_size > 0) {
		err = esl_obj->cb.read_img_from_storage(
			img_idx, esl_obj->img_obj_buf, chunk_size,
			cur_pos + sizeof(struct waveshare_gray_head));

		buf_desc.buf_size = chunk_size;
		if (err < 0) {
			LOG_ERR("read image idx %d failed (err %d)", img_idx, err);
		}

		if (epd_display_fn.epd_write_display) {
			epd_display_fn.epd_write_display(0, cur_y, chunk_size, buf_desc.height,
							   esl_obj->img_obj_buf);
		} else {
			epd_display_fn.epd_display_full(esl_obj->img_obj_buf);
		}

		/**
		 * Decreases the image size by the chunk size, updates the current position, and
		 * determines the chunk size for the next iteration. If the remaining image size is
		 * greater than or equal to the buffer pitch, the chunk size is set to the buffer
		 * pitch. Otherwise, the chunk size is set to the remaining image size, and the
		 * buffer height is adjusted accordingly.
		 */
		img_size -= chunk_size;
		cur_pos += chunk_size;
		if (img_size < chunk_size) {
			chunk_size = img_size;
			buf_desc.height =
				(chunk_size * EPD_MONO_NUMOF_ROWS_PER_PAGE) / buf_desc.width;
		}

		cur_y = (cur_pos * EPD_MONO_NUMOF_ROWS_PER_PAGE) / buf_desc.width;
	}

	if (epd_display_fn.epd_turn_on_display) {
		epd_display_fn.epd_turn_on_display();
	}
#endif /* CONFIG_ESL_OTS_NVS */

	LOG_DBG("Use Raw display interface API");
#if defined(CONFIG_ESL_POWER_PROFILE)
	display_epd_onoff(EPD_POWER_OFF);
#endif
	return 0;
}

void display_unassociated(uint8_t disp_idx)
{
	char tag_str[BT_ADDR_LE_STR_LEN] = {0};
	struct bt_le_oob oob;

	/* print bt addr on display*/
#if defined(CONFIG_BT_ESL_PTS)
	bt_id_read_public_addr(&oob.addr);
#else
	bt_le_oob_get_local(0, &oob);
#endif /* CONFIG_BT_ESL_PTS */

	bt_addr_to_str(&oob.addr.a, tag_str, sizeof(tag_str));
#if defined(CONFIG_ESL_POWER_PROFILE)
	display_epd_onoff(EPD_POWER_ON);
	DEV_Module_Init();
	epd_display_fn.epd_init();
#endif /* CONFIG_ESL_POWER_PROFILE */

#if defined(CONFIG_ESL_POWER_PROFILE)
	display_epd_onoff(EPD_POWER_OFF);
#endif
}

void display_associated(uint8_t disp_idx)
{
	struct bt_esls *esl_obj = esl_get_esl_obj();
	char tag_str[20] = {0};

	snprintk(tag_str, sizeof(tag_str), "ESL TAG 0x%04x", esl_obj->esl_chrc.esl_addr);
#if defined(CONFIG_ESL_POWER_PROFILE)
	display_epd_onoff(EPD_POWER_ON);
	DEV_Module_Init();
	epd_display_fn.epd_init();
#endif

#if defined(CONFIG_ESL_POWER_PROFILE)
	display_epd_onoff(EPD_POWER_OFF);
#endif /* CONFIG_ESL_POWER_PROFILE */
}

#if defined(CONFIG_BT_ESL_PAINT_LIB)
int display_clear_paint(uint8_t disp_idx)
{
#if defined(CONFIG_ESL_POWER_PROFILE)
	display_epd_onoff(EPD_POWER_ON);
	epd_display_fn.epd_init();
#endif /* CONFIG_ESL_POWER_PROFILE */

	epd_display_fn.epd_clear();
	Paint_Clear(&paint_black, UNCOLORED);
#if (CONFIG_ESL_DISPLAY_TYPE == 6)
	Paint_Clear(&paint_red, UNCOLORED);
#endif

#if defined(CONFIG_ESL_POWER_PROFILE)
	display_epd_onoff(EPD_POWER_OFF);
#endif /* CONFIG_ESL_POWER_PROFILE */
	return 0;
}

int display_print_paint(uint8_t disp_idx, const char *text, uint16_t x, uint16_t y)
{
	ARG_UNUSED(disp_idx);
	printk("%s x %d y %d\n", __func__, x, y);
	Paint_DrawStringAt(&paint_black, x, y, text, &Font16, COLORED);

	return 0;
}

int display_update_paint(uint8_t disp_idx)
{
#if defined(CONFIG_ESL_POWER_PROFILE)
	display_epd_onoff(EPD_POWER_ON);
#endif /* CONFIG_ESL_POWER_PROFILE */
	ARG_UNUSED(disp_idx);
	buf_desc.width = capabilities.x_resolution;
	buf_desc.height = capabilities.y_resolution;
	buf_desc.pitch = capabilities.x_resolution;
	buf_desc.buf_size = (buf_desc.width * buf_desc.height) / EPD_MONO_NUMOF_ROWS_PER_PAGE;

	if (epd_display_fn.epd_display_full) {
#if (CONFIG_ESL_DISPLAY_TYPE == 6)
		epd_display_fn.epd_display_full(frame_buffer_black, frame_buffer_red);
#else
		epd_display_fn.epd_display_full(frame_buffer_black);
#endif
	}
#if defined(CONFIG_ESL_POWER_PROFILE)
	display_epd_onoff(EPD_POWER_OFF);
#endif /* CONFIG_ESL_POWER_PROFILE */
	return 0;
}
#endif /* CONFIG_BT_ESL_PAINT_LIB */
