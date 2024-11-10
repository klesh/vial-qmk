// Copyright 2024 Klesh Wong (@klesh)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "matrix.h"
#include "debug.h"
#include "timer.h"

// begin sym_defer_g
#ifndef DEBOUNCE
#    define DEBOUNCE 5
#endif
static bool         debouncing = false;
static fast_timer_t debouncing_time;
// end sym_defer_g

#ifndef SPDT_DEBOUNCE_START_ROW
#    define SPDT_DEBOUNCE_START_ROW 0
#endif

#define ROW_SHIFTER ((matrix_row_t)1)

static uint8_t spdt_debounce_nc_index = 0;

void debounce_init(uint8_t num_rows) {
#ifdef SPDT_DEBOUNCE_END_ROW
    num_rows = SPDT_DEBOUNCE_END_ROW;
#endif
    spdt_debounce_nc_index = (num_rows - SPDT_DEBOUNCE_START_ROW) / 2 + SPDT_DEBOUNCE_START_ROW;
}

void debounce_free(void) {}

bool debounce(matrix_row_t raw[], matrix_row_t cooked[], uint8_t num_rows, bool changed) {
    // Divide rows from START to END into 2 matrices,
    //   1. The 1st one connect to the NO pins of the SPDT switch
    //   2. The 2nd one connect to the NC pins accordingly
    // To debounce:
    //   - A key is pressed when the NO pin is shorted and the NC pin is opened
    //   - A key is released when the NO pin is opened and the NC pin is shorted
    // In short: flip the key only when the corresponding NC pin is in the opposite state
    bool cooked_changed = false;

    if (changed) {
        debouncing      = true;
        debouncing_time = timer_read_fast();
    } else {
        bool debounced = debouncing && timer_elapsed_fast(debouncing_time) >= DEBOUNCE;
        for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
            matrix_row_t existing_row = cooked[i];
            matrix_row_t raw_row      = raw[i];
            matrix_row_t delta        = raw_row ^ existing_row;
            if (!delta) continue;
            if (i >= SPDT_DEBOUNCE_START_ROW && i < spdt_debounce_nc_index) {
                for (uint8_t j = 0; j < MATRIX_COLS; j++) {
                    matrix_row_t col_mask = (ROW_SHIFTER << j);
                    if (delta & col_mask) { // the key state might have changed
                        matrix_row_t debouncing_row = raw[i + spdt_debounce_nc_index];
                        if ((debouncing_row ^ raw_row) & col_mask) { // make sure the NC pin is in the opposite state
                            cooked_changed = true;
                            existing_row ^= col_mask; // flip the bit.
                        }
                    }
                }
            } else if (debounced) {
                existing_row = raw_row;
                cooked_changed = true;
                debouncing     = false;
            }
            cooked[i] = existing_row;
        }
    }

    return cooked_changed;
}
