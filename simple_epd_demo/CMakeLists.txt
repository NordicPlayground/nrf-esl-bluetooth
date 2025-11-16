# Simple E-Paper Display Demo - CMake Build Configuration

cmake_minimum_required(VERSION 3.20.0)

# Find Zephyr package
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(simple_epd_demo)

# Main application source
target_sources(app PRIVATE src/main.c)

# EPD Driver sources
target_sources(app PRIVATE
    driver/DEV_Config.c
    driver/EPD_2in9b_V3.c
)

# Paint Library sources
target_sources(app PRIVATE
    paint/paint.c
    paint/font8.c
    paint/font12.c
    paint/font16.c
    paint/font20.c
    paint/font24.c
)

# Include directories
target_include_directories(app PRIVATE
    driver
    paint/include
)

# Device Tree Source overlay
set(DTC_OVERLAY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/boards/nrf52840dk_nrf52840.overlay")
