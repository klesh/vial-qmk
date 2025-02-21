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

#include "paw3220.h"
#include "wait.h"
#include "debug.h"
#include "gpio.h"
#include "pointing_device_internal.h"

#define REG_PID1 0x00
#define REG_PID2 0x01
#define REG_STAT 0x02
#define REG_X 0x03
#define REG_Y 0x04

#define REG_CPI_X 0x0D
#define REG_CPI_Y 0x0E

#define CPI_STEP_X 39
#define CPI_STEP_Y 38
#define CPI_MULTIPLIER 4
#define CPI_MAX 4000

#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))


uint8_t paw3220_serial_read(void);
void    paw3220_serial_write(uint8_t reg_addr);
uint8_t paw3220_read_reg(uint8_t reg_addr);
void    paw3220_write(uint8_t reg_addr, uint8_t data);
void    paw3220_write_reg(uint8_t reg_addr, uint8_t data);
bool    paw3220_check_pid(void);
void    paw3220_cs_select(void);
void    paw3220_cs_deselect(void);

const pointing_device_driver_t paw3220_pointing_device_driver = {
    .init       = paw3220_init,
    .get_report = paw3220_get_report,
    .set_cpi    = paw3220_set_cpi,
    .get_cpi    = paw3220_get_cpi,
};

bool paw3220_check_pid(void) {
    uint8_t pid1 = paw3220_read_reg(REG_PID1); // read pid1
    uint8_t pid2 = paw3220_read_reg(REG_PID2); // read pid2
    return pid1 == 0x30 && pid2 == 0x02;
}

void paw3220_init(void) {
    gpio_set_pin_output(PAW3220_SCLK_PIN);     // setclockpin to output
    gpio_set_pin_input_high(PAW3220_SDIO_PIN); // set datapin input high
    gpio_set_pin_output(PAW3220_CS_PIN);       // set cs pin to output

    paw3220_cs_select();
    wait_ms(1);                // NCS Low at Power-up Stage for at least 1 ms

    // wait for the sensor
    for (uint8_t counter = 0; counter < 100; counter++) {
        if (paw3220_check_pid()) break;
        wait_ms(1);
    }

    // initialization: High voltage, 3-wires SPI
    paw3220_write(0x09, 0x5A); // to disable Write Protect
    paw3220_write(0x0A, 0x87);
    paw3220_write(0x0D, 0x1A); // to set X-axis resolution to 1000 cpi
    paw3220_write(0x0E, 0x1B); // to set Y-axis resolution to 1000 cpi
    paw3220_write(0x2B, 0x6C);
    paw3220_write(0x30, 0x2E);
    paw3220_write(0x32, 0x0F);
    paw3220_write(0x35, 0x8C);
    paw3220_write(0x36, 0x32);
    paw3220_write(0x3E, 0x26);
    paw3220_write(0x5C, 0xD6); // to set current source mode, 6mA
    paw3220_write(0x7F, 0x01); // to switch to Register Bank1
    paw3220_write(0x00, 0x40);
    paw3220_write(0x06, 0x28);
    paw3220_write(0x07, 0x08);
    paw3220_write(0x16, 0x18);
    paw3220_write(0x1B, 0x4C);
    paw3220_write(0x36, 0x88);
    paw3220_write(0x3A, 0x57);
    paw3220_write(0x3B, 0x34);
    paw3220_write(0x41, 0x22);
    paw3220_write(0x42, 0x6D);
    paw3220_write(0x43, 0x82);
    paw3220_write(0x44, 0x6D);
    paw3220_write(0x45, 0x82);
    paw3220_write(0x46, 0x22);
    paw3220_write(0x47, 0x7C);
    paw3220_write(0x48, 0x82);
    paw3220_write(0x49, 0x7C);
    paw3220_write(0x4A, 0x84);
    paw3220_write(0x55, 0x10);
    paw3220_write(0x5F, 0x04);
    paw3220_write(0x60, 0x02);
    paw3220_write(0x62, 0x5C);
    paw3220_write(0x64, 0x43);
    paw3220_write(0x65, 0x54);
    paw3220_write(0x67, 0x93);
    paw3220_write(0x68, 0x15);
    paw3220_write(0x71, 0x2C);
    paw3220_write(0x72, 0x40);
    paw3220_write(0x79, 0x08);
    paw3220_write(0x7F, 0x00); // to switch to Register Bank0
    paw3220_write(0x2B, 0x6D);
    paw3220_write(0x09, 0x00); // to enable Write Protect
    paw3220_cs_deselect();
}

