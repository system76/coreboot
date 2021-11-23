/* SPDX-License-Identifier: GPL-2.0-only */

// NVIDIA Low Power States

#define JT_FUNC_SUPPORT		0
#define JT_FUNC_CAPS		1
#define JT_FUNC_POWERCONTROL	3
#define JT_FUNC_PLATPOLICY	4

Method (NVJT, 2, Serialized)
{
	Printf("NVJT {")
	Switch (ToInteger(Arg0)) {
		Case (JT_FUNC_SUPPORT) {
			Printf("  JT_FUNC_SUPPORT");
			// Functions supported: 0, 1, 3, 4
			//Local0 = Buffer() { 0x1B, 0, 0, 0 }
			Local0 = Buffer() { 0x13, 0, 0, 0 }
		}

		Case (JT_FUNC_CAPS) {
			Printf("  JT_FUNC_CAPS");
			Local0 = Buffer(4) { 0 }

			// G-SYNC NVSR Power Features
			CreateField (Local0, 0, 1, JTEN)
			JTEN = 0

			// NVSR
			CreateField (Local0, 1, 2, NVSE)
			NVSE = 1

			// Panel Power Rail
			CreateField (Local0, 3, 2, PPR)
			PPR = 2

			// Self-Refresh Control (SRC) Power Rail
			CreateField (Local0, 5, 1, SRPR)
			SRPR = 0

			// FB Power Rail
			CreateField (Local0, 6, 2, FBPR)
			FBPR = 0

			// GPU Power Rail
			CreateField (Local0, 8, 2, GPR)
			GPR = 0

			// GC6 ROM
			CreateField (Local0, 10, 1, GCR)
			GCR = 0

			// Panic Trap Handler
			CreateField (Local0, 11, 1, PTH)
			PTH = 1

			// Supports Notify
			CreateField (Local0, 12, 1, NOTS)
			NOTS = 0

			// MS Hybrid Support
			CreateField (Local0, 13, 1, MHYB)
			MHYB = 1

			// Root Port Control
			CreateField (Local0, 14, 1, RPC)
			RPC = 1

			// GC6 Version
			CreateField (Local0, 15, 2, GC6V)
			GC6V = 2

			// GC6 Exit ISR Support
			CreateField (Local0, 17, 1, GEI)
			GEI = 0

			// GC6 Self Wakeup Support
			CreateField (Local0, 18, 1, GSW)
			GSW = 0

			// Maximum Revision Supported
			CreateField (Local0, 20, 12, MXRV)
			MXRV = JT_REVISION_ID
		}

		Case (JT_FUNC_POWERCONTROL) {
			Printf("  JT_FUNC_POWERCONTROL: Unimplemented!");
			// TODO
			Local0 = NVIDIA_ERROR_UNSUPPORTED
		}

		Case (JT_FUNC_PLATPOLICY) {
			Printf("  JT_FUNC_PLATPOLICY: %o", ToHexString(Arg1));
			//CreateField (Arg1, 2, 1, AUD)		// Azalia Audio Device
			//CreateField (Arg1, 3, 1, ADM)		// Audio Disable Mask
			//CreateField (Arg1, 4, 4, DGS)		// Driver expected State Mask

			// TODO: Save policy settings to NV CMOS?

			Local0 = Buffer(4) { 0, 0, 0, 0 }
			//CreateField (Local0, 2, 1, AUD)		// Audio Device status
			//CreateField (Local0, 4, 3, GRE)		// SBIOS requested GPU state
		}

		Default {
			Printf("  Unsupported JT_FUNC: %o", ToInteger(Arg0))
			Local0 = NVIDIA_ERROR_UNSUPPORTED
		}
	}

	Printf("}")
	Return(Local0)
}
