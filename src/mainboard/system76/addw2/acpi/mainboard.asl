/* SPDX-License-Identifier: GPL-2.0-only */

#define EC_GPE_SCI 0x03 /* GPP_K3 */
#define EC_GPE_SWI 0x06 /* GPP_K6 */
#define EC_COLOR_KEYBOARD 1
#include <ec/system76/acpi/ec.asl>

Scope (\_SB) {
	#include "sleep.asl"
	Scope (PCI0) {
		#include "pegp.asl"
		#include "dgpu.asl"
	}
}

Scope (\_GPE) {
	#include "gpe.asl"
}
