/* SPDX-License-Identifier: GPL-2.0-only */

#define JT_DSM_GUID "CBECA351-067B-4924-9CBD-B46B00B86F34"
#define JT_REVISION_ID_MIN 0x00000100
#define JT_REVISION_ID_MAX 0x00000200
#define JT_FUNC_SUPPORT 0x00000000
#define JT_FUNC_CAPS 0x00000001
#define JT_FUNC_POWERCONTROL 0x00000003

//TODO: SMI traps and EGIN/XCLM
#define JT_GPC_GSS 0 // Get current GPU GCx sleep status
#define JT_GPC_EGNS 1 // Enter GC6 without self-refresh
#define JT_GPC_EGIS 2 // Enter GC6 with self-refresh
#define JT_GPC_XGXS 3 // Exit GC6 and stop self-refresh
#define JT_GPC_XGIS 4 // Exit GC6 for self-refresh update

#define JT_DFGC_NONE 0 // Handle request immediately
#define JT_DFGC_DEFER 1 // Defer GPC and GPCX
//TODO #define JT_DFGC_CLEAR 2 // Clear pending requests

// Deferred GC6 enter/exit until D3-cold (saved DFGC)
Name(DFEN, 0)

// Deferred GC6 enter control (saved GPC)
Name(DFCI, 0)

// Deferred GC6 exit control (saved GPCX)
Name(DFCO, 0)

Method (NVJT, 2, Serialized) {
	Printf("  GPU NVJT")
	Switch (ToInteger(Arg0)) {
		Case (JT_FUNC_SUPPORT) {
			Printf("    Supported Functions")
			Return(ITOB(
				(1 << JT_FUNC_SUPPORT) |
				(1 << JT_FUNC_CAPS) |
				(1 << JT_FUNC_POWERCONTROL)
			))
		}
		Case (JT_FUNC_CAPS) {
			Printf("    Capabilities")
			Return(ITOB(
				(1 << 0) | // G-SYNC NSVR power-saving features are enabled
				(1 << 1) | // NVSR disabled
				(2 << 3) | // Panel power and backlight are on the suspend rail
				(0 << 5) | // self-refresh controller remains powered while panel is powered
				(0 << 6) | // FB is not on the suspend rail but is powered on in GC6
				(0 << 8) | // Combined power rail for all GPUs
				(0 << 10) | // External SPI ROM
				(1 << 11) | // No SMI handler for kernel panic exit while in GC6
				(0 << 12) | // Supports notify on GC6 state done
				(1 << 13) | // Support deferred GC6
				(1 << 14) | // Support fine-grained root port control
				(2 << 15) | // GC6 version is GC6-R
				(0 << 17) | // GC6 exit ISR is not supported
				(0 << 18) | // GC6 self wakeup not supported
				(JT_REVISION_ID_MAX << 20) // Highest revision supported
			))
		}
		Case (JT_FUNC_POWERCONTROL) {
			Printf("    Power Control: %o", SFST(Arg1))

			CreateField (Arg1, 0, 3, GPC) // GPU power control
			CreateField (Arg1, 4, 1, PPC) // Panel power control
			CreateField (Arg1, 14, 2, DFGC) // Defer GC6 enter/exit until D3 cold
			CreateField (Arg1, 16, 3, GPCX) // Deferred GC6 exit control

			// Save deferred GC6 request
			If ((ToInteger(GPC) != 0) || (ToInteger(DFGC) != 0)) {
				DFEN = DFGC
				DFCI = GPC
				DFCO = GPCX
			}

			// Buffer to cache current state
			Name (JTBF, Buffer (4) { 0, 0, 0, 0 })
			CreateField (JTBF, 0, 3, CGCS) // Current GC state
			CreateField (JTBF, 3, 1, CGPS) // Current GPU power status
			CreateField (JTBF, 7, 1, CPSS) // Current panel and SRC state (0 when on)

			// If doing deferred GC6 request, return now
			If (ToInteger(DFGC) != 0) {
				CGCS = 1
				CGPS = 1
				Return (JTBF)
			}

			// Apply requested state
			Switch (ToInteger(GPC)) {
				Case (JT_GPC_GSS) {
					Printf("    Get current GPU GCx sleep status")
					//TODO: include transitions!
					If (GTXS(DGPU_RST_N)) {
						// GPU powered on
						CGCS = 1
						CGPS = 1
					} ElseIf (GTXS(DGPU_PWR_EN)) {
						// GPU powered off, GC6
						CGCS = 3
						CGPS = 0
					} Else {
						// GPU powered off, D3 cold
						CGCS = 2
						CGPS = 0
					}
				}
				Case (JT_GPC_EGNS) {
					Printf("    Enter GC6 without self-refresh")
					GC6I()
					CPSS = 1
				}
				Case (JT_GPC_EGIS) {
					Printf("    Enter GC6 with self-refresh")
					GC6I()
					If (ToInteger(PPC) == 0) {
						CPSS = 0
					}
				}
				Case (JT_GPC_XGXS) {
					Printf("    Exit GC6 and stop self-refresh")
					GC6O()

					CGCS = 1
					CGPS = 1
					If (ToInteger(PPC) != 0) {
						CPSS = 0
					}
				}
				Case (JT_GPC_XGIS) {
					Printf("    Exit GC6 for self-refresh update")
					GC6O()

					CGCS = 1
					CGPS = 1
					If (ToInteger(PPC) != 0) {
						CPSS = 0
					}
				}
				Default {
					Printf("    Unsupported GPU power control: %o", SFST(GPC))
				}
			}

			Return (JTBF)
		}
		Default {
			Printf("    Unsupported function: %o", SFST(Arg0))
			Return (NV_ERROR_UNSUPPORTED)
		}
	}
}
