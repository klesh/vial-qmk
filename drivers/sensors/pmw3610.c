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

#include "pmw3610.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "_wait.h"
#include "config.h"
#include "print.h"
#include "gpio.h"


void pmw3610_cs_select(void) {
    writePinLow(PMW3610_CS_PIN);
    wait_us(T_NCS_SCLK_US);
}

void pmw3610_cs_deselect(void) {
    wait_us(T_NCS_SCLK_US);
    writePinHigh(PMW3610_CS_PIN);
}

static inline bool ensure_reg(uint8_t reg_addr, uint8_t data) {
    uint8_t counter = 0;
    while (counter < 10) {
        uint8_t read_data = pmw3610_read_reg(reg_addr);
        if (read_data == data) {
            return true;
        }
        counter++;
        pmw3610_write_reg(reg_addr, data);
        wait_ms(1);
    }
    return false;
}

static char* pmw3610_status;


void pmw3610_init(void) {
    // Initialize sensor serial pins.
    setPinOutput(PMW3610_SCLK_PIN);
    setPinOutput(PMW3610_SDIO_PIN);
    setPinOutput(PMW3610_CS_PIN);

    // reset the SPI port
    wait_ms(10);
    pmw3610_cs_deselect();
    pmw3610_cs_select();
    pmw3610_status = "spi reset";

    /* not required in datashet, but added any way to have a clear state */
    // pmw3610_write_reg(REG_POWER_UP_RESET, PMW3610_POWERUP_CMD_RESET);
    // wait_ms(200);
    // pmw3610_status = "power up reset";

    // ask pmw3610 to perform the self-test
    wait_ms(50);
    pmw3610_write_reg(REG_OBSERVATION1, 0x00);
    pmw3610_status = "initiate self test";

    // check if self-test passed
    uint8_t self_test_result = 0;
    uint8_t self_test_counter = 0;
    while ((self_test_result & 0x0F) != 0x0F && self_test_counter < 100) {
        wait_ms(10);
        self_test_result = pmw3610_read_reg(REG_OBSERVATION1);
        self_test_counter++;
    }
    pmw3610_status = "self test passed";

    // clear registers first (required in datasheet)
    for (uint8_t reg = REG_MOTION; (reg <= REG_DELTA_XY_H); reg++) {
        pmw3610_read_reg(reg);
    }
    pmw3610_status = "registers cleared";

    // read a burst from the sensor and then discard it.
    // gets the sensor ready for write commands
    // (for example, setting the dpi).
    pmw3610_read_burst();
    pmw3610_status = "burst read and discard";


    // set performance to force awake
    ensure_reg(REG_PERFORMANCE, 0xf1);
    pmw3610_status = "set performance";


    ensure_reg(REG_RUN_DOWNSHIFT, 0x04);
    pmw3610_status = "run_downshift set";

    ensure_reg(REG_REST1_RATE, 0x04);
    ensure_reg(REG_REST1_DOWNSHIFT, 0x0f);

    // looks like QMK would call the set_cpi right after init, which might cause
    // the sensor to act weirdly - movement stuttering, etc. delay 200ms to avoid that
    wait_ms(200);

    pmw3610_status = NULL;
}

uint8_t pmw3610_serial_read(void) {
    setPinInput(PMW3610_SDIO_PIN);
    uint8_t byte = 0;

    for (uint8_t i = 0; i < 8; ++i) {
        writePinLow(PMW3610_SCLK_PIN);
        wait_us(1);

        byte = (byte << 1) | readPin(PMW3610_SDIO_PIN);

        writePinHigh(PMW3610_SCLK_PIN);
        wait_us(1);
    }

    return byte;
}

void pmw3610_serial_write(uint8_t data) {
    setPinOutput(PMW3610_SDIO_PIN);

    for (int8_t b = 7; b >= 0; b--) {
        writePinLow(PMW3610_SCLK_PIN);

        if (data & (1 << b))
            writePinHigh(PMW3610_SDIO_PIN);
        else
            writePinLow(PMW3610_SDIO_PIN);

        wait_us(2);

        writePinHigh(PMW3610_SCLK_PIN);
    }

    // This was taken from ADNS5050 driver.
    // There's no any info in PMW3610 datasheet about this...
    // tSWR. See page 15 of the ADNS5050 spec sheet.
    // Technically, this is only necessary if the next operation is an SDIO
    // read. This is not guaranteed to be the case, but we're being lazy.
    wait_us(4);

    // Note that tSWW is never necessary. All write operations require at
    // least 32us, which exceeds tSWW, so there's never a need to wait for it.
}

