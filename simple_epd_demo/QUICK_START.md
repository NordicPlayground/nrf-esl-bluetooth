# Quick Start Guide - E-Paper Display Demo

Get your Waveshare 2.9" E-Paper display running in 5 minutes!

## Step 1: Hardware Setup

Connect your e-paper display to the nRF52840-preview-DK Arduino headers:

```
EPD Display          nRF52840-preview-DK (Arduino)
-----------          -----------------------------
VCC         ------>  3.3V
GND         ------>  GND
DIN (MOSI)  ------>  D11
CLK (SCK)   ------>  D13
CS          ------>  D10
DC          ------>  D9
RST         ------>  D6  (changed from D8 - not available on preview DK)
BUSY        ------>  D7
```

**Note**: If D6 is not available, you can use D5, D4, D3, or D2. Just update
the `reset-gpios` line in `boards/nrf52840dk_nrf52840.overlay` accordingly.

## Step 2: Build and Flash

```bash
cd simple_epd_demo
west build -b nrf52840dk_nrf52840
west flash
```

## Step 3: See It Work!

The display will:
1. Initialize (takes ~2 seconds)
2. Clear to white
3. Show demo content with text and graphics
4. Go to sleep mode after 5 seconds

## Viewing Debug Output

Connect a serial terminal (115200 baud) to see debug messages:

```bash
# Linux/Mac
screen /dev/ttyACM0 115200

# Or use minicom
minicom -D /dev/ttyACM0 -b 115200

# Windows - use PuTTY or TeraTerm
# Port: COMx, Baud: 115200
```

## Troubleshooting

**Display doesn't update?**
- Check wiring - especially CS, DC, RST, and BUSY pins
- Verify 3.3V power is connected
- Make sure display is properly seated in breadboard/connectors

**Build fails?**
- Ensure nRF Connect SDK is installed and environment is set up
- Run `west update` to sync dependencies
- Check that you're in the simple_epd_demo directory

**Garbage on display?**
- Power cycle both the nRF52840 DK and the display
- Re-flash the firmware
- Check SPI connections (DIN and CLK)

## Customize Your Display

Edit `src/main.c` in the `display_demo_content()` function:

```c
// Change the text
paint_DrawString("Your Text Here", &Font20, BLACK, 10, 40);

// Use different fonts: Font8, Font12, Font16, Font20, Font24
paint_DrawString("Small text", &Font8, BLACK, 10, 80);

// Draw rectangles
paint_rect_t box = { .x = 10, .y = 100, .width = 50, .height = 30 };
paint_FillRect(BLACK, &box);
```

Rebuild and flash to see your changes!

## What's Next?

- Read the full [README.md](README.md) for detailed documentation
- Modify the demo to display sensor data
- Add button controls for interactivity
- Experiment with different layouts and graphics

Happy coding! 🎨
