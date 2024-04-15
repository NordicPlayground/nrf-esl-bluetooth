/**
 ******************************************************************************
 * Copyright (c) 2024 Nordic Semiconductor ASA
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 * Filename : jfpaint.c
 *
 * Date(DD-MM-YYYY)        Modification        Name
 * ----------------        ------------        ----
 *    10-04-2024             Created            JF
 *******************************************************************************
 */

#include <zephyr/sys/util.h>
#include <string.h>
#include <jfpaint.h>

uint8_t *wb_buf;
uint8_t *rw_buf;
static uint16_t buf_size;
static uint8_t scan_mode;
static uint8_t wb_on, rw_on;
void jfpaint_init(uint8_t *in_wb_buf, uint8_t *in_rw_buf)
{
	wb_buf = in_wb_buf;
	rw_buf = in_rw_buf;
	buf_size = CONFIG_ESL_DISPLAY_WIDTH * CONFIG_ESL_DISPLAY_HEIGHT / 8;
}

void setscan(uint8_t mode)
{
	scan_mode = mode;
}

void setcolorlevel(enum BIT_LEVEL black, enum BIT_LEVEL red)
{
	wb_on = black;
	rw_on = red;
}

void fill(Color_t color)
{
	uint8_t wb, rw;

	switch (color) {
	case BLACK:
		wb = (wb_on == BIT_OFF) ? 0x00 : 0xff;
		rw = (rw_on == BIT_OFF) ? 0xff : 0x00;
		break;
	case RED:
		wb = (wb_on == BIT_OFF) ? 0xff : 0x00;
		rw = (rw_on == BIT_OFF) ? 0x00 : 0xff;
		break;
	case WHITE:
	default:
		wb = (wb_on == BIT_OFF) ? 0xff : 0x00;
		rw = (rw_on == BIT_OFF) ? 0xff : 0x00;
		break;
	}

	memset((void *)wb_buf, wb, buf_size);
	if (CONFIG_ESL_DISPLAY_TYPE == 6) {
		memset((void *)rw_buf, rw, buf_size);
	}
}
void drawPoint(Color_t color, int x, int y)
{
	uint8_t mask;

	if (scan_mode == SCAN_UPSIDEDOWN) {
		/* convert y, since scan from B=>T */
		y = CONFIG_ESL_DISPLAY_HEIGHT - y - 1;
	}

	mask = 1 << (7 - x % 8);
	setcolor(y * CONFIG_ESL_DISPLAY_WIDTH / 8 + x / 8, color, mask);
}

void drawHline(Color_t color, int x, int y, int width)
{
	int mask_start, mask_stop, i;
	uint8_t mask;

	if (scan_mode == SCAN_UPSIDEDOWN) {
		/* convert y, since scan from B=>T */
		y = CONFIG_ESL_DISPLAY_HEIGHT - y - 1;
	}

	i = x;
	mask_start = i % 8;
	if (mask_start != 0) {
		if (mask_start + width <= 8) {
			mask_stop = mask_start + width;
			mask = ((1 << (8 - mask_start)) - 1) & (~((1 << mask_stop) - 1));
			setcolor(y * CONFIG_ESL_DISPLAY_WIDTH / 8 + i / 8, color, mask);
			return;
		} else {
			mask = (1 << (8 - mask_start)) - 1;
			setcolor(y * CONFIG_ESL_DISPLAY_WIDTH / 8 + i / 8, color, mask);
			i = (i & (~0x07)) + 8;
		}
	}

	while (i + 8 < x + width) {
		setcolor(y * CONFIG_ESL_DISPLAY_WIDTH / 8 + i / 8, color, 0xff);
		i += 8;
	}

	mask_stop = x + width - i;
	mask = ~((1 << mask_stop) - 1);
	setcolor(y * CONFIG_ESL_DISPLAY_WIDTH / 8 + i / 8, color, mask);
}

void drawRect(Color_t color, struct display_window *rect)
{
	int i;
	if ((rect->x + rect->width > CONFIG_ESL_DISPLAY_WIDTH) ||
	    (rect->y + rect->height > CONFIG_ESL_DISPLAY_HEIGHT) || (rect->width == 0) ||
	    (rect->height == 0)) {
		return;
	}

	for (i = rect->y; i < rect->y + rect->height; i++) {
		drawHline(color, rect->x, i, rect->width);
	}
}

void drawFont(char c, sFONT *font, Color_t color, int x, int y)
{
	uint32_t char_offset;
	int i, j;
	if ((x + font->Width > CONFIG_ESL_DISPLAY_WIDTH) ||
	    (y + font->Height > CONFIG_ESL_DISPLAY_HEIGHT)) {
		return;
	}

	c = c - ' ';
	char_offset = c * font->Height * ((font->Width + 7) / 8);
	const uint8_t *ptr = &font->table[char_offset];
	for (j = 0; j < font->Height; j++) {
		for (i = 0; i < font->Width; i++) {
			if (*ptr & (0x80 >> (i % 8))) {
				drawPoint(color, x + i, y + j);
			}
			if (i % 8 == 7) {
				ptr++;
			}
		}
		if (font->Width % 8 != 0) {
			ptr++;
		}
	}
}

void drawString(const char *s, sFONT *font, Color_t color, int x, int y)
{
	uint32_t s_index = 0;
	uint32_t s_length = strlen(s);
	do {
		if (x + font->Width <= CONFIG_ESL_DISPLAY_WIDTH) {
			drawFont(s[s_index], font, color, x, y);
			x += font->Width;
			s_index++;
		} else {
			break;
		}
	} while (s_index < s_length);
}

void setcolor(int index, Color_t color, uint8_t mask)
{
	switch (color) {
	// case BLACK:
	// 	wb_buf[index] &= ~mask;
	// 	rw_buf[index] &= ~mask;
	// 	break;
	// case RED:
	// 	wb_buf[index] &= ~mask;
	// 	rw_buf[index] |= mask;
	// 	break;
	// case WHITE:
	// default:
	// 	wb_buf[index] |= mask;
	// 	rw_buf[index] &= ~mask;
	// 	break;
	case BLACK:
		wb_buf[index] = (wb_on == BIT_OFF) ? (wb_buf[index] & ~mask) : (wb_buf[index] | mask);
		rw_buf[index] = (rw_on == BIT_OFF) ? (rw_buf[index] | mask) : (rw_buf[index] & ~mask);
		break;
	case RED:
		wb_buf[index] = (wb_on == BIT_OFF) ? (wb_buf[index] | mask) : (wb_buf[index] & ~mask);
		rw_buf[index] = (rw_on == BIT_OFF) ? (rw_buf[index] & ~mask) : (rw_buf[index] | mask);
		break;
	case WHITE:
	default:
		wb_buf[index] = (wb_on == BIT_OFF) ? (wb_buf[index] | mask) : (wb_buf[index] & ~mask);
		rw_buf[index] = (rw_on == BIT_OFF) ? (rw_buf[index] | mask) : (rw_buf[index] & ~mask);
		break;

	}
}
