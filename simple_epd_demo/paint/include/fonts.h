/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef __FONTS_H
#define __FONTS_H

#define MAX_HEIGHT_FONT 24
#define MAX_WIDTH_FONT 22

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct tFont {
	const uint8_t *table;
	uint16_t Width;
	uint16_t Height;

} sFONT;

extern sFONT Font24;
extern sFONT Font20;
extern sFONT Font16;
extern sFONT Font12;
extern sFONT Font8;

#ifdef __cplusplus
}
#endif

#endif /* __FONTS_H */
