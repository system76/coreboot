/* SPDX-License-Identifier: GPL-2.0-only */


	/* AcpiGpe0Blk */
	OperationRegion(GP0B, SystemMemory, 0xfed80814, 0x04)
		Field(GP0B, ByteAcc, NoLock, Preserve) {
		, 11,
		USBS, 1,
	}

	/* GPIO control block -- hardcoded to 0xfed80100 by AGESA */
	OperationRegion (GPIO, SystemMemory, 0xfed80100, 0x100)
	Field (GPIO, ByteAcc, NoLock, Preserve) {
		Offset (0x39),
		, 6,
		GP57, 1,	/* out: WLAN control (rf-kill) */
		Offset (0x76),
		, 7,
		GE22, 1,	/* General event 22 - connected to lid switch */
	}

	/* SMI/SCI control block -- hardcoded to 0xfed80200 by AGESA */
	OperationRegion (SMIX, SystemMemory, 0xfed80200, 0x100)
	Field (SMIX, AnyAcc, NoLock, Preserve) {
		Offset (0x08),	/* SCI level: 0 = active low, 1 = active high */
		, 22,
		LPOL, 1,	/* SCI22 trigger polarity - lid switch */
	}

	/*
	 * Used by EC code on certain events
	 *
	 * From ec/compal/ene932/acpi/ec.asl:
	 * The mainboard must define a PNOT method to handle power state
	 * notifications and Notify CPU device objects to re-evaluate their
	 * _PPC and _CST tables.
	 */
	Method (PNOT)
	{
		Printf ("Received PNOT call (probably from EC)")
		/* TODO: Implement this */
	}

Scope (\_SB) {
	Device (LID)
	{
		Name(_HID, EisaId("PNP0C0D"))
		Name(_PRW, Package () {EC_LID_GPE, 0x04}) /* wake from S1-S4 */
		Method(_LID, 0)
		{
			Return (GE22)	/* GE pin 22 */
		}

		Method (_INI, 0)
		{
			/* Make sure lid trigger polarity is set so that we
			 * trigger an SCI when lid status changes.
			 */
			LPOL = ~GE22
		}
	}

	Device (MB) {
		Name(_HID, EisaId("PNP0C01")) // System Board

		/* Lid open */
		Method (LIDO) { /* Stub */ }
		/* Lid closed */
		Method (LIDC) { /* Stub */ }
		/* Increase brightness */
		Method (BRTU) { /* Stub */ }
		/* Decrease brightness */
		Method (BRTD) { /* Stub */ }
		/* Switch display */
		Method (DSPS) { /* Stub */ }
		/* Toggle wireless */
		Method (WLTG)
		{
			GP57 = ~GP57
		}
		/* Return lid state */
		Method (LIDS)
		{
			Return(GE22)
		}
	}
}
