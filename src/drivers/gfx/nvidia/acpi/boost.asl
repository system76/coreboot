/* SPDX-License-Identifier: GPL-2.0-only */

// NVIDIA GPU Boost for Notebook and All-In-One-Projects

#define GPS_FUNC_SUPPORT		0
#define GPS_FUNC_GETOBJBYTYPE		16
#define GPS_FUNC_GETALLOBJS		17
#define GPS_FUNC_GETCALLBACKS		19
#define GPS_FUNC_PCONTROL		28
#define GPS_FUNC_PSHARESTATUS		32
#define GPS_FUNC_PSHAREPARAMS		42

Method (GPS, 2, Serialized)
{
	Printf("GPS {")

	Switch (ToInteger(Arg0)) {
		// Bit list of supported functions
		Case (GPS_FUNC_SUPPORT) {
			Printf("  GPS_FUNC_SUPPORT")
			// Functions supported: 0, 32, 42
			Local0 = Buffer () {0x01, 0x00, 0x00, 0x00, 0x01, 0x04, 0x00, 0x00}
		}

		// Get current platform status, thermal budget
		Case (GPS_FUNC_PSHARESTATUS) {
			Printf("  GPS_FUNC_PSHARESTATUS: %o", ToHexString(Arg1))
			Local0 = Buffer (4) { 0 }
		}

		// Get GPU Boost platform parameters
		Case (GPS_FUNC_PSHAREPARAMS) {
			Printf("  GPS_FUNC_PSHAREPARAMS: %o", ToHexString(Arg1))
			CreateField (Arg1, 0, 3, QTYP)		// Query Type
			CreateField (Arg1, 8, 1, GTMP)		// GPU temperature status
			CreateField (Arg1, 9, 1, CTMP)		// CPU temperature status

			Local0 = Buffer (36) { 0 }
			CreateDWordField (Local0, 0, STAT)	// Status
			CreateDWordField (Local0, 4, VERS)	// Version
			CreateDWordField (Local0, 8, TGPU)	// GPU temperature (C)
			CreateDWordField (Local0, 12, PDTS)	// CPU package temperature (C)

			VERS = 0x00010000
			STAT = QTYP

			Printf("    Query Type = %o", ToInteger(QTYP))

			Switch (ToInteger(QTYP)) {
				// Get current status
				Case (0) {
					// TGPU must be 0.
				}

				// Get supported fields
				Case (1) {
					STAT |= 0x100
					// TGPU must be 0.
				}

				// Get current operating limits
				Case (2) {
					// GPU temperature status must be 1.
					STAT |= 0x100
					// TGPU should be 0. GPU will use its own default.
				}

				Default {
					Printf("    Unsupported Query Type: %o", ToInteger(QTYP))
					Local0 = NVIDIA_ERROR_UNSUPPORTED
				}
			}
		}

		Default {
			Printf("  Unsupported GPS_FUNC: %o", ToInteger(Arg0))
			Local0 = NVIDIA_ERROR_UNSUPPORTED
		}
	}

	Printf("} GPS")
	Return(Local0)
}
