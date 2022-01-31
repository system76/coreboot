/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <console/console.h>
#if CONFIG(VGA_ROM_RUN)
#include <x86emu/x86emu.h>
#endif
#include <arch/io.h>
#include <arch/interrupt.h>
#include "onboard.h"
#include "ec.h"
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <smbios.h>
#include <ec/google/chromeec/ec.h>

#if CONFIG(VGA_ROM_RUN)
static int int15_handler(void)
{
	int res = 0;

	printk(BIOS_DEBUG, "%s: INT15 function %04x!\n",
			__func__, X86_AX);

	switch (X86_AX) {
	case 0x5f34:
		/*
		 * Set Panel Fitting Hook:
		 *  bit 2 = Graphics Stretching
		 *  bit 1 = Text Stretching
		 *  bit 0 = Centering (do not set with bit1 or bit2)
		 *  0     = video BIOS default
		 */
		X86_AX = 0x005f;
		X86_CL = 0x00; /* Use video BIOS default */
		res = 1;
		break;
	case 0x5f35:
		/*
		 * Boot Display Device Hook:
		 *  bit 0 = CRT
		 *  bit 1 = TV (eDP)
		 *  bit 2 = EFP
		 *  bit 3 = LFP
		 *  bit 4 = CRT2
		 *  bit 5 = TV2 (eDP)
		 *  bit 6 = EFP2
		 *  bit 7 = LFP2
		 */
		X86_AX = 0x005f;
		X86_CX = 0x0000; /* Use video BIOS default */
		res = 1;
		break;
	case 0x5f51:
		/*
		 * Hook to select active LFP configuration:
		 *  00h = No LVDS, VBIOS does not enable LVDS
		 *  01h = Int-LVDS, LFP driven by integrated LVDS decoder
		 *  02h = SVDO-LVDS, LFP driven by SVDO decoder
		 *  03h = eDP, LFP Driven by Int-DisplayPort encoder
		 */
		X86_AX = 0x005f;
		X86_CX = 0x0003; /* eDP */
		res = 1;
		break;
	case 0x5f70:
		switch (X86_CH) {
		case 0:
			/* Get Mux */
			X86_AX = 0x005f;
			X86_CX = 0x0000;
			res = 1;
			break;
		case 1:
			/* Set Mux */
			X86_AX = 0x005f;
			X86_CX = 0x0000;
			res = 1;
			break;
		case 2:
			/* Get SG/Non-SG mode */
			X86_AX = 0x005f;
			X86_CX = 0x0000;
			res = 1;
			break;
		default:
			/* Interrupt was not handled */
			printk(BIOS_DEBUG, "Unknown INT15 5f70 function: 0x%02x\n",
				X86_CH);
			break;
		}
		break;
	case 0x5fac:
		res = 1;
		break;
        default:
		printk(BIOS_DEBUG, "Unknown INT15 function %04x!\n", X86_AX);
		break;
	}
	return res;
}
#endif

static void mainboard_init(struct device *dev)
{
	uint32_t board_version = 0;

	/* Initialize the Embedded Controller */
	link_ec_init();

	google_chromeec_get_board_version(&board_version);
	if (board_version == 0) {
		/* If running on proto1 - enable reversion of gpio11. */
		u32 gpio_inv;
		u16 gpio_base = pci_read_config16
			(pcidev_on_root(0x1f, 0), GPIO_BASE) &
			0xfffc;
		u16 gpio_inv_addr = gpio_base + GPI_INV;
		gpio_inv = inl(gpio_inv_addr);
		outl(gpio_inv | (1 << 11), gpio_inv_addr);
	}
}

static int link_onboard_smbios_data(struct device *dev, int *handle,
				     unsigned long *current)
{
	int len = 0;

	len += smbios_write_type41(
		current, handle,
		BOARD_LIGHTSENSOR_NAME,		/* name */
		BOARD_LIGHTSENSOR_IRQ,		/* instance */
		0,				/* segment */
		BOARD_LIGHTSENSOR_I2C_ADDR,	/* bus */
		0,				/* device */
		0,				/* function */
		SMBIOS_DEVICE_TYPE_OTHER);	/* device type */

	len += smbios_write_type41(
		current, handle,
		BOARD_TRACKPAD_NAME,		/* name */
		BOARD_TRACKPAD_IRQ,		/* instance */
		0,				/* segment */
		BOARD_TRACKPAD_I2C_ADDR,	/* bus */
		0,				/* device */
		0,				/* function */
		SMBIOS_DEVICE_TYPE_OTHER);	/* device type */

	len += smbios_write_type41(
		current, handle,
		BOARD_TOUCHSCREEN_NAME,		/* name */
		BOARD_TOUCHSCREEN_IRQ,		/* instance */
		0,				/* segment */
		BOARD_TOUCHSCREEN_I2C_ADDR,	/* bus */
		0,				/* device */
		0,				/* function */
		SMBIOS_DEVICE_TYPE_OTHER);	/* device type */

	return len;
}

// mainboard_enable is executed as first thing after
// enumerate_buses().

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->get_smbios_data = link_onboard_smbios_data;
#if CONFIG(VGA_ROM_RUN)
	/* Install custom int15 handler for VGA OPROM */
	mainboard_interrupt_handlers(0x15, &int15_handler);
#endif
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
