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
    // PS2 Ports
    OperationRegion (PORT, SystemIO, 0x60, 0x05)
    Field (PORT, ByteAcc, NoLock, Preserve) {
        DATA, 8,
        Offset (4),
        STAT, 8
    }

    // Wait for PS2 Input
    Method (WIN, 0, Serialized) {
        While (STAT & 1 == 0) {
            Sleep(10)
        }
    }

    // Wait for PS2 Output
    Method (WOUT, 0, Serialized) {
        While (STAT & 2 > 0) {
            Sleep(10)
        }
    }

    // PS2 Command
    Method (CMD, 1, Serialized) {
        WOUT()
        STAT = Arg0
    }

    // PS2 Read
    Method (READ, 0, Serialized) {
        WIN()
        Return (DATA)
    }

    // PS2 Write
    Method (WRIT, 1, Serialized) {
        WOUT()
        DATA = Arg0
    }
}
