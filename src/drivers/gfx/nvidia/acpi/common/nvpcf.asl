/* SPDX-License-Identifier: GPL-2.0-only */

#define NVPCF_DSM_GUID "36b49710-2483-11e7-9598-0800200c9a66"
#define NVPCF_REVISION_ID 0x200

#define NVPCF_ERROR_SUCCESS	0
#define NVPCF_ERROR_GENERIC	0x80000001
#define NVPCF_ERROR_UNSUPPORTED	0x80000002

#define NVPCF_FUNC_GET_SUPPORTED			0 // Required
#define NVPCF_FUNC_GET_STATIC_CONFIG_TABLES		1 // Required
#define NVPCF_FUNC_UPDATE_DYNAMIC_PARAMS		2 // Required
#define NVPCF_FUNC_GET_WM2_TBAND_TABLES			3
#define NVPCF_FUNC_GET_WM2_SL_MAP_TABLES		4
#define NVPCF_FUNC_GET_WM2_DYNAMIC_PARAMS		5
#define NVPCF_FUNC_CPU_CONTROL				6
#define NVPCF_FUNC_GPU_INFO				7
#define NVPCF_FUNC_GET_DC_SYSTEM_POWER_LIMITS_TABLE	8
#define NVPCF_FUNC_CPU_TDP_CONTROL			9
#define NVPCF_FUNC_GET_DC_TPP_LIMIT_PREFERENCE		10
#define NVPCF_FUNC_GET_THERMAL_ZONE_STATUS		11

Name (DBAC, 0) // Disable GPU Boost on AC
Name (DBDC, 0) // Disable GPU Boost on DC (XXX: Proprietary default disables on DC)

Name (_HID, "NVDA0820")
Name (_UID, "NPCF")

Method (_STA, 0, NotSerialized)
{
	Return (0x0F) // ACPI_STATUS_DEVICE_ALL_ON
}

Method (_DSM, 4, Serialized)
{
	If (Arg0 == ToUUID (NVPCF_DSM_GUID)) {
		Printf ("NVPCF DSM")
		If (ToInteger (Arg1) == NVPCF_REVISION_ID) {
			Return (NPCF (Arg2, Arg3))
		} Else {
			Printf ("  NVPCF: Unsupported revision: %o", Arg1)
		}
	}
	Else {
		Printf ("  NVPCF: Unsupported GUID: %o", IDST (Arg0))
	}

	Return (NVPCF_ERROR_GENERIC)
}

