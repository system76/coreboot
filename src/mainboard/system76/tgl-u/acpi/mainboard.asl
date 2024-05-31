/* SPDX-License-Identifier: GPL-2.0-only */

#if CONFIG(BOARD_SYSTEM76_GALP5)
#include <variant/gpio.h>
#endif

#define EC_GPE_SCI 0x6E
#define EC_GPE_SWI 0x6B
#include <ec/system76/ec/acpi/ec.asl>

Scope (\_SB) {
	#include "sleep.asl"
	Scope (PCI0) {
		#include "backlight.asl"
#if CONFIG(BOARD_SYSTEM76_GALP5)
		Scope (RP01) { // Remapped from RP05
			#include <drivers/gfx/nvidia/acpi/tigerlake.asl>
		}
#endif
	}
}
