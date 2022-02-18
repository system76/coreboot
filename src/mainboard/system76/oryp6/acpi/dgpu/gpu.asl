/* SPDX-License-Identifier: GPL-2.0-only */

Device (\_SB.PCI0.PEGP) {
	Name (_ADR, CONFIG_DRIVERS_GFX_NVIDIA_BRIDGE << 16)

	#include "port.asl"

	Device (DEV0) {
		Name(_ADR, 0x00000000)

		#include "utility.asl"
		#include "dsm.asl"
		#include "power.asl"
	}
}
