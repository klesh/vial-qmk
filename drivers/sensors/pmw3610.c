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

#include "pmw3610.h"
#include "wait.h"
#include "debug.h"
#include "gpio.h"
#include "pointing_device_internal.h"

#define REG_PID 0x00
#define REG_VID 0x01
#define REG_MOT 0x02
#define REG_X_L 0x03
#define REG_Y_L 0x04
#define REG_XY_H 0x05

#define REG_RES_STEP 0x85

#define REG_SPI_CLK_ON_REQ 0x41
#define SPI_CLK_CMD_ENABLE  0xBA
#define SPI_CLK_CMD_DISABLE 0xB5

#define REG_POWER_UP_RESET 0x3A
#define POWER_UP_CMD_RESET 0x5A

#define REG_OB1 0x2D

#define REG_PERFORMANCE 0x11
#define REG_RUN_DOWNSHIFT 0x1B
#define REG_REST1_RATE 0x1C
#define REG_REST1_DOWNSHIFT 0x1D
#define REG_REST2_RATE 0x1E
#define REG_REST2_DOWNSHIFT 0x1F

#define REG_MOTION_BURST 0x12

#define CPI_STEP 200
#define CPI_MIN 200
#define CPI_MAX 3200

#define REG_SMART 0x32
#define SMART_CMD_ENABLE 0x00
#define SMART_CMD_DISABLE 0x80

#define SHUTTER_HI_INDEX 5
#define SHUTTER_LO_INDEX 6


#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))


uint8_t pmw3610_serial_read(void);
void    pmw3610_serial_write(uint8_t reg_addr);
uint8_t pmw3610_read_reg(uint8_t reg_addr);
void    pmw3610_write(uint8_t reg_addr, uint8_t data);
void    pmw3610_write_reg(uint8_t reg_addr, uint8_t data);
bool    pmw3610_check_pid(void);
void    pmw3610_cs_select(void);
void    pmw3610_cs_deselect(void);
void    pmw3610_enable_spi_clk(void);
void    pmw3610_disable_spi_clk(void);

enum pmw3610_status {
    PMW3610_STATUS_NONE,
    PMW3610_STATUS_OB1_CHECKED,
    PMW3610_STATUS_READY,
};
static uint8_t status = PMW3610_STATUS_NONE;

const pointing_device_driver_t pmw3610_pointing_device_driver = {
    .init       = pmw3610_init,
    .get_report = pmw3610_get_report,
    .set_cpi    = pmw3610_set_cpi,
    .get_cpi    = pmw3610_get_cpi,
};

void pmw3610_init(void) {
    gpio_set_pin_output(PMW3610_SCLK_PIN);      // setclockpin to output
    gpio_set_pin_output(PMW3610_SDIO_PIN);      // set datapin input high
    gpio_set_pin_output(PMW3610_CS_PIN);        // set cs pin to output

    // reset spi port
    pmw3610_cs_deselect();
    pmw3610_cs_select();
    // wait_ms(1);
    wait_ms(5);

    // clean up obsrvation register
    pmw3610_write_reg(REG_OB1, 0x00);
    // wait_ms(10);
    wait_ms(50);
    uint8_t ob1 = pmw3610_read_reg(REG_OB1);
    if ((ob1 & 0b1111) != 0b1111) {
        return;
    }
    status = PMW3610_STATUS_OB1_CHECKED;

    // read and discard x, y, xy_high values
    pmw3610_read_reg(REG_X_L);
    pmw3610_read_reg(REG_Y_L);
    pmw3610_read_reg(REG_XY_H);

    // configuration
    pmw3610_write_reg(REG_PERFORMANCE, 0x0d);
    pmw3610_write_reg(REG_RUN_DOWNSHIFT, 0x04);
    pmw3610_write_reg(REG_REST1_RATE, 0x04);
    pmw3610_write_reg(REG_REST1_DOWNSHIFT, 0x0f);

    status = PMW3610_STATUS_READY;
}

bool pmw3610_check_pid(void) {
    uint8_t pid = pmw3610_read_reg(REG_PID); // read product id
    uint8_t vid = pmw3610_read_reg(REG_VID); // read revision id
    // dprintf("pmw3610 pid: %d vid: %d ob1: %d status: %d\n", pid, vid, status);
    return pid == 0x3e && vid == 0x01;
}

void pmw3610_cs_select(void) {
    gpio_write_pin_low(PMW3610_CS_PIN);
}

void pmw3610_cs_deselect(void) {
    gpio_write_pin_high(PMW3610_CS_PIN);
}

uint8_t pmw3610_serial_read(void) {
    gpio_set_pin_input(PMW3610_SDIO_PIN);
    uint8_t byte = 0;

    for (uint8_t i = 0; i < 8; ++i) {
        gpio_write_pin_low(PMW3610_SCLK_PIN);
        wait_us(1);

        byte = (byte << 1) | gpio_read_pin(PMW3610_SDIO_PIN);

        gpio_write_pin_high(PMW3610_SCLK_PIN);
        wait_us(1);
    }

    return byte;
}

