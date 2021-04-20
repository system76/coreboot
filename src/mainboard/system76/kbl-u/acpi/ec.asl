/* SPDX-License-Identifier: GPL-2.0-only */

Device (EC0)
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
		Debug = Concatenate("EC: _REG", Concatenate(ToHexString(Arg0), Concatenate(" ", ToHexString(Arg1))))
		If (((Arg0 == 0x03) && (Arg1 == One))) {
			// Enable hardware touchpad lock, airplane mode, and keyboard backlight keys
			ECOS = 1

			// Enable software display brightness keys
			WINF = 1

			// Set current AC state
			^^^^AC.ACFG = ADP
			// Update battery information and status
			^^^^BAT0.UPBI()
			^^^^BAT0.UPBS()

			PNOT ()

			// EC is now available
			ECOK = Arg1

			// Reset System76 Device
			^^^^S76D.RSET()
		}
	}

	Method (PTS, 1, Serialized) {
		Debug = Concatenate("EC: PTS: ", ToHexString(Arg0))
		If (ECOK) {
			// Clear wake cause
			WFNO = Zero
		}
	}

	Method (WAK, 1, Serialized) {
		Debug = Concatenate("EC: WAK: ", ToHexString(Arg0))
		If (ECOK) {
			// Set current AC state
			^^^^AC.ACFG = ADP

			// Update battery information and status
			^^^^BAT0.UPBI()
			^^^^BAT0.UPBS()

			// Notify of changes
			Notify(^^^^AC, Zero)
			Notify(^^^^BAT0, Zero)

			Sleep (1000)

			// Reset System76 Device
			^^^^S76D.RSET()
		}
	}

	Method (_Q0A, 0, NotSerialized) // Touchpad Toggle
	{
		Debug = "EC: Touchpad Toggle"
	}

	Method (_Q0B, 0, NotSerialized) // Screen Toggle
	{
		Debug = "EC: Screen Toggle"
	}

	Method (_Q0C, 0, NotSerialized)  // Mute
	{
		Debug = "EC: Mute"
	}

	Method (_Q0D, 0, NotSerialized) // Keyboard Backlight
	{
		Debug = "EC: Keyboard Backlight"
	}

	Method (_Q0E, 0, NotSerialized) // Volume Down
	{
		Debug = "EC: Volume Down"
	}

	Method (_Q0F, 0, NotSerialized) // Volume Up
	{
		Debug = "EC: Volume Up"
	}

	Method (_Q10, 0, NotSerialized) // Switch Video Mode
	{
		Debug = "EC: Switch Video Mode"
	}

	Method (_Q11, 0, NotSerialized) // Brightness Down
	{
		Debug = "EC: Brightness Down"
		if (^^^^HIDD.HRDY) {
			^^^^HIDD.HPEM (20)
		}
	}

	Method (_Q12, 0, NotSerialized) // Brightness Up
	{
		Debug = "EC: Brightness Up"
		if (^^^^HIDD.HRDY) {
			^^^^HIDD.HPEM (19)
		}
	}

	Method (_Q13, 0, NotSerialized) // Camera Toggle
	{
		Debug = "EC: Camera Toggle"
	}

	Method (_Q14, 0, NotSerialized) // Airplane Mode
	{
		Debug = "EC: Airplane Mode"
		if (^^^^HIDD.HRDY) {
			^^^^HIDD.HPEM (8)
		}
		// TODO: hardware airplane mode
	}

	Method (_Q15, 0, NotSerialized) // Suspend Button
	{
		Debug = "EC: Suspend Button"
		Notify (SLPB, 0x80)
	}

	Method (_Q16, 0, NotSerialized) // AC Detect
	{
		Debug = "EC: AC Detect"
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
		Debug = "EC: BAT0 Update (17)"
		Notify (^^^^BAT0, 0x81) // Information Change
	}

	Method (_Q19, 0, NotSerialized)  // BAT0 Update
	{
		Debug = "EC: BAT0 Update (19)"
		Notify (^^^^BAT0, 0x81) // Information Change
	}

	Method (_Q1B, 0, NotSerialized) // Lid Close
	{
		Debug = "EC: Lid Close"
		Notify (LID0, 0x80)
	}

	Method (_Q1C, 0, NotSerialized) // Thermal Trip
	{
		Debug = "EC: Thermal Trip"
		/* TODO
		Notify (\_TZ.TZ0, 0x81) // Thermal Trip Point Change
		Notify (\_TZ.TZ0, 0x80) // Thermal Status Change
		*/
	}

	Method (_Q1D, 0, NotSerialized) // Power Button
	{
		Debug = "EC: Power Button"
		Notify (PWRB, 0x80)
	}

	Method (_Q50, 0, NotSerialized) // Other Events
	{
		Local0 = OEM4
		If (Local0 == 0x8A) {
			Debug = "EC: White Keyboard Backlight"
			Notify (^^^^S76D, 0x80)
		} ElseIf (Local0 == 0x9F) {
			Debug = "EC: Color Keyboard Toggle"
			Notify (^^^^S76D, 0x81)
		} ElseIf (Local0 == 0x81) {
			Debug = "EC: Color Keyboard Down"
			Notify (^^^^S76D, 0x82)
		} ElseIf (Local0 == 0x82) {
			Debug = "EC: Color Keyboard Up"
			Notify (^^^^S76D, 0x83)
		} ElseIf (Local0 == 0x80) {
			Debug = "EC: Color Keyboard Color Change"
			Notify (^^^^S76D, 0x84)
		} Else {
			Debug = Concatenate("EC: Other: ", ToHexString(Local0))
		}
	}
}
