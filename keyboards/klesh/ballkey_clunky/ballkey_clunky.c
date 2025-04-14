#include "ballkey_clunky.h"
#include "action.h"
#include <string.h>

#ifdef CONSOLE_ENABLE
#    include "print.h"
#endif // CONSOLE_ENABLE

#define POINTING_DEVICE_ENABLE
#ifdef POINTING_DEVICE_ENABLE


// User hard coded settings
static uint16_t sensor_cpis[] = {200, 400, 600, 800, 1000, 1600, 2400, 3200};
static uint8_t sensor_cpi_count = sizeof(sensor_cpis) / sizeof(sensor_cpis[0]);
static uint8_t sensor_altmode_threshold = 18; // movement less than threshold would be ignored in SCROLL mode
static uint8_t sensor_altmode_divider = 12; // scroll speed divider

// Persistent settings
typedef union {
    uint32_t raw;
    struct {
        uint8_t sensor_cpi_idx;
    } __attribute__((packed));
} sensor_config_t;
static sensor_config_t sensor_config = {0};

void eeconfig_init_kb(void) {  // EEPROM is getting reset!
    sensor_config.raw = 0;
    sensor_config.sensor_cpi_idx = 3; // Set default CPI to 600
    eeconfig_update_kb(sensor_config.raw); // Write default value to EEPROM now
}

void keyboard_post_init_kb(void) { // Load setting at startup
    sensor_config.raw = eeconfig_read_kb();
    pointing_device_set_cpi(sensor_cpis[sensor_config.sensor_cpi_idx]);
}

static void cycle_cpi(bool forward) { // User chaning CPI on the fly
    uint8_t next_idx = sensor_config.sensor_cpi_idx + forward ? 1 : -1;
    sensor_config.sensor_cpi_idx = (next_idx+sensor_cpi_count) % sensor_cpi_count;
    pointing_device_set_cpi(sensor_cpis[sensor_config.sensor_cpi_idx]);
    eeconfig_update_kb(sensor_config->raw);
}

// Transient runtime state
enum {
    SENSOR_MODE_NORMAL = 0,
    SENSOR_ALTMODE_SCR = 1,
    SENSOR_ALTMODE_DIR = 2,
};
static uint8_t sensor_mode = SENSOR_MODE_NORMAL;
static int16_t altmode_buffer_x = 0;
static int16_t altmode_buffer_y = 0;

static void altmode(report_mouse_t* mouse_report, mouse_hv_report_t* x, mouse_hv_report_t* y) {
    altmode_buffer_x += mouse_report->x;
    altmode_buffer_y += mouse_report->y;
    mouse_report->x = 0
    mouse_report->y = 0;
    if (abs(scroll_buffer_x) > sensor_altmode_threshold) {
        *x = altmode_buffer_x / sensor_altmode_divider;
    }
    if (abs(scroll_buffer_y) > sensor_altmode_threshold) {
        *y = altmode_buffer_y / sensor_altmode_divider;
    }
}

static void pointing_device_task_trackball(report_mouse_t* mouse_report) {
    switch (sensor_mode) {
    case SENSOR_ALTMODE_SCR:
        altmode(mouse_report, &mouse_report->h, &mouse_report->v);
        break;
    case SENSOR_ALTMODE_DIR:
        mouse_hv_report_t x, y;
        altmode(mouse_report, &x, &y);
        // send arrow keys
        tap_code_delay(scroll_buffer_x > 0 ? KC_RGHT : KC_LEFT, 10);
    default:
        altmode_buffer_x = 0;
        altmode_buffer_y = 0;
        break;
    }
        if (abs(scroll_buffer_x) > TRACKBALL_DIRECTION_BUFFER_SIZE) {
            scroll_buffer_x = 0;
            scroll_buffer_y = 0;
        }
        if (abs(scroll_buffer_y) > TRACKBALL_DIRECTION_BUFFER_SIZE) {
            tap_code_delay(scroll_buffer_y > 0 ? KC_UP : KC_DOWN, 10);
            scroll_buffer_x = 0;
            scroll_buffer_y = 0;
        }
    }
}

