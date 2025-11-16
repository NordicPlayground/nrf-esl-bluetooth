# Simple E-Paper Display Demo

A simplified project to control a Waveshare 2.9" black and white e-paper display using the nRF52840 Development Kit.

## Hardware Requirements

- **Board**: nRF52840 Development Kit (nRF52840 DK)
- **Display**: Waveshare 2.9" E-Paper Display (128x296 pixels)
  - Model: 2.9inch e-Paper Module (B) V3
  - Colors: Black and White (with optional Red layer support)
- **Connections**: See wiring diagram below

## Wiring Connections

Connect the e-paper display to the nRF52840 DK using the Arduino headers:

| EPD Pin | Function | nRF52840 DK Pin | Arduino Pin |
|---------|----------|-----------------|-------------|
| VCC     | Power    | 3.3V            | 3.3V        |
| GND     | Ground   | GND             | GND         |
| DIN     | MOSI     | P1.03           | D11         |
| CLK     | SCK      | P1.05           | D13         |
| CS      | Chip Sel | P1.02           | D10         |
| DC      | Data/Cmd | P1.01           | D9          |
| RST     | Reset    | P1.00           | D8          |
| BUSY    | Busy     | P0.31           | D7          |

## Project Structure

```
simple_epd_demo/
├── src/
│   └── main.c                    # Main application code
├── driver/
│   ├── DEV_Config.c/h           # Hardware abstraction layer (SPI, GPIO)
│   └── EPD_2in9b_V3.c/h         # 2.9" E-Paper driver
├── paint/
│   ├── paint.c                   # Graphics library
│   ├── font*.c                   # Font definitions (8, 12, 16, 20, 24 pt)
│   └── include/
│       ├── paint.h              # Graphics API
│       └── fonts.h              # Font declarations
├── boards/
│   └── nrf52840dk_nrf52840.overlay  # Device tree overlay (pin mapping)
├── CMakeLists.txt               # Build configuration
├── prj.conf                     # Zephyr project configuration
├── Kconfig                      # Configuration options
└── README.md                    # This file
```

## Features

- Simple initialization and display control
- Graphics library with:
  - Text rendering with multiple font sizes (8, 12, 16, 20, 24 pt)
  - Basic shapes (rectangles, points)
  - Fill operations
- Low power sleep mode support
- Clear and concise example code

## Building the Project

### Prerequisites

1. Install [nRF Connect SDK](https://www.nordicsemi.com/Products/Development-software/nRF-Connect-SDK) (v2.6.0 or later recommended)
2. Set up your Zephyr development environment
3. Connect the nRF52840 DK to your computer via USB

### Build Commands

```bash
# Navigate to the project directory
cd simple_epd_demo

# Build the project
west build -b nrf52840dk_nrf52840

# Flash to the nRF52840 DK
west flash

# View serial output (optional)
west espressif monitor
# or use a terminal program like minicom, screen, or PuTTY at 115200 baud
```

### Alternative Build (using cmake directly)

```bash
# Create build directory
mkdir build && cd build

# Configure
cmake -DBOARD=nrf52840dk_nrf52840 ..

# Build
make

# Flash
make flash
```

## Usage

1. **Connect Hardware**: Wire the e-paper display to the nRF52840 DK according to the wiring table above
2. **Build & Flash**: Follow the build instructions to compile and flash the firmware
3. **Power On**: The display will automatically:
   - Initialize
   - Clear the screen
   - Display demo content (title, text, border)
   - Enter sleep mode after 5 seconds

## Customizing the Display

### Drawing Text

```c
// Draw text at position (x, y) with specified font and color
paint_DrawString("Hello World", &Font16, BLACK, 10, 50);
```

Available fonts: `Font8`, `Font12`, `Font16`, `Font20`, `Font24`

Available colors: `WHITE`, `BLACK`, `RED` (note: RED shows as black on B/W displays)

### Drawing Shapes

```c
// Draw a single point
paint_DrawPoint(BLACK, x, y);

// Draw a filled rectangle
paint_rect_t rect = {
    .x = 10,
    .y = 10,
    .width = 100,
    .height = 50
};
paint_FillRect(BLACK, &rect);
```

### Clearing the Display

```c
// Clear entire display to white
paint_clear();
// or
paint_Fill(WHITE);
```

### Updating the Display

After drawing, send the buffers to the display:

```c
EPD_2IN9B_V3_Display(black_buffer, red_buffer);
```

## Display Specifications

- **Size**: 2.9 inches
- **Resolution**: 128 x 296 pixels
- **Display Color**: Black, White (2-color)
- **Communication**: SPI
- **Refresh Time**: ~3 seconds (full refresh)
- **Viewing Angle**: >170°
- **Operating Voltage**: 3.3V
- **Operating Temperature**: 0~50°C

## Power Management

The display supports sleep mode to reduce power consumption:

```c
// Enter deep sleep mode
EPD_2IN9B_V3_Sleep();

// To wake up, re-initialize the display
EPD_2IN9B_V3_Init();
```

## Troubleshooting

### Display doesn't update
- Check all wiring connections
- Verify 3.3V power is connected
- Ensure SPI pins are correctly mapped in the overlay file
- Check serial output for error messages

### Build errors
- Make sure you're using a compatible nRF Connect SDK version
- Verify all source files are present
- Check that Zephyr environment variables are set correctly

### Display shows garbage
- Ensure buffers are properly initialized
- Check that buffer size matches display dimensions
- Verify SPI frequency is within spec (max 4 MHz)

## Technical Details

### Buffer Format

The display uses a 1-bit-per-pixel format:
- Buffer size = (width / 8) * height = (128 / 8) * 296 = 4,736 bytes
- Two buffers: one for black/white, one for red (optional)
- Bit value 0 = colored pixel, 1 = white pixel

### SPI Configuration

- Mode: SPI Mode 0 (CPOL=0, CPHA=0)
- Frequency: Up to 4 MHz
- Bit order: MSB first
- Word size: 8 bits

## License

This demo uses components from:
- Waveshare e-Paper library (MIT License)
- Nordic Semiconductor samples (Nordic 5-Clause License)

## References

- [Waveshare 2.9" E-Paper Documentation](https://www.waveshare.com/wiki/2.9inch_e-Paper_Module_(B))
- [nRF52840 DK User Guide](https://infocenter.nordicsemi.com/topic/ug_nrf52840_dk/UG/dk/intro.html)
- [Zephyr Display API](https://docs.zephyrproject.org/latest/hardware/peripherals/display.html)

## Support

For issues or questions:
1. Check the troubleshooting section
2. Review the serial console output
3. Verify hardware connections
4. Consult Waveshare and Nordic documentation

## Next Steps

To extend this demo:
- Add button controls for interactive updates
- Implement partial refresh for faster updates
- Add sensor data display (temperature, humidity, etc.)
- Create a menu system
- Add Bluetooth connectivity to update display wirelessly
