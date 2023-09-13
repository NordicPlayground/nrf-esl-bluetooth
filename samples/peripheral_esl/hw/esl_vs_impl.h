/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef ESL_VS_IMPL_H_
#define ESL_VS_IMPL_H_

/**
 * @file esl_vs_impl.h
 *
 * @brief This file contains the declarations of the functions used for handling commands and
 * responses in the peripheral ESL hardware.
 */

/**
 * @brief Function for handling commands received by the peripheral ESL hardware.
 *
 * @param[in] buf Pointer to the buffer containing the command.
 */
void vs_command_handler(struct net_buf_simple *buf);

/**
 * @brief Function for handling responses received by the peripheral ESL hardware.
 *
 * @param[in] buf Pointer to the buffer containing the response.
 *
 * @return The status of the response handling operation.
 */
uint8_t vs_response_handler(uint8_t *buf);

#endif /* ESL_VS_IMPL_H_ */
