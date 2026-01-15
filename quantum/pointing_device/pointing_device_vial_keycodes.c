#include "pointing_device_vial_keycodes.h"

bool process_pointing_device_cpi(uint16_t keycode, keyrecord_t *record) {
    if (!record) return true;
    if (!record->event.pressed) return true;

    switch (keycode) {
        case PD_CPI_NEXT:
            pointing_device_cpi_next();
            return false;

        case PD_CPI_PREV:
            pointing_device_cpi_prev();
            return false;

        default:
            break;
    }

    if (keycode >= PD_CPI_SET_0 && keycode <= PD_CPI_SET_9) {
        uint8_t idx = (uint8_t)(keycode - PD_CPI_SET_0);
        pointing_device_cpi_set_index(idx);
        return false;
    }

    return true;
}
