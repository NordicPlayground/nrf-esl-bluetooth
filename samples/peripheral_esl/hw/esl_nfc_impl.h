/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef ESL_NFC_IMPL_H_
#define ESL_NFC_IMPL_H_

/**
 * @brief Initializes the NFC module for the ESL peripheral.
 *
 * NFC tag message is telling user to put NFC reader on the tag.
 * @return int Returns 0 on success, otherwise returns an error code.
 */
int esl_nfc_init(void);

/**
 * @brief Updates the ESL message.
 *
 * NFC tag message is telling user to BLE MAC after BLE subsystem is initialized.
 * @return int Returns 0 on success, or a negative error code on failure.
 */
int esl_msg_update(void);

#endif /* ESL_NFC_IMPL_H_ */
