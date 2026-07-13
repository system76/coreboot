/* SPDX-License-Identifier: GPL-2.0-only */

Device (DEV0) {
	Name(_ADR, 0x00000000)

	#include "utility.asl"
	#include "dsm.asl"
	#include "power.asl"
}

#if CONFIG(DRIVERS_GFX_NVIDIA_DYNAMIC_BOOST)
Scope (\_SB) {
	Device(NPCF) {
		#include "utility.asl"
		#include "nvpcf.asl"
	}
}
#endif
