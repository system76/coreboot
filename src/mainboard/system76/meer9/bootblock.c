/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pnp_ops.h>
#include <mainboard/gpio.h>

void bootblock_mainboard_early_init(void)
{
	mainboard_configure_early_gpios();

	//TODO: use superio driver
	const pnp_devfn_t dev = PNP_DEV(0x2E, 0x00);

	printk(BIOS_DEBUG, "entering PNP config mode\n");
	outb(0x87, 0x2E);
	outb(0x87, 0x2E);

	printk(BIOS_DEBUG, "configure global PNP\n");
	//TODO: document these
	pnp_write_config(dev, 0x1A, 0x88); // Default is 0x03
	pnp_write_config(dev, 0x1B, 0x00); // Default is 0x03
	pnp_write_config(dev, 0x1D, 0x08); // Default is 0x00
	pnp_write_config(dev, 0x2C, 0x03); // Default is 0x0F
	pnp_write_config(dev, 0x2F, 0xE4); // Default is 0x74

	printk(BIOS_DEBUG, "configure GPIO (logical device 7)\n");
	pnp_write_config(dev, 0x07, 0x07);
	// Enable GPIO 0, 5, and 6
	pnp_write_config(dev, 0x30, 0x61); // Default is 0x00
	// Set GPIO 00-06 as output, 07 as input
	pnp_write_config(dev, 0xE0, 0x80); // Default is 0xFF
	// Set GPIO 00-02 and 04-05 high
	pnp_write_config(dev, 0xE1, 0x37); // Default is 0x00
	// Set GPIO 53-54 as output, 50-52 and 55-57 as input
	pnp_write_config(dev, 0xF8, 0xE7); // Default is 0xFF
	// Set GPIO 53-53 high
	pnp_write_config(dev, 0xF9, 0x18); // Default is 0x00

	printk(BIOS_DEBUG, "configure GPIO (logical device 8)\n");
	pnp_write_config(dev, 0x07, 0x08);
	// GPIO0 multi-function select, set GPIO0 as SUSLED
	pnp_write_config(dev, 0xE0, 0x01); // Default is 0x00
	pnp_write_config(dev, 0xE9, 0x00); // Default is 0xFF TODO?
	pnp_write_config(dev, 0xEA, 0x00); // Default is 0xFF TODO?

	printk(BIOS_DEBUG, "configure GPIO (logical device 9)\n");
	pnp_write_config(dev, 0x07, 0x09);
	// Enable GPIO 8 and 9
	pnp_write_config(dev, 0x30, 0x03); // Default is 0x00
	// GPIO 80-86 set as input, 87 as output
	pnp_write_config(dev, 0xF0, 0x7F); // Default is 0xFF
	// GPIO 87 set high
	pnp_write_config(dev, 0xF1, 0x80); // Default is 0xFF

	printk(BIOS_DEBUG, "configure hardware monitor (logical device B)\n");
	pnp_write_config(dev, 0x07, 0x0B);
	// Enable hardware monitor
	pnp_write_config(dev, 0x30, 0x01); // Default is 0x00
	// Set address (TODO: what is the IO port range?)
	pnp_write_config(dev, 0x60, 0x02);
	pnp_write_config(dev, 0x61, 0x90);

	printk(BIOS_DEBUG, "configure GPIO (logical device F)\n");
	pnp_write_config(dev, 0x07, 0x0F);
	// Set GPIO 00, 01, and 07 as open drain, and 2-6 as push-pull
	pnp_write_config(dev, 0xE0, 0x83); // Default is 0xFF
	// Set GPIO 52-57 as open drain, and 50-51 as push-pull
	pnp_write_config(dev, 0xE5, 0xFC); // Default is 0xFF
	// Set GPIO 60-62 and 65-67 as open drain, and 63-64 as push-pull
	pnp_write_config(dev, 0xE6, 0xE7); // Default is 0xFF
	// Set GPIO 80-86 as open drain, and 87 as push-pull
	pnp_write_config(dev, 0xE8, 0x7F); // Default is 0xFF

	printk(BIOS_DEBUG, "exiting PNP config mode\n");
	outb(0xAA, 0x2E);
}
