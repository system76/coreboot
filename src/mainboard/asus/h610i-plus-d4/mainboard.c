/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <device/device.h>
#include <identity.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <smbios.h>
#include <string.h>
#include <bootstate.h>
#include <superio/nuvoton/common/hwm.h>

/* HWM base address, set in devicetree.cb. */
#define HWM_IOBASE		0x290

/* Alder Lake desktop Tjmax in degrees Celsius. */
#define CPU_TJMAX		100

/* Per-fan register banks. Only SYSFAN and CPUFAN are wired up. */
#define BANK_SYSFAN		1
#define BANK_CPUFAN		2

/* Fan profiles tuned for an i7-12700. */
static const struct nuvoton_fan_curve fans[] = {
	{
		.name			= "SYSFAN",
		.bank			= BANK_SYSFAN,
		.source			= NUVOTON_FAN_SOURCE_PECI0,
		.temp			= { 40, 60, 75, 90 },
		.duty			= {
			NUVOTON_PERCENT_TO_DUTY(20),
			NUVOTON_PERCENT_TO_DUTY(40),
			NUVOTON_PERCENT_TO_DUTY(70),
			NUVOTON_PERCENT_TO_DUTY(100),
		},
		.crit_temp		= 100,
		.crit_duty_en		= 1,
		.crit_duty		= 255,
		.crit_temp_tolerance	= 2,
	},
	{
		.name			= "CPUFAN",
		.bank			= BANK_CPUFAN,
		.source			= NUVOTON_FAN_SOURCE_PECI0,
		.temp			= { 40, 60, 75, 90 },
		.duty			= {
			NUVOTON_PERCENT_TO_DUTY(20),
			NUVOTON_PERCENT_TO_DUTY(40),
			NUVOTON_PERCENT_TO_DUTY(70),
			NUVOTON_PERCENT_TO_DUTY(100),
		},
		.crit_temp		= 100,
		.crit_duty_en		= 1,
		.crit_duty		= 255,
		.crit_temp_tolerance	= 2,
	},
};

void mainboard_fill_fadt(acpi_fadt_t *fadt)
{
	fadt->preferred_pm_profile = PM_DESKTOP;
	fadt->iapc_boot_arch |= ACPI_FADT_LEGACY_DEVICES | ACPI_FADT_8042;
}

static void mainboard_init(void *chip_info)
{

}

static void mainboard_enable(struct device *dev)
{

}

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	uint8_t aspm, aspm_l1;

	/* ASPM L1 sub-states require CLKREQ, so CLK_PM should be enabled as well */
	if (CONFIG(PCIEXP_L1_SUB_STATE) && CONFIG(PCIEXP_CLK_PM))
		aspm_l1 = 2; // 2 - L1.1 and L1.2
	else
		aspm_l1 = 0;

	if (CONFIG(PCIEXP_ASPM)) {
		aspm = CONFIG(PCIEXP_L1_SUB_STATE) ? 3 : 1; // 3 - L0sL1, 1 - L0s
	} else {
		aspm = 0;
		aspm_l1 = 0;
	}

	memset(params->PcieRpEnableCpm, 0, sizeof(params->PcieRpEnableCpm));
	memset(params->CpuPcieRpEnableCpm, 0, sizeof(params->CpuPcieRpEnableCpm));
	memset(params->CpuPcieClockGating, 0, sizeof(params->CpuPcieClockGating));
	memset(params->CpuPciePowerGating, 0, sizeof(params->CpuPciePowerGating));

	params->UsbPdoProgramming = 1;

	params->CpuPcieFiaProgramming = 1;

	params->PcieRpFunctionSwap = 0;
	params->CpuPcieRpFunctionSwap = 0;

	params->PchLegacyIoLowLatency = 1;
	params->PchDmiAspmCtrl = 0;

	params->CpuPcieRpPmSci[2] = 1; // PCI_E1
	params->PcieRpPmSci[1]    = 1; // M2_1
	params->PcieRpPmSci[5]    = 1; // Ethernet
	params->PcieRpPmSci[6]    = 1; // WiFi

	params->PcieRpMaxPayload[0]  = 1; // PCI_E2
	params->PcieRpMaxPayload[1]  = 1; // M2_1
	params->PcieRpMaxPayload[5]  = 1; // Ethernet
	params->PcieRpMaxPayload[6]  = 1; // WiFi

	params->CpuPcieRpTransmitterHalfSwing[2] = 1; // PCI_E1
	params->PcieRpTransmitterHalfSwing[1]    = 1; // M2_1
	params->PcieRpTransmitterHalfSwing[5]    = 1; // Ethernet
	params->PcieRpTransmitterHalfSwing[6]    = 1; // WiFi

	params->CpuPcieRpEnableCpm[2] = CONFIG(PCIEXP_CLK_PM); // PCI_E1
	params->PcieRpEnableCpm[1]    = CONFIG(PCIEXP_CLK_PM); // M2_1
	params->PcieRpEnableCpm[5]    = CONFIG(PCIEXP_CLK_PM); // Ethernet
	params->PcieRpEnableCpm[6]    = CONFIG(PCIEXP_CLK_PM); // WiFi

	params->CpuPcieRpL1Substates[2] = aspm_l1; // PCI_E1
	params->PcieRpL1Substates[1]    = aspm_l1; // M2_1
	params->PcieRpL1Substates[5]    = aspm_l1; // Ethernet
	params->PcieRpL1Substates[6]    = aspm_l1; // WiFi

	params->CpuPcieRpAspm[2] = aspm; // PCI_E1
	params->PcieRpAspm[1]    = aspm; // M2_1
	params->PcieRpAspm[5]    = aspm; // Ethernet
	params->PcieRpAspm[6]    = aspm; // WiFi

	params->PcieRpAcsEnabled[1]  = 1; // M2_1
	params->PcieRpAcsEnabled[5]  = 1; // Ethernet
	params->PcieRpAcsEnabled[6]  = 1; // WiFi

	params->CpuPcieClockGating[2] = CONFIG(PCIEXP_CLK_PM);
	params->CpuPciePowerGating[2] = CONFIG(PCIEXP_CLK_PM);
	params->CpuPcieRpPeerToPeerMode[2] = 1;
	params->CpuPcieRpMaxPayload[2] = 2; // 512B
	params->CpuPcieRpAcsEnabled[2] = 1;

	params->SataLedEnable = 1;
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};

static void hwm_init(void *arg)
{
	/* Devicetree must set superio pin 121 to PECI mode first. */
	nuvoton_hwm_enable_peci(HWM_IOBASE, CPU_TJMAX);
	nuvoton_hwm_enable_peci_calibration(HWM_IOBASE);

	for (size_t i = 0; i < ARRAY_SIZE(fans); i++)
		nuvoton_hwm_configure_fan(HWM_IOBASE, &fans[i]);
}

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_EXIT, hwm_init, NULL);
