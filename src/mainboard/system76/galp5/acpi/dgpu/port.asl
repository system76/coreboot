/* SPDX-License-Identifier: GPL-2.0-only */

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
	Offset (0xE0),
	, 7,
	NCB7, 1,       /* Scratch bit */
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
	Local0 = 8
	While ((Local0 > 0)) {
		If ((L23E == 0)) {
			Break
		}
		Sleep (16)
		Local0--
	}
	NCB7 = 1
	Printf("      GPU PORT DL23 FINISH")
}

// Exit L23
Method (L23D, 0, Serialized) {
	Printf("      GPU PORT L23D START")
	If ((NCB7 == 1)) {
		L23R = 1
		Local0 = 20
		While ((Local0 > 0)) {
			If ((L23R == 0)) {
				Break
			}
			Sleep (16)
			Local0--
		}

		NCB7 = 0
		Local0 = 8
		While ((Local0 > 0)) {
			If ((LASX == 1)) {
				Break
			}
			Sleep (16)
			Local0--
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
