// Copyright 2022 Diego Palacios (@diepala)
// SPDX-License-Identifier: GPL-2.0

#include QMK_KEYBOARD_H
#include "quantum/qmk_settings.h"

// Defines names for use in layer keycodes and the keymap
enum layer_names {
    _QW,
    _LO,
    _RA,
    _SC
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[_QW] = LAYOUT_split_3x5_4(
  KC_Q,         KC_W,         KC_E,         KC_R,         KC_T,              KC_Y,    KC_U,         KC_I,         KC_O,         KC_P,
  LALT_T(KC_A), LSFT_T(KC_S), LGUI_T(KC_D), LCTL_T(KC_F), LT(_SC, KC_G),     LT(_SC, KC_H),    RCTL_T(KC_J), RGUI_T(KC_K), RSFT_T(KC_L), RALT_T(KC_SCLN),
  LCTL_T(KC_Z), KC_X,         KC_C,         KC_V,         KC_B,              KC_N,    KC_M,         KC_COMM,      KC_DOT,       LCTL_T(KC_SLSH),
                  KC_SLEP, LT(_LO, KC_ENT), LT(_SC, KC_ESC), KC_BTN2,      KC_BTN1, LT(_RA, KC_BSPC), RSFT_T(KC_SPC),  KC_MPLY
),


[_LO] = LAYOUT_split_3x5_4(
  KC_F9,   KC_F10,  KC_F11,   KC_F12, LGUI(KC_F3),      _______,  KC_7, KC_8, KC_9, KC_EQL,
  KC_F5,   KC_F6,   KC_F7,    KC_F8,  LGUI(KC_F2),      KC_PMNS, KC_4, KC_5, KC_6, KC_PPLS,
  KC_F1,   KC_F2,   KC_F3,    KC_F4,  LGUI(KC_F1),      KC_PAST, KC_1, KC_2, KC_3, KC_PSLS,

           _______, _______, _______, _______,          _______, KC_0, KC_DOT, _______
),


[_RA] = LAYOUT_split_3x5_4(
  KC_QUOT, KC_DQUO, KC_EXLM, KC_ASTR, KC_TILD,      _______, KC_UNDS, _______, KC_GRV,  KC_PIPE,
  KC_AT,   KC_HASH, KC_DLR,  KC_LPRN, KC_RPRN,      _______, KC_LBRC, KC_RBRC, KC_LCBR, KC_RCBR,
  _______, KC_PERC, _______, KC_CIRC, KC_BSLS,      KC_AMPR, _______, KC_PGUP, KC_PGDN, _______,
           _______, _______, _______, _______,      _______, _______, _______, _______
),


[_SC] = LAYOUT_split_3x5_4(
  LALT(KC_F4),  _______, KC_END,  _______,       _______,            KC_WBAK, LCTL(KC_PGUP), LCTL(KC_PGDN), KC_WFWD, KC_MPRV,
  KC_HOME,      KC_BSPC, KC_DEL,  LCTL(KC_RGHT), _______,            KC_LEFT, KC_DOWN,       KC_UP,         KC_RGHT, RGB_TOG,
  KC_WH_L,      KC_WH_R, KC_CAPS, _______,       LCTL(KC_LEFT),      KC_MNXT, _______,       KC_WH_U,       KC_WH_D,  KC_MPLY,
                _______, _______, _______, _______,                  KC_BTN3, KC_BTN2, _______, _______
),

};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [_QW] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_LEFT, KC_RIGHT) },
    [_LO] = { ENCODER_CCW_CW(RGB_HUD, RGB_HUI), ENCODER_CCW_CW(RGB_SAD, RGB_SAI)  },
    [_RA] = { ENCODER_CCW_CW(KC_WH_U, KC_WH_D), ENCODER_CCW_CW(RGB_RMOD, RGB_MOD) },
    [_SC] = { ENCODER_CCW_CW(RGB_SPD, RGB_SPI), ENCODER_CCW_CW(RGB_VAD, RGB_VAI)  },
};
#endif


void keyboard_post_init_user(void) {
    vial_combo_entry_t tab = {
      {KC_E, KC_R, KC_NO, KC_NO},
      KC_TAB,
    };
    dynamic_keymap_set_combo(0, &tab);
    vial_combo_entry_t btn3 = {
      {KC_BTN1, KC_BTN2, KC_NO, KC_NO},
      KC_BTN3,
    };
    dynamic_keymap_set_combo(1, &btn3);
    char taphold_permissive_hold = 1 << 0;
    char taphold_ignore_mod_tap_interrupt= 1 << 1;
    char taphold_qsid_8 = taphold_permissive_hold | taphold_ignore_mod_tap_interrupt;
    qmk_settings_set(8, &taphold_qsid_8, sizeof(taphold_qsid_8));
}
