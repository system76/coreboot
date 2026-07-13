/* SPDX-License-Identifier: GPL-2.0-only */

#define GPS_DSM_GUID "A3132D01-8CDA-49BA-A52E-BC9D46DF6B81"
#define GPS_REVISION_ID 0x00000200
#define GPS_FUNC_SUPPORT 0x00000000
#define GPS_FUNC_PSHARESTATUS 0x00000020
#define GPS_FUNC_PSHAREPARAMS 0x0000002A

Method(GPS, 2, Serialized) {
	Printf("  GPU GPS")
	Switch(ToInteger(Arg0)) {
		Case(GPS_FUNC_SUPPORT) {
			Printf("    Supported Functions")
			Return(ITOB(
				(1 << GPS_FUNC_SUPPORT) |
				(1 << GPS_FUNC_PSHARESTATUS) |
				(1 << GPS_FUNC_PSHAREPARAMS)
			))
		}
		Case(GPS_FUNC_PSHARESTATUS) {
			Printf("    Power Share Status")
			Return(ITOB(0))
		}
		Case(GPS_FUNC_PSHAREPARAMS) {
			Printf("    Power Share Parameters")

			CreateField(Arg1, 0, 4, QTYP) // Query type

			Name(GPSP, Buffer(36) { 0x00 })
			CreateDWordField(GPSP, 0, RSTS) // Response status
			CreateDWordField(GPSP, 4, VERS) // Version

			// Set query type of response
			RSTS = QTYP
			// Set version of response
			VERS = 0x00010000

			Switch(ToInteger(QTYP)) {
				Case(0) {
					Printf("      Request Current Information")
					// No required information
					Return(GPSP)
				}
				Case(1) {
					Printf("      Request Supported Fields")
					// Support GPU temperature field
					RSTS |= (1 << 8)
					Return(GPSP)
				}
				Case(2) {
					Printf("      Request Current Limits")
					// No required limits
					Return(GPSP)
				}
				Default {
					Printf("      Unknown Query: %o", SFST(QTYP))
					Return(NV_ERROR_UNSUPPORTED)
				}
			}
		}
		Default {
			Printf("    Unsupported function: %o", SFST(Arg0))
			Return(NV_ERROR_UNSUPPORTED)
		}
	}
}
