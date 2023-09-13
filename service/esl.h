/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef BT_ESL_H_
#define BT_ESL_H_

/**
 * @file
 * @defgroup bt_esl Electronics Shelf Label (ESL) GATT Service
 * @{
 * @brief Electronics Shelf Label (ESL) GATT Service API.
 */

#include <zephyr/types.h>
#include <zephyr/kernel.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/smf.h>
#include <bluetooth/gatt_pool.h>
#include <bluetooth/conn_ctx.h>

#include "esl_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define INDICATION_LED_IDX                                                                         \
	COND_CODE_1(CONFIG_BT_ESL_LED_INDICATION, MAX((CONFIG_BT_ESL_LED_MAX - 1), 0), (0))
#define SIMULATE_DISPLAY_LED_IDX                                                                   \
	COND_CODE_1(CONFIG_ESL_SIMULATE_DISPLAY, MAX((CONFIG_BT_ESL_LED_MAX - 2), 0), (0))

#ifndef CONFIG_BT_ESL_DEFAULT_PERM_RW_AUTHEN
#define CONFIG_BT_ESL_DEFAULT_PERM_RW_AUTHEN 0
#endif
#ifndef CONFIG_BT_ESL_DEFAULT_PERM_RW_ENCRYPT
#define CONFIG_BT_ESL_DEFAULT_PERM_RW_ENCRYPT 0
#endif
#ifndef CONFIG_BT_ESL_DEFAULT_PERM_RW
#define CONFIG_BT_ESL_DEFAULT_PERM_RW 0
#endif

#define ESL_GATT_PERM_DEFAULT                                                                      \
	(CONFIG_BT_ESL_DEFAULT_PERM_RW_AUTHEN                                                      \
		 ? (BT_GATT_PERM_READ_AUTHEN | BT_GATT_PERM_WRITE_AUTHEN)                          \
	 : CONFIG_BT_ESL_DEFAULT_PERM_RW_ENCRYPT                                                   \
		 ? (BT_GATT_PERM_READ_ENCRYPT | BT_GATT_PERM_WRITE_ENCRYPT)                        \
		 : (BT_GATT_PERM_READ | BT_GATT_PERM_WRITE))

/** @brief helper of ESL service. **/
#define BT_ESL_DEF(_name, ...)                                                                     \
	static struct bt_esls _name = {                                                            \
		.gp = BT_GATT_POOL_INIT(CONFIG_BT_ESL_ATTR_MAX),                                   \
	}
/** @brief ESL ECP RESPONSE structure */
struct esl_ecp_resp {
	uint8_t resp_op;
	uint8_t error_code;
	union {
		uint8_t led_idx;
		uint8_t display_idx;
		uint8_t sensor_idx;
	};

	uint8_t img_idx;
};

/** @brief Pointers to the callback functions for service events. */
struct bt_esl_cb {
	/** @brief Display control callback.
	 *
	 * This function is called when the ESL tag decides to change image.
	 *
	 * @param[in] disp_idx Index of dispaly device.
	 * @param[in] img_idx  Index of image.
	 * @param[in] enable   Turning on or not.
	 *
	 * @retval 0 If the operation was successful.
	 * Otherwise, a (negative) error code is returned.
	 */
	int (*display_control)(uint8_t disp_idx, uint8_t img_idx, bool enable);

	/** @brief Display unassociated callback.
	 *
	 * This function is called when the ESL tag is booting up and unassociated.
	 * Show information on display device to help ESL Tag to be associated.
	 *
	 * @param[in] disp_idx Index of dispaly device.
	 */
	void (*display_unassociated)(uint8_t disp_idx);

	/** @brief Display associated callback.
	 *
	 * This function is called when the ESL tag is associated but not synced or not
	 * commanded to display image.
	 * Show information on display device to indicate ESL Tag been associated.
	 *
	 * @param[in] disp_idx Index of dispaly device.
	 */
	void (*display_associated)(uint8_t disp_idx);

