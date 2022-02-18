/* SPDX-License-Identifier: GPL-2.0-only */

#define NV_ERROR_SUCCESS 0x0
#define NV_ERROR_UNSPECIFIED 0x80000001
#define NV_ERROR_UNSUPPORTED 0x80000002

#include "nbci.asl"
#include "nvjt.asl"

Method (_DSM, 4, Serialized) {
	Printf("GPU _DSM")
	If (Arg0 == ToUUID (NBCI_DSM_GUID)) {
		If (ToInteger(Arg1) == NBCI_REVISION_ID) {
			Return (NBCI(Arg2, Arg3))
		} Else {
			Printf("  Unsupported NBCI revision: %o", SFST(Arg1))
			Return (NV_ERROR_UNSUPPORTED)
		}
	} ElseIf (Arg0 == ToUUID (JT_DSM_GUID)) {
		If (ToInteger(Arg1) >= JT_REVISION_ID_MIN) {
			Return (NVJT(Arg2, Arg3))
		} Else {
			Printf("  Unsupported JT revision: %o", SFST(Arg1))
			Return (NV_ERROR_UNSUPPORTED)
		}
	} Else {
		Printf("  Unsupported GUID: %o", IDST(Arg0))
		Return (NV_ERROR_UNSPECIFIED)
	}
}
