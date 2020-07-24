/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include "gpio.h"

#define SERIAL_DEV PNP_DEV(0x4e, SIO1036_SP1)

void bootblock_mainboard_early_init(void)
{
	mainboard_program_early_gpios();
}