report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
    if (is_keyboard_master()) {
        pointing_device_task_trackball(&mouse_report);
        mouse_report = pointing_device_task_user(mouse_report);
    }
    return mouse_report;
}

#    if defined(POINTING_DEVICE_ENABLE) && !defined(NO_TRACKBALL_KEYCODES)
/** \brief Whether SHIFT mod is enabled. */
static bool has_shift_mod(void) {
#        ifdef NO_ACTION_ONESHOT
    return mod_config(get_mods()) & MOD_MASK_SHIFT;
#        else
    return mod_config(get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT;
#        endif // NO_ACTION_ONESHOT
}
#    endif // POINTING_DEVICE_ENABLE && !NO_TRACKBALL_KEYCODES

/**
 * \brief Outputs the trackball configuration to console.
 *
 * Prints the in-memory configuration structure to console, for debugging.
 * Includes:
 *   - raw value
 *   - drag-scroll: on/off
 *   - sniping: on/off
 *   - default DPI: internal table index/actual DPI
 *   - sniping DPI: internal table index/actual DPI
 */
static void debug_trackball_config_to_console(trackball_config_t* config) {
#    ifdef CONSOLE_ENABLE
    dprintf("(trackball) process_record_kb: config = {\n"
            "\traw = 0x%X,\n"
            "\t{\n"
            "\t\tis_dragscroll_enabled=%u\n"
            "\t\tis_sniping_enabled=%u\n"
            "\t\tis_direction_enabled=%u\n"
            "\t\tdefault_dpi=0x%X (%u)\n"
            "\t\tsniping_dpi=0x%X (%u)\n"
            "\t}\n"
            "}\n",
            config->raw, config->is_dragscroll_enabled, config->is_sniping_enabled, config->is_direction_enabled, config->pointer_default_dpi, get_pointer_default_dpi(config), config->pointer_sniping_dpi, get_pointer_sniping_dpi(config));
#    endif // CONSOLE_ENABLE
}

bool process_record_kb(uint16_t keycode, keyrecord_t* record) {
    if (!process_record_user(keycode, record)) {
        debug_trackball_config_to_console(&g_trackball_config);
        return false;
    }
#    ifdef POINTING_DEVICE_ENABLE
#        ifndef NO_TRACKBALL_KEYCODES
    switch (keycode) {
        case POINTER_DEFAULT_DPI_FORWARD:
            if (record->event.pressed) {
                // Step backward if shifted, forward otherwise.
                trackball_cycle_pointer_default_dpi(/* forward= */ !has_shift_mod());
            }
            break;
        case POINTER_DEFAULT_DPI_REVERSE:
            if (record->event.pressed) {
                // Step forward if shifted, backward otherwise.
                trackball_cycle_pointer_default_dpi(/* forward= */ has_shift_mod());
            }
            break;
        case POINTER_SNIPING_DPI_FORWARD:
            if (record->event.pressed) {
                // Step backward if shifted, forward otherwise.
                trackball_cycle_pointer_sniping_dpi(/* forward= */ !has_shift_mod());
            }
            break;
        case POINTER_SNIPING_DPI_REVERSE:
            if (record->event.pressed) {
                // Step forward if shifted, backward otherwise.
                trackball_cycle_pointer_sniping_dpi(/* forward= */ has_shift_mod());
            }
            break;
        case SNIPING_MODE:
            trackball_set_pointer_sniping_enabled(record->event.pressed);
            break;
        case SNIPING_MODE_TOGGLE:
            if (record->event.pressed) {
                trackball_set_pointer_sniping_enabled(!trackball_get_pointer_sniping_enabled());
            }
            break;
        case DRAGSCROLL_MODE:
            trackball_set_pointer_dragscroll_enabled(record->event.pressed);
            break;
        case DRAGSCROLL_MODE_TOGGLE:
            if (record->event.pressed) {
                trackball_set_pointer_dragscroll_enabled(!trackball_get_pointer_dragscroll_enabled());
            }
            break;
    }
#        endif // !NO_TRACKBALL_KEYCODES
#    endif     // POINTING_DEVICE_ENABLE
    if (IS_QK_KB(keycode) || IS_MOUSEKEY(keycode)) {
        debug_trackball_config_to_console(&g_trackball_config);
    }
    return true;
}

void eeconfig_init_kb(void) {
    g_trackball_config.raw = 0;
    write_trackball_config_to_eeprom(&g_trackball_config);
    maybe_update_pointing_device_cpi(&g_trackball_config);
    eeconfig_init_user();
}

void matrix_init_kb(void) {
    read_trackball_config_from_eeprom(&g_trackball_config);
    matrix_init_user();
}

#    ifdef TRACKBALL_CONFIG_SYNC
void trackball_config_sync_handler(uint8_t initiator2target_buffer_size, const void* initiator2target_buffer, uint8_t target2initiator_buffer_size, void* target2initiator_buffer) {
    if (initiator2target_buffer_size == sizeof(g_trackball_config)) {
        memcpy(&g_trackball_config, initiator2target_buffer, sizeof(g_trackball_config));
    }
}
#    endif

void keyboard_post_init_kb(void) {
    maybe_update_pointing_device_cpi(&g_trackball_config);
#    ifdef TRACKBALL_CONFIG_SYNC
    transaction_register_rpc(RPC_ID_KB_CONFIG_SYNC, trackball_config_sync_handler);
#    endif
    keyboard_post_init_user();
}

#    ifdef TRACKBALL_CONFIG_SYNC
void housekeeping_task_kb(void) {
    if (is_keyboard_master()) {
        // Keep track of the last state, so that we can tell if we need to propagate to slave.
        static trackball_config_t last_trackball_config = {0};
        static uint32_t           last_sync             = 0;
        bool                      needs_sync            = false;

        // Check if the state values are different.
        if (memcmp(&g_trackball_config, &last_trackball_config, sizeof(g_trackball_config))) {
            needs_sync = true;
            memcpy(&last_trackball_config, &g_trackball_config, sizeof(g_trackball_config));
        }
        // Send to slave every 500ms regardless of state change.
        if (timer_elapsed32(last_sync) > 500) {
            needs_sync = true;
        }

        // Perform the sync if requested.
        if (needs_sync) {
            if (transaction_rpc_send(RPC_ID_KB_CONFIG_SYNC, sizeof(g_trackball_config), &g_trackball_config)) {
                last_sync = timer_read32();
            }
        }
    }
    // No need to invoke the user-specific callback, as it's been called
    // already.
}
#    endif // TRACKBALL_CONFIG_SYNC
#endif     // POINTING_DEVICE_ENABLE

#if defined(KEYBOARD_bastardkb_trackball_3x5_blackpill) || defined(KEYBOARD_bastardkb_trackball_4x6_blackpill)
void keyboard_pre_init_kb(void) {
    gpio_set_pin_input_high(A0);
    keyboard_pre_init_user();
}

void matrix_scan_kb(void) {
    if (!gpio_read_pin(A0)) {
        reset_keyboard();
    }
    matrix_scan_user();
}
#endif // KEYBOARD_bastardkb_trackball_3x5_blackpill || KEYBOARD_bastardkb_trackball_4x6_blackpill

bool shutdown_kb(bool jump_to_bootloader) {
    if (!shutdown_user(jump_to_bootloader)) {
        return false;
    }
#ifdef RGBLIGHT_ENABLE
    rgblight_enable_noeeprom();
    rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
    rgblight_setrgb(RGB_RED);
#endif // RGBLIGHT_ENABLE
#ifdef RGB_MATRIX_ENABLE
    rgb_matrix_set_color_all(RGB_RED);
    rgb_matrix_update_pwm_buffers();
#endif // RGB_MATRIX_ENABLE
    return true;
}


#ifdef OLED_ENABLE

oled_rotation_t oled_init_kb(oled_rotation_t rotation) {
    if (!is_keyboard_master()) {
        return OLED_ROTATION_180; // flips the display 180 degrees if offhand
    }
    return rotation;
}

static void oled_render_layer_state(void) {
    oled_write_P(PSTR("Layer: "), false);
    switch (get_highest_layer(layer_state)) {
        case 0:
            oled_write_ln_P(PSTR("Default"), false);
            break;
        case 1:
            oled_write_ln_P(PSTR("Lower"), false);
            break;
        case 2:
            oled_write_ln_P(PSTR("Raise"), false);
            break;
        case 3:
            oled_write_ln_P(PSTR("Adjust"), false);
            break;
        default:
            oled_write_ln_P(PSTR("Undef"), false);
            break;
    }
}

char     key_name = ' ';
uint16_t last_keycode;
uint8_t  last_row;
uint8_t  last_col;

static const char PROGMEM code_to_name[60] = {' ', ' ', ' ', ' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 'R', 'E', 'B', 'T', '_', '-', '=', '[', ']', '\\', '#', ';', '\'', '`', ',', '.', '/', ' ', ' ', ' '};

static void set_keylog(uint16_t keycode, keyrecord_t *record) {
    // save the row and column (useful even if we can't find a keycode to show)
    last_row = record->event.key.row;
    last_col = record->event.key.col;

    key_name     = ' ';
    last_keycode = keycode;
    if (IS_QK_MOD_TAP(keycode)) {
        if (record->tap.count) {
            keycode = QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
        } else {
            keycode = 0xE0 + biton(QK_MOD_TAP_GET_MODS(keycode) & 0xF) + biton(QK_MOD_TAP_GET_MODS(keycode) & 0x10);
        }
    } else if (IS_QK_LAYER_TAP(keycode) && record->tap.count) {
        keycode = QK_LAYER_TAP_GET_TAP_KEYCODE(keycode);
    } else if (IS_QK_MODS(keycode)) {
        keycode = QK_MODS_GET_BASIC_KEYCODE(keycode);
    } else if (IS_QK_ONE_SHOT_MOD(keycode)) {
        keycode = 0xE0 + biton(QK_ONE_SHOT_MOD_GET_MODS(keycode) & 0xF) + biton(QK_ONE_SHOT_MOD_GET_MODS(keycode) & 0x10);
    }
    if (keycode > ARRAY_SIZE(code_to_name)) {
        return;
    }

    // update keylog
    key_name = pgm_read_byte(&code_to_name[keycode]);
}

static const char *depad_str(const char *depad_str, char depad_char) {
    while (*depad_str == depad_char)
        ++depad_str;
    return depad_str;
}

static void oled_render_keylog(void) {
    oled_write_char('0' + last_row, false);
    oled_write_P(PSTR("x"), false);
    oled_write_char('0' + last_col, false);
    oled_write_P(PSTR(", k"), false);
    const char *last_keycode_str = get_u16_str(last_keycode, ' ');
    oled_write(depad_str(last_keycode_str, ' '), false);
    oled_write_P(PSTR(":"), false);
    oled_write_char(key_name, false);
    oled_advance_page(true);
}

// static void render_bootmagic_status(bool status) {
//     /* Show Ctrl-Gui Swap options */
//     static const char PROGMEM logo[][2][3] = {
//         {{0x97, 0x98, 0}, {0xb7, 0xb8, 0}},
//         {{0x95, 0x96, 0}, {0xb5, 0xb6, 0}},
//     };
//     if (status) {
//         oled_write_ln_P(logo[0][0], false);
//         oled_write_ln_P(logo[0][1], false);
//     } else {
//         oled_write_ln_P(logo[1][0], false);
//         oled_write_ln_P(logo[1][1], false);
//     }
// }

__attribute__((weak)) void oled_render_logo(void) {
    // clang-format off
    static const char PROGMEM crkbd_logo[] = {
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94,
        0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4,
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4,
        0};
    // clang-format on
    oled_write_P(crkbd_logo, false);
}

bool oled_task_kb(void) {
    if (!oled_task_user()) {
        return false;
    }
    if (is_keyboard_master()) {
        oled_render_layer_state();
        oled_render_keylog();
    } else {
        oled_render_logo();
    }
    return false;
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        set_keylog(keycode, record);
    }
    return process_record_user(keycode, record);
}
#endif // OLED_ENABLE
