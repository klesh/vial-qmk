/*
Copyright 2011 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <stdbool.h>
#include "debug.h"
#include "report.h"

#define PS2_MOUSE_SEND(command, message)                                                \
    do {                                                                                \
        __attribute__((unused)) uint8_t rcv = ps2_host_send(command);                   \
        if (debug_mouse) {                                                              \
            print((message));                                                           \
            xprintf(" command: %X, result: %X, error: %X \n", command, rcv, ps2_error); \
        }                                                                               \
    } while (0)

#define PS2_MOUSE_SEND_SAFE(command, message)          \
    do {                                               \
        if (PS2_MOUSE_STREAM_MODE == ps2_mouse_mode) { \
            ps2_mouse_disable_data_reporting();        \
        }                                              \
        PS2_MOUSE_SEND(command, message);              \
        if (PS2_MOUSE_STREAM_MODE == ps2_mouse_mode) { \
            ps2_mouse_enable_data_reporting();         \
        }                                              \
    } while (0)

#define PS2_MOUSE_SET_SAFE(command, value, message)    \
    do {                                               \
        if (PS2_MOUSE_STREAM_MODE == ps2_mouse_mode) { \
            ps2_mouse_disable_data_reporting();        \
        }                                              \
        PS2_MOUSE_SEND(command, message);              \
        PS2_MOUSE_SEND(value, "Sending value");        \
        if (PS2_MOUSE_STREAM_MODE == ps2_mouse_mode) { \
            ps2_mouse_enable_data_reporting();         \
        }                                              \
    } while (0)

#define PS2_MOUSE_RECEIVE(message)                                      \
    do {                                                                \
        __attribute__((unused)) uint8_t rcv = ps2_host_recv_response(); \
        if (debug_mouse) {                                              \
            print((message));                                           \
            xprintf(" result: %X, error: %X \n", rcv, ps2_error);       \
        }                                                               \
    } while (0)

__attribute__((unused)) static enum ps2_mouse_mode_e {
    PS2_MOUSE_STREAM_MODE,
    PS2_MOUSE_REMOTE_MODE,
} ps2_mouse_mode = PS2_MOUSE_STREAM_MODE;

/*
 * Data format:
 * byte|7       6       5       4       3       2       1       0
 * ----+----------------------------------------------------------------
 *    0|[Yovflw][Xovflw][Ysign ][Xsign ][ 1    ][Middle][Right ][Left  ]
 *    1|[                    X movement(0-255)                         ]
 *    2|[                    Y movement(0-255)                         ]
 */
#define PS2_MOUSE_BTN_MASK 0x07
#define PS2_MOUSE_BTN_LEFT 0
#define PS2_MOUSE_BTN_RIGHT 1
#define PS2_MOUSE_BTN_MIDDLE 2
#define PS2_MOUSE_X_SIGN 4
#define PS2_MOUSE_Y_SIGN 5
#define PS2_MOUSE_X_OVFLW 6
#define PS2_MOUSE_Y_OVFLW 7

/* mouse button to start scrolling; set 0 to disable scroll */
#ifndef PS2_MOUSE_SCROLL_BTN_MASK
#    define PS2_MOUSE_SCROLL_BTN_MASK (1 << PS2_MOUSE_BTN_MIDDLE)
#endif
/* send button event when button is released within this value(ms); set 0 to disable  */
#ifndef PS2_MOUSE_SCROLL_BTN_SEND
#    define PS2_MOUSE_SCROLL_BTN_SEND 300
#endif
/* divide virtical and horizontal mouse move by this to convert to scroll move */
#ifndef PS2_MOUSE_SCROLL_DIVISOR_V
#    define PS2_MOUSE_SCROLL_DIVISOR_V 2
#endif
#ifndef PS2_MOUSE_SCROLL_DIVISOR_H
#    define PS2_MOUSE_SCROLL_DIVISOR_H 2
#endif
/* multiply reported mouse values by these */
#ifndef PS2_MOUSE_X_MULTIPLIER
#    define PS2_MOUSE_X_MULTIPLIER 1
#endif
#ifndef PS2_MOUSE_Y_MULTIPLIER
#    define PS2_MOUSE_Y_MULTIPLIER 1
#endif
#ifndef PS2_MOUSE_V_MULTIPLIER
#    define PS2_MOUSE_V_MULTIPLIER 1
#endif
/* For some mice this will need to be 0x0F */
#ifndef PS2_MOUSE_SCROLL_MASK
#    define PS2_MOUSE_SCROLL_MASK 0xFF
#endif
#ifndef PS2_MOUSE_INIT_DELAY
#    define PS2_MOUSE_INIT_DELAY 1000
#endif

enum ps2_mouse_command_e {
    PS2_MOUSE_RESET                  = 0xFF,
    PS2_MOUSE_RESEND                 = 0xFE,
    PS2_MOUSE_SET_DEFAULTS           = 0xF6,
    PS2_MOUSE_DISABLE_DATA_REPORTING = 0xF5,
    PS2_MOUSE_ENABLE_DATA_REPORTING  = 0xF4,
    PS2_MOUSE_SET_SAMPLE_RATE        = 0xF3,
    PS2_MOUSE_GET_DEVICE_ID          = 0xF2,
    PS2_MOUSE_SET_REMOTE_MODE        = 0xF0,
    PS2_MOUSE_SET_WRAP_MODE          = 0xEC,
    PS2_MOUSE_READ_DATA              = 0xEB,
    PS2_MOUSE_SET_STREAM_MODE        = 0xEA,
    PS2_MOUSE_STATUS_REQUEST         = 0xE9,
    PS2_MOUSE_SET_RESOLUTION         = 0xE8,
    PS2_MOUSE_SET_SCALING_2_1        = 0xE7,
    PS2_MOUSE_SET_SCALING_1_1        = 0xE6,
};

