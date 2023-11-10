/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/** @file
 *  @brief Nordic Electronic Shelf Label Service (ESL) application
 */

#include <stdio.h>
#include <zephyr/types.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <host/id.h>
#include <zephyr/bluetooth/controller.h>
#include <zephyr/drivers/led.h>
#include <zephyr/drivers/gpio.h>
#include <hal/nrf_gpio.h>
#include <hal/nrf_power.h>
#if !NRF_POWER_HAS_RESETREAS
#include <hal/nrf_reset.h>
#endif
#include <zephyr/pm/pm.h>
#include <zephyr/pm/policy.h>

#include <zephyr/settings/settings.h>
#include <zephyr/logging/log.h>
#include <dk_buttons_and_leds.h>

#include "esl.h"
#include "esl_hw_impl.h"
#if defined(CONFIG_BT_ESL_VENDOR_SPECIFIC_SUPPORT)
#include "esl_vs_impl.h"
#endif /* CONFIG_BT_ESL_VENDOR_SPECIFIC_SUPPORT */
#if defined(CONFIG_ESL_NFC_SUPPORT)
#include "esl_nfc_impl.h"
#endif /* CONFIG_ESL_NFC_SUPPORT */


LOG_MODULE_REGISTER(peripheral_esl, CONFIG_PERIPHERAL_ESL_LOG_LEVEL);

BT_ESL_DEF(esl_obj);

extern struct bt_conn *auth_conn;
static struct bt_esl_init_param init_param;

#if defined(AUTH_PASSKEY_MANUAL)
static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_INF("Passkey for %s: %06u", addr, passkey);
}

static void auth_passkey_confirm(struct bt_conn *conn, unsigned int passkey)
{
	char addr[BT_ADDR_LE_STR_LEN];

	auth_conn = bt_conn_ref(conn);

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_INF("Passkey for %s: %06u", addr, passkey);
	LOG_INF("Press Button 1 to confirm, Button 2 to reject.");
}
#endif
static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_INF("Pairing cancelled: %s", addr);
}

static void pairing_complete(struct bt_conn *conn, bool bonded)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_INF("Pairing completed: %s, bonded: %d", addr, bonded);
}

static void pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_INF("Pairing failed conn: %s, reason %d", addr, reason);
}

static void pairing_confirm(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	bt_conn_auth_pairing_confirm(conn);

	LOG_INF("Pairing confirmed: %s", addr);
}

void bond_deleted(uint8_t id, const bt_addr_le_t *peer)
{
	char addr[BT_ADDR_STR_LEN];

	bt_addr_le_to_str(peer, addr, sizeof(addr));
	LOG_INF("Bond deleted for %s, id %u", addr, id);
}

static struct bt_conn_auth_cb conn_auth_callbacks = {
#if defined(AUTH_PASSKEY_MANUAL)
	.passkey_display = auth_passkey_display,
	.passkey_confirm = auth_passkey_confirm,
#endif
	.pairing_confirm = pairing_confirm,
	.cancel = auth_cancel,
};
static struct bt_conn_auth_info_cb conn_auth_info_callbacks = {.pairing_complete = pairing_complete,
							       .pairing_failed = pairing_failed,
							       .bond_deleted = bond_deleted};

static void button_changed(uint32_t button_state, uint32_t has_changed)
{
	uint32_t buttons = button_state & has_changed;

	if (buttons & DK_BTN1_MSK) {
		printk("#DEBUG#FACTORYRESET:1\n");
		bt_esl_factory_reset();
	}
}

#if defined(CONFIG_ESL_WAKE_GPIO)
static void config_wakeup_gpio(void)
{
	nrf_gpio_cfg_input(CONFIG_ESL_WAKE_GPIO, NRF_GPIO_PIN_PULLUP);
	nrf_gpio_cfg_sense_set(CONFIG_ESL_WAKE_GPIO, NRF_GPIO_PIN_SENSE_LOW);
}
#endif /* CONFIG_ESL_WAKE_GPIO */

#if defined(CONFIG_ESL_SHIPPING_MODE)

