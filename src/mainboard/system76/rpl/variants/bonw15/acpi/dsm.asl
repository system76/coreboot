/* SPDX-License-Identifier: GPL-2.0-only */

#define NV_ERROR_SUCCESS 0x0
#define NV_ERROR_UNSPECIFIED 0x80000001
#define NV_ERROR_UNSUPPORTED 0x80000002

#include "gps.asl"

Method (_DSM, 4, Serialized) {
	Printf("GPU _DSM")
	If (Arg0 == ToUUID (GPS_DSM_GUID)) {
		If (ToInteger(Arg1) == GPS_REVISION_ID) {
			Return (GPS(Arg2, Arg3))
		} Else {
			Printf("  Unsupported GPS revision: %o", SFST(Arg1))
			Return (NV_ERROR_UNSUPPORTED)
		}
	} Else {
		Printf("  Unsupported GUID: %o", IDST(Arg0))
		Return (NV_ERROR_UNSPECIFIED)
	}
}
