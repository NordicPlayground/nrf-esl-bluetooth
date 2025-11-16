# Changelog

All notable changes to the Simple E-Paper Display Demo will be documented in this file.

## [1.2.0] - 2025-01-16

### Changed
- **Updated pin configuration for nRF52840-preview-DK**
  - Changed RST (Reset) pin from D8 (P1.00) to D6 (P0.30)
  - D8/P1.00 is not available on nRF52840-preview-DK Arduino headers
  - Updated device tree overlay reset-gpios to use arduino_header 12 (D6)

### Added
- Documentation for alternative RST pin options (D5, D4, D3, D2)
- Instructions for changing RST pin in device tree overlay
- nRF52840-preview-DK specific notes in all documentation

### Updated
- README.md: Updated wiring table and added RST pin configuration note
- QUICK_START.md: Updated hardware setup with new pin mapping
- PINOUT.txt: Updated connection table for preview DK
- boards/nrf52840dk_nrf52840.overlay: Comments with alternative pin options

## [1.1.0] - 2025-01-16

### Changed
- **Updated for Zephyr 4.2.1 compatibility**
  - Changed device tree compatible string from `generic,epd` to `zephyr,epd-waveshare`
  - Added proper device tree binding file (`dts/bindings/zephyr,epd-waveshare.yaml`)
  - Fixed CMakeLists.txt to set `DTS_ROOT` before `find_package(Zephyr)`
  - Removed deprecated `DTC_OVERLAY_FILE` setting (now auto-detected)
  - Updated `DEV_Config.c` to use correct `DT_DRV_COMPAT` macro

### Fixed
- Resolved "unknown vendor prefix 'generic'" warning
- Fixed "DTC_OVERLAY_FILE set too late" warning
- Corrected GPIO devicetree property access for Zephyr 4.x
- Fixed SPI frequency configuration for newer Zephyr versions

### Added
- Device tree binding file with proper vendor prefix
- Version compatibility section in README
- Tested with Zephyr SDK 0.17.0

### Technical Details
The main compatibility issues addressed:
1. Zephyr 4.x requires registered vendor prefixes in device tree compatibles
2. Device tree bindings must be in `dts/bindings/` directory with `DTS_ROOT` set
3. Overlay files are now auto-detected from `boards/` directory
4. Updated devicetree macro usage for GPIO and SPI property access

## [1.0.0] - 2025-01-16

### Added
- Initial release
- Simple E-Paper display control for Waveshare 2.9" display
- nRF52840 DK support
- Paint library with 5 font sizes (8, 12, 16, 20, 24pt)
- Basic graphics functions (rectangles, points, text)
- Complete documentation (README, Quick Start, Pinout diagram)
- Example demo application
- SPI driver abstraction
- EPD 2.9" V3 hardware driver
- Power management with sleep mode
