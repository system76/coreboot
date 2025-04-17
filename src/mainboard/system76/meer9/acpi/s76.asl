/* SPDX-License-Identifier: GPL-2.0-only */

// Notifications:
//   0x80 - hardware backlight toggle
//   0x81 - backlight toggle
//   0x82 - backlight down
//   0x83 - backlight up
//   0x84 - backlight color change
//   0x85 - OLED screen toggle
Device (S76D) {
	Name (_HID, "17761776")
	Name (_UID, 0)
	// Hide the device so that Windows does not warn about a missing driver.
	Name (_STA, 0xB)

	OperationRegion (HMIO, SystemIO, 0x295, 0x02)
	Field (HMIO, ByteAcc, NoLock, Preserve) {
		// Hardware manager index
		HMID, 8,
		// Hardware manager data
		HMDT, 8,
	}

	Method (INIT, 0, Serialized) {
		Printf ("S76D: INIT")
		Return (0)
	}

	Method (FINI, 0, Serialized) {
		Printf ("S76D: FINI")
		Return (0)
	}

	// Get Airplane LED
	Method (GAPL, 0, Serialized) {
		Return (0)
	}

	// Set Airplane LED
	Method (SAPL, 1, Serialized) {}

	// Get Keyboard Backlight Kind
	// 0 - No backlight
	// 1 - White backlight
	// 2 - RGB backlight
	Method (GKBK, 0, Serialized) {
		Return (0)
	}

	// Get Keyboard Brightness
	Method (GKBB, 0, Serialized) {
		Return (0)
	}

	// Set Keyboard Brightness
	Method (SKBB, 1, Serialized) {}

	// Get Keyboard Color
	Method (GKBC, 0, Serialized) {
		Return (0)
	}

	// Set Keyboard Color
	Method (SKBC, 1, Serialized) {}

	// Fan names
	Method (NFAN, 0, Serialized) {
		Return (Package() {
			"CPU fan"
		})
	}

	// Get fan duty cycle and RPM as a single value
	Method (GFAN, 1, Serialized) {
		// Set bank 0
		HMID = 0x4E
		HMDT = 0x80

		// Read fan duty cycle
		HMID = 0x4B
		Local0 = HMDT

		// Read fan RPM (low)
		HMID = 0x33
		Local1 = HMDT

		// Read fan RPM (high)
		HMID = 0x32
		Local2 = HMDT

		Return ((Local2 << 16) | (Local1 << 8) | Local0)
	}

	// Temperature names
	Method (NTMP, 0, Serialized) {
		Return (Package() {
			"CPU temp"
		})
	}

	// Get temperature
	Method (GTMP, 1, Serialized) {
		// Set bank 0
		HMID = 0x4E
		HMDT = 0x80

		// Read temperature
		HMID = 0x19
		Local0 = HMDT

		Return (Local0)
	}
}