	/**
	 * @brief Clears the framebuffer allocated by the font library for the specified display.
	 *
	 * This function is a callback for the font library and clears the framebuffer allocated by
	 * the font library for the specified display. The function disables all images on the
	 * display by clearing the framebuffer.
	 *
	 * @param disp_idx The index of the display to clear.
	 *
	 * @return 0 if the operation is successful, or a negative error code if the operation
	 * fails.
	 */
	int (*display_clear_font)(uint8_t disp_idx);

	/**
	 * @brief Prints text on the specified display using the font library.
	 *
	 * This function is a callback for the font library and prints text on the specified display
	 * using the font library. The text is printed at the specified position with the specified
	 * font and color. The function returns an error code if the text cannot be printed.
	 *
	 * @param disp_idx The index of the display to print the text on.
	 * @param text The text to print.
	 * @param x The x-coordinate of the top-left corner of the text.
	 * @param y The y-coordinate of the top-left corner of the text.
	 *
	 * @return 0 if the text is printed successfully, or a negative error code if the text
	 * cannot be printed.
	 */
	int (*display_print_font)(uint8_t disp_idx, const char *text, uint16_t x, uint16_t y);

	/**
	 * @brief Finalizes(Write to framebuffer of EPD) the CFB and checks if the
	 * EPD (Electronic Paper Display) needs to be re-initialized.
	 *
	 * This function finalizes the CFB by flushing any pending updates to the display buffer and
	 * checking if the EPD needs to be re-initialized.
	 *
	 * @param disp_idx The index of the display to update.
	 *
	 * @return 0 if the operation is successful, or a negative error code if the operation
	 * fails.
	 */
	int (*display_update_font)(uint8_t disp_idx);

	/** @brief LED control callback.
	 *
	 * This function is called when the ESL tag decides to change LED flashing pattern.
	 *
	 * @param[in] led_idx Index of LED.
	 * @param[in] color_brightness Color and brightness to be controlled.
	 * @param[in] onoff Turn on or off LED.
	 */
	void (*led_control)(uint8_t led_idx, uint8_t color_brightness, bool onoff);

	/** @brief Sensor control callback.
	 *
	 * This function is called when the ESl tag decides to read sensor data.
	 * @param[in]  sensor_idx Index of sensor.
	 * @param[out] len Length of sensor data.
	 * @param[out] data Pointer of sensor data reading.
	 *
	 * @retval 0 If the operation was successful.
	 * Otherwise, a (negative) error code is returned.
	 */
	int (*sensor_control)(uint8_t sensor_idx, uint8_t *len, uint8_t *data);

	/** @brief Display init callback.
	 *
	 * Display device initialization.
	 *
	 * @retval 0 If the operation was successful.
	 * Otherwise, a (negative) error code is returned.
	 */
	int (*display_init)(void);

	/** @brief LED init callback.
	 * LED device initialization.
	 *
	 * @retval 0 If the operation was successful.
	 * Otherwise, a (negative) error code is returned.
	 */
	int (*led_init)(void);

	/** @brief Sensor init callback.
	 * Sensor device initialization.
	 *
	 * @retval 0 If the operation was successful.
	 * Otherwise, a (negative) error code is returned.
	 */
	int (*sensor_init)(void);

	/** @brief Buffer image to img_obj_buf.
	 * Image data from OTS may come in fragment, some filesystem support seek, some doesn't.
	 * Buffer all fragments into img_obj_buf then write once.
	 *
	 * @param[out] data Pointer to hold OTS data.
	 * @param[in]  len Length of OTS data.
	 * @param[in]  offset Offset of OTS data.
	 */
	void (*buffer_img)(const void *data, size_t len, off_t offset);

