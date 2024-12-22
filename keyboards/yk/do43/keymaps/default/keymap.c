#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    // default layer
	LAYOUT(
		LT(3, KC_TAB),  KC_Q, KC_W, KC_E, KC_R, KC_T,                                       KC_Y, KC_U, KC_I, KC_O, KC_P, LT(3, KC_BSPC),
		CTL_T(KC_GRV),  ALT_T(KC_A), SFT_T(KC_S), GUI_T(KC_D), CTL_T(KC_F), KC_G,           KC_H, CTL_T(KC_J), GUI_T(KC_K), SFT_T(KC_L), ALT_T(KC_SCLN),
		LSFT_T(KC_LBRC), KC_Z, KC_X, KC_C, KC_V, KC_B,                                      KC_N, KC_M, KC_COMM,  KC_DOT, RSFT_T(KC_SLSH),
		CTL_T(KC_VOLD), GUI_T(KC_VOLU), ALT_T(KC_TAB), LT(1, KC_ENT), LT(2, KC_ESC),        SFT_T(KC_SPC), LT(3, KC_LEFT), KC_DOWN,  KC_RGHT),

    // numbers and f-keys
	LAYOUT(
		_______, KC_F9, KC_F10, KC_F11, KC_F12, KC_PGUP,                                    KC_DOT,  KC_7, KC_8, KC_9, _______, _______,
		_______, KC_F5, KC_F6,  KC_F7,  KC_F8,  _______,                                    KC_PMNS, KC_4, KC_5, KC_6, KC_PPLS,
		_______, KC_F1, KC_F2,  KC_F3,  KC_F4,  KC_PGDN,                                    KC_PAST, KC_1, KC_2, KC_3, KC_PSLS,
		_______, _______, _______, _______, _______,                                        LT(3, KC_0), KC_DOT, KC_DOT, _______),

    // shortcut keys
	LAYOUT(
		_______, _______, C(KC_RGHT), KC_END, _______, _______,                             KC_WBAK, C(KC_PGUP), C(KC_PGDN), KC_WFWD, KC_MPRV, _______,
		_______, KC_HOME, KC_BSPC, KC_DEL, _______, _______,                                KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_MPLY,
		_______, _______, _______, _______, _______, C(KC_LEFT),                            KC_MNXT, KC_MUTE, KC_PGUP, KC_UP, KC_PGDN,
		KC_VOLD, KC_VOLU, _______, _______, TO(0),                                          C(KC_SPC), KC_LEFT, KC_DOWN, KC_RGHT),

    // punctuation
	LAYOUT(
		_______, KC_QUOT, KC_DQUO, KC_EXLM, KC_ASTR, _______,                               _______, KC_UNDS, _______, _______, KC_PIPE, _______,
		_______, KC_AT,   KC_HASH, KC_DLR,  KC_LPRN, KC_RPRN,                               _______, KC_LBRC, KC_RBRC, KC_LCBR, KC_RCBR,
		_______, _______, KC_PERC, KC_CIRC, _______, KC_BSLS,                               KC_AMPR, KC_MINS, KC_PLUS, KC_EQL, _______,
		_______, _______, _______, _______, _______,                                        _______, _______, _______, _______),

    // template
	LAYOUT(
		_______, _______, _______, _______, _______, _______,                               _______, _______, _______, _______, _______, _______,
		_______, _______, _______, _______, _______, _______,                               _______, _______, _______, _______, _______,
		_______, _______, _______, _______, _______, _______,                               _______, _______, _______, _______, _______,
		_______, _______, _______, _______, _______,                                        _______, _______, _______, _______),


};

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        /*case ALT_T(KC_A):*/
        /*case SFT_T(KC_S):*/
        /*case GUI_T(KC_D):*/
        /*case CTL_T(KC_F):*/
        /*case CTL_T(KC_J):*/
        /*case GUI_T(KC_K):*/
        /*case SFT_T(KC_L):*/
        /*case ALT_T(KC_SCLN):*/
        /*case SFT_T(KC_ENT):*/
        /*case LT(1, KC_G):*/
        /*case LT(1, KC_H):*/
        /*case LT(3, KC_SPC):*/
            /*return TAPPING_TERM * 2;*/
        default:
            return TAPPING_TERM;
    }
}
