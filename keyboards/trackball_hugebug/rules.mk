SERIAL_DRIVER = vendor
POINTING_DEVICE_ENABLE = yes
POINTING_DEVICE_DRIVER = pmw3610
DEBOUNCE_TYPE = custom
SRC += debounce.c

# for touch button
CUSTOM_MATRIX = lite
SRC += matrix.c
