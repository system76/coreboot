/* SPDX-License-Identifier: GPL-2.0-only */

#if CONFIG(BOARD_SYSTEM76_ORYP5)
#define EC_GPE_SCI 0x17 /* GPP_B23 */
#define EC_GPE_SWI 0x26 /* GPP_G6 */
#else
#define EC_GPE_SCI 0x03 /* GPP_K3 */
#define EC_GPE_SWI 0x06 /* GPP_K6 */
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