void pmw3610_serial_write(uint8_t data) {
    gpio_write_pin_low(PMW3610_SDIO_PIN);
    gpio_set_pin_output(PMW3610_SDIO_PIN);

    for (int8_t b = 7; b >= 0; b--) {
        gpio_write_pin_low(PMW3610_SCLK_PIN);
        if (data & (1 << b)) {
            gpio_write_pin_high(PMW3610_SDIO_PIN);
        } else {
            gpio_write_pin_low(PMW3610_SDIO_PIN);
        }
        gpio_write_pin_high(PMW3610_SCLK_PIN);
    }

    wait_us(4);
}

// Convert a two's complement byte from an unsigned data type into a signed
// data type.
int8_t convert_twoscomp(uint8_t data) {
    if ((data & 0x80) == 0x80)
        return -128 + (data & 0x7F);
    else
        return data;
}

report_pmw3610_t pmw3610_read(void) {
    // read data from sensor
    pmw3610_cs_select();
    pmw3610_serial_write(REG_MOTION_BURST);
    wait_us(4);
    uint8_t buf[3] = {0};
    for (uint8_t i = 0; i<3; i++) {
        buf[i] = pmw3610_serial_read();
    }
    pmw3610_cs_deselect();
    wait_us(1);

    // enhance surface coverage
    // static bool smart_flag = false;
    // uint8_t shutter_hi = buf[SHUTTER_HI_INDEX];
    // uint8_t shutter_lo = buf[SHUTTER_LO_INDEX];
    // if (smart_flag && shutter_hi == 0 && shutter_lo < 45) {
    //     pmw3610_enable_spi_clk();
    //     pmw3610_write_reg(REG_SMART, SMART_CMD_ENABLE);
    //     pmw3610_disable_spi_clk();
    //     smart_flag = false;
    //     dprintf("pmw3610: smart mode enabled\n");
    // } else if (!smart_flag && shutter_hi == 0 && shutter_lo > 45) {
    //     pmw3610_enable_spi_clk();
    //     pmw3610_write_reg(REG_SMART, SMART_CMD_DISABLE);
    //     pmw3610_disable_spi_clk();
    //     smart_flag = true;
    //     dprintf("pmw3610: smart mode disabled\n");
    // }

    // convert to report
    report_pmw3610_t data = {0};
    data.isMotion = pmw3610_read_reg(buf[0]) & (1 << 7); // check for motion only (bit 7 in field)
    data.x        = convert_twoscomp(buf[1]);
    data.y        = convert_twoscomp(buf[2]);
    return data;
}

void pmw3610_write(uint8_t reg_addr, uint8_t data) {
    pmw3610_serial_write(0b10000000 | reg_addr);
    pmw3610_serial_write(data);
}

void pmw3610_write_reg(uint8_t reg_addr, uint8_t data) {
    pmw3610_cs_select();
    pmw3610_write(reg_addr, data);
    wait_us(10);
    pmw3610_cs_deselect();
    wait_us(30);
}

uint8_t pmw3610_read_reg(uint8_t reg_addr) {
    pmw3610_cs_select();
    pmw3610_serial_write(reg_addr);
    wait_us(5);
    uint8_t byte = pmw3610_serial_read();
    pmw3610_cs_deselect();
    return byte;
}

void pmw3610_enable_spi_clk(void) {
    pmw3610_write_reg(REG_SPI_CLK_ON_REQ, SPI_CLK_CMD_ENABLE);
    wait_us(300);
}

void pmw3610_disable_spi_clk(void) {
    pmw3610_write_reg(REG_SPI_CLK_ON_REQ, SPI_CLK_CMD_DISABLE);
}

void pmw3610_burst_write(uint8_t *addr, uint8_t *data, uint8_t len) {
    pmw3610_enable_spi_clk();
    for (uint8_t i = 0; i<len; i++) {
        pmw3610_write_reg(addr[i], data[i]);
    }
    pmw3610_disable_spi_clk();
}

void pmw3610_set_cpi(uint16_t cpi) {
    if (cpi > CPI_MAX) {
        cpi = CPI_MAX;
    } else if (cpi < CPI_MIN) {
        cpi = CPI_MIN;
    }
    uint8_t cpival = cpi / CPI_STEP;

    /* set the cpi */
    dprintf("pmw3610: setting cpi to %d\n", cpi);
    uint8_t addr[] = {0x7F, REG_RES_STEP, 0x7F};
    uint8_t data[] = {0xFF, cpival, 0x00};
    pmw3610_burst_write(addr, data, 3);
}

uint16_t pmw3610_get_cpi(void) {
    return 0;
}

report_mouse_t pmw3610_get_report(report_mouse_t mouse_report) {
    report_pmw3610_t data = pmw3610_read();
    if (data.isMotion) {
        mouse_report.x = data.x;
        mouse_report.y = data.y;
    }

    return mouse_report;
}