static void system_off(void)
{
#if defined(CONFIG_ESL_SHIPPING_WAKE_BY_NFC)
#define WAKEUP_MSG "Approach an NFC reader to wake up.\n"
#elif defined(CONFIG_ESL_SHIPPING_WAKE_BY_BOTH)
#define WAKEUP_MSG "Approach an NFC reader or press the button to wake up.\n"
#elif defined(CONFIG_ESL_SHIPPING_WAKE_BY_BUTTON)
#define WAKEUP_MSG "Press the button to wake up.\n"
#else
#error "Invalid wake-up configuration"
#endif

	printk("Entering system off.\n");
	printk("%s", WAKEUP_MSG);

#if defined(CONFIG_ESL_POWER_PROFILE)
	/* Get rid of this by using user own driver */
	display_epd_onoff(EPD_POWER_OFF_IMMEDIATELY);
#endif
	/* Before we disabled entry to deep sleep. Here we need to override
	 * that, then force a sleep so that the deep sleep takes effect.
	 */
	const struct pm_state_info si = {PM_STATE_SOFT_OFF, 0, 0};

	pm_state_force(0, &si);

	/* Going into sleep will actually go to system off mode, because we
	 * forced it above.
	 */
	k_sleep(K_MSEC(1));

	/* k_sleep will never exit, so below two lines will never be executed
	 * if system off was correct. On the other hand if someting gone wrong
	 * we will see it on terminal and LED.
	 */
	printk("ERROR: System off failed\n");
}

/**
 * @brief  Helper function for printing the reason of the last reset.
 * Can be used to confirm that NCF field actually woke up the system.
 */
static uint32_t print_reset_reason(void)
{
	uint32_t reas;

#if NRF_POWER_HAS_RESETREAS
	reas = nrf_power_resetreas_get(NRF_POWER);
	printk("rr 0x%08x ", reas);
	nrf_power_resetreas_clear(NRF_POWER, reas);
	if (reas & NRF_POWER_RESETREAS_NFC_MASK) {
		printk("Wake up by NFC field detect\n");
	} else if (reas & NRF_POWER_RESETREAS_RESETPIN_MASK) {
		printk("Reset by pin-reset\n");
	} else if (reas & NRF_POWER_RESETREAS_SREQ_MASK) {
		printk("Reset by soft-reset\n");
	} else if (reas & NRF_POWER_RESETREAS_OFF_MASK) {
		printk("Reset by Sense pin\n");
	} else if (reas) {
		printk("Reset by a different source (0x%08X)\n", reas);
	} else {
		printk("Power-on-reset\n");
	}
#else
	reas = nrf_reset_resetreas_get(NRF_RESET);
	printk("rr 0x%08x\n", reas);
	nrf_reset_resetreas_clear(NRF_RESET, reas);
	if (reas & NRF_RESET_RESETREAS_NFC_MASK) {
		printk("Wake up by NFC field detect\n");
	} else if (reas & NRF_RESET_RESETREAS_RESETPIN_MASK) {
		printk("Reset by pin-reset\n");
	} else if (reas & NRF_RESET_RESETREAS_SREQ_MASK) {
		printk("Reset by soft-reset\n");
	} else if (reas & NRF_RESET_RESETREA_OFF_MASK) {
		printk("Reset by Sense pin\n");
	} else if (reas) {
		printk("Reset by a different source (0x%08X)\n", reas);
	} else {
		printk("Power-on-reset\n");
	}
#endif
	return reas;
}

#endif /* CONFIG_ESL_SHIPPING_MODE */

static void configure_dk_button(void)
{
	int err;

	err = dk_buttons_init(button_changed);
	if (err) {
		LOG_ERR("Cannot init buttons (err: %d)", err);
	}
}

