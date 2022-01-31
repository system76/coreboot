/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci_ops.h>
#include <device/smbus_host.h>
#include <southbridge/intel/common/gpio.h>
#include <southbridge/intel/common/pmbase.h>
#include "i82801gx.h"
#include "chip.h"

void i82801gx_lpc_setup(void)
{
	const pci_devfn_t d31f0 = PCI_DEV(0, 0x1f, 0);
	const struct device *dev = pcidev_on_root(0x1f, 0);
	const struct southbridge_intel_i82801gx_config *config;

	/* Configure serial IRQs.*/
	pci_write_config8(d31f0, SERIRQ_CNTL, 0xd0);
	/*
	 * Enable some common LPC IO ranges:
	 * - 0x2e/0x2f, 0x4e/0x4f often SuperIO
	 * - 0x60/0x64, 0x62/0x66 often KBC/EC
	 * - 0x3f0-0x3f5/0x3f7 FDD
	 * - 0x378-0x37f and 0x778-0x77f LPT
	 * - 0x2f8-0x2ff COMB
	 * - 0x3f8-0x3ff COMA
	 * - 0x208-0x20f GAMEH
	 * - 0x200-0x207 GAMEL
	 */
	pci_write_config16(d31f0, LPC_IO_DEC, 0x0010);
	pci_write_config16(d31f0, LPC_EN, CNF2_LPC_EN | CNF1_LPC_EN
			   | MC_LPC_EN | KBC_LPC_EN | GAMEH_LPC_EN
			   | GAMEL_LPC_EN | FDD_LPC_EN | LPT_LPC_EN
			   | COMB_LPC_EN | COMA_LPC_EN);

	/* Set up generic decode ranges */
	if (!dev || !dev->chip_info)
		return;
	config = dev->chip_info;

	pci_write_config32(d31f0, GEN1_DEC, config->gen1_dec);
	pci_write_config32(d31f0, GEN2_DEC, config->gen2_dec);
	pci_write_config32(d31f0, GEN3_DEC, config->gen3_dec);
	pci_write_config32(d31f0, GEN4_DEC, config->gen4_dec);
}

void i82801gx_setup_bars(void)
{
	const pci_devfn_t d31f0 = PCI_DEV(0, 0x1f, 0);
	pci_write_config32(d31f0, RCBA, CONFIG_FIXED_RCBA_MMIO_BASE | 1);
	pci_write_config32(d31f0, PMBASE, DEFAULT_PMBASE | 1);
	pci_write_config8(d31f0, ACPI_CNTL, ACPI_EN);

	pci_write_config32(d31f0, GPIOBASE, DEFAULT_GPIOBASE | 1);
	pci_write_config8(d31f0, GPIO_CNTL, GPIO_EN);
}

#define TCO_BASE 0x60

#if ENV_ROMSTAGE
void i82801gx_early_init(void)
{
	enable_smbus();

	printk(BIOS_DEBUG, "Setting up static southbridge registers...");
	i82801gx_setup_bars();

	setup_pch_gpios(&mainboard_gpio_map);
	printk(BIOS_DEBUG, " done.\n");

	printk(BIOS_DEBUG, "Disabling Watchdog reboot...");
	RCBA32(GCS) = RCBA32(GCS) | (1 << 5);	/* No reset */
	write_pmbase16(TCO_BASE + 0x8, (1 << 11));	/* halt timer */
	write_pmbase16(TCO_BASE + 0x4, (1 << 3));	/* clear timeout */
	write_pmbase16(TCO_BASE + 0x6, (1 << 1));	/* clear 2nd timeout */
	printk(BIOS_DEBUG, " done.\n");

	/* program secondary mlt XXX byte? */
	pci_write_config8(PCI_DEV(0, 0x1e, 0), SMLT, 0x20);

	/* reset rtc power status */
	pci_and_config8(PCI_DEV(0, 0x1f, 0), GEN_PMCON_3, ~RTC_BATTERY_DEAD);

	/* USB transient disconnect */
	pci_or_config8(PCI_DEV(0, 0x1f, 0), 0xad, 3 << 0);

	pci_or_config32(PCI_DEV(0, 0x1d, 7), 0xfc, (1 << 29) | (1 << 17));

	pci_or_config32(PCI_DEV(0, 0x1d, 7), 0xdc, (1 << 31) | (1 << 27));

	/* Enable IOAPIC */
	RCBA8(OIC) = 0x03;
	RCBA8(OIC);

	/* A lot of CIR bits relate DMI setup which is likely not correctly
	   done for x4x. The issue is also present on ICH10. */
	if (!CONFIG(NORTHBRIDGE_INTEL_X4X))
		ich7_setup_cir();
}
#endif
