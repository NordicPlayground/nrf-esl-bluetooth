/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef ESL_CLIENT_TAG_STORAGE_H_
#define ESL_CLIENT_TAG_STORAGE_H_

#include <zephyr/bluetooth/bluetooth.h>

#include "esl_common.h"

/**
 * @def ESL_BT_KEY_OFFSET
 * @brief Macro that defines the offset of the BT key in the tag storage buffer.
 *
 * The BT key offset is calculated as the sum of the length of the ESL address, the length of the
 * EAD key material, and the size of a uint8_t data type.
 */
#define ESL_BT_KEY_OFFSET (ESL_ADDR_LEN + EAD_KEY_MATERIAL_LEN + sizeof(uint8_t))

/**
 * @brief Saves the given ESL data in the storage.
 *
 * This function saves the given tag data in the storage with the BLE address and the ESL address.
 * The tag data includes the BLE address, ESL address, ESL response key, and DIS PnP ID.
 * The function creates a file for each tag with the BLE address and the ESL address as the file
 * name.
 *
 * @param tag Pointer to the tag data to be saved.
 * @return 0 on success, negative errno code on error.
 */
int save_tag_in_storage(const struct bt_esl_chrc_data *tag);

/**
 * @brief Loads the ESL data from the file system storage.
 *
 * This function loads the tag data from the file system storage by opening the file with the given
 * BLE address, reading the data from the file and storing it in the given tag structure.
 *
 * @param ble_addr The BLE address of the tag to be loaded.
 * @param tag The tag structure to store the loaded data.
 *
 * @return 0 on success, negative errno code on error.
 */
int load_tag_in_storage(const bt_addr_le_t *ble_addr, struct bt_esl_chrc_data *tag);

/**
 * Searches for a ESL Bluetooth address in the storage with the given ESL address
 *
 * @param [in] esl_addr The ESL address to search for.
 * @param [out] ble_addr The Bluetooth address to search for.
 * @return 0 if found, or negative value if not found.
 */
int find_tag_in_storage_with_esl_addr(uint16_t esl_addr, bt_addr_le_t *ble_addr);

/**
 * @brief Lists all the tags in the storage directory based on the type.
 *
 * @param type The type of the list to be displayed. 0 for ESL list and 1 for BLE list.
 *
 * @return Returns 0 on success or an error code on failure.
 */
int list_tags_in_storage(uint8_t type);

/**
 * @brief Removes a tag from storage based on its ESL address and peer address.
 *
 * This function removes a tag from storage based on its ESL address and peer address. If the peer
 * address is NULL, the function uses the ESL address to find the tag in BLE_ADDR storage. If the
 * tag is found, it is unbonded and the corresponding BLE_ADDR and ESL_ADDR files are removed from
 * storage.
 *
 * @param esl_addr The ESL address of the tag to be removed.
 * @param peer_addr The peer address of the tag to be removed. If NULL, the function uses the ESL
 * address to find the tag in BLE_ADDR storage.
 *
 * @return 0 on success, negative errno code on error.
 */
int remove_tag_in_storage(uint16_t esl_addr, const bt_addr_le_t *peer_addr);

/**
 * @brief Loads the Bluetooth key in the storage for the specified Bluetooth address.
 *
 * @param ble_addr Pointer to the Bluetooth address.
 * @param key Pointer to the Bluetooth keys structure.
 * @return int Returns 0 on success, otherwise a negative error code.
 *
 * @note The function loads the Bluetooth key in the storage for the specified Bluetooth address.
 */
int load_bt_key_in_storage(const bt_addr_le_t *ble_addr, struct bt_keys *key);

/** @brief Remove tag information from storage backend.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a negative error code is returned.
 *
 */
int remove_all_tags_in_storage(void);

/** @brief Load tag's response from storage backend.
 *
 * @param[in] group_id Which group of ESL to load.RFU bit on means read all esl id in storage.
 *
 * @retval 0 If the operation was successful.
 *           Otherwise, a negative error code is returned.
 *
 */
int load_all_tags_in_storage(uint8_t group_id);

/**
 * @brief Imports a Bluetooth key into the ESL client.
 *
 * This function imports a Bluetooth key into the ESL client by copying the key data from the
 * provided buffer into the specified bt_keys structure. The length of the key data must also be
 * provided.
 *
 * @param keys Pointer to the bt_keys structure to copy the key data into.
 * @param data Pointer to the buffer containing the key data to import.
 *
 * @return 0 on success, or a negative error code on failure.
 */
int esl_c_import_bt_key(struct bt_keys *keys);

#endif /* ESL_CLIENT_TAG_STORAGE_H_ */
