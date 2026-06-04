/* SPDX-License-Identifier: GPL-2.0-only */

#define EC_GPE_SCI GPE0_ESPI
#define EC_GPE_SWI GPE0_PME
#include <ec/system76/ec/acpi/ec.asl>

Scope (\_SB) {
	#include "sleep.asl"
	Scope (PCI0) {
#if !CONFIG(EC_SYSTEM76_EC_OLED)
		#include "backlight.asl"
#endif
		/* TODO: Add NVIDIA include */
	}
}
