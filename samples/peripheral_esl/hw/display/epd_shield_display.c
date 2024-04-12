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

LOG_MODULE_DECLARE(peripheral_esl);
#if DT_HAS_COMPAT_STATUS_OKAY(solomon_ssd16xxfb)
/* Choose this value according EPD datasheet */
#define SSD16XX_FULL_UPDATE_TIME 3500
#define DT_DRV_COMPAT solomon_ssd16xxfb
#endif /* DT_HAS_COMPAT_STATUS_OKAY(solomon_ssd16xxfb) */

#if defined(CONFIG_CHARACTER_FRAMEBUFFER)
#include <zephyr/display/cfb.h>
#include "cfbv_1016.h"
static uint8_t font_height;
#endif /* CONFIG_CHARACTER_FRAMEBUFFER */

const struct device *display_dev;
/* nRF52 family Devkit has arduino header as SPI interface */
#if IS_ENABLED(CONFIG_DT_HAS_ARDUINO_HEADER_R3_ENABLED)
#define SPI_NODE DT_NODELABEL(arduino_spi)
/* nRF54L Devkit uses SPI00 for now */
#elif IS_ENABLED(CONFIG_NRFX_SPIM00)
#define SPI_NODE DT_NODELABEL(spi00)
#else
#error "No SPI node found"
#endif /* CONFIG_DT_HAS_ARDUINO_HEADER_R3_ENABLED */
const struct device *spi = DEVICE_DT_GET(SPI_NODE);
const struct gpio_dt_spec reset_gpio = GPIO_DT_SPEC_INST_GET(0, reset_gpios);
const struct gpio_dt_spec dc_gpio = GPIO_DT_SPEC_INST_GET(0, dc_gpios);
const struct gpio_dt_spec busy_gpio = GPIO_DT_SPEC_INST_GET(0, busy_gpios);
PINCTRL_DT_DEFINE(SPI_NODE);
const struct pinctrl_dev_config *pcfg = PINCTRL_DT_DEV_CONFIG_GET(SPI_NODE);
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
#if (CONFIG_DISPLAY)
	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Display is not ready, aborting test");
		return -ENODEV;
	}

	display_get_capabilities(display_dev, &capabilities);
#endif /* CONFIG_DISPLAY */

#if defined(CONFIG_CHARACTER_FRAMEBUFFER)
	uint8_t font_width;

	if (display_set_pixel_format(display_dev, PIXEL_FORMAT_MONO10) != 0) {
		LOG_INF("Failed to set required pixel format\n");
		return 0;
	}

	LOG_INF("Initialized %s\n", display_dev->name);
	if (cfb_framebuffer_init(display_dev)) {
		LOG_INF("Framebuffer initialization failed!\n");
		return 0;
	}

#if DT_HAS_COMPAT_STATUS_OKAY(solomon_ssd16xxfb)
	cfb_framebuffer_set_font(display_dev, 0);
	cfb_get_font_size(display_dev, 0, &font_width, &font_height);

	LOG_DBG("font width %d, font height %d rows %d, cols %d\n", font_width, font_height,
		cfb_get_display_parameter(display_dev, CFB_DISPLAY_ROWS),
		cfb_get_display_parameter(display_dev, CFB_DISPLAY_COLS));
#endif /* DT_HAS_COMPAT_STATUS_OKAY(solomon_ssd16xxfb) */
#endif /* CONFIG_CHARACTER_FRAMEBUFFER */

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
#if DT_HAS_COMPAT_STATUS_OKAY(solomon_ssd16xxfb)
	/**
	 * To optimize power remove static declaration of ssd16xx_init in
	 * zephyr/drivers/display/ssd16xx.c
	 **/
	ssd16xx_init(display_dev);
#endif /* DT_HAS_COMPAT_STATUS_OKAY(solomon_ssd16xxfb) */
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
			    (esl_obj->img_obj_buf + sizeof(struct waveshare_gray_head)));
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
			goto end;
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

	return err;
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
#if DT_HAS_COMPAT_STATUS_OKAY(solomon_ssd16xxfb)
	/**
	 * To optimize power remove static declaration of ssd16xx_init in
	 * zephyr/drivers/display/ssd16xx.c
	 **/
	ssd16xx_init(display_dev);
#endif /* DT_HAS_COMPAT_STATUS_OKAY(solomon_ssd16xxfb) */
#endif /* ESL_POWER_PROFILE */

	/* Use Character Frame Buffer to draw text */
