/* SPDX-License-Identifier: GPL-2.0-only */

#define NVPCF_DSM_GUID "36b49710-2483-11e7-9598-0800200c9a66"
#define NVPCF_REVISION_ID 0x00000200
#define NVPCF_ERROR_SUCCESS 0x0
#define NVPCF_ERROR_GENERIC 0x80000001
#define NVPCF_ERROR_UNSUPPORTED 0x80000002
#define NVPCF_FUNC_GET_SUPPORTED 0x00000000
#define NVPCF_FUNC_GET_STATIC_CONFIG_TABLES 0x00000001
#define NVPCF_FUNC_UPDATE_DYNAMIC_PARAMS 0x00000002

Name(_HID, "NVDA0820")

Name(_UID, "NPCF")

Method(_DSM, 4, Serialized) {
	Printf("NVPCF _DSM")
	If (Arg0 == ToUUID(NVPCF_DSM_GUID)) {
		If (ToInteger(Arg1) == NVPCF_REVISION_ID) {
			Return(NPCF(Arg2, Arg3))
		} Else {
			Printf("  Unsupported NVPCF revision: %o", SFST(Arg1))
			Return(NVPCF_ERROR_GENERIC)
		}
	} Else {
		Printf("  Unsupported GUID: %o", IDST(Arg0))
		Return(NVPCF_ERROR_GENERIC)
	}
}

Method(NPCF, 2, Serialized) {
	Printf("  NVPCF NPCF")
	Switch(ToInteger(Arg0)) {
		Case(NVPCF_FUNC_GET_SUPPORTED) {
			Printf("    Supported Functions")
			Return(ITOB(
				(1 << NVPCF_FUNC_GET_SUPPORTED) |
				(1 << NVPCF_FUNC_GET_STATIC_CONFIG_TABLES) |
				(1 << NVPCF_FUNC_UPDATE_DYNAMIC_PARAMS)
			))
		}
		Case(NVPCF_FUNC_GET_STATIC_CONFIG_TABLES) {
			Printf("    Get Static Config")
			Return(Buffer(14) {
				// Device table header
				0x20, 0x03, 0x01,
				// Intel + NVIDIA
				0x00,
				// Controller table header
				0x23, 0x04, 0x05, 0x01,
				// Dynamic boost controller
				0x01,
				// Supports DC
				0x01,
				// Reserved
				0x00, 0x00, 0x00,
				// Checksum
				0xAD
			})
		}
		Case(NVPCF_FUNC_UPDATE_DYNAMIC_PARAMS) {
			Printf("    Update Dynamic Boost")

			CreateField(Arg1, 0x28, 2, ICMD) // Input command

			Name(PCFP, Buffer(49) {
				// Table version
				0x23,
				// Table header size
				0x05,
				// Size of common status in bytes
				0x10,
				// Size of controller entry in bytes
				0x1C,
				// Other fields filled in later
			})
			CreateByteField(PCFP, 0x04, CCNT) // Controller count
			CreateWordField(PCFP, 0x19, ATPP) // AC TPP offset
			CreateWordField(PCFP, 0x1D, AMXP) // AC maximum TGP offset
			CreateWordField(PCFP, 0x21, AMNP) // AC minimum TGP offset

			Switch(ToInteger(ICMD)) {
				Case(0) {
					Printf("      Get Controller Params")
					// Number of controllers
					CCNT = 1
					// AC total processor power offset from default TGP in 1/8 watt units
					ATPP = (CONFIG_DRIVERS_GFX_NVIDIA_DYNAMIC_BOOST_TPP << 3)
					// AC maximum TGP offset from default TGP in 1/8 watt units
					AMXP = (CONFIG_DRIVERS_GFX_NVIDIA_DYNAMIC_BOOST_MAX << 3)
					// AC minimum TGP offset from default TGP in 1/8 watt units
					AMNP = (CONFIG_DRIVERS_GFX_NVIDIA_DYNAMIC_BOOST_MIN << 3)
					Printf("PCFP: %o", SFST(PCFP))
					Return(PCFP)
				}
				Case(1) {
					Printf("      Set Controller Status")
					//TODO
					Printf("PCFP: %o", SFST(PCFP))
					Return(PCFP)
				}
				Default {
					Printf("      Unknown Input Command: %o", SFST(ICMD))
					Return(NV_ERROR_UNSUPPORTED)
				}
			}
		}
		Default {
			Printf("    Unsupported function: %o", SFST(Arg0))
			Return(NVPCF_ERROR_UNSUPPORTED)
		}
	}
}