Method (NPCF, 2, Serialized)
{
	Switch (ToInteger (Arg0)) {
		Case (NVPCF_FUNC_GET_SUPPORTED) {
			Printf ("  NVPCF[0]: GET_SUPPORTED")
			Return (ITOB (
				(1 << NVPCF_FUNC_GET_SUPPORTED) |
				(1 << NVPCF_FUNC_GET_STATIC_CONFIG_TABLES) |
				(1 << NVPCF_FUNC_UPDATE_DYNAMIC_PARAMS)
			))
		}
		Case (NVPCF_FUNC_GET_STATIC_CONFIG_TABLES) {
			Printf ("  NVPCF[1]: GET_STATIC_CONFIG_TABLE")
			Return (Buffer (14) {
				// System Device Table Header (v2.0)
				0x20, 0x03, 0x01,
				// System Device Table Entries
				//   [3:0] CPU Type (0=Intel, 1=AMD)
				//   [7:4] GPU Type (0=Nvidia)
				0x00,
				// System Controller Table Header (v2.3)
				0x23, 0x04, 0x05, 0x01,
				// System Controller Table Controller Entry
				// Controller Flags
				//   [3:0] Controller Class Type
				//      0=Disabled
				//      1=Dynamic Boost Controller
				//      2=Configurable TGP-only Controller
				//   [7:4] Reserved
				0x01,
				// Controller Params
				//   [0:0] DC support (0=Not supported, 1=Supported)
				//   [31:1] Reserved
				0x01, 0x00, 0x00, 0x00,
				// Single byte checksum value
				0xAD
			})
		}
		Case (NVPCF_FUNC_UPDATE_DYNAMIC_PARAMS) {
			Printf ("  NVPCF[2]: UPDATE_DYNAMIC_PARAMS")

			// Dynamic Params Common Status, Input
			//   0=Get Controller Params
			//   1=Set Controller Status
			CreateField (Arg1, 0x28, 2, ICMD)

			// XXX: All input params unused?
			// Controller Entry, Input
			//CreateByteField (Arg1, 0x15, IIDX) // Controller index
			//CreateField (Arg1, 0xB0, 0x01, PWCS) // Power control capability (0=Disabled, 1=Enabled)
			//CreateField (Arg1, 0xB1, 0x01, PWTS) // Power transfer status (0=Disabled, 1=Enabled)
			//CreateField (Arg1, 0xB2, 0x01, CGPS) // CTGP status (0=Disabled, 1=Enabled)

			Name (PBD2, Buffer(49) {
				// Dynamic Params Table Header
				0x23, // Version 2.3
				0x05, // Table header size in bytes
				0x10, // Size of Common Status in bytes
				0x1C, // Size of Controller Entry in bytes
				0x01, // Number of Controller Entries
			})

			// Dynamic Params Common Status, Output
			// Baseline (configurable) TGP in AC for the intended TPP
			// limit, expressed as a signed offset relative to the
			// static TGP rates, AC, in 1/8-watt units.
			CreateWordField (PBD2, 0x05, TGPA)

			// Controller Entry, Output - Dynamic Boost Controller
			CreateByteField (PBD2, 0x15, OIDX) // Controller index
			// Disable controller on AC/DC
			//   [0:0] Disable controller on AC (0=Enable, 1=Disable)
			//   [1:1] Disable controller on DC (0=Enable, 1=Disable)
			CreateByteField (PBD2, 0x16, PC02)
			CreateWordField (PBD2, 0x19, TPPA) // TPP target on AC
			CreateWordField (PBD2, 0x1D, MAGA) // Max TGP on AC
			CreateWordField (PBD2, 0x21, MIGA) // Min TGP on AC
			CreateDWordField (PBD2, 0x25, DROP) // DC Rest of system reserved power
			CreateDWordField (PBD2, 0x29, LTBC) // Long Timescale Battery Current Limit
			CreateDWordField (PBD2, 0x2D, STBC) // Short Timescale Battery Current Limit

			Switch (ToInteger (ICMD)) {
				Case (0) {
					Printf ("    Get Controller Params")

					TGPA = 0
					OIDX = 0
					PC02 = DBAC | (DBDC << 1)
					TPPA = (CONFIG_DRIVERS_GFX_NVIDIA_DYNAMIC_BOOST_TPP << 3)
					MAGA = (CONFIG_DRIVERS_GFX_NVIDIA_DYNAMIC_BOOST_MAX << 3)
					MIGA = (CONFIG_DRIVERS_GFX_NVIDIA_DYNAMIC_BOOST_MIN << 3)

					// TODO: Handle v2.3+ fields

					Printf ("PBD2: %o", SFST(PBD2))
					Return (PBD2)
				}
				Case (1) {
					Printf ("    Set Controller Status")

					// XXX: Match proprietary firmware behavior,
					// which just explicitly sets fields to zero.
					TGPA = 0
					OIDX = 0
					PC02 = 0
					TPPA = 0
					MAGA = 0
					MIGA = 0

					Printf ("PBD2: %o", SFST(PBD2))
					Return (PBD2)
				}
				Default {
					Printf ("      Unknown Input Command: %o", SFST(ICMD))
					Return (NV_ERROR_UNSUPPORTED)
				}
			}
		}
		Case (NVPCF_FUNC_GET_WM2_TBAND_TABLES) {
			Printf ("  NVPCF[3]: GET_WM2_TBAND_TABLES")
		}
		Case (NVPCF_FUNC_GET_WM2_SL_MAP_TABLES) {
			Printf ("  NVPCF[4]: GET_WM2_SL_MAP_TABLES")
		}
		Case (NVPCF_FUNC_GET_WM2_DYNAMIC_PARAMS) {
			Printf ("  NVPCF[5]: GET_WM2_DYNAMIC_PARAMS")
		}
		Case (NVPCF_FUNC_CPU_CONTROL) {
			Printf ("  NVPCF[6]: CPU_CONTROL")
		}
		Case (NVPCF_FUNC_GPU_INFO) {
			Printf ("  NVPCF[7]: GPU_INFO")
		}
		Case (NVPCF_FUNC_GET_DC_SYSTEM_POWER_LIMITS_TABLE) {
			Printf ("  NVPCF[8]: GET_DC_SYSTEM_POWER_LIMITS_TABLE")
		}
		Case (NVPCF_FUNC_CPU_TDP_CONTROL) {
			Printf ("  NVPCF[9]: CPU_TDP_CONTROL")
		}
		Case (NVPCF_FUNC_GET_DC_TPP_LIMIT_PREFERENCE) {
			Printf ("  NVPCF[10]: GET_DC_TPP_LIMIT_PREFERENCE")
		}
		Case (NVPCF_FUNC_GET_THERMAL_ZONE_STATUS) {
			Printf ("  NVPCF[11]: GET_THERMAL_ZONE_STATUS")
		}
		Default {
			Printf ("  NVPCF: Unknown function: %o", Arg0)
		}
	}

	// XXX: DG says unsupported functions should return a
	// buffer, but even the example immediately following
	// this statement returns a DWORD, and this is what
	// proprietary firmware also does.
	Return (NVPCF_ERROR_UNSUPPORTED)
}
