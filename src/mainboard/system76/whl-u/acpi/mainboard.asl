/* SPDX-License-Identifier: GPL-2.0-only */

#define EC_GPE_SCI 0x50 /* GPP_E16 */
#define EC_GPE_SWI 0x29 /* GPP_D9 */

#if defined(CONFIG_BOARD_SYSTEM76_DARP5)
	#define EC_COLOR_KEYBOARD 1
#elif defined(CONFIG_BOARD_SYSTEM76_GALP3_C)
	#define EC_COLOR_KEYBOARD 0
#else
	#error Unknown Mainboard
#endif
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

#include "tbt.asl"
