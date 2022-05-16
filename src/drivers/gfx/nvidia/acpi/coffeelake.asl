/* SPDX-License-Identifier: GPL-2.0-only */
/* NVIDIA GC6 on CFL and CML CPU PCIe ports */

// Memory mapped PCI express config space
OperationRegion (PCIC, SystemMemory, CONFIG_ECAM_MMCONF_BASE_ADDRESS + (CONFIG_DRIVERS_GFX_NVIDIA_BRIDGE << 15), 0x1000)

Field (PCIC, ByteAcc, NoLock, Preserve) {
	PVID,   16,
	PDID,   16,

	Offset (0x248),
		,   7,
	L23E,   1,      /* L23_Rdy Entry Request */
	L23R,   1,      /* L23_Rdy to Detect Transition */

	Offset (0xC20),
		,   4,
	P0AP,   2,      /* Additional power savings */

	Offset (0xC38),
		,   3,
	P0RM,   1,      /* Robust squelch mechanism */
}

// Enter L23
Method (DL23, 0, Serialized) {
	Printf("      GPU PORT DL23 START")

	L23E = 1
	Sleep (16)
	Local0 = 0
	While (L23E) {
		If ((Local0 > 4)) {
			Break
		}

		Sleep (16)
		Local0++
	}

	P0RM = 1
	P0AP = 3

	Printf("      GPU PORT DL23 FINISH")
}

// Exit L23
Method (L23D, 0, Serialized) {
	Printf("      GPU PORT L23D START")

	L23R = 1
	Sleep (16)
	Local0 = 0
	While (L23R) {
		If ((Local0 > 4)) {
			Break
		}

		Sleep (16)
		Local0++
	}

	P0RM = 0
	P0AP = 0

	Printf("      GPU PORT L23D FINISH")
}

// Main power resource
PowerResource (PWRR, 0, 0) {
	Name (_STA, 1)

	Method (_ON, 0, Serialized) {
		Printf("GPU PORT PWRR._ON")

		^^DEV0._ON()

		_STA = 1
	}

	Method (_OFF, 0, Serialized) {
		Printf("GPU PORT PWRR._OFF")

		^^DEV0._OFF()

		_STA = 0
	}
}

// Power resources for entering D0
Name (_PR0, Package () { PWRR })

// Power resources for entering D3
Name (_PR3, Package () { PWRR })

#include "common/gpu.asl"
