/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <amdblocks/lpc.h>
#include <console/console.h>
#include "gpio.h"

void bootblock_mainboard_early_init(void)
{
	post_code(0x76);
	post_code(0x01);

	mainboard_program_early_gpios();

	post_code(0x76);
	post_code(0x02);

	// AP/EC command
	lpc_set_wideio_range(0xE00, 256);

	// AP/EC debug
	lpc_set_wideio_range(0xF00, 256);

	post_code(0x76);
	post_code(0x03);
}

void bootblock_mainboard_init(void)
{
	post_code(0x76);
	post_code(0x10);
}
