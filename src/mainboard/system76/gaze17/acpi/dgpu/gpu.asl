/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.PEG2) {
	#include "port.asl"

	Device (DEV0) {
		Name(_ADR, 0x00000000)

		#include "utility.asl"
		#include "dsm.asl"
		#include "power.asl"
	}
}
