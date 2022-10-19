// Copyright 2022 Klesh Wong (@klesh)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define HAL_USE_SERIAL TRUE

#if defined(POINTING_DEVICE_DRIVER_analog_joystick) || defined(ANALOG_JOYSTICK_ENABLE)
#  define HAL_USE_ADC TRUE
#endif

#if defined(PS2_MOUSE_ENABLE)
#  define PAL_USE_CALLBACKS TRUE
#endif

#include_next <halconf.h>
