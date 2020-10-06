/* SPDX-License-Identifier: GPL-2.0-only */

#include "../gpio.h"
#include <drivers/system76/dgpu/acpi/dgpu.asl>

#define EC_GPE_SCI 0x17 /* GPP_B23 */
#define EC_GPE_SWI 0x26 /* GPP_G6 */
#define EC_COLOR_KEYBOARD 1
#include <ec/system76/ec/acpi/ec.asl>

Scope (\_SB)
{
	#include "sleep.asl"
	Scope (PCI0)
	{
		#include "backlight.asl"
	}
}

Scope (\_GPE)
{
	#include "gpe.asl"
}
