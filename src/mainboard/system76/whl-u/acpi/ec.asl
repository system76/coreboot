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

Device (EC)
{
    Name (_HID, EisaId ("PNP0C09") /* Embedded Controller Device */)  // _HID: Hardware ID
    Name (_GPE, 0x50 /* GPP_E16 */)  // _GPE: General Purpose Events
    Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
    {
        IO (Decode16,
            0x0062,             // Range Minimum
            0x0062,             // Range Maximum
            0x00,               // Alignment
            0x01,               // Length
            )
        IO (Decode16,
            0x0066,             // Range Minimum
            0x0066,             // Range Maximum
            0x00,               // Alignment
            0x01,               // Length
            )
    })

    #include "acpi/ec_ram.asl"

    Name (ECOK, Zero)
    Method (_REG, 2, Serialized)  // _REG: Region Availability
    {
        Store ("EC: _REG", Debug)
        If (((Arg0 == 0x03) && (Arg1 == One))) {
            // Enable software touchpad lock and airplane mode keys
            ECOS = 2

            // Enable software backlight keys
            WINF = 1

            // Set current AC state
            ^^^^AC.ACFG = ADP

            PNOT ()

            // EC is now available
            ECOK = Arg1
        }
    }

    Method (_Q0A, 0, NotSerialized) // Touchpad Toggle
    {
        Store ("EC: Touchpad Toggle", Debug)
    }

    Method (_Q0B, 0, NotSerialized) // Screen Toggle
    {
        Store ("EC: Screen Toggle", Debug)
    }

    Method (_Q0C, 0, NotSerialized)  // Mute
    {
        Store ("EC: Mute", Debug)
    }

    Method (_Q0D, 0, NotSerialized) // Keyboard Backlight
    {
        Store ("EC: Keyboard Backlight", Debug)
    }

    Method (_Q0E, 0, NotSerialized) // Volume Down
    {
        Store ("EC: Volume Down", Debug)
    }

    Method (_Q0F, 0, NotSerialized) // Volume Up
    {
        Store ("EC: Volume Up", Debug)
    }

    Method (_Q10, 0, NotSerialized) // Switch Video Mode
    {
        Store ("EC: Switch Video Mode", Debug)
    }

    Method (_Q11, 0, NotSerialized) // Brightness Down
    {
        Store ("EC: Brightness Down", Debug)
        ^^^^HIDD.HPEM (20)
    }

    Method (_Q12, 0, NotSerialized) // Brightness Up
    {
        Store ("EC: Brightness Up", Debug)
        ^^^^HIDD.HPEM (19)
    }

    Method (_Q13, 0, NotSerialized) // Camera Toggle
    {
        Store ("EC: Camera Toggle", Debug)
    }

    Method (_Q14, 0, NotSerialized) // Airplane Mode
    {
        Store ("EC: Airplane Mode", Debug)
        ^^^^HIDD.HPEM (8)
    }

    Method (_Q15, 0, NotSerialized) // Suspend Button
    {
        Store ("EC: Suspend Button", Debug)
        Notify (SLPB, 0x80)
    }

    Method (_Q16, 0, NotSerialized) // AC Detect
    {
        Store ("EC: AC Detect", Debug)
        ^^^^AC.ACFG = ADP
        Notify (AC, 0x80) // Status Change
        Sleep (0x01F4)
        If (BAT0)
        {
            Notify (^^^^BAT0, 0x81) // Information Change
            Sleep (0x32)
            Notify (^^^^BAT0, 0x80) // Status Change
            Sleep (0x32)
        }
    }

    Method (_Q17, 0, NotSerialized)  // BAT0 Update
    {
        Store ("EC: BAT0 Update (17)", Debug)
        Notify (^^^^BAT0, 0x81) // Information Change
    }

    Method (_Q19, 0, NotSerialized)  // BAT0 Update
    {
        Store ("EC: BAT0 Update (19)", Debug)
        Notify (^^^^BAT0, 0x81) // Information Change
    }

    Method (_Q1B, 0, NotSerialized) // Lid Close
    {
        Store ("EC: Lid Close", Debug)
        Notify (LID0, 0x80)
    }

    Method (_Q1C, 0, NotSerialized) // Thermal Trip
    {
        Store ("EC: Thermal Trip", Debug)
        /* TODO
        Notify (\_TZ.TZ0, 0x81) // Thermal Trip Point Change
        Notify (\_TZ.TZ0, 0x80) // Thermal Status Change
        */
    }

    Method (_Q1D, 0, NotSerialized) // Power Button
    {
        Store ("EC: Power Button", Debug)
        Notify (PWRB, 0x80)
    }

    Method (_Q50, 0, NotSerialized) // Other Events
    {
        Local0 = OEM4
        If (Local0 == 0x8A) {
            Store ("EC: White Keyboard Backlight", Debug)
            Notify (^^^^S76D, 0x80)
        } ElseIf (Local0 == 0x9F) {
            Store ("EC: Color Keyboard Toggle", Debug)
            Notify (^^^^S76D, 0x81)
        } ElseIf (Local0 == 0x81) {
            Store ("EC: Color Keyboard Down", Debug)
            Notify (^^^^S76D, 0x82)
        } ElseIf (Local0 == 0x82) {
            Store ("EC: Color Keyboard Up", Debug)
            Notify (^^^^S76D, 0x83)
        } ElseIf (Local0 == 0x80) {
            Store ("EC: Color Keyboard Color Change", Debug)
            Notify (^^^^S76D, 0x84)
        } Else {
            Store ("EC: Other", Debug)
            Store (Local0, Debug)
        }
    }
}
