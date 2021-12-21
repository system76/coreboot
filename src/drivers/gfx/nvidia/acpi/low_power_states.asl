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
			Printf("  JT_FUNC_POWERCONTROL: %o", ToHexString(Arg1));
			// TODO
			Local0 = NVIDIA_ERROR_UNSUPPORTED

			/*
			CreateField (Arg1, 0, 3, GPC)		// GPU Power Control
			CreateField (Arg1, 3, 1, GGP)		// Global GPU Power
			CreateField (Arg1, 4, 1, PPC)		// Panel Power Control
			CreateField (Arg1, 6, 2, NOC)		// Notify on complete
			CreateField (Arg1, 8, 2, PRGX)		// PCIe Root Power GC6 Exit Sequence
			CreateField (Arg1, 10, 2, PRGE)		// PCIe Root Power GC6 Entry Sequence
			CreateField (Arg1, 12, 1, PRPC)		// Poll for Root Port Completion
			CreateField (Arg1, 13, 1, PLON)		// PCIe Root Port Control
			CreateField (Arg1, 14, 2, DFGC)		// Defer GC6 Enter/Exit until D3cold
			CreateField (Arg1, 16, 3, GPCX)		// Deferred GC6 Exit Control
			CreateField (Arg1, 19, 1, EGEI)		// Enable GC6 Exit ISR
			CreateField (Arg1, 20, 2, PLCM)		// PCIe Root Port Control Method for PLON

			Local0 = Buffer(4) {0, 0, 0, 0}
			CreateField (Local0, 0, 3, GCS)		// GC State
			CreateField (Local0, 3, 1, GPS)		// GPU Power Status
			CreateField (Local0, 7, 1, PSS)		// Panel and SRC State
			*/

			/*
			 * DFGC
			 * 0: Perform request immediately
			 * 1: Defer GPC and GPCX to be processed when setting Device Power State
			 * 2: Clear any pending deferred requests
			 */
			/*
			If (DFGC == 2) {
				DPC = 0
				DPCX = 0
			}
			*/

			/*
			 * GPC
			 * 0 GSS) Get current GPU GCx Sleep Status
			 * 1 EGNS: Entery GC6 only. No SMI trap, No Self-Refresh. Panel
			 *         and GPU will be powred down as normal. FB will remain powered.
			 * 2 EGIS: Enter GC6, keep Panel in Self-Refresh. Enable SMI trap on
			 *         VGA I/O regiters. Control of screen is transitioned to the SRC and
			 *         then the GPU is powered down. Panel and FB remain powered while
			 *         the GPU is off.
			 * 3 XGXS: Exit GC6. Exit Panel Self-Refresh (Sparse). GPU is powered on.
			 *         Disable SMI traps.
			 * 4 XGIS: Exit GC6 for Self-Refresh Update (Burst). GPU is powered on, but
			 *         the SRC continues to retain control of screen refresh, while the
			 *         GPU sends an update to SRC for display. Disable SMI traps.
			 * 5 EGIN: Enter GC6, keep Pnael in Self-Refresh. No SMI trap on VGA I/O
			 *         registers. Control of screen is transitioned to SRC and then
			 *         GPU is powred down. Panel and FB remain powered while the GPU is off.
			 * 6 XCLM: Trigger GPU_EVENT only. GPU_EVENT would be assered and de-asserted,
			 *         regardless of GPU power state, without waiting for any GPU-side
			 *         signaling. No change in GPU power state is made. SMI traps disabled.
			 */
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

	Printf("} NVJT")
	Return(Local0)
}
