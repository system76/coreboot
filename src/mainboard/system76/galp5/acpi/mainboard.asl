/* SPDX-License-Identifier: GPL-2.0-only */

#include "../gpio.h"
//TODO #include <drivers/system76/dgpu/acpi/dgpu.asl>

#define EC_GPE_SCI 0x6E // TODO
#define EC_GPE_SWI 0x6B // TODO
#include <ec/system76/ec/acpi/ec.asl>

Scope (\_SB) {
	#include "sleep.asl"
	Scope (PCI0) {
		#include "backlight.asl"
	}
}
