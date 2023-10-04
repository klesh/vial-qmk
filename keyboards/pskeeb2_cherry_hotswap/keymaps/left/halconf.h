// Copyright 2022 Klesh Wong (@klesh)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once


#if defined(OLED_ENABLE)
#  define HAL_USE_I2C TRUE
#endif

#include_next "halconf.h"