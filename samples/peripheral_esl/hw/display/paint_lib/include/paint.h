/**
 ******************************************************************************
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 *
 * Filename : paint.h
 *******************************************************************************
 */

#ifndef __PAINT_H
#define __PAINT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <fonts.h>

/* Macros -------- -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef enum {
	PAINT_DIRECTION_0,
	PAINT_DIRECTION_90,
	PAINT_DIRECTION_180,
	PAINT_DIRECTION_270,
} paint_direction_t;

typedef enum {
	/* scam mode 1: Row to Row; H from left to right; V from top to bottom */
	PAINT_SCAN_MODE_1,
	/* scam mode 2: Row to Row; H from left to right; V from bottom to top */
	PAINT_SCAN_MODE_2,
} paint_scan_mode_t;

typedef enum {
	WHITE,
	BLACK,
	RED,
} paint_color_t;

typedef struct paint_obj_s {
	paint_scan_mode_t scanmode;
	paint_direction_t direction;
	uint32_t width;
	uint32_t height;
	uint32_t x_end;
	uint32_t y_end;
	uint8_t *wb_buffer;
#if (CONFIG_PAINT_COLORS == 3)
	uint8_t *rw_buffer;
#endif
	uint32_t buffer_size;
} paint_obj_t;

typedef struct paint_rect_s {
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
} paint_rect_t;

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
void paint_Init(paint_obj_t *paint_obj);
void paint_SetDirection(paint_direction_t direction);

void paint_Fill(paint_color_t color);
void paint_DrawPoint(paint_color_t color, uint32_t x, uint32_t y);
void paint_FillRect(paint_color_t color, paint_rect_t *rect);
void paint_DrawString(const char *s, sFONT *font, paint_color_t color, int x, int y);
static inline void paint_clear(void)
{
	paint_Fill(WHITE);
}

#ifdef __cplusplus
}
#endif
#endif /* __PAINT_H */

/* End of File */
