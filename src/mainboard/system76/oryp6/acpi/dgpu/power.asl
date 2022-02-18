/* SPDX-License-Identifier: GPL-2.0-only */

//TODO: evaluate sleeps

OperationRegion (PCIC, PCI_Config, 0x00, 0xFF)
Field (PCIC, DwordAcc, NoLock, Preserve) {
	Offset (0x40),
	SSID, 32, // Subsystem vendor and product ID
}

// Enter GC6
Method(GC6I, 0, Serialized) {
	Printf("    GPU GC6I START")

	// Enter L23
	^^DL23()
	Sleep(5)

	// Put GPU into reset
	Printf("      Put GPU into reset")
	CTXS(DGPU_RST_N)
	Sleep(5)

	Debug = "      While DGPU_GC6 low"
	Local0 = Zero
	While (! GRXS(DGPU_GC6)) {
		If ((Local0 > 4)) {
			Debug = "        While DGPU_GC6 low timeout"

			Debug = "        DGPU_PWR_EN low"
			CTXS (DGPU_PWR_EN)
			Break
		}

		Sleep (16)
		Local0++
	}

	Printf("    GPU GC6I FINISH")
}

// Exit GC6
Method(GC6O, 0, Serialized) {
	Printf("    GPU GC6O START")

	Debug = "      If DGPU_PWR_EN low"
	If (! GTXS (DGPU_PWR_EN)) {
		Debug = "        DGPU_PWR_EN high"
		STXS (DGPU_PWR_EN)

		Debug = "        Sleep 16"
		Sleep (16)
	}

	// Bring GPU out of reset
	Printf("      Bring GPU out of reset")
	STXS(DGPU_RST_N)
	Sleep(5)

	// Exit L23
	^^L23D()
	Sleep(5)

	Printf("    GPU GC6O FINISH")
}

Method (_ON, 0, Serialized) {
	Printf("  GPU _ON START")

	If (DFEN == JT_DFGC_DEFER) {
		Switch (ToInteger(DFCO)) {
			Case (JT_GPC_XGXS) {
				Printf("    Exit GC6 and stop self-refresh")
				GC6O()
			}
			Default {
				Printf("    Unsupported DFCO: %o", SFST(DFCO))
			}
		}
		DFEN = JT_DFGC_NONE
	} Else {
		Printf("    Standard RTD3 power on")
		GC6O()
	}

	Printf("  GPU _ON FINISH")
}

Method (_OFF, 0, Serialized) {
	Printf("  GPU _OFF START")

	If (DFEN == JT_DFGC_DEFER) {
		Switch (ToInteger(DFCI)) {
			Case (JT_GPC_EGNS) {
				Printf("    Enter GC6 without self-refresh")
				GC6I()
			}
			Case (JT_GPC_EGIS) {
				Printf("    Enter GC6 with self-refresh")
				GC6I()
			}
			Default {
				Printf("    Unsupported DFCI: %o", SFST(DFCI))
			}
		}
		DFEN = JT_DFGC_NONE
	} Else {
		Printf("    Standard RTD3 power off")
		GC6I()
	}

	Printf("  GPU _OFF FINISH")
}

// Main power resource
PowerResource (PWRR, 0, 0) {
	Name (_STA, 1)

	Method (_ON, 0, Serialized) {
		Printf("GPU PWRR._ON")

		// Restore SSID
		^^SSID = 0x65E51558
		Printf("  Restore SSID: %o", SFST(^^SSID))

		_STA = 1
	}

	Method (_OFF, 0, Serialized) {
		Printf("GPU PWRR._OFF")

		_STA = 0
	}
}

// Power resources for entering D0
Name (_PR0, Package () { PWRR })

// Power resources for entering D3
Name (_PR3, Package () { PWRR })
