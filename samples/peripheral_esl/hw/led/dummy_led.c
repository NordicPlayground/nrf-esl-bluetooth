/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(peripheral_esl);

nt led_init(void)
{
	LOG_WRN("No LED device init");

	return 0;
}

void led_control(uint8_t led_idx, uint8_t color_brightness, bool onoff)
{
	ARG_UNUSED(led_idx);
	ARG_UNUSED(color_brightness);
	ARG_UNUSED(onoff);

	LOG_WRN("No LED device to control");

	return;
}
