/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <option.h>
#include <pc80/keyboard.h>
#include <soc/ramstage.h>
#include "gpio.h"

void mainboard_silicon_init_params(FSPS_UPD *supd) {
	/* Configure pads prior to SiliconInit() in case there's any
	 * dependencies during hardware initialization. */
	cnl_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}

static u8 superio_read(u8 reg) {
	outb(reg, 0x2E);
	return inb(0x2F);
}

static void superio_write(u8 reg, u8 value) {
	outb(reg, 0x2E);
	outb(value, 0x2F);
}

static u8 d2_read(u8 reg) {
	superio_write(0x2E, reg);
	return superio_read(0x2F);
}

static void d2_write(u8 reg, u8 value) {
	superio_write(0x2E, reg);
	superio_write(0x2F, value);
}

static u8 i2ec_read(u16 addr) {
	d2_write(0x11, (u8)(addr >> 8));
	d2_write(0x10, (u8)addr);
	return d2_read(0x12);
}

static void i2ec_write(u16 addr, u8 value) {
	d2_write(0x11, (u8)(addr >> 8));
	d2_write(0x10, (u8)addr);
	d2_write(0x12, value);
}

static void mainboard_init(struct device *dev) {
	printk(BIOS_INFO, "system76: keyboard init\n");
	pc_keyboard_init(NO_AUX_DEVICE);

	printk(BIOS_INFO, "system76: EC init\n");

	// Black magic - force enable camera toggle
	u16 addr = 0x01CA;
	u8 value = i2ec_read(addr);
	if ((value & (1 << 2)) == 0) {
		printk(BIOS_INFO, "system76: enabling camera toggle\n");
		i2ec_write(addr, value | (1 << 2));
	} else {
		printk(BIOS_INFO, "system76: camera toggle already enabled\n");
	}
}

static bool mainboard_pcie_hotplug(int port_number) {
	printk(BIOS_DEBUG, "system76: pcie_hotplug(%d)\n", port_number);
	/* RP01 */
	return port_number == 0;
}

static void pcie_hotplug_generator(int port_number)
{
	int port;
	int have_hotplug = 0;

	for (port = 0; port < port_number; port++) {
		if (mainboard_pcie_hotplug(port)) {
			have_hotplug = 1;
		}
	}

	if (!have_hotplug) {
		return;
	}

	for (port = 0; port < port_number; port++) {
		if (mainboard_pcie_hotplug(port)) {
			char scope_name[] = "\\_SB.PCI0.RP0x";
			scope_name[sizeof("\\_SB.PCI0.RP0x") - 2] = '1' + port;
			acpigen_write_scope(scope_name);

			/*
			  Device (SLOT)
			  {
				Name (_ADR, 0x00)
				Method (_RMV, 0, NotSerialized)
				{
					Return (0x01)
				}
			  }
			*/

			acpigen_write_device("SLOT");

			acpigen_write_name_byte("_ADR", 0x00);

			acpigen_write_method("_RMV", 0);
			/* ReturnOp  */
			acpigen_emit_byte (0xa4);
			/* One  */
			acpigen_emit_byte (0x01);
			acpigen_pop_len();
			acpigen_pop_len();
			acpigen_pop_len();
		}
	}

	/* Method (_L01, 0, NotSerialized)
	{
		If (\_SB.PCI0.RP04.HPCS)
		{
			Sleep (100)
			Store (0x01, \_SB.PCI0.RP04.HPCS)
			If (\_SB.PCI0.RP04.PDC)
			{
				Store (0x01, \_SB.PCI0.RP04.PDC)
				Notify (\_SB.PCI0.RP04, 0x00)
			}
		}
	}

	*/
	acpigen_write_scope("\\_GPE");
	acpigen_write_method("_L01", 0);
	for (port = 0; port < port_number; port++) {
		if (mainboard_pcie_hotplug(port)) {
			char reg_name[] = "\\_SB.PCI0.RP0x.HPCS";
			reg_name[sizeof("\\_SB.PCI0.RP0x") - 2] = '1' + port;
			acpigen_emit_byte(0xa0); /* IfOp. */
			acpigen_write_len_f();
			acpigen_emit_namestring(reg_name);

			/* Sleep (100) */
			acpigen_emit_byte(0x5b); /* SleepOp. */
			acpigen_emit_byte(0x22);
			acpigen_write_byte(100);

			/* Store (0x01, \_SB.PCI0.RP04.HPCS) */
			acpigen_emit_byte(0x70);
			acpigen_emit_byte(0x01);
			acpigen_emit_namestring(reg_name);

			memcpy(reg_name + sizeof("\\_SB.PCI0.RP0x.") - 1, "PDC", 4);

			/* If (\_SB.PCI0.RP04.PDC) */
			acpigen_emit_byte(0xa0); /* IfOp. */
			acpigen_write_len_f();
			acpigen_emit_namestring(reg_name);

			/* Store (0x01, \_SB.PCI0.RP04.PDC) */
			acpigen_emit_byte(0x70);
			acpigen_emit_byte(0x01);
			acpigen_emit_namestring(reg_name);

			reg_name[sizeof("\\_SB.PCI0.RP0x") - 1] = '\0';

			/* Notify(\_SB.PCI0.RP04, 0x00) */
			acpigen_emit_byte(0x86);
			acpigen_emit_namestring(reg_name);
			acpigen_emit_byte(0x00);
			acpigen_pop_len();
			acpigen_pop_len();
		}
	}
	acpigen_pop_len();
	acpigen_pop_len();
}

static void fill_ssdt(const struct device *device) {
	printk(BIOS_INFO, "system76: fill_ssdt\n");
	pcie_hotplug_generator(CONFIG_MAX_ROOT_PORTS);
}

static void mainboard_enable(struct device *dev) {
	dev->ops->init = mainboard_init;
	dev->ops->acpi_fill_ssdt = fill_ssdt;

	// Configure pad for DisplayPort
	uint32_t config = 0x44000200;

	uint8_t nvram = 0;
	if (get_option(&nvram, "DisplayPort_Output") == CB_SUCCESS) {
		if (nvram) {
			config |= 1;
		}
	}

	if (nvram) {
		printk(BIOS_INFO, "system76: DisplayPort_Output set to USB-C: 0x%x\n", config);
	} else {
		printk(BIOS_INFO, "system76: DisplayPort_Output set to Mini_DisplayPort: 0x%x\n", config);
	}

	struct pad_config displayport_gpio_table[] = {
		/* PS8338B_SW */
		_PAD_CFG_STRUCT(GPP_A22, config, 0x0),
	};
	gpio_configure_pads(displayport_gpio_table, ARRAY_SIZE(displayport_gpio_table));
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
