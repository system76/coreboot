/* SPDX-License-Identifier: GPL-2.0-only */

#define NVIDIA_ERROR_UNSPECIFIED	0x80000001
#define NVIDIA_ERROR_UNSUPPORTED	0x80000002

#define NBCI_DSM_GUID		"D4A50B75-65C7-46F7-BFB7-41514CEA0244"
#define NBCI_REVISION_ID	0x102

#define GPS_DSM_GUID		"A3132D01-8CDA-49BA-A52E-BC9D46DF6B81"
#define GPS_REVISION_ID		0x200

#define JT_DSM_GUID		"CBECA351-067B-4924-9CBD-B46B00B86F34"
#define JT_REVISION_ID		0x200

#define NVOP_DSM_GUID		"A486D8F8-0BDA-471B-A72B-6042A6B5BEE0"
#define NVOP_REVISION_ID	0x100

Device (\_SB.PCI0.PEG0)
{
	Name (_ADR, CONFIG_DRIVERS_GFX_NVIDIA_BRIDGE << 16)

	PowerResource (PWRR, 0, 0) {
		Name (_STA, 1)

		Method (_ON)
		{
			_STA = 1
		}

		Method (_OFF)
		{
			_STA = 0
		}
	}

	Name (_PR0, Package () { PWRR })
	Name (_PR3, Package () { PWRR })
}

Device (\_SB.PCI0.PEG0.DGPU)
{
	Name(_ADR, 0x00000000)

	// GPU Power
	Name (GPWR, 0)

	// GCx State
	Name (GCST, 0)

	// For supporting Hybrid Graphics, the package refers to the PCIe controller
	// itself, which leverages GC6 Control methods under the dGPU namespace.
	Name (_PR0, Package() { \_SB.PCI0.PEG0.PWRR })
	Name (_PR3, Package() { \_SB.PCI0.PEG0.PWRR })

	Method (_STA)
	{
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
		Printf("_ON {")
		Printf("  Enable GPU power")
		STXS(DGPU_PWR_EN)
		Sleep(10)

		Printf("  Take GPU out of reset")
		STXS(DGPU_RST_N)
		Sleep(10)

		GPWR = 1
		Printf("} _ON")
	}

	Method (_OFF)
	{
		Printf("_OFF {")
		Printf("  Put GPU in reset")
		CTXS(DGPU_RST_N)
		Sleep(10)

		Printf("  Disable GPU power")
		CTXS(DGPU_PWR_EN)
		Sleep(10)

		GPWR = 0
		Printf("} _OFF")
	}

	Method (_PS0)
	{
		// XGXS, XGIS, XCLM
	}

	Method (_PS3)
	{
		// EGNS, EGIS, EGIN
	}

	Method (_DSM, 4, NotSerialized)
	{
		// NVIDIA GPU Boost
		If (Arg0 == ToUUID(GPS_DSM_GUID)) {
			If (Arg1 <= GPS_REVISION_ID) {
				Return(NVGB(Arg2, Arg3))
			}
		}

		// NVIDIA Low Power States
		If (Arg0 == ToUUID(JT_DSM_GUID)) {
			If (Arg1 <= JT_REVISION_ID) {
				Return(NVJT(Arg2, Arg3))
			}
		}

		Printf("Unsupported GUID")
		Return(NVIDIA_ERROR_UNSUPPORTED)
	}

	#include "boost.asl"
	#include "low_power_states.asl"
}
