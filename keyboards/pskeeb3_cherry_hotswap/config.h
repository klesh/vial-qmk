// Copyright 2022 Klesh Wong (@klesh)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/* key matrix size */
#define MATRIX_ROWS 8
#define MATRIX_COLS 5

#define SERIAL_USART_FULL_DUPLEX   // Enable full duplex operation mode.
#define SERIAL_USART_TX_PIN GP12   // USART TX pin
#define SERIAL_USART_RX_PIN GP13   // USART RX pin
// #define SERIAL_PIO_USE_PIO1

/* state sync for oled/rgb on the slave */
#define MASTER_RIGHT
#define SPLIT_LAYER_STATE_ENABLE
#define SPLIT_LED_STATE_ENABLE
#define SPLIT_TRANSPORT_MIRROR
// #define SPLIT_POINTING_ENABLE

#define IS_COMMAND() ( \
    keyboard_report->mods == (MOD_BIT(KC_LSFT) | MOD_BIT(KC_RSFT)) || \
    keyboard_report->mods == (MOD_BIT(KC_LALT) | MOD_BIT(KC_RALT)) \
)
