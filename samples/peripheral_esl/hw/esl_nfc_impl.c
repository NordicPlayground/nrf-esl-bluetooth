/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/logging/log.h>
#include <nfc_t2t_lib.h>
#include <nfc/ndef/msg.h>
#include <nfc/ndef/text_rec.h>
#include <dk_buttons_and_leds.h>

#include "esl.h"

LOG_MODULE_DECLARE(peripheral_esl);

#define MAX_REC_COUNT	  1
#define NDEF_MSG_BUF_SIZE 64

static const uint8_t en_code[] = {'e', 'n'};
static const uint8_t power_off_payload[] = {'P', 'u', 't', ' ', 'N', 'F', 'C', ' ', 'r', 'e',
					    'a', 'd', 'e', 'r', ' ', 't', 'o', ' ', 'w', 'a',
					    'k', 'e', ' ', 'E', 'S', 'L', ' ', 'u', 'p', '.'};
/* Buffer used to hold an NFC NDEF message. */
static uint8_t ndef_msg_buf[NDEF_MSG_BUF_SIZE];

static void nfc_callback(void *context, nfc_t2t_event_t event, const uint8_t *data,
			 size_t data_length)
{
	ARG_UNUSED(context);
	ARG_UNUSED(data);
	ARG_UNUSED(data_length);

	switch (event) {
	case NFC_T2T_EVENT_FIELD_ON:
		LOG_INF("NFC field detected");
		dk_set_led(DK_LED2, 1);

		break;
	case NFC_T2T_EVENT_FIELD_OFF:
		LOG_INF("NFC field lost");
		dk_set_led(DK_LED2, 0);
		break;
	default:
		break;
	}
}

/**
 * @brief Function for encoding the ESL mode welcome NDEF text message.
 */
static int welcome_msg_encode(uint8_t *buffer, uint32_t *len)
{
	int err;
	char ble_addr_str[BT_ADDR_LE_STR_LEN] = {0};
	char tag_str[BT_ADDR_LE_STR_LEN + 10] = {0};
	struct bt_esls *esl_obj = esl_get_esl_obj();

	struct bt_le_oob oob;

	/* print bt addr on display*/
#if defined(CONFIG_BT_ESL_PTS)
	bt_id_read_public_addr(&oob.addr);
#else
	bt_le_oob_get_local(0, &oob);
#endif /* CONFIG_BT_ESL_PTS */

	bt_addr_to_str(&oob.addr.a, ble_addr_str, sizeof(ble_addr_str));

	if (esl_is_configuring_state(esl_obj)) {
		snprintk(tag_str, sizeof(tag_str), "Associated\nMAC: %s\nESL: 0x%04x", ble_addr_str,
			 esl_obj->esl_chrc.esl_addr);
	} else {
		snprintk(tag_str, sizeof(tag_str), "Unassociated\nMAC: %s\n", ble_addr_str);
	}

	/* Create NFC NDEF text record description in English */
	NFC_NDEF_TEXT_RECORD_DESC_DEF(nfc_en_text_rec, UTF_8, en_code, sizeof(en_code), tag_str,
				      sizeof(tag_str));

	/* Create NFC NDEF message description, capacity - MAX_REC_COUNT
	 * records
	 */
	NFC_NDEF_MSG_DEF(nfc_text_msg, MAX_REC_COUNT);

	/* Add text records to NDEF text message */
	err = nfc_ndef_msg_record_add(&NFC_NDEF_MSG(nfc_text_msg),
				      &NFC_NDEF_TEXT_RECORD_DESC(nfc_en_text_rec));
	if (err < 0) {
		LOG_ERR("Cannot add first record!\n");
		return err;
	}

	err = nfc_ndef_msg_encode(&NFC_NDEF_MSG(nfc_text_msg), buffer, len);
	if (err < 0) {
		LOG_ERR("Cannot encode message!\n");
	}

	return err;
}

/**
 * @brief Function for encoding the system off mode welcome NDEF text message.
 */
static int system_off_msg_encode(uint8_t *buffer, uint32_t *len)
{
	int err;

	/* Create NFC NDEF text record description in English.
	 * This message should not be read since the power is off.
	 */
	NFC_NDEF_TEXT_RECORD_DESC_DEF(nfc_en_text_rec, UTF_8, en_code, sizeof(en_code),
				      power_off_payload, sizeof(power_off_payload));

	/* Create NFC NDEF message description, capacity - MAX_REC_COUNT
	 * records
	 */
	NFC_NDEF_MSG_DEF(nfc_text_msg, MAX_REC_COUNT);

	/* Add text records to NDEF text message */
	err = nfc_ndef_msg_record_add(&NFC_NDEF_MSG(nfc_text_msg),
				      &NFC_NDEF_TEXT_RECORD_DESC(nfc_en_text_rec));
	if (err < 0) {
		LOG_ERR("Cannot add first record!\n");
		return err;
	}

	err = nfc_ndef_msg_encode(&NFC_NDEF_MSG(nfc_text_msg), buffer, len);
	if (err < 0) {
		LOG_ERR("Cannot encode message!\n");
	}

	return err;
}

int esl_msg_update(void)
{
	uint32_t len = sizeof(ndef_msg_buf);

	/* turn off NFC first */
	(void)nfc_t2t_emulation_stop();

	welcome_msg_encode(ndef_msg_buf, &len);
	/* Set created message as the NFC payload */
	if (nfc_t2t_payload_set(ndef_msg_buf, len) < 0) {
		LOG_ERR("Cannot set payload!\n");
		goto fail;
	}

	/* Start sensing NFC field */
	if (nfc_t2t_emulation_start() < 0) {
		LOG_ERR("Cannot start emulation!\n");
		goto fail;
	}

	LOG_INF("NFC update welcome message done\n");
fail:
	return -EIO;
}
int esl_nfc_init(void)
{
	uint32_t len = sizeof(ndef_msg_buf);

	LOG_INF("NFC record configuration start");
	/* Set up NFC */
	if (nfc_t2t_setup(nfc_callback, NULL) < 0) {
		LOG_ERR("Cannot setup NFC T2T library!\n");
		goto fail;
	}

	if (IS_ENABLED(CONFIG_ESL_SHIPPING_WAKE_BY_NFC) ||
	    IS_ENABLED(CONFIG_ESL_SHIPPING_WAKE_BY_BOTH)) {
		/* Encode welcome message */
		if (system_off_msg_encode(ndef_msg_buf, &len) < 0) {
			LOG_ERR("Cannot encode message!\n");
			goto fail;
		}
	} else {
		if (welcome_msg_encode(ndef_msg_buf, &len) < 0) {
			LOG_ERR("Cannot encode message!\n");
			goto fail;
		}
	}

	/* Set created message as the NFC payload */
	if (nfc_t2t_payload_set(ndef_msg_buf, len) < 0) {
		LOG_ERR("Cannot set payload!\n");
		goto fail;
	}

	/* Start sensing NFC field here is supported wake by nfc, otherwise start it later */
	if (IS_ENABLED(CONFIG_ESL_SHIPPING_WAKE_BY_NFC) ||
	    IS_ENABLED(CONFIG_ESL_SHIPPING_WAKE_BY_BOTH)) {

		if (nfc_t2t_emulation_start() < 0) {
			LOG_ERR("Cannot start emulation!\n");
			goto fail;
		}
	}

	LOG_INF("NFC configuration done\n");

	return 0;
fail:
	return -EIO;
}
