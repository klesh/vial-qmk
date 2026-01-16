/*
 * Helper keycodes for selectable pointing-device CPI presets.
 *
 * Usage:
 * - Include this header in a Vial/keymap file to get the keycode names.
 * - Call `process_pointing_device_cpi(keycode, record)` from your
 *   `process_record_user` and return early if it returns `true`.
 *
 * Example (in keymap.c):
 *   #include "pointing_device_vial_keycodes.h"
 *
 *   bool process_record_user(uint16_t keycode, keyrecord_t *record) {
 *       if (process_pointing_device_cpi(keycode, record)) return false;
 *       // ... other handlers
 *   }
 */

#pragma once

#include "quantum_keycodes.h"
#include "pointing_device_cpi_roller.h"
#include "action.h"

/* Define a compact set of CPI-related keycodes starting at SAFE_RANGE.
 * These are intended to be globally available to keymaps (including Vial
 * keymaps). They occupy the custom keycode range and therefore should not
 * collide with other custom keycodes in a keymap; include this header
 * early in the keymap to reserve the enum values.
 */
enum pointing_device_keycodes {
    PD_CPI_PREV = SAFE_RANGE,
    PD_CPI_NEXT,
    PD_CPI_SET_0, PD_CPI_SET_1, PD_CPI_SET_2, PD_CPI_SET_3, PD_CPI_SET_4,
    PD_CPI_SET_5, PD_CPI_SET_6, PD_CPI_SET_7, PD_CPI_SET_8, PD_CPI_SET_9,
};

/* Process key event
 */
bool process_pointing_device_cpi_roller(uint16_t keycode, keyrecord_t *record);
