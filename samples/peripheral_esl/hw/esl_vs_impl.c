/*
 * This file contains the implementation of the Vendor Specific (VS) commands for the
 * Nordic Semiconductor's External Sensor Library (ESL) peripheral. It defines the
 * VS command handler and response handler functions.
 */

#include <zephyr/net/buf.h>
#include <zephyr/logging/log.h>

#include "esl.h"
#include "esl_internal.h"

LOG_MODULE_DECLARE(peripheral_esl);

/* Structure to hold the response to a VS command */
struct esl_vs_ecp_resp {
	uint8_t resp_op;     /* Response operation code */
	uint8_t error_code;  /* Error code in case of an error response */
	uint8_t dfu_enabled; /* DFU enable flag */
};

/* Initialize the response structure */
static struct esl_vs_ecp_resp vs_ecp_resp;

/* Enumeration of the VS operation codes */
enum ESL_VS_OP_CODE {
	VS_DFU_REQ = 0x1F, /* DFU request */
};

/* Enumeration of the VS response codes */
enum ESL_VS_RESP_CODE {
	ESL_VS_OK = 0x0F,  /* OK response */
	ESL_VS_DFU = 0x1F, /* DFU response */
};

/* Enumeration of the VS response error codes */
enum ESL_VS_RSP_ERR_CODE {
	VS_ECP_NOT_SUPP = 0xf0, /* ECP not supported error */
};

/* Function to enter or exit DFU mode */
static void esl_enter_dfu(bool enable)
{
	if (enable) {
		LOG_INF("Entering DFU mode");
	} else {
		LOG_INF("Exiting DFU mode");
	}
}

/* Function to handle the VS command */
void vs_command_handler(struct net_buf_simple *buf)
{
	uint8_t command;
	uint8_t cp_len;

	LOG_INF("Received VS command");

	/* Extract the command and its length from the buffer */
	command = buf->data[0];
	cp_len = ECP_LEN(command);

	/* Check if the buffer length matches the expected command length */
	if (buf->len != (cp_len)) {
		LOG_ERR("ESL VS control command length not match %d %d", buf->len, (cp_len));
		vs_ecp_resp.resp_op = OP_ERR;
		vs_ecp_resp.error_code = ERR_INV_PARAMS;
		return;
	}

	/* Handle the command based on its type */
	switch (command) {
	case VS_DFU_REQ:
		LOG_INF("Received VS_DFU_REQ");
		esl_enter_dfu(buf->data[2]);
		vs_ecp_resp.resp_op = ESL_VS_DFU;
		vs_ecp_resp.dfu_enabled = buf->data[2];
		break;
	default:
		LOG_ERR("Unknown VS command");
		vs_ecp_resp.resp_op = OP_ERR;
		vs_ecp_resp.error_code = VS_ECP_NOT_SUPP;
		break;
	}
}

/* Function to handle the VS response */
uint8_t vs_response_handler(uint8_t *buf)
{
	uint8_t ret;

	/* Set the response operation code in the buffer */
	buf[0] = vs_ecp_resp.resp_op;

	/* Set the response data based on the response operation code */
	ret = ECP_LEN(vs_ecp_resp.resp_op);
	switch (vs_ecp_resp.resp_op) {
	case ESL_VS_DFU:
		LOG_INF("Received VS_DFU_REQ");
		vs_ecp_resp.resp_op = ESL_VS_DFU;
		buf[1] = vs_ecp_resp.dfu_enabled;
		break;
	default:
		buf[1] = vs_ecp_resp.error_code;
		break;
	}

	return ret;
}
