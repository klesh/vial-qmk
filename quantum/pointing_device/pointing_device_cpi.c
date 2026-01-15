/* Implementation for selectable CPI presets for pointing devices.
 * Calls `pointing_device_set_cpi()` on init so sensor and firmware stay synced.
 */

#include "pointing_device_cpi.h"
#include <stddef.h>
#include "eeconfig.h"

/* Build the static list from the macro. */
static const uint16_t pointing_device_cpi_list[] = POINTING_DEVICE_CPI_LIST;
static const size_t pointing_device_cpi_list_len = sizeof(pointing_device_cpi_list) / sizeof(pointing_device_cpi_list[0]);

/* Stored selected index (0..len-1). Initialize from macro; clipped on init. */
static uint8_t selected_cpi_index = 0;
static uint16_t selected_cpi_value = 0;

void pointing_device_cpi_init(void) {
    if (pointing_device_cpi_list_len == 0) {
        return;
    }
    /* If EEPROM is available, load stored selected index. */
    if (eeconfig_is_enabled()) {
        uint32_t val = eeconfig_read_user();
        selected_cpi_index = (uint8_t)(val & 0xFF);
    }
    /* Apply to device so it's synced at boot */
    pointing_device_cpi_set_index(selected_cpi_index);
}

uint16_t pointing_device_cpi_get_selected(void) {
    return pointing_device_cpi_list[selected_cpi_index];
}

uint8_t pointing_device_cpi_get_selected_index(void) {
    return selected_cpi_index;
}

void pointing_device_cpi_set_index(uint8_t idx) {
    selected_cpi_index = idx % (uint8_t)pointing_device_cpi_list_len;
    selected_cpi_value = pointing_device_cpi_list[selected_cpi_index];
    pointing_device_set_cpi(selected_cpi_value);
    /* Persist selection to EEPROM (lower 8 bits of user data). */
    uint32_t cur = eeconfig_read_user();
    cur = (cur & ~0xFFu) | (uint32_t)selected_cpi_index;
    eeconfig_update_user(cur);
}

void pointing_device_cpi_next(void) {
    pointing_device_cpi_set_index((selected_cpi_index + 1) % (uint8_t)pointing_device_cpi_list_len);
}

void pointing_device_cpi_prev(void) {
    pointing_device_cpi_set_index((selected_cpi_index - 1) % (uint8_t)pointing_device_cpi_list_len);
}

/* for state sync */
uint16_t pointing_device_cpi_get_value(void) {
    return selected_cpi_value;
}
void pointing_device_cpi_set_value(uint16_t cpi_value) {
    selected_cpi_value = cpi_value;
}

/* Provide a module init hook called by the core pointing device init path.
 * The core defines a weak `pointing_device_init_modules()` which we override
 * here to ensure CPI sync is run on boot. If other modules also need that
 * hook they'll need to call this helper from their own init implementation.
 */
void pointing_device_init_modules(void) {
    pointing_device_cpi_init();
}
