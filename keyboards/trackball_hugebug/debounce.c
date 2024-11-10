// Copyright 2024 Klesh Wong (@klesh)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "matrix.h"
#include "debug.h"

#ifndef SPDT_DEBOUNCE_START_ROW
#  define SPDT_DEBOUNCE_START_ROW 0
#endif

#define ROW_SHIFTER ((matrix_row_t)1)

static uint8_t spdt_debounce_nc_index = 0;

void debounce_init(uint8_t num_rows) {
#ifdef SPDT_DEBOUNCE_END_ROW
  num_rows = SPDT_DEBOUNCE_END_ROW
#endif
  spdt_debounce_nc_index = (num_rows - SPDT_DEBOUNCE_START_ROW) / 2 + SPDT_DEBOUNCE_START_ROW;
}

void debounce_free(void) {

}

bool debounce(matrix_row_t raw[], matrix_row_t cooked[], uint8_t num_rows, bool changed) {
  // Divide rows from START to END into 2 matrices,
  //   1. The 1st one connect to the NO pins of the SPDT switch
  //   2. The 2nd one connect to the NC pins accordingly
  // To debounce:
  //   - A key is pressed when the NO pin is shorted and the NC pin is opened
  //   - A key is released when the NO pin is opened and the NC pin is shorted
  // In short: flip the key only when the corresponding NC pin is in the opposite state
  bool cooked_changed = false;
  for (uint8_t i = SPDT_DEBOUNCE_START_ROW; i < spdt_debounce_nc_index; i++) {
    matrix_row_t existing_row = cooked[i];
    matrix_row_t raw_row = raw[i];
    matrix_row_t delta = raw_row ^ cooked[i];
    for (uint8_t j = 0; j < MATRIX_COLS; j++) {
      matrix_row_t col_mask = (ROW_SHIFTER << j);
      if (delta & col_mask) { // the key state might have changed
        matrix_row_t debouncing_row = raw[i+spdt_debounce_nc_index];
        if ((debouncing_row ^ raw_row) & col_mask) { // make sure the NC pin is in the opposite state
          cooked_changed = true;
          existing_row ^= col_mask; // flip the bit.
        }
        if (debug_matrix) {
          uprintf("debounce[%d][%d]: raw=%02x cooked=%02x debouncing=%02x\n", i, j, raw_row, existing_row, debouncing_row);
        }
      }
    }
    cooked[i] = existing_row;
  }

  return cooked_changed;
}