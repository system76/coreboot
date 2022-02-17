#define NBCI_DSM_GUID "D4A50B75-65C7-46F7-BFB7-41514CEA0244"
#define NBCI_REVISION_ID 0x00000102
#define NBCI_FUNC_SUPPORT 0x00000000
#define NBCI_FUNC_GETOBJBYTYPE 0x00000010

Method (NBCI, 2, Serialized) {
	Printf("  GPU NBCI")
	Switch (ToInteger(Arg0)) {
		Case (NBCI_FUNC_SUPPORT) {
			Printf("    Supported Functions")
			Return (ITOB(
				(1 << NBCI_FUNC_SUPPORT) |
				(1 << NBCI_FUNC_GETOBJBYTYPE)
			))
		}
		Case (NBCI_FUNC_GETOBJBYTYPE) {
			Printf("    Get Object By Type")
			Switch (ToInteger((BTOI(Arg1) >> 16) & 0xFFFF)) {
				Case (0x4452) {
					Printf("    Get Driver Object")
					Return (Buffer(0xA1) {
						0x57, 0x74, 0xDC, 0x86, 0x75, 0x84, 0xEC, 0xE7,
						0x52, 0x44, 0xA1, 0x00, 0x00, 0x00, 0x00, 0x01,
						0x00, 0x00, 0x00, 0x00, 0xDE, 0x10, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x00,
						0x00, 0x00, 0x01, 0x00, 0x47, 0x00, 0x00, 0x00,
						0x02, 0x00, 0x45, 0x00, 0x00, 0x00, 0x03, 0x00,
						0x51, 0x00, 0x00, 0x00, 0x04, 0x00, 0x4F, 0x00,
						0x00, 0x00, 0x05, 0x00, 0x4D, 0x00, 0x00, 0x00,
						0x06, 0x00, 0x4B, 0x00, 0x00, 0x00, 0x07, 0x00,
						0x49, 0x00, 0x00, 0x00, 0x08, 0x00, 0x47, 0x00,
						0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD9, 0x1C,
						0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
						0x41, 0x5D, 0xC9, 0x00, 0x01, 0x24, 0x2E, 0x00,
						0x02, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x01,
						0x00, 0x00, 0x00, 0xD9, 0x1C, 0x04, 0x00, 0x00,
						0x00, 0x01, 0x00, 0x00, 0x00, 0x60, 0x68, 0x9E,
						0x35, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00
					})
				}
				Case (0x564B) {
					Printf("    Get Validation Key Object")
					Return (Buffer(0xF0) {
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
					})
				}
				Default {
					Printf("    Unsupported object: %o", SFST(Arg1))
					Return (NV_ERROR_UNSUPPORTED)
				}
			}
		}
		Default {
			Printf("    Unsupported function: %o", SFST(Arg0))
			Return (NV_ERROR_UNSUPPORTED)
		}
	}
}
