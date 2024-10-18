/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/smbus_host.h>
#include <mainboard/gpio.h>
#include <soc/ramstage.h>

static void mainboard_init(void *chip_info)
{
	mainboard_configure_gpios();

	// The DACC feature resets CMOS if the firmware does not send this message
	printk(BIOS_DEBUG, "Handling DACC\n");
	do_smbus_write_byte(CONFIG_FIXED_SMBUS_IO_BASE, 0xBA >> 1, 0x0F, 0xAA);
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};
