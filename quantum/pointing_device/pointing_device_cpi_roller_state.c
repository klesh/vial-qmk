#include <stdint.h>

static uint16_t cpi_roller_value = 0;

uint16_t pointing_device_cpi_roller_current_value(void) {
    return cpi_roller_value;
}
void pointing_device_cpi_roller_set_value(uint16_t cpi_value) {
    cpi_roller_value = cpi_value;
}
