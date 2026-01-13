/*
 * Minimal driver header for YDX-C02 trackpoint (HID-over-I2C)
 */
#pragma once

#include <stdint.h>
#include "report.h"
#include "i2c_master.h"
#include "pointing_device.h"

#ifndef YDX_C02_ADDRESS
#    define YDX_C02_ADDRESS 0x15
#endif

#ifndef YDX_C02_TIMEOUT
#    define YDX_C02_TIMEOUT 1000
#endif

typedef struct {
    uint8_t chk1;
    uint8_t buttons;
    int8_t  x;
    int8_t  y;
    int8_t  reserve;
} ydx_c02_data_t;

const pointing_device_driver_t ydx_c02_pointing_device_driver;

void           ydx_c02_device_init(void);
uint16_t       ydx_c02_get_cpi(void);
void           ydx_c02_set_cpi(uint16_t cpi);
void           ydx_c02_reset(void);
i2c_status_t   read_ydx_c02(ydx_c02_data_t* data);
report_mouse_t ydx_c02_get_report(report_mouse_t mouse_report);
