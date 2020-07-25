/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/lpc.h>
#include <bootblock_common.h>
#include "gpio.h"

void bootblock_mainboard_early_init(void)
{
	// Map EC memory
	lpc_set_wideio_range(0xE00, 0x200);

	mainboard_program_early_gpios();
}
