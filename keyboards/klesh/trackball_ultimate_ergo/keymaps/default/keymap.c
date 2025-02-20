// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_default(
        LT(1, MS_BTN4), MS_BTN1,           MS_BTN2,         LT(2, MS_BTN5),     _______
    ),

    [1] = LAYOUT_default(
        _______,        KC_LSFT,           LCTL(KC_W),      KC_LCTL,            _______
    ),

    [2] = LAYOUT_default(
        DPI_RMOD,       DPI_MOD,            _______,        _______,            _______
    )
};

const uint16_t PROGMEM btn123_combo[] = {MS_BTN1, MS_BTN2, COMBO_END};
combo_t key_combos[] = {
    COMBO(btn123_combo, MS_BTN3),
};


void keyboard_post_init_user(void) {
    // Customise these values to desired behaviour
    debug_enable=true;
    // debug_matrix=true;
    // debug_keyboard=true;
    debug_mouse=true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t highest_layer = get_highest_layer(state);
    trackball_set_pointer_dragscroll_enabled(highest_layer == DRAGSCROLL_LAYER);
    trackball_set_pointer_direction_enabled(highest_layer == DIRECTION_LAYER);
    return state;
}