	/** @brief Write img_obj_buf to storage callback.
	 * Write image data to storage backend when all of fragments from OTS is received.
	 *
	 * @param[in] img_idx Image index to be stored.
	 * @param[in] len Image data length to be stored.
	 * @param[in] offset Offset of Image data to be stored for filesystem supported
	 * seek feature. 0 if filesystem is not supported seek feature.
	 *
	 * @retval >=0 a number of bytes written, on success.
	 * Otherwise, a (negative) error code is returned.
	 */
	int (*write_img_to_storage)(uint8_t img_idx, size_t len, off_t offset);

	/** @brief Read img from storage callback.
	 * Read image data from storage backend when ESL decides to change image on display.
	 *
	 * @param[in]  img_idx Image index to be stored.
	 * @param[out] data Pointer to hold image data.
	 * @param[in]  len Image data length to be stored.
	 * @param[in]  offset Offset of Image data to be stored for filesystem supported
	 * seek feature. 0 if filesystem is not supported seek feature.
	 *
	 * @retval >=0 a number of bytes written, on success.
	 * Otherwise, a (negative) error code is returned.
	 */
	int (*read_img_from_storage)(uint8_t img_idx, void *data, size_t len, off_t offset);

	/** @brief read img size from storage callback.
	 * Read image size from storage backend when ESL decides to change image on display.
	 * @param[in] img_idx Image index to be stored.
	 *
	 * @retval >0 a number of bytes written, on success.
	 * 0 means no such image.
	 */
	size_t (*read_img_size_from_storage)(uint8_t img_idx);

	/** @brief read all imgs from storage callback.
	 * Remove all images in storage.
	 *
	 * @retval 0 success. Otherwise, a (negative) error code is returned.
	 */
	int (*delete_imgs)(void);

	/** @brief Vendor-specific OP Code handler callback.
	 *
	 * @param[in] buf ECP command includes OP code and parameter
	 *
	 */
	void (*vs_command_handler)(struct net_buf_simple *buf);

	/** @brief Vendoer-spefic OP Code response callback
	 *
	 * @param[in] buf ECP response includes OP code and parameter
	 *
	 * @retval length of ECP response
	 */
	uint8_t (*vs_response_handler)(uint8_t *buf);
};

/** @brief ESL LED flashing work item. */
struct led_dwork {
	uint8_t led_idx;
	struct k_work_delayable work;
	uint8_t pattern_idx;
	uint8_t repeat_counts;
	uint32_t stop_abs_time;
	uint32_t start_abs_time;
	bool work_enable;
	bool active;
};

/** @brief ESL Display work item. */
struct display_dwork {
	uint8_t display_idx;
	struct k_work_delayable work;
	uint8_t img_idx;
	uint32_t start_abs_time;
	bool active;
};

/** @brief ESL Sensor data. */
struct esl_sensor_data {
	bool data_available;
	uint8_t size;
	uint8_t data[ESL_RESPONSE_SENSOR_LEN];
};

#if !defined(CONFIG_ESL_IMAGE_FILE_SIZE)
#define CONFIG_ESL_IMAGE_FILE_SIZE 0
#endif

#if defined(CONFIG_BT_ESL_IMAGE_AVAILABLE)
/* OTS related */
#define OBJ_IMG_MAX_SIZE CONFIG_ESL_IMAGE_FILE_SIZE
struct ots_img_object {
	char name[CONFIG_BT_OTS_OBJ_MAX_NAME_LEN + 1];
};
#endif

/** @brief ESL Service internal data structure. */
struct bt_esls {
	/** Descriptor of the service attribute array. */
	struct bt_gatt_pool gp;

	/** Struct holding ESL elements data*/
	struct bt_esl_chrc_data esl_chrc;

	/** Bluetooth connection contexts. */
	struct bt_conn *conn;

	/** control point cccd */
	struct _bt_gatt_ccc ecp_ccc;

	/** control point dummy attr*/
	uint8_t cont_point;

	/** ECP response notify*/
	struct bt_gatt_notify_params ecp_notify_params;

	/** ESL service Basic State*/
	atomic_t basic_state;

	/** ESL service State Machine var*/
	uint8_t esl_state;

