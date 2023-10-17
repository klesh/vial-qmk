
/* keyboard uid */
#define VIAL_KEYBOARD_UID {0xEE, 0x7E, 0x02, 0x5F, 0x78, 0xF7, 0x19, 0xFE}

#define VIAL_UNLOCK_COMBO_ROWS { 0, 4 }
#define VIAL_UNLOCK_COMBO_COLS { 4, 4 }

/* default layer count */
#define DYNAMIC_KEYMAP_LAYER_COUNT 4

/* trackpoint */
#define PS2_DATA_PIN GP10
#define PS2_CLOCK_PIN GP11
#define PS2_MOUSE_ROTATE 270
#define PS2_DRIVER vendor
#define PS2_PIO_USE_PIO1
#define PS2_MOUSE_SCROLL_LAYER_MASK (1<<3)
#define PS2_MOUSE_SCROLL_DIVISOR_H 2
#define PS2_MOUSE_SCROLL_DIVISOR_V 2