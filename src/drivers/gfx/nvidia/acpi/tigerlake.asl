/* SPDX-License-Identifier: GPL-2.0-only */
/* NVIDIA GC6 on (TGL and ADL) (CPU and PCH) PCIe ports */

// Port mapped PCI express config space
OperationRegion (PCIC, PCI_Config, 0x00, 0xFF)

Field (PCIC, AnyAcc, NoLock, Preserve) {
	Offset(0x52),  /* LSTS - Link Status Register */
	, 13,
	LASX, 1,       /* 0, Link Active Status */

	Offset(0x60),  /* RSTS - Root Status Register */
	, 16,
	PSPX, 1,       /* 16,  PME Status */

	Offset(0xD8),  /* 0xD8, MPC - Miscellaneous Port Configuration Register */
	, 30,
	HPEX, 1,       /* 30,  Hot Plug SCI Enable */
	PMEX, 1,       /* 31,  Power Management SCI Enable */

	Offset (0xE0), /* 0xE0, SPR - Scratch Pad Register */
	SCB0, 1,       /* Scratch bit 0 */

	Offset(0xE2),  /* 0xE2, RPPGEN - Root Port Power Gating Enable */
	, 2,
	L23E, 1,       /* 2,   L23_Rdy Entry Request (L23ER) */
	L23R, 1,       /* 3,   L23_Rdy to Detect Transition (L23R2DT) */
}

Field (PCIC, AnyAcc, NoLock, WriteAsZeros) {
	Offset(0xDC),  /* 0xDC, SMSCS - SMI/SCI Status Register */
	, 30,
	HPSX, 1,       /* 30,  Hot Plug SCI Status */
	PMSX, 1        /* 31,  Power Management SCI Status */
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
	SCB0 = 1

	Printf("      GPU PORT DL23 FINISH")
}

// Exit L23
Method (L23D, 0, Serialized) {
	Printf("      GPU PORT L23D START")

	If ((SCB0 == 1)) {
		L23R = 1
		Local0 = 0
		While (L23R) {
			If ((Local0 > 4)) {
				Break
			}
			Sleep (16)
			Local0++
		}

		SCB0 = 0
		Local0 = 0
		While ((LASX == 0)) {
			If ((Local0 > 8)) {
				Break
			}
			Sleep (16)
			Local0++
		}
	}

	Printf("      GPU PORT L23D FINISH")
}

Method (HPME, 0, Serialized) {
	Printf("  GPU PORT HPME START")

	If (PMSX == 1) {
		Printf("    Notify GPU driver of PME SCI")
		Notify(DEV0, 0x2)
		Printf("    Clear PME SCI")
		PMSX = 1
		Printf("    Consume PME notification")
		PSPX = 1
	}

	Printf("  GPU PORT HPME FINISH")
}

// Main power resource
PowerResource (PWRR, 0, 0) {
	Name (_STA, 1)

	Method (_ON, 0, Serialized) {
		Printf("GPU PORT PWRR._ON")

		HPME();
		If (PMEX == 1) {
			Printf("  Disable power management SCI")
			PMEX = 0
		}

		^^DEV0._ON()

		_STA = 1
	}

	Method (_OFF, 0, Serialized) {
		Printf("GPU PORT PWRR._OFF")

		^^DEV0._OFF()

		If (PMEX == 0) {
			Printf("  Enable power management SCI")
			PMEX = 1
			HPME()
		}

		_STA = 0
	}
}

// Power resources for entering D0
Name (_PR0, Package () { PWRR })

// Power resources for entering D3
Name (_PR3, Package () { PWRR })

#include "common/gpu.asl"