	/** ESL Configuring State */
	atomic_t configuring_state;

	/** ESL ECP response */
	struct esl_ecp_resp resp;

	/* ESL ECP response needed
	 * Only factory_reset does not have resposnse 3.10.2.4.2 Response
	 */
	bool esl_ecp_resp_needed;

	/** @brief Unsynchronized or unassociated state machine timeout renew needed */
	bool esl_sm_timeout_update_needed;

	/** Callback to control elements **/
	struct bt_esl_cb cb;

	/** worker of led control **/
	struct led_dwork led_dworks[CONFIG_BT_ESL_LED_MAX];

	/** state for diaply control */
	struct display_dwork display_works[CONFIG_BT_ESL_DISPLAY_MAX];

	/** sensor data */
	struct esl_sensor_data sensor_data[CONFIG_BT_ESL_SENSOR_MAX];
	/** BUSY/RETRY indication */
	bool busy;

#if defined(CONFIG_BT_ESL_IMAGE_AVAILABLE)
	/** how much size used transferred by OTS*/
	uint32_t stored_image_size[CONFIG_BT_ESL_IMAGE_MAX];

	/* OTS related */
	struct ots_img_object img_objects[CONFIG_BT_ESL_IMAGE_MAX];
	uint8_t img_obj_buf[CONFIG_ESL_IMAGE_BUFFER_SIZE];
	/* OTS server instance for image transfer */
	struct bt_ots *ots;
#endif
};

/** @brief ESL Service initialization parameters. */
struct bt_esl_init_param {
	struct esl_disp_inf displays[CONFIG_BT_ESL_DISPLAY_MAX];
	struct esl_sensor_inf sensors[CONFIG_BT_ESL_SENSOR_MAX];
	struct esl_sensor_data sensor_data[CONFIG_BT_ESL_SENSOR_MAX];
	/** Data holding static LED information */
	uint8_t led_type[CONFIG_BT_ESL_LED_MAX];
	struct bt_esl_cb cb;
};

/** @brief Initialize the service.
 *
 * @details This function registers a GATT service with several characteristics.
 *          A AP device that is connected to this service.
 *
 * @param[in] esl_obj    ESL Service object.
 * @param[in] init_param Struct with function pointers to callbacks for service
 *                       events. If no callbacks are needed, this parameter can
 *                       be NULL.
 *
 * @retval 0 If initialization is successful.
 *           Otherwise, a negative value is returned.
 */
int bt_esl_init(struct bt_esls *esl_obj, const struct bt_esl_init_param *init_param);

/** @brief Get ESL object.
 * Get ESL object.
 *
 * @retval Point to hold ESL object.
 */
struct bt_esls *esl_get_esl_obj(void);

/** @brief Remove remove all data configured by Access Point */
void bt_esl_factory_reset(void);

/**
 * @brief Check if the given ESL ID matches the expected ID and handle errors accordingly.
 *
 * @param esl_id The ESL ID to check.
 * @param resp Pointer to the response structure to fill in case of errors.
 * @return int Returns 0 on success, or a negative error code on failure.
 *
 * This function checks if the given ESL ID matches the expected ID, and handles errors accordingly.
 * If the given ID does not match the expected ID or is a broadcast ID, an error response is
 * generated. If the ESL object is busy, a retry error response is generated.
 *
 * The function returns 0 on success, or a negative error code on failure.
 */
int check_ecp_esl_id(uint8_t esl_id, struct esl_ecp_resp *resp);

/**
 * @brief Checks if all the required data for configuring the Bluetooth Electronic Shelf Label (ESL)
 * service object is set.
 *
 * @param esl_obj Pointer to the Electronic Shelf Label (ESL) service object.
 * @return true if all the required data is set, false otherwise.
 */
bool esl_is_configuring_state(struct bt_esls *esl_obj);

#ifdef __cplusplus
}
#endif

/**
 *@}
 */

#endif /* BT_ESL_H_ */
