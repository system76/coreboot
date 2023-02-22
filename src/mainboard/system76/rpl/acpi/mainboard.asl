/* SPDX-License-Identifier: GPL-2.0-only */

#if CONFIG(DRIVERS_GFX_NVIDIA)
#include <variant/gpio.h>
#endif

#define EC_GPE_SCI 0x6E
#define EC_GPE_SWI 0x6B
#include <ec/system76/ec/acpi/ec.asl>

Scope (\_SB) {
	#include "sleep.asl"
	Scope (PCI0) {
		#include "backlight.asl"

#if CONFIG(DRIVERS_GFX_NVIDIA)
#if CONFIG(SOC_INTEL_ALDERLAKE_PCH_S)
		Scope (PEG1) {
			#include <drivers/gfx/nvidia/acpi/tigerlake.asl>
		}
#else // CONFIG(SOC_INTEL_ALDERLAKE_PCH_S)
		Scope (PEG2) {
			#include <drivers/gfx/nvidia/acpi/tigerlake.asl>
		}
#endif // CONFIG(SOC_INTEL_ALDERLAKE_PCH_S)
#endif // CONFIG(DRIVERS_GFX_NVIDIA)
	}
}
