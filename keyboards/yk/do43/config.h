#pragma once

/* key matrix size */
#define MATRIX_ROWS 4
#define MATRIX_COLS 12

/* key matrix pins */
#define MATRIX_ROW_PINS { B5, B4, E6, D7 }
#define MATRIX_COL_PINS { C6, D4, D0, D1, B6, B2, B3, B1, F7, F6, F5, F4 }

/* COL2ROW or ROW2COL */
#define DIODE_DIRECTION COL2ROW

/* Set 0 if debouncing isn't needed */
#define DEBOUNCING 5