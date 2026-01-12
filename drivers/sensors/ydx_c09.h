/*
 * Minimal driver header for YDX-C09 trackpoint (HID-over-I2C)
 */
#pragma once

#include <stdint.h>
#include "report.h"
#include "i2c_master.h"
#include "pointing_device.h"

#ifndef YDX_C09_ADDRESS
#    define YDX_C09_ADDRESS 0x15
#endif

#ifndef YDX_C09_TIMEOUT
#    define YDX_C09_TIMEOUT 1000
#endif

typedef struct {
    uint8_t chk1;
    uint8_t chk2;
    uint8_t chk3;
    uint8_t buttons;
    int8_t  x;
    int8_t  y;
    int8_t  reserve;
} ydx_c09_data_t;

const pointing_device_driver_t ydx_c09_pointing_device_driver;

void           ydx_c09_device_init(void);
uint16_t       ydx_c09_get_cpi(void);
void           ydx_c09_set_cpi(uint16_t cpi);
void           ydx_c09_reset(void);
i2c_status_t   read_ydx_c09(ydx_c09_data_t* data);
report_mouse_t ydx_c09_get_report(report_mouse_t mouse_report);
