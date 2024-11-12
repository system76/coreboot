/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <superio/nuvoton/common/nuvoton.h>

#define SERIAL_DEV PNP_DEV(0x2e, 0x02)

void bootblock_mainboard_early_init(void)
{
	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
