/*
 * Minimal YDX-C02 trackpoint driver (HID over I2C)
 */

#include "pointing_device_internal.h"
#include "ydx_c02.h"
#include "timer.h"
#include "wait.h"

static uint16_t cpi_value = 50; /* default CPI in 1..100 */


const pointing_device_driver_t ydx_c02_pointing_device_driver = {
    .init       = ydx_c02_device_init,
    .get_report = ydx_c02_get_report,
    .set_cpi    = ydx_c02_set_cpi,
    .get_cpi    = ydx_c02_get_cpi,
};

uint16_t ydx_c02_get_cpi(void) {
    return cpi_value;
}

void ydx_c02_set_cpi(uint16_t cpi) {
    /* Clamp CPI to 1..100 as requested */
    if (cpi < 1) {
        cpi = 1;
    } else if (cpi > 100) {
        cpi = 100;
    }
    cpi_value = cpi;

    /* Device accepts CPI directly (1..100) — use as speed byte */
    uint8_t speed = (uint8_t)cpi;

    /* Build HID-over-I2C moving speed commands.
     * Up/Down:    25 00 06 00 29 42 xx yy  (xx = up, yy = down)
     * Left/Right: 25 00 06 00 29 43 xx yy  (xx = left, yy = right)
     */
    uint8_t updown[9]   = {0x25, 0x00, 0x06, 0x00, 0x29, 0x42, speed, speed};
    uint8_t leftright[9] = {0x25, 0x00, 0x06, 0x00, 0x29, 0x43, speed, speed};

    i2c_status_t s1 = i2c_write_register(YDX_C02_ADDRESS << 1, 0x00, updown, sizeof(updown), YDX_C02_TIMEOUT);
    i2c_status_t s2 = i2c_write_register(YDX_C02_ADDRESS << 1, 0x00, leftright, sizeof(leftright), YDX_C02_TIMEOUT);

    #ifdef POINTING_DEVICE_DEBUG
        printf("ydx_c02 set speed: %d %d\n", s1, s2);
    #endif
}

void ydx_c02_reset(void) {
    /* Send reset command: 25 00 06 00 29 77 77 77 */
    uint8_t reset_cmd[8] = {0x25, 0x00, 0x06, 0x00, 0x29, 0x77, 0x77, 0x77};
    i2c_status_t s = i2c_write_register(YDX_C02_ADDRESS << 1, 0x00, reset_cmd, sizeof(reset_cmd), YDX_C02_TIMEOUT);
#ifdef POINTING_DEVICE_DEBUG
    printf("ydx_c02 reset status: %d\n", s);
#endif
}


__attribute__((weak)) void ydx_c02_device_init(void) {
    i2c_init();
    wait_ms(100);
    ydx_c02_reset();
    wait_ms(100);
}

i2c_status_t read_ydx_c02(ydx_c02_data_t *data) {
    // Read HID report starting at register 0x00
    i2c_status_t status = i2c_receive(YDX_C02_ADDRESS << 1, (uint8_t*)data, sizeof(*data), YDX_C02_TIMEOUT);;

#ifdef POINTING_DEVICE_DEBUG
    static uint16_t d_timer;
    if (timer_elapsed(d_timer) > 250) {
        printf("ydx_c02 READ i2c_status_t: %d B: %02x X: %d Y: %d \n", status, data->buttons, data->x, data->y);
        d_timer = timer_read();
    }
#endif

    return status;
}

report_mouse_t ydx_c02_get_report(report_mouse_t mouse_report) {
    static uint8_t       error_count = 0;
    ydx_c02_data_t       d = {0};

    if (error_count < 10) {
        i2c_status_t status = read_ydx_c02(&d);

        if (status == I2C_STATUS_SUCCESS) {
            error_count = 0;

            // X/Y are signed 8-bit HID deltas
            mouse_report.x = (int8_t)d.x;
            mouse_report.y = (int8_t)d.y;

        } else {
            error_count++;
        }
    }

    return mouse_report;
}