#if defined(CONFIG_CHARACTER_FRAMEBUFFER)
	int err;

	display_blanking_on(display_dev);
	cfb_framebuffer_clear(display_dev, true);
	cfb_print(display_dev, "Hello Nordic!", 0, 0 * font_height);
	cfb_print(display_dev, "UNAssociated", 0, 1 * font_height);
	cfb_print(display_dev, "ESL TAG", 0, 2 * font_height);
	cfb_print(display_dev, tag_str, 0, 3 * font_height);
	cfb_print(display_dev, "APAC", 0, 4 * font_height);
	err = cfb_framebuffer_finalize(display_dev);
	if (err) {
		LOG_ERR("cfb_framebuffer_finalize (rc %d)", err);
	}

	display_blanking_off(display_dev);
#endif /* CONFIG_CHARACTER_FRAMEBUFFER */

#if defined(CONFIG_ESL_POWER_PROFILE)
	display_epd_onoff(EPD_POWER_OFF);
#endif
}

void display_associated(uint8_t disp_idx)
{

#if defined(CONFIG_ESL_POWER_PROFILE)
	display_epd_onoff(EPD_POWER_ON);
#if DT_HAS_COMPAT_STATUS_OKAY(solomon_ssd16xxfb)
	/**
	 * To optimize power remove static declaration of ssd16xx_init in
	 * zephyr/drivers/display/ssd16xx.c
	 **/
	ssd16xx_init(display_dev);
#endif /* DT_HAS_COMPAT_STATUS_OKAY(solomon_ssd16xxfb) */
#endif /* ESL_POWER_PROFILE */

	/* Use Character Frame Buffer to draw text */
#if defined(CONFIG_CHARACTER_FRAMEBUFFER)
	struct bt_esls *esl_obj = esl_get_esl_obj();
	char tag_str[20] = {0};
	int err;

	snprintk(tag_str, sizeof(tag_str), "ESL TAG 0x%04x", esl_obj->esl_chrc.esl_addr);
	display_blanking_on(display_dev);
	cfb_framebuffer_clear(display_dev, true);
	cfb_print(display_dev, "Hello Nordic!", 0, 0 * font_height);
	cfb_print(display_dev, "Associated", 0, 1 * font_height);
	cfb_print(display_dev, tag_str, 0, 2 * font_height);
	cfb_print(display_dev, "APAC", 0, 3 * font_height);
	err = cfb_framebuffer_finalize(display_dev);
	if (err) {
		LOG_ERR("display_write (rc %d)", err);
	}

	display_blanking_off(display_dev);
#endif /* CONFIG_CHARACTER_FRAMEBUFFER */

#if defined(CONFIG_ESL_POWER_PROFILE)
	display_epd_onoff(EPD_POWER_OFF);
#endif
}

#if defined(CONFIG_CHARACTER_FRAMEBUFFER)
int display_clear_cfb(uint8_t disp_idx)
{
	int err;
#if defined(CONFIG_ESL_POWER_PROFILE)
	display_epd_onoff(EPD_POWER_ON);
#endif /* CONFIG_ESL_POWER_PROFILE */
	ARG_UNUSED(disp_idx);
	err = cfb_framebuffer_clear(display_dev, true);
	if (err) {
		LOG_ERR("cfb_framebuffer_clear (rc %d)", err);
	}
#if defined(CONFIG_ESL_POWER_PROFILE)
	display_epd_onoff(EPD_POWER_OFF);
#endif /* CONFIG_ESL_POWER_PROFILE */
	return err;
}

int display_print_cfb(uint8_t disp_idx, const char *text, uint16_t x, uint16_t y)
{
	int err;

	ARG_UNUSED(disp_idx);
	err = cfb_print(display_dev, text, x, y);
	if (err) {
		LOG_ERR("cfb_print (rc %d)", err);
	}

	return err;
}

int display_update_cfb(uint8_t disp_idx)
{
	int err;
#if defined(CONFIG_ESL_POWER_PROFILE)
	display_epd_onoff(EPD_POWER_ON);
#endif /* CONFIG_ESL_POWER_PROFILE */
	ARG_UNUSED(disp_idx);
	err = cfb_framebuffer_finalize(display_dev);
	if (err) {
		LOG_ERR("cfb_framebuffer_finalize (rc %d)", err);
	}

#if defined(CONFIG_ESL_POWER_PROFILE)
	display_epd_onoff(EPD_POWER_OFF);
#endif /* CONFIG_ESL_POWER_PROFILE */
	return err;
}
#endif /* CONFIG_CHARACTER_FRAMEBUFFER */
