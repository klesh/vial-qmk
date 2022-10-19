// Copyright 2022 Klesh Wong (@klesh)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "config_common.h"

/* key matrix size */
#define MATRIX_ROWS 8
#define MATRIX_COLS 5

#define MASTER_RIGHT

#define DIRECT_PINS {{B10, B1, B0, A7, A6}, \
                     {A5, A4, A3, A2, B8}, \
                     {C15, C14, B12, B13, B14}, \
                     {B5, B4, B3, A15, NO_PIN}}

/*
#define DIRECT_PINS_RIGHT {{A6, A7, B0, B1, B10}, \
                     {B8, A2, A3, A4, A5}, \
                     {B14, B13, B12, C14, C15}, \
                     {NO_PIN, A15, B3, B4, B5}}
                     */

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
#define PS2_CLOCK_PIN A0
#define PS2_DATA_PIN  A1
#define PS2_MOUSE_ROTATE 270


/* joystick */
#define ANALOG_JOYSTICK_X_AXIS_PIN A0
#define ANALOG_JOYSTICK_Y_AXIS_PIN A1
#define ANALOG_JOYSTICK_CLICK_PIN B9
#define ANALOG_JOYSTICK_READ_INTERVAL 1
#define ANALOG_JOYSTICK_SPEED_REGULATOR 10


/* encoder */
#define ENCODERS_PAD_A {B15}
#define ENCODERS_PAD_B {A8}
#define ENCODER_RESOLUTION 2
#define ENCODERS_A_REVERSE
#define ENCODERS_B_REVERSE