typedef enum ps2_mouse_resolution_e {
    PS2_MOUSE_1_COUNT_MM,
    PS2_MOUSE_2_COUNT_MM,
    PS2_MOUSE_4_COUNT_MM,
    PS2_MOUSE_8_COUNT_MM,
} ps2_mouse_resolution_t;

typedef enum ps2_mouse_sample_rate_e {
    PS2_MOUSE_10_SAMPLES_SEC  = 10,
    PS2_MOUSE_20_SAMPLES_SEC  = 20,
    PS2_MOUSE_40_SAMPLES_SEC  = 40,
    PS2_MOUSE_60_SAMPLES_SEC  = 60,
    PS2_MOUSE_80_SAMPLES_SEC  = 80,
    PS2_MOUSE_100_SAMPLES_SEC = 100,
    PS2_MOUSE_200_SAMPLES_SEC = 200,
} ps2_mouse_sample_rate_t;

void ps2_mouse_init(void);

void ps2_mouse_init_user(void);

void ps2_mouse_task(void);

void ps2_mouse_disable_data_reporting(void);

void ps2_mouse_enable_data_reporting(void);

void ps2_mouse_set_remote_mode(void);

void ps2_mouse_set_stream_mode(void);

void ps2_mouse_set_scaling_2_1(void);

void ps2_mouse_set_scaling_1_1(void);

void ps2_mouse_set_resolution(ps2_mouse_resolution_t resolution);

void ps2_mouse_set_sample_rate(ps2_mouse_sample_rate_t sample_rate);

void ps2_mouse_moved_user(report_mouse_t *mouse_report);

/* ============================= TRACKPOINT ============================ */
/*
 * TrackPoint-specific commands as defined in the "IBM TrackPoint System
 * Version 4.0 Engineering Specification" (YKT3Eext.pdf).
 *
 * The TrackPoint extends the standard PS/2 mouse command set with the
 * 0xE2 command prefix, followed by a sub-command byte and an optional
 * argument byte:
 *   0xE2 0x80 <sub>  -> read a setting
 *   0xE2 0x81 <sub>  -> write a setting (next byte is the value)
 *
 * The commands below cover the most commonly adjusted TrackPoint
 * parameters. See the spec for the full list and behaviour.
 */

#define PS2_MOUSE_TP_CMD_PREFIX 0xE2

/* Sub-command bytes (the 3rd byte of the 0xE2 0x80/0x81 sequence). */
#define PS2_MOUSE_TP_SUB_SENSITIVITY 0x4A
#define PS2_MOUSE_TP_SUB_NEG_INERTIA 0x4D
#define PS2_MOUSE_TP_SUB_PTS_THRESHOLD 0x5C
#define PS2_MOUSE_TP_SUB_VALUE6_UPPER_PLATEAU_SPEED 0x60

/* Argument ranges and defaults. */
#define PS2_MOUSE_TP_SENSITIVITY_MIN 0
#define PS2_MOUSE_TP_SENSITIVITY_MAX 255
#define PS2_MOUSE_TP_SENSITIVITY_DEFAULT 128

#define PS2_MOUSE_TP_NEG_INERTIA_MIN 0
#define PS2_MOUSE_TP_NEG_INERTIA_MAX 255
#define PS2_MOUSE_TP_NEG_INERTIA_DEFAULT 0x06

#define PS2_MOUSE_TP_VALUE6_UPPER_PLATEAU_SPEED_MIN 0
#define PS2_MOUSE_TP_VALUE6_UPPER_PLATEAU_SPEED_MAX 255
#define PS2_MOUSE_TP_VALUE6_UPPER_PLATEAU_SPEED_DEFAULT 0x61

#define PS2_MOUSE_TP_PTS_THRESHOLD_MIN 0
#define PS2_MOUSE_TP_PTS_THRESHOLD_MAX 255
#define PS2_MOUSE_TP_PTS_THRESHOLD_DEFAULT 0x08

/*
 * Read the current value of a TrackPoint setting.
 * Returns 0 on success and stores the value in *value, or a non-zero
 * error code on failure. The functions are no-ops (return 0 with no
 * side effects) when the device is not a TrackPoint, i.e. when the
 * 0xE2 command is not acknowledged.
 */
uint8_t ps2_mouse_tp_sensitivity_get(uint8_t *value);
uint8_t ps2_mouse_tp_neg_inertia_get(uint8_t *value);
uint8_t ps2_mouse_tp_value6_upper_plateau_speed_get(uint8_t *value);
uint8_t ps2_mouse_tp_pts_threshold_get(uint8_t *value);

/*
 * Set a TrackPoint setting to an absolute value.
 * Returns 0 on success or a non-zero error code on failure.
 */
uint8_t ps2_mouse_tp_sensitivity_set(uint8_t value);
uint8_t ps2_mouse_tp_neg_inertia_set(uint8_t value);
uint8_t ps2_mouse_tp_value6_upper_plateau_speed_set(uint8_t value);
uint8_t ps2_mouse_tp_pts_threshold_set(uint8_t value);

/*
 * Adjust a TrackPoint setting by a signed delta, clamping to the
 * supported range. The new value is stored internally so that the
 * get/change functions keep working without re-reading from the
 * device.
 */
uint8_t ps2_mouse_tp_sensitivity_change(int8_t delta);
uint8_t ps2_mouse_tp_neg_inertia_change(int8_t delta);
uint8_t ps2_mouse_tp_value6_upper_plateau_speed_change(int8_t delta);
uint8_t ps2_mouse_tp_pts_threshold_change(int8_t delta);

/*
 * Restore all TrackPoint settings to their factory defaults.
 */
void ps2_mouse_tp_settings_reset(void);
