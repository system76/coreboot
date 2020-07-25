/* SPDX-License-Identifier: GPL-2.0-only */

//TODO
#define EC_GPE_SCI 10 /* GPIO_6 */
#define EC_GPE_SWI 23 /* GPIO_8 */
#define EC_COLOR_KEYBOARD 1
#include <ec/system76/ec/acpi/ec.asl>

Scope (\_SB) {
	#include "sleep.asl"
}
