/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 System76
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

Device (S76D) {
    Name (_HID, "17761776")
    Name (_UID, 0)

    // Get Airplane LED
    Method (GAPL, 0, Serialized) {
        If (^^PCI0.LPCB.EC.ECOK) {
            If (^^PCI0.LPCB.EC.AIRP & 0x40) {
                Return (1)
            }
        }
        Return (0)
    }

    // Set Airplane LED
    Method (SAPL, 1, Serialized) {
        If (^^PCI0.LPCB.EC.ECOK) {
            If (Arg0) {
                ^^PCI0.LPCB.EC.AIRP |= 0x40
            } Else {
                ^^PCI0.LPCB.EC.AIRP &= 0xBF
            }
        }
    }

    // Get KB LED
    Method (GKBL, 0, Serialized) {
        Local0 = 0
        If (^^PCI0.LPCB.EC.ECOK) {
            ^^PCI0.LPCB.EC.FDAT = One
            ^^PCI0.LPCB.EC.FCMD = 0xCA
            Local0 = ^^PCI0.LPCB.EC.FBUF
            ^^PCI0.LPCB.EC.FCMD = Zero
        }
        Return (Local0)
    }

    // Set KB Led
    Method (SKBL, 1, Serialized) {
        If (^^PCI0.LPCB.EC.ECOK) {
            ^^PCI0.LPCB.EC.FDAT = Zero
            ^^PCI0.LPCB.EC.FBUF = Arg0
            ^^PCI0.LPCB.EC.FCMD = 0xCA
        }
    }
}
