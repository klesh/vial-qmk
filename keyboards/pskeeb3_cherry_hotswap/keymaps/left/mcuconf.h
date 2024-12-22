// Copyright 2022 Klesh Wong (@klesh)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include_next <mcuconf.h> // for i2c

#undef RP_I2C_USE_I2C0
#define RP_I2C_USE_I2C0 FALSE
#undef RP_I2C_USE_I2C1
#define RP_I2C_USE_I2C1 TRUE