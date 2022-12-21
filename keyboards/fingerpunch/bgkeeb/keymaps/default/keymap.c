#include QMK_KEYBOARD_H

#ifdef PIMORONI_TRACKBALL_ENABLE
#include "drivers/sensors/pimoroni_trackball.h"
#include "pointing_device.h"
#include "color.h"
#endif

// Defines names for use in layer keycodes and the keymap
enum layer_names {
    _QWERTY,
    _LOWER,
    _RAISE,
    _SHORTCUT,
    _ADJUST
};


bool is_caps_lock_on;


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* Qwerty */
[_QWERTY] = LAYOUT_split_3x5_4(
  KC_Q,         KC_W,         KC_E,         KC_R,         KC_T,         KC_Y,    KC_U,         KC_I,            KC_O,           KC_P,
  LALT_T(KC_A), LSFT_T(KC_S), LGUI_T(KC_D), LCTL_T(KC_F), KC_G,         KC_H,    RCTL_T(KC_J), RGUI_T(KC_K),    RSFT_T(KC_L),   RALT_T(KC_SCLN),
  LCTL_T(KC_Z), LGUI_T(KC_X), LSFT_T(KC_C), LALT_T(KC_V), KC_B,         KC_N,    RALT_T(KC_M), RSFT_T(KC_COMM), RGUI_T(KC_DOT), RCTL_T(KC_SLSH),
       KC_MUTE,   KC_TAB,  LT(_LOWER, KC_ENT),  LT(_SHORTCUT, KC_ESC),  RSFT_T(KC_SPC) ,  LT(_RAISE, KC_BSPC),   LT(_ADJUST, KC_ENT),    KC_MPLY
),



/* Lower */
[_LOWER] = LAYOUT_split_3x5_4(
  KC_F9,   KC_F10,  KC_F11,   KC_F12, KC_PGUP,      KC_DOT,  KC_7, KC_8, KC_9, KC_EQL,
  KC_F5,   KC_F6,   KC_F7,    KC_F8,  KC_INS ,      KC_PMNS, KC_4, KC_5, KC_6, KC_PPLS,
  KC_F1,   KC_F2,   KC_F3,    KC_F4,  KC_PGDN,      KC_PAST, KC_1, KC_2, KC_3, KC_PSLS,

           _______, _______, _______, _______,      KC_0,    KC_ENT, _______, _______
),



/* Raise */
[_RAISE] = LAYOUT_split_3x5_4(
  KC_QUOT, KC_QUES, KC_EXLM, KC_ASTR, KC_TILD,      KC_CIRC, KC_UNDS, _______, KC_GRV,  KC_PIPE,
  KC_AT,   KC_HASH, KC_DLR,  KC_LPRN, KC_LBRC,      KC_RBRC, KC_RPRN, _______, _______, KC_EQL,
  KC_DQUO, KC_PERC, KC_COLN, KC_LCBR, KC_BSLS,      KC_AMPR, KC_RCBR, _______, _______, _______,
           _______, _______, _______, _______,      _______, _______, _______, _______
),



/* Shortcut */
[_SHORTCUT] = LAYOUT_split_3x5_4(
  LALT(KC_F4),  _______, KC_END,  _______, RGB_TOG,      KC_WBAK, LCTL(KC_PGUP), LCTL(KC_PGDN), KC_WFWD, KC_MPRV,
  KC_HOME,      KC_BSPC, KC_DEL,  RGB_MOD, _______,      KC_LEFT, KC_DOWN,       KC_UP,         KC_RGHT, _______,
  _______,      _______, KC_CAPS, _______, RGB_RMOD,      KC_MNXT, _______,       _______,      _______, KC_MPLY,
                _______, _______, _______, _______,      _______, _______, _______, _______
),

