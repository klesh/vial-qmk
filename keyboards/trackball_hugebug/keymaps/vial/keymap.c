// Copyright 2022 Diego Palacios (@diepala)
// SPDX-License-Identifier: GPL-2.0

#include "keycodes.h"
#include QMK_KEYBOARD_H
#include "trackball_hugebug.h"
// #include "quantum/qmk_settings.h"

// Defines names for use in layer keycodes and the keymap
enum layer_names {
    _RH, // right hand
    _LH, // left hand
    _RA, // raise
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[_RH] = LAYOUT_default(
  KC_BTN1,                                                  KC_BTN2,
  LT(_RA, KC_WBAK),                                         KC_WFWD
),


[_LH] = LAYOUT_default(
  KC_BTN2,                                                  KC_BTN1,
  KC_WFWD,                                                  LT(_RA, KC_WBAK)
),


[_RA] = LAYOUT_default(
  // POINTER_SNIPING_DPI_FORWARD,                              POINTER_DEFAULT_DPI_FORWARD,
  // POINTER_SNIPING_DPI_REVERSE,                              POINTER_DEFAULT_DPI_REVERSE
  KC_BTN1,                                                  KC_BTN2,
  LT(_RA, KC_WBAK),                                         KC_WFWD
),

};


void keyboard_post_init_user(void) {
    debug_enable=true;
    debug_mouse=true;
    debug_keyboard=true;
    debug_matrix=true;
    uprintf("Trackball Sym8 initialized\n");
}
