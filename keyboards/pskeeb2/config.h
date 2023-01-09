// Copyright 2022 Klesh Wong (@klesh)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "config_common.h"

/* key matrix size */
#define MATRIX_ROWS 8
#define MATRIX_COLS 5

#define MASTER_RIGHT

#define DIRECT_PINS {{B12, B15, B10, B1, B0}, \
                     {B13, A8, B8, A4, A3}, \
                     {B14, A15, B9, A1, A2}, \
                     {B3, B4, B5, A0, NO_PIN}}

#define SERIAL_USART_FULL_DUPLEX   // Enable full duplex operation mode.
#define SERIAL_USART_TX_PIN B6     // USART TX pin
#define SERIAL_USART_RX_PIN B7     // USART RX pin

/* Top left key on left half */
#define BOOTMAGIC_LITE_ROW    0
#define BOOTMAGIC_LITE_COLUMN 0
/* Top right key on right half */
#define BOOTMAGIC_LITE_ROW_RIGHT    4
#define BOOTMAGIC_LITE_COLUMN_RIGHT 4

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
//#define NO_DEBUG

/* disable print */
//#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT


/* important for home-row-modifiers */
#define IGNORE_MOD_TAP_INTERRUPT


/* trackpoint */
#define PS2_CLOCK_PIN A7
#define PS2_DATA_PIN  A6
#define PS2_MOUSE_ROTATE 90


// /* joystick */
// #define ANALOG_JOYSTICK_X_AXIS_PIN A0
// #define ANALOG_JOYSTICK_Y_AXIS_PIN A1
// #define ANALOG_JOYSTICK_CLICK_PIN B9
// #define ANALOG_JOYSTICK_READ_INTERVAL 1
// #define ANALOG_JOYSTICK_SPEED_REGULATOR 10


/* encoder */
#define ENCODERS_PAD_A {C14}
#define ENCODERS_PAD_B {C15}
#define ENCODER_RESOLUTION 2
#define ENCODERS_A_REVERSE
#define ENCODERS_B_REVERSE
