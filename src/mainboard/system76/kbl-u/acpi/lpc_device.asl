/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 System76
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

// To use this, define LPC_DEVICE_PORT include
#ifndef LPC_DEVICE_PORT
#error LPC_DEVICE_PORT not defined
#endif

// Ports
OperationRegion (LPRT, SystemIO, LPC_DEVICE_PORT, 0x05)
Field (LPRT, ByteAcc, NoLock, Preserve) {
    // Data port
    LDAT, 8,
    Offset (4),
    // Command and status port
    LSTS, 8
}

// Can read
Method (LCRD, 0, Serialized) {
    Return (LSTS & 1 == 1)
}

// Wait for read ability
Method (LWRD, 0, Serialized) {
    While (! LCRD()) {
        Sleep(10)
    }
}

// Can write
Method (LCWR, 0, Serialized) {
    Return (LSTS & 2 == 0)
}

// Wait for write ability
Method (LWWR, 0, Serialized) {
    While (! LCWR()) {
        Sleep(10)
    }
}

// Command
Method (LCMD, 1, Serialized) {
    LWWR()
    LSTS = Arg0
    LWWR()
}

// Read
Method (LRD, 0, Serialized) {
    LWRD()
    Return (LDAT)
}

// Write
Method (LWR, 1, Serialized) {
    LWWR()
    LDAT = Arg0
    LWWR()
}