static int start_execute(void)
{
	int err;
	uint8_t own_addr_type;

	configure_dk_button();
	if (IS_ENABLED(CONFIG_BT_ESL_PTS)) {
		uint8_t pub_addr[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};

		bt_ctlr_set_public_addr(pub_addr);
		own_addr_type = BT_ADDR_LE_PUBLIC;
	} else {
		own_addr_type = BT_ADDR_LE_RANDOM;
	}

	err = bt_enable(NULL);
	if (err) {
		LOG_ERR("bt_enable error %d", err);
		return err;
	}

	LOG_INF("Bluetooth initialized");

	if (IS_ENABLED(CONFIG_SETTINGS)) {
		settings_load();
	}

	bt_id_set_scan_own_addr(true, &own_addr_type);
#if defined(CONFIG_ESL_NFC_SUPPORT)
	err = esl_msg_update();
	if (err) {
		LOG_WRN("esl_msg_update error %d", err);
	}
#endif /* CONFIG_ESL_NFC_SUPPORT */

	/* Assign hardware characteristics*/
	hw_chrc_init(&init_param);

	/* Assign hardware cb */
	init_param.cb.sensor_init = sensor_init;
	init_param.cb.sensor_control = sensor_control;
	init_param.cb.display_init = display_init;
	init_param.cb.display_control = display_control;
	init_param.cb.led_init = led_init;
	init_param.cb.led_control = led_control;
	init_param.cb.display_unassociated = display_unassociated;
	init_param.cb.display_associated = display_associated;
#if defined(CONFIG_CHARACTER_FRAMEBUFFER)
	init_param.cb.display_update_font = display_update_cfb;
	init_param.cb.display_clear_font = display_clear_cfb;
	init_param.cb.display_print_font = display_print_cfb;
#endif /* CONFIG_CHARACTER_FRAMEBUFFER */
#if defined(CONFIG_BT_ESL_IMAGE_AVAILABLE)
	init_param.cb.buffer_img = buffer_img;
	init_param.cb.write_img_to_storage = write_img_to_storage;
	init_param.cb.read_img_from_storage = read_img_from_storage;
	init_param.cb.read_img_size_from_storage = read_img_size_from_storage;
	init_param.cb.delete_imgs = delete_imgs_from_storage;

	err = ots_storage_init();
	if (err != 0) {
		LOG_ERR("Failed to init image storage (err:%d)\n", err);
		return err;
	}

#endif /* CONFIG_BT_ESL_IMAGE_AVAILABLE*/
#if defined(CONFIG_BT_ESL_VENDOR_SPECIFIC_SUPPORT)
	init_param.cb.vs_command_handler = vs_command_handler;
	init_param.cb.vs_response_handler = vs_response_handler;
#endif /* CONFIG_BT_ESL_VENDOR_SPECIFIC_SUPPORT */
	err = bt_esl_init(&esl_obj, &init_param);
	printk("bt_esl_init (err %d)\n", err);
	if (err != 0) {
		LOG_ERR("Failed to initialize ESL service (err: %d)", err);
		return err;
	}

	if (IS_ENABLED(CONFIG_BT_ESL_SECURITY_ENABLED)) {
		err = bt_conn_auth_cb_register(&conn_auth_callbacks);
		if (err) {
			LOG_ERR("Failed to register authorization callbacks.");
			return err;
		}

		err = bt_conn_auth_info_cb_register(&conn_auth_info_callbacks);
		if (err) {
			LOG_ERR("Failed to register authentication information callbacks.");
			return err;
		}
	}

#if defined(CONFIG_MCUBOOT_IMAGE_VERSION)
	printk("CONFIG_MCUBOOT_IMAGE_VERSION %s\n", CONFIG_MCUBOOT_IMAGE_VERSION);
#endif /* CONFIG_MCUBOOT_IMAGE_VERSION */

	for (;;) {
		k_sleep(K_FOREVER);
	}
}

int main(void)
{
#if defined(CONFIG_ESL_NFC_SUPPORT)
	int err;

	err = esl_nfc_init();
	if (err) {
		LOG_WRN("esl_nfc_init error %d", err);
	}
#endif /* CONFIG_ESL_NFC_GREETING */

#if defined(CONFIG_ESL_SHIPPING_MODE)
	uint32_t rr = print_reset_reason();

#if NRF_POWER_HAS_RESETREAS
	if ((rr & NRF_POWER_RESETREAS_NFC_MASK) || (rr & NRF_POWER_RESETREAS_OFF_MASK)) {
		return start_execute();
#else
	if ((rr & NRF_RESET_RESETREAS_NFC_MASK) || (rr & NRF_RESET_RESETREAS_OFF_MASK)) {
		return start_execute();
#endif /* NRF_POWER_HAS_RESETREAS */
	} else {
#if defined(CONFIG_ESL_WAKE_GPIO)
		config_wakeup_gpio();
#endif /* CONFIG_ESL_WAKE_GPIO */
		system_off();
	}
#else
	return start_execute();
#endif /* CONFIG_ESL_SHIPPING_MODE */
}
