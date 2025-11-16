/**
 * Simple E-Paper Display Demo
 *
 * This demo shows how to control a Waveshare 2.9" Black/White E-Paper Display
 * using the nRF52840 Development Kit.
 *
 * Display: Waveshare 2.9" E-Paper (128x296 pixels)
 * Board: nRF52840 DK
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>

#include "../driver/DEV_Config.h"
#include "../driver/EPD_2in9b_V3.h"
#include "../paint/include/paint.h"

LOG_MODULE_REGISTER(epd_demo, LOG_LEVEL_INF);

/* Display dimensions */
#define EPD_WIDTH  EPD_2IN9B_V3_WIDTH
#define EPD_HEIGHT EPD_2IN9B_V3_HEIGHT

/* Buffer size calculation: width/8 * height (1 bit per pixel) */
#define BUFFER_SIZE ((EPD_WIDTH / 8) * EPD_HEIGHT)

/* Buffers for black and red layers */
static uint8_t black_buffer[BUFFER_SIZE];
static uint8_t red_buffer[BUFFER_SIZE];

/* Paint object for drawing */
static paint_obj_t paint;

/**
 * Initialize the E-Paper Display
 */
static int epd_init(void)
{
	int ret;

	LOG_INF("Initializing E-Paper Display...");

	/* Initialize hardware (SPI, GPIO) */
	ret = DEV_Module_Init();
	if (ret != 0) {
		LOG_ERR("Failed to initialize hardware: %d", ret);
		return ret;
	}

	/* Initialize the EPD */
	EPD_2IN9B_V3_Init();
	LOG_INF("EPD initialized successfully");

	return 0;
}

/**
 * Display demo content on the E-Paper
 */
static void display_demo_content(void)
{
	LOG_INF("Preparing display content...");

	/* Initialize paint library with black buffer */
	paint.width = EPD_WIDTH;
	paint.height = EPD_HEIGHT;
	paint.wb_buffer = black_buffer;
	paint.rw_buffer = red_buffer;
	paint.buffer_size = BUFFER_SIZE;
	paint.direction = PAINT_DIRECTION_0;
	paint.scanmode = PAINT_SCAN_MODE_1;

	paint_Init(&paint);

	/* Clear both buffers to white */
	memset(black_buffer, 0xFF, BUFFER_SIZE);
	memset(red_buffer, 0xFF, BUFFER_SIZE);

	/* Draw title text in black */
	paint_DrawString("nRF52840", &Font24, BLACK, 10, 10);
	paint_DrawString("E-Paper Demo", &Font20, BLACK, 10, 40);

	/* Draw some text information */
	paint_DrawString("Waveshare 2.9\"", &Font16, BLACK, 10, 80);
	paint_DrawString("128x296 pixels", &Font12, BLACK, 10, 100);

	/* Draw a rectangle border */
	paint_rect_t border = {
		.x = 5,
		.y = 5,
		.width = EPD_WIDTH - 10,
		.height = 120
	};

	/* Draw border lines (simplified - just draw rectangles) */
	for (int i = 0; i < 2; i++) {
		paint_rect_t line;

		/* Top line */
		line.x = border.x + i;
		line.y = border.y + i;
		line.width = border.width - 2*i;
		line.height = 1;
		paint_FillRect(BLACK, &line);

		/* Bottom line */
		line.y = border.y + border.height - i - 1;
		paint_FillRect(BLACK, &line);

		/* Left line */
		line.x = border.x + i;
		line.y = border.y + i;
		line.width = 1;
		line.height = border.height - 2*i;
		paint_FillRect(BLACK, &line);

		/* Right line */
		line.x = border.x + border.width - i - 1;
		paint_FillRect(BLACK, &line);
	}

	/* Draw some status text */
	paint_DrawString("Status: OK", &Font12, BLACK, 10, 140);
	paint_DrawString("Ready to use!", &Font12, BLACK, 10, 160);

	LOG_INF("Displaying content on EPD...");

	/* Send the buffers to the display */
	EPD_2IN9B_V3_Display(black_buffer, red_buffer);

	LOG_INF("Display update complete!");
}

/**
 * Put the E-Paper Display to sleep mode
 */
static void epd_sleep(void)
{
	LOG_INF("Putting EPD to sleep...");
	EPD_2IN9B_V3_Sleep();
}

/**
 * Main application entry point
 */
int main(void)
{
	LOG_INF("===========================================");
	LOG_INF("   nRF52840 E-Paper Display Demo");
	LOG_INF("   Waveshare 2.9\" Black/White Display");
	LOG_INF("===========================================");

	/* Initialize the display */
	if (epd_init() != 0) {
		LOG_ERR("Failed to initialize EPD!");
		return -1;
	}

	/* Clear the display first */
	LOG_INF("Clearing display...");
	EPD_2IN9B_V3_Clear();
	k_msleep(2000);

	/* Display demo content */
	display_demo_content();

	/* Wait a bit before putting to sleep */
	k_msleep(5000);

	/* Put display to sleep to save power */
	epd_sleep();

	LOG_INF("Demo complete! Display is now in sleep mode.");

	/* Main loop - just keep the application running */
	while (1) {
		k_msleep(10000);
	}

	return 0;
}
