// Copyright 2024 Klesh Wong (@klesh)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once


// Enables support for extended i16 mouse reports (instead of i8).
#define MOUSE_EXTENDED_REPORT

// Limits the frequency that the sensor is polled for motion.
#define POINTING_DEVICE_TASK_THROTTLE_MS 10

// Adjust trackpad rotation.
#define POINTING_DEVICE_ROTATION_90
#define POINTING_DEVICE_INVERT_X
// #define POINTING_DEVICE_INVERT_Y
#define TRACKBALL_SYM8_DRAGSCROLL_REVERSE_Y

//#define POINTING_DEVICE_DEBUG

#define PMW3610_SDIO_PIN    GP9
#define PMW3610_SCLK_PIN    GP10
#define PMW3610_CS_PIN      GP11
#define POINTING_DEVICE_MOTION_PIN  GP12