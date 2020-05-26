// From https://review.coreboot.org/c/coreboot/+/40625
/* SPDX-License-Identifier: GPL-2.0-only */

Device (PEGP)
{
	Name (_ADR, 0x00010000)

	OperationRegion (PEGC, PCI_Config, 0x00, 0x0100)
	Field (PEGC, AnyAcc, NoLock, Preserve)
	{
	Offset (0x04),	// Device Command; Primary Status
		PCMR, 8,	// SC, BM, Mem and I/O bitmask
/*	Offset (0x4a),	// Device Control; Device Status
		CEDR, 1,	// Correctable Error Detected	*/
	Offset (0x50),	// Link Control; Link Status
		ASPM, 2,	// ASPM Control
		    , 2,
		LNKD, 1,	// Link Disable
		RTRL, 1,	// Retrain Link
		    , 21,
		LNKT, 1,	// Link Training
	Offset (0x69),	// Device Control 2; Device Status 2
		    , 2,
		LREN, 1,	// LTR Enable
/*	Offset (0x80),	// MSI Control
		    , 16,
		MSIE, 1,	// MSI enable	*/
	Offset (0xa4),	// Power Management
		PMST, 2,	// Power State
/*	Offset (0x328),
		    , 19,
		LNKS, 4,	// Link Status?	*/
	}

	OperationRegion (PCAP, PCI_Config, 0x40, 0x14)
	Field (PCAP, DWordAcc, NoLock, Preserve)
	{
		Offset (0x10),
		LCTL, 16,	// Link Control
	}

	PowerResource (PWRR, 0, 0)
	{
		Name (_STA, 1)

		Method (_ON)
		{
			Debug = "PEGP.PWRR._ON"
			\_SB.PCI0.PEGP._ON ()
			PCMR = 0x07	// Bus Master, Memory and I/O space enabled
			PMST = 0	// Power state: D0
//			MSIE = 1	// Enable MSI interrupts
			_STA = 1
		}

		Method (_OFF)
		{
			Debug = "PEGP.PWRR._OFF"
			\_SB.PCI0.PEGP._OFF ()
			_STA = 0
		}
	}

	/* Depend on the CLK to be active. _PR3 is also searched by nouveau to
	 * detect "Windows 8 compatible Optimus _DSM handler".
	 */
	Name (_PRE, Package () { \_SB.PCI0.PEGP.PWRR })
	Name (_PR0, Package () { \_SB.PCI0.PEGP.PWRR })
	Name (_PR3, Package () { \_SB.PCI0.PEGP.PWRR })

	Method (_PS0)
	{
		Debug = "PEGP._PS0"
		LNKD = 0
		RTRL = 1

		Local1 = 0x32
		While (Local1)
		{
			Sleep (2)
			If (LNKT == 0)
			{
				Break
			}

			Local1--
		}
	}

	Method (_PS3)
	{
		Debug = "PEGP._PS3"
		LNKD = 1
	}

	Method (_PSC, 0, Serialized)
	{
		If (LNKD == 1)
		{
		    Debug = "PEGP._PSC 3"
			Return (3)
		}
		Else
		{
		    Debug = "PEGP._PSC 0"
			Return (0)
		}
	}

	Method (_ON)
	{
		Debug = "PEGP._ON"
		If (^DEV0.ONOF == 0)
		{
			^DEV0.ONOF = 1
			^DEV0._ON ()
			LREN = ^DEV0.LTRE	// Restore LTR enable bit
//			CEDR = 1
			// Restore the Link Control register
			LCTL = ((^DEV0.ELCT & 0x43) | (LCTL & 0xFFBC))
			//Notify (\_SB.PCI0, 0)	// Bus Check
		}
	}

	Method (_OFF)
	{
		Debug = "PEGP._OFF"
		If (^DEV0.ONOF == 1)
		{
			^DEV0.ONOF = 0
			^DEV0.ELCT = LCTL	// Save the Link Control register
			^DEV0.LTRE = LREN	// Save LTR enable bit
			^DEV0._OFF ()
			//Notify (\_SB.PCI0, 0)	// Bus Check
		}
	}

	Method (_STA)
	{
		If (^DEV0.ONOF)
		{
		    Debug = "PEGP._STA F"
			Return (0xF)
		}
		Else
		{
		    Debug = "PEGP._STA 5"
			Return (0x5)
		}
	}

	Device (DEV0)
	{
		Name(_ADR, 0x00000000)
		Name (ONOF, 1)
		Name (ELCT, 0)
		Name (LTRE, 0)
	}

	Device (DEV1)
	{
		Name(_ADR, 0x00000001)
	}
}
