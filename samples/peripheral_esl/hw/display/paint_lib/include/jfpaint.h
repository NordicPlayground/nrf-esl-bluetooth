/**
 ******************************************************************************
 * Copyright (c) 2024 Nordic Semiconductor ASA
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 * Filename : jfpaint.h
 *
 * Date(DD-MM-YYYY)        Modification        Name
 * ----------------        ------------        ----
 *    10-04-2024             Created            JF
 *******************************************************************************
 */
#ifndef JFPAINT_H
#define JFPAINT_H

#include "fonts.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum {
	WHITE = 0,
	BLACK,
	RED,
} Color_t;

enum {
	SCAN_NORMAL,
	SCAN_UPSIDEDOWN,
};
enum BIT_LEVEL {
	BIT_OFF,
	BIT_ON,
};

struct display_window {
	/** The starting coordinate in the x direction */
	uint16_t x;
	/** The starting coordinate in the y direction */
	uint16_t y;
	/** The Width of window in pixels*/
	uint16_t width;
	/** The Height of window in pixels*/
	uint16_t height;
};

/**
 * @brief setcolorlevel
 *
 * @param black	BiT level for black color
 * @param red BIT level for red color
 */
void setcolorlevel(enum BIT_LEVEL black, enum BIT_LEVEL red);
void setcolor(int index, Color_t color, uint8_t mask);
void setscan(uint8_t mode);
void drawPoint(Color_t color, int x, int y);
void drawHline(Color_t color, int x, int y, int width);
void drawRect(Color_t color, struct display_window *rect);
void drawFont(char c, sFONT *font, Color_t color, int x, int y);
void drawString(const char *s, sFONT *font, Color_t color, int x, int y);
void fill(Color_t color);
void jfpaint_init(uint8_t *in_wb_buf, uint8_t *in_rw_buf);

#endif /* JFPAINT */
