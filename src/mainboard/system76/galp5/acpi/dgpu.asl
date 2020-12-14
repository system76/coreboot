/* SPDX-License-Identifier: GPL-2.0-only */

#define DGPU_RP \_SB.PCI0.RP01

External (DGPU_RP.RTD3)

Device (DGPU_RP.DEV0) {
	Name(_ADR, 0x00000000)

	Method (_PS0, 0) {
		Debug = "GPU _PS0"
	}

	Method (_PS3, 0) {
		Debug = "GPU _PS3"
	}
}
