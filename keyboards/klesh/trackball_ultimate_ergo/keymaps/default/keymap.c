// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_default(
        MS_BTN2,  MS_BTN1,    MS_BTN3,    LT(1, MS_BTN4),    MS_BTN4
    ),

    [1] = LAYOUT_default(
        DPI_RMOD,  DPI_MOD,    MS_BTN3,    MS_BTN4,    MS_BTN4
    )
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
    uint8_t layer_mask = 1 << highest_layer;
    trackball_set_pointer_dragscroll_enabled(layer_mask & DRAGSCROLL_LAYERS);
    return state;
}
