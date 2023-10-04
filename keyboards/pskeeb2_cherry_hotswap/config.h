// Copyright 2022 Klesh Wong (@klesh)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/* key matrix size */
#define MATRIX_ROWS 8
#define MATRIX_COLS 5

#define MASTER_RIGHT

#define SERIAL_USART_FULL_DUPLEX   // Enable full duplex operation mode.
#define SERIAL_USART_TX_PIN GP12     // USART TX pin
#define SERIAL_USART_RX_PIN GP13     // USART RX pin
// #define SERIAL_PIO_USE_PIO1 

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


// /* oled */


/* encoder */
#define ENCODERS_PAD_A {GP19}
#define ENCODERS_PAD_B {GP20}
#define ENCODER_RESOLUTION 4
#define ENCODERS_A_REVERSE
#define ENCODERS_B_REVERSE


/* ws2812 RGB LED */
#ifdef RGB_MATRIX_ENABLE
#    define WS2812_DI_PIN   GP18
#    define RGB_MATRIX_DRIVER ws2812  
#    define RGB_MATRIX_LED_COUNT 12
#    define RGB_MATRIX_SPLIT \
        { 6, 6 }
#    define RGB_MATRIX_FRAMEBUFFER_EFFECTS
#    define RGB_MATRIX_KEYPRESSES
#    define RGB_MATRIX_KEYPRESSES
#endif


/* state sync for oled/rgb on the slave */
#define SPLIT_LAYER_STATE_ENABLE
#define SPLIT_LED_STATE_ENABLE
#define SPLIT_TRANSPORT_MIRROR