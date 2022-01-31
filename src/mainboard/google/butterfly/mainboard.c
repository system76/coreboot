/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <cbfs.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <drivers/intel/gma/int15.h>
#include <fmap.h>
#include <arch/io.h>
#include "onboard.h"
#include "ec.h"
#include <southbridge/intel/bd82x6x/pch.h>
#include <smbios.h>
#include <ec/quanta/ene_kb3940q/ec.h>

static unsigned int search(char *p, char *a, unsigned int lengthp,
			   unsigned int lengtha)
{
	int i, j;

	/* Searching */
	for (j = 0; j <= lengtha - lengthp; j++) {
		for (i = 0; i < lengthp && p[i] == a[i + j]; i++)
			;
		if (i >= lengthp)
			return j;
	}
	return lengtha;
}

static unsigned char get_hex_digit(char *offset)
{
	unsigned char retval = 0;

	retval = *offset - '0';
	if (retval > 0x09) {
		retval = *offset - 'A' + 0x0A;
		if (retval > 0x0F)
			retval = *offset - 'a' + 0x0a;
	}
	if (retval > 0x0F) {
		printk(BIOS_DEBUG, "Error: Invalid Hex digit found: %c - 0x%02x\n",
			*offset, (unsigned char)*offset);
		retval = 0;
	}

	return retval;
}

static int get_mac_address(u32 *high_dword, u32 *low_dword,
			   u32 search_address, u32 search_length)
{
	char key[] = "ethernet_mac";
	unsigned int offset;
	int i;

	offset = search(key, (char *)search_address,
			sizeof(key) - 1, search_length);
	if (offset == search_length) {
		printk(BIOS_DEBUG,
		       "Error: Could not locate '%s' in VPD\n", key);
		return 0;
	}
	printk(BIOS_DEBUG, "Located '%s' in VPD\n", key);

	offset += sizeof(key);	/* move to next character */
	*high_dword = 0;

	/* Fetch the MAC address and put the octets in the correct order to
	 * be programmed.
	 *
	 * From RTL8105E_Series_EEPROM-Less_App_Note_1.1
	 * If the MAC address is 001122334455h:
	 * Write 33221100h to I/O register offset 0x00 via double word access
	 * Write 00005544h to I/O register offset 0x04 via double word access
	 */

	for (i = 0; i < 4; i++) {
		*high_dword |= (get_hex_digit((char *)(search_address + offset))
				<< (4 + (i * 8)));
		*high_dword |= (get_hex_digit((char *)(search_address + offset + 1))
				<< (i * 8));
		offset += 3;
	}

	*low_dword = 0;
	for (i = 0; i < 2; i++) {
		*low_dword |= (get_hex_digit((char *)(search_address + offset))
			       << (4 + (i * 8)));
		*low_dword |= (get_hex_digit((char *)(search_address + offset + 1))
			       << (i * 8));
		offset += 3;
	}

	return *high_dword | *low_dword;
}

static void program_mac_address(u16 io_base, u32 search_address,
				u32 search_length)
{
	/* Default MAC Address of A0:00:BA:D0:0B:AD */
	u32 high_dword = 0xD0BA00A0;	/* high dword of mac address */
	u32 low_dword = 0x0000AD0B;	/* low word of mac address as a dword */

	if (search_length != -1)
		get_mac_address(&high_dword, &low_dword, search_address,
				search_length);

	if (io_base) {
		printk(BIOS_DEBUG, "Realtek NIC io_base = 0x%04x\n", io_base);
		printk(BIOS_DEBUG, "Programming MAC Address\n");

		outb(0xc0, io_base + 0x50);	/* Disable register protection */
		outl(high_dword, io_base);
		outl(low_dword, io_base + 0x04);
		outb(0x60, io_base + 54);
		outb(0x00, io_base + 0x50);	/* Enable register protection again */
	}
}

