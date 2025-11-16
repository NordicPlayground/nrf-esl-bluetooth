# Simple E-Paper Display Demo

## Quick Access

Looking for a simple way to control the Waveshare 2.9" E-Paper display with your nRF52840 DK?

**Navigate to: [`simple_epd_demo/`](simple_epd_demo/)**

This simplified demo removes all the ESL (Electronic Shelf Label) and Bluetooth complexity, providing a clean, easy-to-understand project focused solely on controlling the e-paper display.

## What's Included

- ✅ Simple initialization and display control
- ✅ Graphics library with text and shapes
- ✅ Complete wiring diagrams and pinout guide
- ✅ Step-by-step build instructions
- ✅ Example code with comments
- ✅ No Bluetooth or ESL complexity

## Get Started in 3 Steps

1. **Wire** your display using the pinout guide in `simple_epd_demo/PINOUT.txt`
2. **Build** the project:
   ```bash
   cd simple_epd_demo
   west build -b nrf52840dk_nrf52840
   west flash
   ```
3. **Watch** your display show the demo content!

## Documentation

- [Quick Start Guide](simple_epd_demo/QUICK_START.md) - Get running in 5 minutes
- [Full README](simple_epd_demo/README.md) - Complete documentation
- [Pinout Diagram](simple_epd_demo/PINOUT.txt) - Detailed wiring guide

## For ESL/Bluetooth Features

If you need the full Electronic Shelf Label functionality with Bluetooth support, see the main project documentation at:
https://nordicplayground.github.io/nrf-esl-bluetooth/index.html
