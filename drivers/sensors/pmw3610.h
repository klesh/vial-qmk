/* Copyright 2021 Colin Lam (Ploopy Corporation)
 * Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
 * Copyright 2019 Sunjun Kim
 * Copyright 2019 Hiroyuki Okada
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

// Definitions for the PMW3610 serial line.
#ifndef PMW3610_SCLK_PIN
#    ifdef POINTING_DEVICE_SCLK_PIN
#        define PMW3610_SCLK_PIN POINTING_DEVICE_SCLK_PIN
#    else
#        error "No clock pin defined -- missing POINTING_DEVICE_SCLK_PIN or PMW3610_SCLK_PIN"
#    endif
#endif

#ifndef PMW3610_SDIO_PIN
#    ifdef POINTING_DEVICE_SDIO_PIN
#        define PMW3610_SDIO_PIN POINTING_DEVICE_SDIO_PIN
#    else
#        error "No data pin defined -- missing POINTING_DEVICE_SDIO_PIN or PMW3610_SDIO_PIN"
#    endif
#endif

#ifndef PMW3610_CS_PIN
#    ifdef POINTING_DEVICE_CS_PIN
#        define PMW3610_CS_PIN POINTING_DEVICE_CS_PIN
#    else
#        error "No chip select pin defined -- missing POINTING_DEVICE_CS_PIN or PMW3610_CS_PIN define"
#    endif
#endif

typedef struct {
    int8_t dx;
    int8_t dy;
} report_pmw3610_t;

// A bunch of functions to implement the PMW3610-specific serial protocol.
// Mostly taken from ADNS5050 driver.
// Note that the "serial.h" driver is insufficient, because it does not
// manually manipulate a serial clock signal.
void             pmw3610_init(void);
void             pmw3610_sync(void);
uint8_t          pmw3610_serial_read(void);
void             pmw3610_serial_write(uint8_t data);
uint8_t          pmw3610_read_reg(uint8_t reg_addr);
void             pmw3610_write_reg(uint8_t reg_addr, uint8_t data);
report_pmw3610_t pmw3610_read_burst(void);
void             pmw3610_set_cpi(uint16_t cpi);
uint16_t         pmw3610_get_cpi(void);
int8_t           convert_twoscomp(uint8_t data);
bool             pmw3610_check_signature(void);

#if !defined(PMW3610_CPI)
#    define PMW3610_CPI 1200
#endif

#define PMW3610_CPI_STEP 200
#define PMW3610_CPI_MIN 200
#define PMW3610_CPI_MAX 3200

// PMW3610 register addresses
// clang-format off
#define REG_PRODUCT_ID                  0x00
#define REG_REVISION_ID                 0x01
#define REG_MOTION                      0x02
#define REG_DELTA_X_L                   0x03
#define REG_DELTA_Y_L                   0x04
#define REG_DELTA_XY_H                  0x05
#define REG_SQUAL                       0x06
#define REG_SHUTTER_HIGHER              0x07
#define REG_SHUTTER_LOWER               0x08
#define REG_PIX_MAX                     0x09
#define REG_PIX_AVG                     0x0a
#define REG_PIX_MIN                     0x0b
#define REG_CRC0                        0x0c
#define REG_CRC1                        0x0d
#define REG_CRC2                        0x0e
#define REG_CRC3                        0x0f
#define REG_SELF_TEST                   0x10
#define REG_PERFORMANCE                 0x11
#define REG_BURST_READ                  0x12
#define REG_RUN_DOWNSHIFT               0x1b
#define REG_REST1_RATE                  0x1c
#define REG_REST1_DOWNSHIFT             0x1d
#define REG_REST2_RATE                  0x1e
#define REG_REST2_DOWNSHIFT             0x1f
#define REG_REST3_RATE                  0x20
#define REG_OBSERVATION1                0x2d
#define REG_PIXEL_GRAB                  0x35
#define REG_FRAME_GRAB                  0x36
#define REG_POWER_UP_RESET              0x3a
#define REG_SHUTDOWN                    0x3b
#define REG_NOT_REV_ID                  0x3e
#define REG_NOT_PROD_ID                 0x3f
#define REG_SPI_CLK_ON_REQ              0x41
#define REG_PRBS_TEST_CTL               0x47
#define REG_SPI_PAGE0                   0x7f
#define REG_RES_STEP                    0x85
#define REG_VCSEL_CTL                   0x9e
#define REG_LSR_CONTROL                 0x9f
#define REG_SPI_PAGE1                   0xff

/* Power-up register commands */
#define PMW3610_POWERUP_CMD_RESET       0x5A
#define PMW3610_POWERUP_CMD_WAKEUP      0x96

/* spi clock enable/disable commands */
#define PMW3610_SPI_CLOCK_CMD_ENABLE   0xBA
#define PMW3610_SPI_CLOCK_CMD_DISABLE  0xB5


#define TSWW_US                        30
#define TSWR_US                        20
#define T_NCS_SCLK_US                   1

#define PMW3610_SHUTTER_H_POS           5
#define PMW3610_SHUTTER_L_POS           5

#define PMW3610_X_L_POS        1
#define PMW3610_Y_L_POS        2
#define PMW3610_XY_H_POS       3
// clang-format on

#define TOINT16(val, bits) (((struct {int16_t value: bits;}){val}).value)
