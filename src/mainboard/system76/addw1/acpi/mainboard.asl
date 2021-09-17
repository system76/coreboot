/* SPDX-License-Identifier: GPL-2.0-only */

#include <variant/gpio.h>
#include <drivers/gfx/nvidia/acpi/gpu.asl>

#define EC_GPE_SCI 0x03 /* GPP_K3 */
#define EC_GPE_SWI 0x06 /* GPP_K6 */
#include <ec/system76/ec/acpi/ec.asl>

Scope (\_SB) {
	#include "sleep.asl"
}

Scope (\_GPE) {
	#include "gpe.asl"
}