void paw3220_cs_select(void) {
    gpio_write_pin_low(PAW3220_CS_PIN);
}

void paw3220_cs_deselect(void) {
    gpio_write_pin_high(PAW3220_CS_PIN);
}

uint8_t paw3220_serial_read(void) {
    gpio_set_pin_input(PAW3220_SDIO_PIN);
    uint8_t byte = 0;

    for (uint8_t i = 0; i < 8; ++i) {
        gpio_write_pin_low(PAW3220_SCLK_PIN);
        wait_us(1);

        byte = (byte << 1) | gpio_read_pin(PAW3220_SDIO_PIN);

        gpio_write_pin_high(PAW3220_SCLK_PIN);
        wait_us(1);
    }

    return byte;
}

void paw3220_serial_write(uint8_t data) {
    gpio_write_pin_low(PAW3220_SDIO_PIN);
    gpio_set_pin_output(PAW3220_SDIO_PIN);

    for (int8_t b = 7; b >= 0; b--) {
        gpio_write_pin_low(PAW3220_SCLK_PIN);
        if (data & (1 << b)) {
            gpio_write_pin_high(PAW3220_SDIO_PIN);
        } else {
            gpio_write_pin_low(PAW3220_SDIO_PIN);
        }
        gpio_write_pin_high(PAW3220_SCLK_PIN);
    }

    wait_us(4);
}

report_paw3220_t paw3220_read(void) {
    report_paw3220_t data = {0};

    data.isMotion = paw3220_read_reg(REG_STAT) & (1 << 7); // check for motion only (bit 7 in field)
    data.x        = (int8_t)paw3220_read_reg(REG_X);
    data.y        = (int8_t)paw3220_read_reg(REG_Y);

    return data;
}

void paw3220_write(uint8_t reg_addr, uint8_t data) {
    paw3220_serial_write(0b10000000 | reg_addr);
    paw3220_serial_write(data);
}

void paw3220_write_reg(uint8_t reg_addr, uint8_t data) {
    paw3220_cs_select();
    paw3220_write(reg_addr, data);
    paw3220_cs_deselect();
}

uint8_t paw3220_read_reg(uint8_t reg_addr) {
    paw3220_cs_select();
    paw3220_serial_write(reg_addr);
    wait_us(5);
    uint8_t byte = paw3220_serial_read();
    paw3220_cs_deselect();
    return byte;
}

void paw3220_set_cpi(uint16_t cpi) {
    if (cpi > CPI_MAX) {
        cpi = CPI_MAX;
    }
    uint8_t cpival_x = cpi / CPI_STEP_X;
    uint8_t cpival_y = cpi / CPI_STEP_Y;
    paw3220_write_reg(REG_CPI_X, cpival_x);
    paw3220_write_reg(REG_CPI_Y, cpival_y);
}

uint16_t paw3220_get_cpi(void) {
    uint8_t cpi = paw3220_read_reg(REG_CPI_X);
    uint16_t cpival = (uint16_t)cpi * (uint16_t)CPI_STEP_X;
    return cpival;
}

uint8_t read_pid_paw3220(void) {
    return paw3220_read_reg(REG_PID1);
}

report_mouse_t paw3220_get_report(report_mouse_t mouse_report) {
    report_paw3220_t data = paw3220_read();
    if (data.isMotion) {
        pd_dprintf("Raw ] X: %d, Y: %d\n", data.x, data.y);

        mouse_report.x = data.x;
        mouse_report.y = data.y;
    }

    return mouse_report;
}
