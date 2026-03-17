/* SPDX-License-Identifier: GPL-2.0-only */

Device (SIO) {
	Name (_ADR, 0x2E)
	OperationRegion (SIOA, SystemIO, 0x2E, 0x02)
	Field (SIOA, ByteAcc, NoLock, Preserve)
	{
		SI2E, 8,
		SI2F, 8,
	}
	IndexField (SI2E, SI2F, ByteAcc, NoLock, Preserve)
	{
		Offset (0x07),
		SLDN, 8,	/* Logical Device Number */
		Offset(0xE5),
		SRE5, 8,	/* Register 0xE5 */
	}

	Method (ENTR, 0, Serialized) {
		// Enter config mode
		SI2E = 0x87
		SI2E = 0x87
	}

	Method (EXIT, 0, Serialized) {
		// Exit config mode
		SI2E = 0xAA
	}

	Method (PTS, 0, Serialized) {
		ENTR()

		// Turn on fading LED
		SLDN = 0x15
		SRE5 = 0x43

		EXIT()
	}

	Method (WAK, 0, Serialized) {
		ENTR()

		// Turn off fading LED
		SLDN = 0x15
		SRE5 = 0x42

		EXIT()
	}
}
