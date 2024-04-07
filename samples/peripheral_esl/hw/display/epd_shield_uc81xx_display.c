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
#if (CONFIG_BT_ESL_PAINT_LIB)
#include "epdpaint.h"
#include "fonts.h"
static Paint paint_black;
#endif /* CONFIG_BT_ESL_PAINT_LIB */
#include <zephyr/logging/log.h>

#include "esl.h"
#include "esl_hw_impl.h"

LOG_MODULE_DECLARE(peripheral_esl);
#if DT_HAS_COMPAT_STATUS_OKAY(ultrachip_uc8176)
/* Choose this value according EPD datasheet */
#define UC81XX_FULL_UPDATE_TIME 8000

#define DT_DRV_COMPAT ultrachip_uc8176
#elif DT_HAS_COMPAT_STATUS_OKAY(ultrachip_uc8179)
#define DT_DRV_COMPAT ultrachip_uc8179
#endif

const struct device *display_dev;
const struct device *spi = DEVICE_DT_GET(DT_NODELABEL(arduino_spi));
const struct gpio_dt_spec reset_gpio = GPIO_DT_SPEC_INST_GET(0, reset_gpios);
const struct gpio_dt_spec dc_gpio = GPIO_DT_SPEC_INST_GET(0, dc_gpios);
const struct gpio_dt_spec busy_gpio = GPIO_DT_SPEC_INST_GET(0, busy_gpios);
PINCTRL_DT_DEFINE(DT_NODELABEL(arduino_spi));
const struct pinctrl_dev_config *pcfg = PINCTRL_DT_DEV_CONFIG_GET(DT_NODELABEL(arduino_spi));
static struct display_capabilities capabilities;
static struct display_buffer_descriptor buf_desc;

#if defined(CONFIG_ESL_POWER_PROFILE)
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
			k_msleep(UC81XX_FULL_UPDATE_TIME);
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
	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Display is not ready, aborting test");
		return -ENODEV;
	}

	display_get_capabilities(display_dev, &capabilities);

	if (display_set_pixel_format(display_dev, PIXEL_FORMAT_MONO10) != 0) {
		LOG_INF("Failed to set required pixel format\n");
		return 0;
	}

	return 0;
}

