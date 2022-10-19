// Copyright 2022 Klesh Wong (@klesh)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include_next <mcuconf.h>

#undef STM32_SERIAL_USE_USART1
#define STM32_SERIAL_USE_USART1 TRUE

#if defined(POINTING_DEVICE_DRIVER_analog_joystick) || defined(ANALOG_JOYSTICK_ENABLE)
#  undef STM32_ADC_USE_ADC0
#  define STM32_ADC_USE_ADC0 TRUE
#  undef STM32_ADC_USE_ADC1
#  define STM32_ADC_USE_ADC1 TRUE
#endif
