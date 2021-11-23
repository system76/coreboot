/* SPDX-License-Identifier: GPL-2.0-only */

#define NVIDIA_ERROR_UNSPECIFIED	0x80000001
#define NVIDIA_ERROR_UNSUPPORTED	0x80000002

#define NBCI_DSM_GUID		"D4A50B75-65C7-46F7-BFB7-41514CEA0244"
#define NBCI_REVISION_ID	0x102

#define GPS_DSM_GUID		"A3132D01-8CDA-49BA-A52E-BC9D46DF6B81"
#define GPS_REVISION_ID		0x200

#define JT_DSM_GUID		"CBECA351-067B-4924-9CBD-B46B00B86F34"
#define JT_REVISION_ID		0x103

#define NVOP_DSM_GUID		"A486D8F8-0BDA-471B-A72B-6042A6B5BEE0"
#define NVOP_REVISION_ID	0x100

// 00:01.0
Device (\_SB.PCI0.PEG0)
{
	Name (_ADR, CONFIG_DRIVERS_GFX_NVIDIA_BRIDGE << 16)

	PowerResource (PWRR, 0, 0) {
		Name (_STA, 1)

		Method (_ON)
		{
			Printf("PEG0._ON {")
			// TODO: Check for deferred GCx action
			\_SB.PCI0.PEG0.DGPU._ON()
			_STA = 1
			Printf("} PEG0._ON")
		}

		Method (_OFF)
		{
			Printf("PEG0._OFF {")
			// TODO: Check for deferred GCx action
			\_SB.PCI0.PEG0.DGPU._OFF()
			_STA = 0
			Printf("} PEG0._OFF")
		}
	}

	Name (_PR0, Package () { PWRR })
	Name (_PR3, Package () { PWRR })
}

// 01:00.0
Device (\_SB.PCI0.PEG0.DGPU)
{
	Name(_ADR, 0x00000000)

	Name (GPWR, 0)		// GPU Power
	Name (GCST, 6)		// GCx State

	// For supporting Hybrid Graphics, the package refers to the PCIe controller
	// itself, which leverages GC6 Control methods under the dGPU namespace.
	Name (_PR0, Package() { \_SB.PCI0.PEG0.PWRR })
	Name (_PR3, Package() { \_SB.PCI0.PEG0.PWRR })

	Method (_STA)
	{
		Printf("DGPU._STA")
		/*
		 * Only return "On" when:
		 * - GPU power is good
		 * - GPU has completed return to GC0
		 *
		 * In all other cases, return "Off".
		 */
		If ((GPWR == 1) && (GCST == 0)) {
			Return (0xF)
		} Else {
			Return (0)
		}
	}

	Method (_ON)
	{
		Printf("DGPU._ON {")
		Printf("  Enable GPU power")
		STXS(DGPU_PWR_EN)
		Sleep(10)

		Printf("  Take GPU out of reset")
		STXS(DGPU_RST_N)
		Sleep(10)

		GPWR = 1
		GCST = 0
		Printf("} DGPU._ON")
	}

	Method (_OFF)
	{
		Printf("DGPU._OFF {")
		Printf("  Put GPU in reset")
		CTXS(DGPU_RST_N)
		Sleep(10)

		Printf("  Disable GPU power")
		CTXS(DGPU_PWR_EN)
		Sleep(10)

		GPWR = 0
		GCST = 6
		Printf("} DGPU._OFF")
	}

	Method (_PS0)
	{
		// XGXS, XGIS, XCLM
		Printf("_PS0 {}")
	}

	Method (_PS3)
	{
		// EGNS, EGIS, EGIN
		Printf("_PS3 {}")
	}

	Method (_DSM, 4, Serialized)
	{
		// Notebook Common Interface
		If (Arg0 == ToUUID(NBCI_DSM_GUID)) {
			Printf("NBCI_DSM_GUID")
			If (Arg1 <= NBCI_REVISION_ID) {
				Printf("  TODO: Unimplemented!")
			}
		}

		// NVIDIA GPU Boost
		If (Arg0 == ToUUID(GPS_DSM_GUID)) {
			Printf("GPS_DSM_GUID")
			If (Arg1 <= GPS_REVISION_ID) {
				Return(GPS(Arg2, Arg3))
			}
		}

		// NVIDIA Low Power States
		If (Arg0 == ToUUID(JT_DSM_GUID)) {
			Printf("JT_DSM_GUID")
			If (Arg1 <= JT_REVISION_ID) {
				Return(NVJT(Arg2, Arg3))
			}
		}

		// Advanced Optimus
		If (Arg0 == ToUUID(NVOP_DSM_GUID)) {
			Printf("NVOP_DSM_GUID")
			If (Arg1 <= NVOP_REVISION_ID) {
				Printf("  TODO: Unimplemented!")
			}
		}

		Printf("Unsupported GUID: %o", ToHexString(Arg0))
		Return(NVIDIA_ERROR_UNSUPPORTED)
	}

	#include "boost.asl"
	#include "low_power_states.asl"
}
