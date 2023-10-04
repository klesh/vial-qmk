// Copyright 2022 Klesh Wong (@klesh)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once


#if defined(PS2_MOUSE_ENABLE)
#  define PAL_USE_CALLBACKS TRUE
#endif

#include_next <halconf.h>