int display_control(uint8_t disp_idx, uint8_t img_idx, bool enable)
{
	ARG_UNUSED(disp_idx);
	size_t img_size;
	struct waveshare_gray_head *img_head;
	struct bt_esls *esl_obj = esl_get_esl_obj();
	int err;

#if defined(CONFIG_ESL_POWER_PROFILE)
	display_epd_onoff(EPD_POWER_ON);
#if DT_HAS_COMPAT_STATUS_OKAY(ultrachip_uc8176) || DT_HAS_COMPAT_STATUS_OKAY(ultrachip_uc8179)
	/**
	 * To optimize power remove static declaration of uc81xx_init in
	 * zephyr/drivers/display/uc81xx.c
	 **/
	uc81xx_init(display_dev);

#endif /* DT_HAS_COMPAT_STATUS_OKAY(ultrachip_uc8176) || DT_HAS_COMPAT_STATUS_OKAY(ultrachip_uc8179) */
#endif /* ESL_POWER_PROFILE */

	LOG_DBG("display %d img %d on/off %d", disp_idx, img_idx, enable);

	/* Load image to buffer for all kind of E-paper*/
	memset(esl_obj->img_obj_buf, 0, CONFIG_ESL_IMAGE_BUFFER_SIZE);
	img_size = esl_obj->cb.read_img_size_from_storage(img_idx);
	/* Read header first */
	if (esl_obj->cb.open_image_from_storage) {
		esl_obj->cb.open_image_from_storage(img_idx);
	}

	err = esl_obj->cb.read_img_from_storage(img_idx, esl_obj->img_obj_buf,
						sizeof(struct waveshare_gray_head), 0);
	if (err < 0) {
		LOG_ERR("read image idx %d failed (err %d)", img_idx, err);
	}

	img_head = (struct waveshare_gray_head *)esl_obj->img_obj_buf;
	buf_desc.width = img_head->w;
	buf_desc.height = img_head->h;
	buf_desc.pitch = buf_desc.width;
	buf_desc.buf_size = (img_head->w * img_head->h) / EPD_MONO_NUMOF_ROWS_PER_PAGE;
	/* Check image header rational */
	if (img_size < buf_desc.buf_size) {
		LOG_ERR("Invalid file size %d, image size %d", img_size, buf_desc.buf_size);
		err = -EINVAL;
		goto end;
	}

	if (img_head->w == 0 || img_head->h == 0) {
		LOG_ERR("Invalid resolution width or height is zero");
		err = -EINVAL;
		goto end;
	}

	if (img_head->w > capabilities.x_resolution || img_head->h > capabilities.y_resolution) {
		LOG_ERR("Invalid resolution, width or height is over display resolution");
		err = -EINVAL;
		goto end;
	}

	display_blanking_on(display_dev);

#if defined(CONFIG_ESL_OTS_NVS)
	err = esl_obj->cb.read_img_from_storage(img_idx, esl_obj->img_obj_buf, img_size, 0);
	if (err < 0) {
		LOG_ERR("read image idx %d failed (err %d)", img_idx, err);
	}

	err = display_write(display_dev, 0, 0, &buf_desc,
			    (esl_obj->img_obj_buf + sizeof(struct display_buffer_descriptor)));
	if (err) {
		LOG_ERR("display_write (err %d)", err);
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
		err = esl_obj->cb.read_img_from_storage(img_idx, esl_obj->img_obj_buf, chunk_size,
							cur_pos +
								sizeof(struct waveshare_gray_head));
		buf_desc.buf_size = chunk_size;
		if (err < 0) {
			LOG_ERR("read image idx %d failed (err %d)", img_idx, err);
		}

		err = display_write(display_dev, 0, cur_y, &buf_desc, esl_obj->img_obj_buf);
		if (err) {
			LOG_ERR("display_write (err %d)", err);
			return err;
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
#endif /* CONFIG_ESL_OTS_NVS */

	display_blanking_off(display_dev);
	LOG_DBG("Use Raw display interface API");
#if defined(CONFIG_ESL_POWER_PROFILE)
	display_epd_onoff(EPD_POWER_OFF);
#endif
end:
	if (esl_obj->cb.close_image_from_storage) {
		esl_obj->cb.close_image_from_storage();
	}

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
#if DT_HAS_COMPAT_STATUS_OKAY(ultrachip_uc8176) || DT_HAS_COMPAT_STATUS_OKAY(ultrachip_uc8179)
	/**
	 * To optimize power remove static declaration of uc81xx_init in
	 * zephyr/drivers/display/uc81xx.c
	 **/
	uc81xx_init(display_dev);
#endif /* DT_HAS_COMPAT_STATUS_OKAY(ultrachip_uc8176) || DT_HAS_COMPAT_STATUS_OKAY(ultrachip_uc8179) */
#endif /* CONFIG_ESL_POWER_PROFILE */

	/* Use Font Paint lib to draw text*/
#if defined(CONFIG_BT_ESL_PAINT_LIB)
	struct bt_esls *esl_obj = esl_get_esl_obj();
	int err;

	buf_desc.width = capabilities.x_resolution;
	buf_desc.height = capabilities.y_resolution;
	buf_desc.pitch = capabilities.x_resolution;
	buf_desc.buf_size = (buf_desc.width * buf_desc.height) / EPD_MONO_NUMOF_ROWS_PER_PAGE;
	Paint_Init(&paint_black, esl_obj->img_obj_buf, capabilities.x_resolution,
		   capabilities.y_resolution);
	Paint_Clear(&paint_black, UNCOLORED);
	Paint_SetRotate(&paint_black, 0);
	Paint_DrawStringAt(&paint_black, 10, 10, "Hello Nordic!", &Font16, COLORED);
	Paint_DrawStringAt(&paint_black, 10, 30, "UNAssociated", &Font16, COLORED);
	Paint_DrawStringAt(&paint_black, 10, 50, "ESL TAG", &Font16, COLORED);
	Paint_DrawStringAt(&paint_black, 10, 70, tag_str, &Font16, COLORED);
	Paint_DrawStringAt(&paint_black, 10, 90, "APAC", &Font16, COLORED);
	Paint_DrawStringAt(&paint_black, 10, 110, CONFIG_BT_DIS_MODEL, &Font16, COLORED);
	display_blanking_on(display_dev);
	err = display_write(display_dev, 0, 0, &buf_desc, paint_black.image);
	if (err) {
		LOG_ERR("display_write (rc %d)", err);
	}

	display_blanking_off(display_dev);

#endif /* CONFIG_BT_ESL_PAINT_LIB */

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
#if DT_HAS_COMPAT_STATUS_OKAY(ultrachip_uc8176) || DT_HAS_COMPAT_STATUS_OKAY(ultrachip_uc8179)
	/**
	 * To optimize power remove static declaration of uc81xx_init in
	 * zephyr/drivers/display/uc81xx.c
	 **/
	uc81xx_init(display_dev);
#endif /* DT_HAS_COMPAT_STATUS_OKAY(ultrachip_uc8176) || DT_HAS_COMPAT_STATUS_OKAY(ultrachip_uc8179) */
#endif /* CONFIG_ESL_POWER_PROFILE */
	/* Use Font Paint lib to draw text*/
#if defined(CONFIG_BT_ESL_PAINT_LIB)
	int err;

	buf_desc.width = capabilities.x_resolution;
	buf_desc.height = capabilities.y_resolution;
	buf_desc.pitch = capabilities.x_resolution;
	buf_desc.buf_size = (buf_desc.width * buf_desc.height) / EPD_MONO_NUMOF_ROWS_PER_PAGE;
	Paint_Init(&paint_black, esl_obj->img_obj_buf, capabilities.x_resolution,
		   capabilities.y_resolution);
	Paint_Clear(&paint_black, UNCOLORED);
	Paint_SetRotate(&paint_black, 0);
	Paint_DrawStringAt(&paint_black, 10, 10, "Hello Nordic!", &Font16, COLORED);
	Paint_DrawStringAt(&paint_black, 10, 30, "Associated", &Font16, COLORED);
	Paint_DrawStringAt(&paint_black, 10, 50, tag_str, &Font16, COLORED);
	Paint_DrawStringAt(&paint_black, 10, 70, "APAC", &Font16, COLORED);
	Paint_DrawStringAt(&paint_black, 10, 90, CONFIG_BT_DIS_MODEL, &Font16, COLORED);
	display_blanking_on(display_dev);
	err = display_write(display_dev, 0, 0, &buf_desc, paint_black.image);
	if (err) {
		LOG_ERR("display_write (rc %d)", err);
	}

	display_blanking_off(display_dev);
#endif /* CONFIG_BT_ESL_PAINT_LIB */

#if defined(CONFIG_ESL_POWER_PROFILE)
	display_epd_onoff(EPD_POWER_OFF);
#endif /* CONFIG_ESL_POWER_PROFILE */
}

#if defined(CONFIG_BT_ESL_PAINT_LIB)
int display_clear_paint(uint8_t disp_idx)
{
	int err;

#if defined(CONFIG_ESL_POWER_PROFILE)
	display_epd_onoff(EPD_POWER_ON);
#endif /* CONFIG_ESL_POWER_PROFILE */
	ARG_UNUSED(disp_idx);

	buf_desc.width = capabilities.x_resolution;
	buf_desc.height = capabilities.y_resolution;
	buf_desc.pitch = capabilities.x_resolution;
	buf_desc.buf_size = (buf_desc.width * buf_desc.height) / EPD_MONO_NUMOF_ROWS_PER_PAGE;

	Paint_Clear(&paint_black, UNCOLORED);
	display_blanking_on(display_dev);
	err = display_write(display_dev, 0, 0, &buf_desc, paint_black.image);
	if (err) {
		LOG_ERR("display_write (rc %d)", err);
	}

	display_blanking_off(display_dev);

#if defined(CONFIG_ESL_POWER_PROFILE)
	display_epd_onoff(EPD_POWER_OFF);
#endif /* CONFIG_ESL_POWER_PROFILE */
	return err;
}

int display_print_paint(uint8_t disp_idx, const char *text, uint16_t x, uint16_t y)
{
	ARG_UNUSED(disp_idx);
	Paint_DrawStringAt(&paint_black, x, y, text, &Font16, COLORED);

	return 0;
}

int display_update_paint(uint8_t disp_idx)
{
	int err;

#if defined(CONFIG_ESL_POWER_PROFILE)
	display_epd_onoff(EPD_POWER_ON);
#endif /* CONFIG_ESL_POWER_PROFILE */
	ARG_UNUSED(disp_idx);
	buf_desc.width = capabilities.x_resolution;
	buf_desc.height = capabilities.y_resolution;
	buf_desc.pitch = capabilities.x_resolution;
	buf_desc.buf_size = (buf_desc.width * buf_desc.height) / EPD_MONO_NUMOF_ROWS_PER_PAGE;
	display_blanking_on(display_dev);
	err = display_write(display_dev, 0, 0, &buf_desc, paint_black.image);
	if (err) {
		LOG_ERR("display_write (rc %d)", err);
	}

	display_blanking_off(display_dev);

#if defined(CONFIG_ESL_POWER_PROFILE)
	display_epd_onoff(EPD_POWER_OFF);
#endif /* CONFIG_ESL_POWER_PROFILE */
	return err;
}
#endif /* CONFIG_BT_ESL_PAINT_LIB */