static void program_keyboard_type(u32 search_address, u32 search_length)
{
	char key[] = "keyboard_layout";
	char kbd_jpn[] = "xkb:jp::jpn";
	unsigned int offset;
	char kbd_type = EC_KBD_EN;	/* Default keyboard type is English */

	if (search_length != -1) {

		/*
		 * Search for keyboard_layout identifier
		 * The only options in the EC are Japanese or English.
		 * The English keyboard layout is actually used for multiple
		 * different languages - English, Spanish, French...  Because
		 * of this the code only searches for Japanese, and sets the
		 * keyboard type to English if Japanese is not found.
		 */
		offset = search(key, (char *)search_address, sizeof(key) - 1,
				search_length);
		if (offset != search_length) {
			printk(BIOS_DEBUG, "Located '%s' in VPD\n", key);

			offset += sizeof(key);	/* move to next character */
			search_length = sizeof(kbd_jpn);
			offset = search(kbd_jpn, (char *)(search_address + offset),
					sizeof(kbd_jpn) - 1, search_length);
			if (offset != search_length)
				kbd_type = EC_KBD_JP;
		}
	} else {
		printk(BIOS_DEBUG, "Error: Could not locate VPD area\n");
	}

	printk(BIOS_DEBUG, "Setting Keyboard type in EC to ");
	printk(BIOS_DEBUG, (kbd_type == EC_KBD_JP) ? "Japanese" : "English");
	printk(BIOS_DEBUG, ".\n");

	ec_mem_write(EC_KBID_REG, kbd_type);
}

static void mainboard_init(struct device *dev)
{
	u32 search_address = 0x0;
	size_t search_length = -1;
	u16 io_base = 0;
	struct device *ethernet_dev = NULL;
	void *vpd_file;

	if (CONFIG(VPD)) {
		struct region_device rdev;

		if (fmap_locate_area_as_rdev("RO_VPD", &rdev) == 0) {
			vpd_file = rdev_mmap_full(&rdev);

			if (vpd_file != NULL) {
				search_length = region_device_sz(&rdev);
				search_address = (uintptr_t)vpd_file;
			}
		}
	} else {
		vpd_file = cbfs_map("vpd.bin", &search_length);
		if (vpd_file) {
			search_address = (unsigned long)vpd_file;
		} else {
			search_length = -1;
			search_address = 0;
		}
	}

	/* Initialize the Embedded Controller */
	butterfly_ec_init();

	/* Program EC Keyboard locale based on VPD data */
	program_keyboard_type(search_address, search_length);

	/* Get NIC's IO base address */
	ethernet_dev = dev_find_device(BUTTERFLY_NIC_VENDOR_ID,
				       BUTTERFLY_NIC_DEVICE_ID, dev);
	if (ethernet_dev != NULL) {
		io_base = pci_read_config16(ethernet_dev, 0x10) & 0xfffe;

		/*
		 * Battery life time - LAN PCIe should enter ASPM L1 to save
		 * power when LAN connection is idle.
		 * enable CLKREQ: LAN pci config space 0x81h=01
		 */
		pci_write_config8(ethernet_dev, 0x81, 0x01);
	}

	if (io_base) {
		/* Program MAC address based on VPD data */
		program_mac_address(io_base, search_address, search_length);

		/*
		 * Program NIC LEDS
		 *
		 * RTL8105E Series EEPROM-Less Application Note,
		 * Section 5.6 LED Mode Configuration
		 *
		 * Step1: Write C0h to I/O register 0x50 via byte access to
		 *        disable 'register protection'
		 * Step2: Write xx001111b to I/O register 0x52 via byte access
		 *        (bit7 is LEDS1 and bit6 is LEDS0)
		 * Step3: Write 0x00 to I/O register 0x50 via byte access to
		 *        enable 'register protection'
		 */
		outb(0xc0, io_base + 0x50);	/* Disable protection */
		outb((BUTTERFLY_NIC_LED_MODE << 6) | 0x0f, io_base + 0x52);
		outb(0x00, io_base + 0x50);	/* Enable register protection */
	}
}

static int butterfly_onboard_smbios_data(struct device *dev, int *handle,
					 unsigned long *current)
{
	int len = 0;

	len += smbios_write_type41(
		current, handle,
		BOARD_TRACKPAD_NAME,		/* name */
		BOARD_TRACKPAD_IRQ,		/* instance */
		0,				/* segment */
		BOARD_TRACKPAD_I2C_ADDR,	/* bus */
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
	dev->ops->get_smbios_data = butterfly_onboard_smbios_data;
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_INT_LVDS, GMA_INT15_PANEL_FIT_DEFAULT, GMA_INT15_BOOT_DISPLAY_DEFAULT, 0);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
