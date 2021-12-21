/* SPDX-License-Identifier: GPL-2.0-only */

// NVIDIA Advanced Optimus

#define NVOP_FUNC_SUPPORT		0
#define NVOP_FUNC_DISPLAYSTATUS		5
#define NVOP_FUNC_MDTL			6
#define NVOP_FUNC_GETOBJBYTYPE		16
#define NVOP_FUNC_GETALLOBJS		17
#define NVOP_FUNC_OPTIMUSCAPS		26
#define NVOP_FUNC_OPTIMUSFLAGS		27

Method (NVOP, 2, Serialized)
{
	Printf("NVOP {")
	Local0 = NVIDIA_ERROR_UNSUPPORTED

	Switch (ToInteger(Arg0)) {
		Case (NVOP_FUNC_SUPPORT) {
		}

		Case (NVOP_FUNC_OPTIMUSCAPS) {
			CreateField (Arg1, 0, 1, FLGS)	// Flag updates
			CreateField (Arg1, 1, 1, PCOT)	// PCIe Configuration Space Owner Target
			CreateField (Arg1, 2, 1, PCOW)	// PCIe Configuration Space Owner Write
			CreateField (Arg1, 24, 2, OPCE)	// Optimus Power Control Enable
		}

		Default {
			Printf("  Unsupported NVOP_FUNC: %o", ToInteger(Arg0))
			Local0 = NVIDIA_ERROR_UNSUPPORTED
		}
	}

	Printf("} NVOP")
	Return(Local0)
}