// Read a byte of data from a register on the sensor.
uint8_t pmw3610_read_reg(uint8_t reg_addr) {
    pmw3610_cs_select();

    pmw3610_serial_write(reg_addr);

    uint8_t byte = pmw3610_serial_read();

    // This was taken directly from ADNS5050 driver...
    // tSRW & tSRR. See page 15 of the ADNS5050 spec sheet.
    // Technically, this is only necessary if the next operation is an SDIO
    // read or write. This is not guaranteed to be the case.
    // Honestly, this wait could probably be removed.
    wait_us(1);

    pmw3610_cs_deselect();

    return byte;
}

void pmw3610_write_reg(uint8_t reg_addr, uint8_t data) {
    pmw3610_cs_select();
    pmw3610_serial_write(0b10000000 | reg_addr);
    pmw3610_serial_write(data);
    pmw3610_cs_deselect();
}


report_pmw3610_t pmw3610_read_burst(void) {
    report_pmw3610_t data;
    data.dx = 0;
    data.dy = 0;

    if (pmw3610_status != NULL) {
        uprintf("pmw3610 failed to initialize, status: %s\n", pmw3610_status);
        return data;
    }

    pmw3610_cs_select();
    pmw3610_serial_write(REG_BURST_READ);
    wait_us(4);

    uint8_t buf[8];
    for (uint8_t i = 0; i < 8; i++) {
        buf[i] = pmw3610_serial_read();
    }

    pmw3610_cs_deselect();
    wait_us(1);

    data.dx = TOINT16((buf[PMW3610_X_L_POS] + ((buf[PMW3610_XY_H_POS] & 0xF0) << 4)),12);
    data.dy = TOINT16((buf[PMW3610_Y_L_POS] + ((buf[PMW3610_XY_H_POS] & 0x0F) << 8)),12);

    /* begin smart algo for surface coverage */
    static bool smart_flag = false;
    int16_t shutter = ((int16_t)(buf[PMW3610_SHUTTER_H_POS] & 0x01) << 8)
                      + buf[PMW3610_SHUTTER_L_POS];
    if (smart_flag && shutter < 45 ) {
      pmw3610_write_reg(0x32, 0x00);
      smart_flag = false;
    }
    if (!smart_flag && shutter > 45 ) {
      pmw3610_write_reg(0x32, 0x80);
      smart_flag = true;
    }
    /* end smart algo for surface coverage */

    return data;
}

uint16_t pmw3610_get_cpi(void) {
    // I have no idea how to retrieve the cpi value from the sensor because
    // the CONFIDENTIAL datasheet doesn't provide any information about it.
    // YES, the datasheet is CONFIDENTIAL! Pixart, really? why?
    return 1;
}

void pmw3610_set_cpi(uint16_t cpi) {
    uint8_t cpival = constrain(cpi / PMW3610_CPI_STEP, 1, (PMW3610_CPI_MAX / PMW3610_CPI_STEP) - 1U);
    // construct the data
    uint8_t addr[] = {REG_SPI_PAGE0, REG_RES_STEP, REG_SPI_PAGE0};
    uint8_t data[] = {0xff, cpival, 0x00};

    // enable spi clock
    pmw3610_write_reg(REG_SPI_CLK_ON_REQ, PMW3610_SPI_CLOCK_CMD_ENABLE);
	// write data
	for (size_t i = 0; i < 3; i++) {
        pmw3610_write_reg(addr[i], data[i]);
	}
    // disable spi clock to save power
    pmw3610_write_reg(REG_SPI_CLK_ON_REQ, PMW3610_SPI_CLOCK_CMD_DISABLE);
    uprintf("set cpi to: %d / %d = %d\n", cpi, PMW3610_CPI_STEP, cpival);
}

bool pmw3610_check_signature(void) {
    uint8_t pid  = pmw3610_read_reg(REG_PRODUCT_ID);
    return pid == 0x3e;
}
