/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/gpio.h>

#define EC_GPE_SCI 0x17 /* GPP_B23 */
#define EC_GPE_SWI 0x26 /* GPP_G6 */
#include <ec/system76/ec/acpi/ec.asl>

Scope (\_SB)
{
	#include "sleep.asl"
	Scope (PCI0) {
		Device (PEGP) {
			Name (_ADR, CONFIG_DRIVERS_GFX_NVIDIA_BRIDGE << 16)
			#include <drivers/gfx/nvidia/acpi/coffeelake.asl>
		}
	}
}

Scope (\_GPE)
{
	#include "gpe.asl"
}
