/* Helper module to manage a selectable CPI list for pointing devices
 * Keymaps can include this header and call the provided functions from
 * `process_record_user` to change CPI via keys.
 */
#pragma once

#include <stdint.h>
#include "pointing_device.h"


/*
# If `POINTING_DEVICE_CPI_SELECTED_IDX` is defined by the keymap/config,
# it will be used as the initial selected index. If it is not defined,
# the implementation will default to the middle index of
# `POINTING_DEVICE_CPI_LIST` at runtime.
#*/

void pointing_device_cpi_init(void);
uint16_t pointing_device_cpi_get_selected(void);
uint8_t pointing_device_cpi_get_selected_index(void);
void pointing_device_cpi_set_index(uint8_t idx);
void pointing_device_cpi_next(void);
void pointing_device_cpi_prev(void);

/* for state sync */
uint16_t pointing_device_cpi_get_value(void);
void pointing_device_cpi_set_value(uint16_t);
