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

	Printf("    GPU GC6I FINISH")
}

// Exit GC6
Method(GC6O, 0, Serialized) {
	Printf("    GPU GC6O START")

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
	Printf("GPU _ON START")

	// Power on GPU
	Printf("    Power on GPU")
	STXS(DGPU_PWR_EN)
	Sleep(5)

	// Exit GC6
	GC6O()

	Printf("GPU _ON FINISH")
}

Method (_OFF, 0, Serialized) {
	Printf("GPU _OFF START")

	// Enter GC6
	GC6I()

	// Power off GPU
	Printf("    Power off GPU")
	CTXS(DGPU_PWR_EN)
	Sleep(5)

	Printf("GPU _OFF FINISH")
}

Method (_STA, 0, Serialized) {
	//TODO
	Return (0xF)
}

// Main power resource
PowerResource (PWRR, 0, 0) {
	Name (_STA, 1)

	Method (_ON, 0, Serialized) {
		Printf("GPU PWRR._ON START")
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
			^^_ON()
		}
		// Restore SSID
		^^SSID = 0x40181558
		Printf("  Restore SSID: %o", SFST(^^SSID))
		_STA = 1
		Printf("GPU PWRR._ON FINISH")
	}

	Method (_OFF, 0, Serialized) {
		Printf("GPU PWRR._OFF START")
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
			^^_OFF()
		}
		_STA = 0
		Printf("GPU PWRR._OFF FINISH")
	}
}

// Power resources for entering D0
Name (_PR0, Package () { PWRR })

// Power resources for entering D3
Name (_PR3, Package () { PWRR })

// Current power state
Name (_PSC, 0)

// Place device in D0
Method (_PS0, 0, Serialized) {
	Printf("GPU _PS0")
	_PSC = 0
}

// Place device in D3
Method (_PS3, 0, Serialized) {
	Printf("GPU _PS3")
	_PSC = 3
}
