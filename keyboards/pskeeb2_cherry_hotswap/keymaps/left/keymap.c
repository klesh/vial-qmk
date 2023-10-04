// Copyright 2022 Diego Palacios (@diepala)
// SPDX-License-Identifier: GPL-2.0

#include QMK_KEYBOARD_H

// Defines names for use in layer keycodes and the keymap
enum layer_names {
    _QWERTY,
    _LOWER,
    _RAISE,
    _SHORTCUT
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[_QWERTY] = LAYOUT_split_3x5_4(
  KC_Q,         KC_W,         KC_E,         KC_R,         KC_T,              KC_Y,    KC_U,         KC_I,         KC_O,         KC_P,
  LALT_T(KC_A), LSFT_T(KC_S), LGUI_T(KC_D), LCTL_T(KC_F), KC_G,              LT(_SHORTCUT, KC_H),    RCTL_T(KC_J), RGUI_T(KC_K), RSFT_T(KC_L), RALT_T(KC_SCLN),
  LCTL_T(KC_Z), KC_X,         KC_C,         KC_V,         KC_B,              KC_N,    KC_M,         KC_COMM,      KC_DOT,       LCTL_T(KC_SLSH),
    TO(_QWERTY),   KC_TAB,  LT(_LOWER, KC_ENT),  LT(_SHORTCUT, KC_ESC),      KC_BTN1, RSFT_T(KC_SPC),  LT(_RAISE, KC_BSPC),  KC_MPLY
),


[_LOWER] = LAYOUT_split_3x5_4(
  KC_F9,   KC_F10,  KC_F11,   KC_F12, LGUI(KC_F3),      _______,  KC_7, KC_8, KC_9, KC_EQL,
  KC_F5,   KC_F6,   KC_F7,    KC_F8,  LGUI(KC_F2),      KC_PMNS, KC_4, KC_5, KC_6, KC_PPLS,
  KC_F1,   KC_F2,   KC_F3,    KC_F4,  LGUI(KC_F1),      KC_PAST, KC_1, KC_2, KC_3, KC_PSLS,

           _______, _______, _______, _______,          KC_0,    KC_DOT, _______, _______
),


[_RAISE] = LAYOUT_split_3x5_4(
  KC_QUOT, KC_DQUO, KC_EXLM, KC_ASTR, KC_TILD,      _______, KC_UNDS, _______, KC_GRV,  KC_PIPE,
  KC_AT,   KC_HASH, KC_DLR,  KC_LPRN, KC_RPRN,      _______, KC_LBRC, KC_RBRC, KC_LCBR, KC_RCBR,
  _______, KC_PERC, _______, KC_CIRC, KC_BSLS,      KC_AMPR, _______, _______, _______, _______,
           _______, _______, _______, _______,      _______, _______, _______, _______
),


[_SHORTCUT] = LAYOUT_split_3x5_4(
  LALT(KC_F4),  _______, KC_END,  _______,       _______,            KC_WBAK, LCTL(KC_PGUP), LCTL(KC_PGDN), KC_WFWD, KC_MPRV,
  KC_HOME,      KC_BSPC, KC_DEL,  LCTL(KC_RGHT), _______,            KC_LEFT, KC_DOWN,       KC_UP,         KC_RGHT, _______,
  _______,      _______, KC_CAPS, _______,       LCTL(KC_LEFT),      KC_MNXT, _______,       _______,      _______,  KC_MPLY,
                _______, _______, _______, _______,                  KC_BTN3, KC_BTN2, _______, _______
),

};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [_QWERTY]   = { ENCODER_CCW_CW(KC_RIGHT, KC_LEFT),    ENCODER_CCW_CW(KC_VOLD, KC_VOLU)  },
    [_LOWER]    = { ENCODER_CCW_CW(RGB_HUD, RGB_HUI),     ENCODER_CCW_CW(RGB_SAD, RGB_SAI)  },
    [_RAISE]    = { ENCODER_CCW_CW(RGB_VAD, RGB_VAI),     ENCODER_CCW_CW(RGB_SPD, RGB_SPI)  },
    [_SHORTCUT] = { ENCODER_CCW_CW(RGB_RMOD, RGB_MOD),    ENCODER_CCW_CW(KC_MS_WH_UP, KC_MS_WH_DOWN) },
};
#endif

#ifdef OLED_ENABLE
oled_rotation_t oled_init_kb(oled_rotation_t rotation) {
    if (!is_keyboard_master()) {
        return OLED_ROTATION_180;  // flips the display 180 degrees if offhand
    }

    return rotation;
}
bool oled_task_user(void) {
    // Host Keyboard Layer Status
    oled_write_P(PSTR("Layer: "), false);

    switch (get_highest_layer(layer_state)) {
        case _QWERTY:
            oled_write_P(PSTR("Default\n"), false);
            break;
        case _LOWER:
            oled_write_P(PSTR("FN_NUM\n"), false);
            break;
        case _RAISE:
            oled_write_P(PSTR("SYMBOL\n"), false);
            break;
        case _SHORTCUT:
            oled_write_P(PSTR("SHORTCUT\n"), false);
            break;
        default:
            // Or use the write_ln shortcut over adding '\n' to the end of your string
            oled_write_ln_P(PSTR("Undefined"), false);
    }

    // Host Keyboard LED Status
    led_t led_state = host_keyboard_led_state();
    oled_write_P(led_state.num_lock ? PSTR("NUM ") : PSTR("    "), false);
    oled_write_P(led_state.caps_lock ? PSTR("CAP ") : PSTR("    "), false);
    oled_write_P(led_state.scroll_lock ? PSTR("SCR ") : PSTR("    "), false);
    
    return false;
}
#endif