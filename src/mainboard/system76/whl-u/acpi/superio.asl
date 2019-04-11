/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <drivers/pc80/pc/ps2_controller.asl>

// PS2 Keyboard Scope
Scope (PS2K) {
    #define LPC_DEVICE_PORT 0x60
    #include "lpc_device.asl"
    #undef LPC_DEVICE_PORT
}
