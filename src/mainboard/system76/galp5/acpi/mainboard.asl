/* SPDX-License-Identifier: GPL-2.0-only */

#include "../gpio.h"
#if CONFIG(DRIVERS_SYSTEM76_DGPU)
	#include <drivers/system76/dgpu/acpi/dgpu.asl>
#endif

#define EC_GPE_SCI 0x6E
#define EC_GPE_SWI 0x6B
#include <ec/system76/ec/acpi/ec.asl>

Scope (\_SB) {
	/* Method called from _PTS prior to enter sleep state */
	Method (MPTS, 1) {
		\_SB.PCI0.LPCB.EC0.PTS (Arg0)

#if CONFIG(DRIVERS_SYSTEM76_DGPU)
		// Turn DGPU on before sleeping
		\_SB.PCI0.PEGP.DEV0._ON()
#endif
	}

	/* Method called from _WAK prior to wakeup */
	Method (MWAK, 1) {
		\_SB.PCI0.LPCB.EC0.WAK (Arg0)
	}
}
