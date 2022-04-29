/* SPDX-License-Identifier: GPL-2.0-or-later */

Device (COM1) {
	Name (_HID, EISAID ("PNP0501"))
	Name (_UID, One)

	Method (_STA, 0, NotSerialized)
	{
		Return (0x0F)
	}

	Name (_CRS, ResourceTemplate ()
	{
		FixedIO (0x03F8, 0x08)
		IRQNoFlags () {4}
	})
}
