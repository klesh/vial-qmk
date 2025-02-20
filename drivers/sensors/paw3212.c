/* Copyright 2021 Gompa (@Gompa)
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

// https://github.com/shinoaliceKabocha/choco60_track/tree/master/keymaps/default

#include "paw3212.h"
#include "wait.h"
#include "debug.h"
#include "gpio.h"
#include "pointing_device_internal.h"

#define REG_PID1 0x00
#define REG_PID2 0x01
#define REG_STAT 0x02
#define REG_X 0x03
#define REG_Y 0x04

#define REG_SETUP 0x06
#define REG_CPI_X 0x0D
#define REG_CPI_Y 0x0E

#define CPI_STEP 38
#define CPI_MULTIPLIER 4
#define CPI_MAX 2400

#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))


uint8_t paw3212_serial_read(void);
void    paw3212_serial_write(uint8_t reg_addr);
uint8_t paw3212_read_reg(uint8_t reg_addr);
void    paw3212_write_reg(uint8_t reg_addr, uint8_t data);

const pointing_device_driver_t paw3212_pointing_device_driver = {
    .init       = paw3212_init,
    .get_report = paw3212_get_report,
    .set_cpi    = paw3212_set_cpi,
    .get_cpi    = paw3212_get_cpi,
};

void paw3212_init(void) {
    gpio_set_pin_output(PAW3212_SCLK_PIN);     // setclockpin to output
    gpio_set_pin_input_high(PAW3212_SDIO_PIN); // set datapin input high

    // paw3212_read_reg(0x00); // read id
    // paw3212_read_reg(0x01); // read id2
    // PAW3212_write_reg(REG_SETUP,0x06);  // dont reset sensor and set cpi 1600
}

void pmw3212_cs_select(void) {
    gpio_write_pin_low(PMW3212_CS_PIN);
}

void pmw3212_cs_deselect(void) {
    gpio_write_pin_high(PMW3212_CS_PIN);
}

uint8_t paw3212_serial_read(void) {
    gpio_set_pin_input(PAW3212_SDIO_PIN);
    uint8_t byte = 0;

    for (uint8_t i = 0; i < 8; ++i) {
        gpio_write_pin_low(PAW3212_SCLK_PIN);
        wait_us(1);

        byte = (byte << 1) | gpio_read_pin(PAW3212_SDIO_PIN);

        gpio_write_pin_high(PAW3212_SCLK_PIN);
        wait_us(1);
    }

    return byte;
}

void paw3212_serial_write(uint8_t data) {
    gpio_write_pin_low(PAW3212_SDIO_PIN);
    gpio_set_pin_output(PAW3212_SDIO_PIN);

    for (int8_t b = 7; b >= 0; b--) {
        gpio_write_pin_low(PAW3212_SCLK_PIN);
        if (data & (1 << b)) {
            gpio_write_pin_high(PAW3212_SDIO_PIN);
        } else {
            gpio_write_pin_low(PAW3212_SDIO_PIN);
        }
        gpio_write_pin_high(PAW3212_SCLK_PIN);
    }

    wait_us(4);
}

report_paw3212_t paw3212_read(void) {
    report_paw3212_t data = {0};

    data.isMotion = paw3212_read_reg(REG_STAT) & (1 << 7); // check for motion only (bit 7 in field)
    data.x        = (int8_t)paw3212_read_reg(REG_X);
    data.y        = (int8_t)paw3212_read_reg(REG_Y);

    return data;
}

void paw3212_write_reg(uint8_t reg_addr, uint8_t data) {
    pmw3212_cs_select();
    paw3212_serial_write(0b10000000 | reg_addr);
    paw3212_serial_write(data);
    pmw3212_cs_deselect();
}

uint8_t paw3212_read_reg(uint8_t reg_addr) {
    pmw3212_cs_select();
    paw3212_serial_write(reg_addr);
    wait_us(5);
    uint8_t byte = paw3212_serial_read();
    pmw3212_cs_deselect();
    return byte;
}

void paw3212_set_cpi(uint16_t cpi) {
    uint8_t cpival = cpi / CPI_STEP;
    paw3212_write_reg(REG_CPI_X, cpival);
    paw3212_write_reg(REG_CPI_Y, cpival);
}

uint16_t paw3212_get_cpi(void) {
    uint8_t cpi = paw3212_read_reg(REG_CPI_X);
    uint16_t cpival = (uint16_t)cpi * (uint16_t)CPI_STEP;
    return cpival;
}

uint8_t read_pid_paw3212(void) {
    return paw3212_read_reg(REG_PID1);
}

report_mouse_t paw3212_get_report(report_mouse_t mouse_report) {
    report_paw3212_t data = paw3212_read();
    if (data.isMotion) {
        pd_dprintf("Raw ] X: %d, Y: %d\n", data.x, data.y);

        mouse_report.x = data.x;
        mouse_report.y = data.y;
    }

    return mouse_report;
}
