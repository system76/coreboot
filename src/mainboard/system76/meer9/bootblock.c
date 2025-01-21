/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pnp_ops.h>
#include <mainboard/gpio.h>

// nuvoton_pnp_enter_conf_state
static void pnp_enter_conf_state(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(0x87, port);
	outb(0x87, port);
}

// nuvoton_pnp_exit_conf_state
static void pnp_exit_conf_state(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(0xaa, port);
}

static void superio_init(void)
{
	//TODO: use superio driver?
	pnp_devfn_t dev = PNP_DEV(0x2E, 0x00);

	printk(BIOS_DEBUG, "entering PNP config mode\n");
	pnp_enter_conf_state(dev);

	printk(BIOS_DEBUG, "configure global PNP\n");
	//TODO: document these
	pnp_write_config(dev, 0x1A, 0x88); // Default is 0x03
	pnp_write_config(dev, 0x1B, 0x00); // Default is 0x03
	pnp_write_config(dev, 0x1D, 0x08); // Default is 0x00
	pnp_write_config(dev, 0x2C, 0x03); // Default is 0x0F
	pnp_write_config(dev, 0x2F, 0xE4); // Default is 0x74

	printk(BIOS_DEBUG, "configure GPIO (logical device 7)\n");
	dev = PNP_DEV(0x2E, 0x07);
	pnp_set_logical_device(dev);
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
	dev = PNP_DEV(0x2E, 0x08);
	pnp_set_logical_device(dev);
	// Disable WDT1
	pnp_write_config(dev, 0x30, 0x00); // Default is 0x01
	// GPIO0 multi-function select, set GPIO0 as SUSLED
	pnp_write_config(dev, 0xE0, 0x01); // Default is 0x00
	pnp_write_config(dev, 0xE9, 0x00); // Default is 0xFF TODO?
	pnp_write_config(dev, 0xEA, 0x00); // Default is 0xFF TODO?

	printk(BIOS_DEBUG, "configure GPIO (logical device 9)\n");
	dev = PNP_DEV(0x2E, 0x09);
	pnp_set_logical_device(dev);
	// Enable GPIO 8 and 9
	pnp_write_config(dev, 0x30, 0x03); // Default is 0x00
	// GPIO 80-86 set as input, 87 as output
	pnp_write_config(dev, 0xF0, 0x7F); // Default is 0xFF
	// GPIO 87 set high
	pnp_write_config(dev, 0xF1, 0x80); // Default is 0xFF

	printk(BIOS_DEBUG, "configure ACPI (logical device A)\n");
	dev = PNP_DEV(0x2E, 0x0A);
	pnp_set_logical_device(dev);
	// User-defined resume state after power loss
	pnp_write_config(dev, 0xE4, 0x60); // Default is 0x00

	printk(BIOS_DEBUG, "configure hardware monitor (logical device B)\n");
	dev = PNP_DEV(0x2E, 0x0B);
	pnp_set_logical_device(dev);
	// Enable hardware monitor
	pnp_write_config(dev, 0x30, 0x01); // Default is 0x00
	// Set address base to 0x290
	pnp_write_config(dev, 0x60, 0x02);
	pnp_write_config(dev, 0x61, 0x90);

	printk(BIOS_DEBUG, "configure GPIO (logical device F)\n");
	dev = PNP_DEV(0x2E, 0x0F);
	pnp_set_logical_device(dev);
	// Set GPIO 00, 01, and 07 as open drain, and 2-6 as push-pull
	pnp_write_config(dev, 0xE0, 0x83); // Default is 0xFF
	// Set GPIO 52-57 as open drain, and 50-51 as push-pull
	pnp_write_config(dev, 0xE5, 0xFC); // Default is 0xFF
	// Set GPIO 60-62 and 65-67 as open drain, and 63-64 as push-pull
	pnp_write_config(dev, 0xE6, 0xE7); // Default is 0xFF
	// Set GPIO 80-86 as open drain, and 87 as push-pull
	pnp_write_config(dev, 0xE8, 0x7F); // Default is 0xFF

	printk(BIOS_DEBUG, "configure fading LED (logical device 15)\n");
	dev = PNP_DEV(0x2E, 0x15);
	pnp_set_logical_device(dev);
	// Configure fading LED (divide by 4, frequency 1 Khz, off)
	pnp_write_config(dev, 0xE5, 0x42);

	printk(BIOS_DEBUG, "configure deep sleep (logical device 16)\n");
	dev = PNP_DEV(0x2E, 0x16);
	pnp_set_logical_device(dev);
	// Set deep sleep delay time to 0s
	pnp_write_config(dev, 0xE2, 0x00);

	printk(BIOS_DEBUG, "exiting PNP config mode\n");
	pnp_exit_conf_state(dev);
}

static void hm_write(uint8_t reg, uint8_t value)
{
	outb(reg, 0x295);
	outb(value, 0x296);
}

static void hm_init(void)
{
	// Bank 2
	hm_write(0x4E, 0x82);

	// Enable PECI 3.0 with routine function
	hm_write(0x00, 0x85);

	// Enable PECI agent 30
	hm_write(0x02, 0x10);

	// PECI Tbase0 = 110C
	hm_write(0x04, 110);

	// Bank 3
	hm_write(0x4E, 0x83);

	// Enable PECI agent 0 mode
	hm_write(0x90, 0x01);

	// Bank 1
	hm_write(0x4E, 0x81);

	// CPUFAN T1 = 50C
	hm_write(0x70, 50);
	// CPUFAN FD1 = 25% = 0x3F
	hm_write(0x74, 0x3F);

	// CPUFAN T2 = 70C
	hm_write(0x71, 70);
	// CPUFAN FD2 = 50% = 0x7F
	hm_write(0x75, 0x7F);

	// CPUFAN T3 = 80C
	hm_write(0x72, 80);
	// CPUFAN FD3 = 65% = 0xA5
	hm_write(0x76, 0xA5);

	// CPUFAN T4 = 90C
	hm_write(0x73, 90);
	// CPUFAN FD4 = 85% = 0xD8
	hm_write(0x77, 0xD8);

	// CPUFAN critical temperature = 95C
	hm_write(0x2A, 95);
	// By default critical duty is 0xFF

	// CPUFAN step up time = 1s
	hm_write(0x24, 10);

	// CPUFAN step down time = 0.5s
	hm_write(0x25, 5);

	// Use PECI agent 0 as CPUFAN monitoring source
	hm_write(0x20, 0b01100);

	// CPUFAN Smart Fan IV mode
	hm_write(0x23, 0x40);
}

void bootblock_mainboard_early_init(void)
{
	mainboard_configure_early_gpios();
	superio_init();
	hm_init();
}
