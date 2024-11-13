// Copyright 2024 Klesh Wong (@klesh)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "matrix.h"
#include "timer.h"
#include "gpio.h"
#include "debug.h"

#if (MATRIX_ROWS <= 8)
typedef uint8_t matrix_row_mask_t;
#elif (MATRIX_ROWS <= 16)
typedef uint16_t matrix_row_mask_t;
#elif (MATRIX_ROWS <= 32)
typedef uint32_t matrix_row_mask_t;
#else
#    error "MATRIX_ROWS: invalid value"
#endif

#ifndef TOUCH_TIMEOUT_US
#    define TOUCH_TIMEOUT_US 100
#endif

#ifndef CAPACITIVE_THRESHOLD_SCALE
#    define CAPACITIVE_THRESHOLD_SCALE 2
#endif

#ifndef COUNT_LIMIT
#    define COUNT_LIMIT 250
#endif

#ifndef POLARITY_MODE
#    define POLARITY_MODE 0
#endif

#ifndef MATRIX_INPUT_PRESSED_STATE
#    define MATRIX_INPUT_PRESSED_STATE 0
#endif

static pin_t             direct_pins[MATRIX_ROWS][MATRIX_COLS]   = DIRECT_PINS;
static matrix_row_mask_t capacitive_rows_mask                    = 0;
static uint8_t           capacitive_rows_thresholds[MATRIX_ROWS] = {0};

uint8_t capacitive_get_count(pin_t output_pin, pin_t input_pin, bool polarity) {
    // toggle output pin polarity and measure the delay on the input pin
    bool    exceeded = false;
    uint8_t counter  = 0;

    // round 1: flip
    gpio_write_pin(output_pin, POLARITY_MODE);
    while (gpio_read_pin(input_pin) != POLARITY_MODE) {
        counter++;
        // wait for input to go high
        if (counter > COUNT_LIMIT) {
            exceeded = true;
            break;
        }
    }
    // round 2: flip back
    gpio_write_pin(output_pin, !POLARITY_MODE);
    while (gpio_read_pin(input_pin) == POLARITY_MODE) {
        counter++;
        // wait for input to go high
        if (counter > COUNT_LIMIT) {
            exceeded = true;
            break;
        }
    }

    uint32_t elapsed;
    if (!exceeded)
        elapsed = counter;
    else
        elapsed = COUNT_LIMIT;
    return elapsed;
}

uint8_t capacitive_sample_untouch_threshold(pin_t output_pin, pin_t input_pin, bool polarity) {
    uint8_t threshold = 0;
    uint32_t time_start = timer_read32();
    while (timer_elapsed32(time_start) < TOUCH_TIMEOUT_US) {
        uint8_t count = capacitive_get_count(output_pin, input_pin, polarity);
        if (count > threshold) threshold = count;
    }
    return threshold * CAPACITIVE_THRESHOLD_SCALE;
}

void matrix_init_custom(void) {
    // TODO: initialize hardware here
    uint8_t capacitive_rows[] = CAPACITIVE_ROWS;
    for (uint8_t i = 0; i < sizeof(capacitive_rows) / sizeof(capacitive_rows[0]); i++) {
        uint8_t row = capacitive_rows[i];
        capacitive_rows_mask |= (1 << row);
    }
    // init pins
    for (int row = 0; row < MATRIX_ROWS; row++) {
        if ((1 << row) & capacitive_rows_mask) {
            pin_t output_pin = direct_pins[row][0];
            pin_t input_pin  = direct_pins[row][1];
            gpio_set_pin_output(output_pin);
            gpio_set_pin_input(input_pin);
            uint8_t threshold = capacitive_sample_untouch_threshold(output_pin, input_pin, POLARITY_MODE);
            if (threshold == COUNT_LIMIT) {
                // failed to get a good threshold, disable capacitive sensing
                uprintf("Failed to get a good threshold for row %d, disabling capacitive sensing\n", row);
                capacitive_rows_mask &= ~(1 << row);
            } else {
                capacitive_rows_thresholds[row] = threshold;
            }
        } else {
            for (int col = 0; col < MATRIX_COLS; col++) {
                pin_t pin = direct_pins[row][col];
                if (pin != NO_PIN) {
                    gpio_set_pin_input_high(pin);
                }
            }
        }
    }
}

void matrix_read_capacitive_row(matrix_row_t current_matrix[], uint8_t current_row) {
    // Start with a clear matrix row
    matrix_row_t current_row_value   = 0;

    pin_t output_pin = direct_pins[current_row][0];
    pin_t input_pin  = direct_pins[current_row][1];

    uint8_t      count = capacitive_get_count(output_pin, input_pin, POLARITY_MODE);
    if (count > capacitive_rows_thresholds[current_row]) { // touched, set the whole row to 0b1111....
        current_row_value = ~current_row_value;
        uprintf("c %d, t: %d\n", count, capacitive_rows_thresholds[current_row]);
    }
    current_matrix[current_row] = current_row_value;
}

static inline uint8_t readMatrixPin(pin_t pin) {
    if (pin != NO_PIN) {
        return (gpio_read_pin(pin) == MATRIX_INPUT_PRESSED_STATE) ? 0 : 1;
    } else {
        return 1;
    }
}

void matrix_read_directpins(matrix_row_t current_matrix[], uint8_t current_row) {
    // Start with a clear matrix row
    matrix_row_t current_row_value = 0;

    matrix_row_t row_shifter = MATRIX_ROW_SHIFTER;
    for (uint8_t col_index = 0; col_index < MATRIX_COLS; col_index++, row_shifter <<= 1) {
        pin_t pin = direct_pins[current_row][col_index];
        current_row_value |= readMatrixPin(pin) ? 0 : row_shifter;
    }

    // Update the matrix
    current_matrix[current_row] = current_row_value;
}

bool matrix_scan_custom(matrix_row_t raw_matrix[]) {
    matrix_row_t curr_matrix[MATRIX_ROWS] = {0};

    // Set row, read cols
    for (uint8_t current_row = 0; current_row < MATRIX_ROWS; current_row++) {
        if ((1 << current_row) & capacitive_rows_mask) {
            matrix_read_capacitive_row(curr_matrix, current_row);
        } else {
            matrix_read_directpins(curr_matrix, current_row);
        }
    }

    bool changed = memcmp(raw_matrix, curr_matrix, sizeof(curr_matrix)) != 0;
    if (changed) memcpy(raw_matrix, curr_matrix, sizeof(curr_matrix));

    if (debug_matrix && changed) {
        uprintf("matrix changed: %d\n", changed);
    }

    return (uint8_t)changed;
}
