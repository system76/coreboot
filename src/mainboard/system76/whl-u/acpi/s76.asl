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

// Notifications:
//   0x80 - hardware backlight toggle
//   0x81 - backlight toggle
//   0x82 - backlight down
//   0x83 - backlight up
//   0x84 - backlight color change
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

#if CONFIG_MAINBOARD_PCI_SUBSYSTEM_DEVICE_ID == 0x1325
    // Set KB LED Brightness
    Method (SKBL, 1, Serialized) {
        If (^^PCI0.LPCB.EC.ECOK) {
            ^^PCI0.LPCB.EC.FDAT = 6
            ^^PCI0.LPCB.EC.FBUF = Arg0
            ^^PCI0.LPCB.EC.FBF1 = 0
            ^^PCI0.LPCB.EC.FBF2 = Arg0
            ^^PCI0.LPCB.EC.FCMD = 0xCA
        }
    }

    // Set Keyboard Color
    Method (SKBC, 1, Serialized) {
        If (^^PCI0.LPCB.EC.ECOK) {
            ^^PCI0.LPCB.EC.FDAT = 0x3
            ^^PCI0.LPCB.EC.FBUF = (Arg0 & 0xFF)
            ^^PCI0.LPCB.EC.FBF1 = ((Arg0 >> 16) & 0xFF)
            ^^PCI0.LPCB.EC.FBF2 = ((Arg0 >> 8) & 0xFF)
            ^^PCI0.LPCB.EC.FCMD = 0xCA
            Return (Arg0)
        } Else {
            Return (0)
        }
    }
#elif CONFIG_MAINBOARD_PCI_SUBSYSTEM_DEVICE_ID == 0x1323
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
#else
    #error Unknown Mainboard
#endif
}
