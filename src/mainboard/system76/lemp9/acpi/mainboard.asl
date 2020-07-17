/* SPDX-License-Identifier: GPL-2.0-only */

#define EC_GPE_SCI 0x50 /* GPP_E16 */
#define EC_GPE_SWI 0x29 /* GPP_D9 */
#define EC_COLOR_KEYBOARD 0
#include <ec/system76/ec/acpi/ec.asl>

Scope (\_SB) {
	#include "sleep.asl"
	Scope (PCI0) {
		#include "backlight.asl"
	}
}

Scope (\_GPE) {
	#include "gpe.asl"
}
