/* Copyright 2021 Klesh Wong
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

#include "config_common.h"

#define VENDOR_ID       0xFEED
#define PRODUCT_ID      0x3063
#define DEVICE_VER      0x0001
#define MANUFACTURER    "kleshwong"
#define PRODUCT         "RK61"


#define MATRIX_ROWS 8
#define MATRIX_COLS 8
#define DIODE_DIRECTION COL2ROW

#define MATRIX_ROW_PINS \
    { D1, D0, D4, C6, D7, E6, B4, B5 }
#define MATRIX_COL_PINS \
    { F4, F5, F6, F7, B1, B3, B2, B6 }

/* Set 0 if debouncing isn't needed */
#define DEBOUNCE 1
