// Copyright 2022 Diego Palacios (@diepala)
// SPDX-License-Identifier: GPL-2.0

#include "keycodes.h"
#include QMK_KEYBOARD_H
#include "trackball_sym8.h"
// #include "quantum/qmk_settings.h"

// Defines names for use in layer keycodes and the keymap
enum layer_names {
    _RH, // right hand
    _LH, // left hand
    _RA, // raise
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[_RH] = LAYOUT_default(
                                TO(_LH), P_DBG_I,
  KC_WBAK,                                                  DRAGSCROLL_MODE_TOGGLE,
  KC_BTN1,                                                  KC_BTN2,
                                KC_BTN3, LT(_RA, KC_WFWD) 
),


[_LH] = LAYOUT_default(
                                TO(_LH), TO(_RH),
  DRAGSCROLL_MODE,                                          KC_WBAK,
  KC_BTN3,                                                  KC_BTN1,
                                LT(_RA, KC_WFWD), KC_BTN2
),


[_RA] = LAYOUT_default(
                                _______, _______,
  POINTER_SNIPING_DPI_FORWARD,                              POINTER_DEFAULT_DPI_FORWARD,
  POINTER_SNIPING_DPI_REVERSE,                              POINTER_DEFAULT_DPI_REVERSE,
                                _______, _______
),

};


void keyboard_post_init_user(void) {
    debug_enable=true;
    debug_mouse=true;
    debug_keyboard=true;
    // debug_matrix=true;
    uprintf("Trackball Sym8 initialized\n");
}
