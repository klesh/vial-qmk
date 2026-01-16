/* Helper module to manage a selectable CPI list for pointing devices
 */
#pragma once

#include <stdint.h>
#include "pointing_device.h"
#include "pointing_device_cpi_roller_state.h"

void pointing_device_cpi_roller_init(void);
uint8_t pointing_device_cpi_roller_current_index(void);
void pointing_device_cpi_roller_set_index(uint8_t idx);
void pointing_device_cpi_roller_next(void);
void pointing_device_cpi_roller_prev(void);
void pointing_device_cpi_roller_set_list(uint16_t*, uint8_t);