/* Adjust (Lower + Raise)
 *
 * ,----------------------------------.           ,----------------------------------.
 * | RGB_T| RGB_R| RGB_F|      |QWERTY|           |   F1 |  F2  |  F3  |  F4  |  F5 |
 * |------+------+------+------+------|           |------+------+------+------+------|
 * | SPD_I| HUE_I| SAT_I| VAL_I|COLEMK|           |   F6 |  F7  |  F8  |  F9  |  F10 |
 * |------+------+------+------+------|           |------+------+------+------+------|
 * | SPD_D| HUE_D| SAT_D| VAL_D|      |           |  F11 |  F12 |      |      | Reset|
 * `----------------------------------'           `----------------------------------'
 *        |      |      |                                       |      |      |
 *        `------+------'                                       `------+------'
 *                  ,--------------------.    ,--------------------.
 *                  | LOWER|      |      |    |      |      | RAISE|
 *                  `--------------------'    `--------------------.
 */
[_ADJUST] =  LAYOUT_split_3x5_4(
  RGB_TOG, RGB_RMOD, RGB_MOD, _______, _______,      _______,  KC_F21,  KC_F22,  KC_F23,  KC_F24,
  RGB_SPI, RGB_HUI,  RGB_SAI, RGB_VAI, _______,      _______,  KC_F17,  KC_F18,  KC_F19,  KC_F20,
  RGB_SPD, RGB_HUD,  RGB_SAD, RGB_VAD, _______,      _______,  KC_F13,  KC_F14,  KC_F15,  KC_F16,
           _______,  _______, _______, _______,       _______, _______, _______, _______
),
};

/*
layer_state_t layer_state_set_user(layer_state_t state) {
    #ifdef PIMORONI_TRACKBALL_ENABLE
    switch(get_highest_layer(state)) {
        case _QWERTY:
            if (is_caps_lock_on) {
                trackball_set_rgbw(RGB_RED, 0x00);
            } else {
                trackball_set_rgbw(RGB_BLUE, 0x00);
            }
            break;
        case _COLEMAK:
            trackball_set_rgbw(RGB_GREEN, 0x00);
            break;
        case _LOWER:
            trackball_set_rgbw(RGB_PURPLE, 0x00);
            break;
        case _RAISE:
            trackball_set_rgbw(RGB_YELLOW, 0x00);
            break;
        case _ADJUST:
            trackball_set_rgbw(RGB_ORANGE, 0x00);
            break;
        default: //  for any other layers, or the default layer
            if (is_caps_lock_on) {
                trackball_set_rgbw(RGB_RED, 0x00);
            } else {
                trackball_set_rgbw(RGB_BLUE, 0x00);
            }
            break;
    }
    #endif
    return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}
*/

void keyboard_post_init_user(void) {
    is_caps_lock_on = false;
    #ifdef PIMORONI_TRACKBALL_ENABLE
    trackball_set_rgbw(RGB_BLUE, 0x00);
    #endif
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_CAPSLOCK:
            if (record->event.pressed) {
                if (is_caps_lock_on) {
                    is_caps_lock_on = false;
                } else {
                    is_caps_lock_on = true;
                }
            }
            break;
        default:
            break;
    }

    return true;
}

#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {
    // default behavior if undefined
    if (index == 0) {
        // Conditional to reverse the direction of encoder number 1
        // The reason I have this is that for some of my boards, it supports two different types of encoders, and they may differ in direction
        #ifdef ENCODERS_A_REVERSE
        if (!clockwise) {
        #else
        if (clockwise) {
        #endif
            tap_code(KC_VOLU);
        } else {
            tap_code(KC_VOLD);
        }
    }
    else if (index == 1) {
      // Conditional to reverse the direction of encoder number 1
      // The reason I have this is that for some of my boards, it supports two different types of encoders, and they may differ in direction
      #ifdef ENCODERS_B_REVERSE
      if (!clockwise) {
      #else
      if (clockwise) {
      #endif
        tap_code16(KC_RGHT);
      }
      else{
        tap_code16(KC_LEFT);
      }
    }

    return true;
}
#endif

#ifdef TAPPING_TERM_PER_KEY
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LT(_QWERTY, KC_A):
        case LT(_QWERTY, KC_S):
        case LT(_QWERTY, KC_D):
        case LT(_QWERTY, KC_F):
        case LT(_QWERTY, KC_J):
        case LT(_QWERTY, KC_K):
        case LT(_QWERTY, KC_L):
        case LT(_QWERTY, KC_SCLN):
            return 400;
        default:
            return TAPPING_TERM;
    }
}
#endif
