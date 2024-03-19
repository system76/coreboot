/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <bootmode.h>
#include <bootsplash.h>
#include <cbfs.h>
#include <console/console.h>
#include <cpu/intel/cpu_ids.h>
#include <cpu/intel/microcode.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/api.h>
#include <fsp/fsp_debug_event.h>
#include <fsp/fsp_gop_blt.h>
#include <fsp/ppi/mp_service_ppi.h>
#include <fsp/util.h>
#include <option.h>
#include <intelblocks/cse.h>
#include <intelblocks/irq.h>
#include <intelblocks/lpss.h>
#include <intelblocks/mp_init.h>
#include <intelblocks/systemagent.h>
#include <intelblocks/xdci.h>
#include <intelpch/lockdown.h>
#include <security/vboot/vboot_common.h>
#include <soc/cpu.h>
#include <soc/gpio_soc_defs.h>
#include <soc/intel/common/vbt.h>
#include <soc/pci_devs.h>
#include <soc/pcie.h>
#include <soc/ramstage.h>
#include <soc/soc_chip.h>
#include <soc/soc_info.h>
#include <stdlib.h>
#include <string.h>
#include <types.h>

/* THC assignment definition */
#define THC_NONE	0
#define THC_0		1
#define THC_1		2

/* SATA DEVSLP idle timeout default values */
#define DEF_DMVAL	15
#define DEF_DITOVAL	625

#define MAX_ONBOARD_PCIE_DEVICES 256

static const struct slot_irq_constraints irq_constraints[] = {
	{
		.slot = PCI_DEV_SLOT_PCIE_3,
		.fns = {
			FIXED_INT_PIRQ(PCI_DEVFN_PCIE12, PCI_INT_A, PIRQ_A),
		},
	},
	{
		.slot = PCI_DEV_SLOT_IGD,
		.fns = {
			/* INTERRUPT_PIN is RO/0x01 */
			FIXED_INT_ANY_PIRQ(PCI_DEV_SLOT_IGD, PCI_INT_A),
		},
	},
	{
		.slot = PCI_DEV_SLOT_DPTF,
		.fns = {
			/* Dynamic Tuning Technology (DTT) device IRQ is not
			   programmable and is INT_A/PIRQ_A (IRQ 16) */
			FIXED_INT_PIRQ(PCI_DEVFN_DPTF, PCI_INT_A, PIRQ_A),
		},
	},
	{
		.slot = PCI_DEV_SLOT_IPU,
		.fns = {
			/* INTERRUPT_PIN is RO/0x01, and INTERRUPT_LINE is RW,
			   but S0ix fails when not set to 16 (b/193434192) */
			FIXED_INT_PIRQ(PCI_DEVFN_IPU, PCI_INT_A, PIRQ_A),
		},
	},
	{
		.slot = PCI_DEV_SLOT_PCIE_2,
		.fns = {
			FIXED_INT_PIRQ(PCI_DEVFN_PCIE9, PCI_INT_A, PIRQ_A),
			FIXED_INT_PIRQ(PCI_DEVFN_PCIE10, PCI_INT_B, PIRQ_B),
			FIXED_INT_PIRQ(PCI_DEVFN_PCIE11, PCI_INT_C, PIRQ_C),
		},
	},
	{
		.slot = PCI_DEV_SLOT_TBT,
		.fns = {
			ANY_PIRQ(PCI_DEVFN_TBT0),
			ANY_PIRQ(PCI_DEVFN_TBT1),
			ANY_PIRQ(PCI_DEVFN_TBT2),
			ANY_PIRQ(PCI_DEVFN_TBT3),
		},
	},
	{
		.slot = PCI_DEV_SLOT_GNA,
		.fns = {
			/* INTERRUPT_PIN is RO/0x01 */
			FIXED_INT_ANY_PIRQ(PCI_DEVFN_GNA, PCI_INT_A),
		},
	},
	{
		.slot = PCI_DEV_SLOT_VPU,
		.fns = {
			/* INTERRUPT_PIN is RO/0x01 */
			FIXED_INT_ANY_PIRQ(PCI_DEVFN_VPU, PCI_INT_A),
		},
	},
	{
		.slot = PCI_DEV_SLOT_TCSS,
		.fns = {
			ANY_PIRQ(PCI_DEVFN_TCSS_XHCI),
			ANY_PIRQ(PCI_DEVFN_TCSS_XDCI),
		},
	},
	{
		.slot = PCI_DEV_SLOT_THC,
		.fns = {
			ANY_PIRQ(PCI_DEVFN_THC0),
			ANY_PIRQ(PCI_DEVFN_THC1),
		},
	},
	{
		.slot = PCI_DEV_SLOT_ISH,
		.fns = {
			DIRECT_IRQ(PCI_DEVFN_ISH),
			DIRECT_IRQ(PCI_DEVFN_GSPI2),
			ANY_PIRQ(PCI_DEVFN_UFS),
		},
	},
	{
		.slot = PCI_DEV_SLOT_XHCI,
		.fns = {
			ANY_PIRQ(PCI_DEVFN_XHCI),
			DIRECT_IRQ(PCI_DEVFN_USBOTG),
			ANY_PIRQ(PCI_DEVFN_CNVI_WIFI),
		},
	},
	{
		.slot = PCI_DEV_SLOT_SIO0,
		.fns = {
			DIRECT_IRQ(PCI_DEVFN_I2C0),
			DIRECT_IRQ(PCI_DEVFN_I2C1),
			DIRECT_IRQ(PCI_DEVFN_I2C2),
			DIRECT_IRQ(PCI_DEVFN_I2C3),
		},
	},
	{
		.slot = PCI_DEV_SLOT_CSE,
		.fns = {
			ANY_PIRQ(PCI_DEVFN_CSE),
			ANY_PIRQ(PCI_DEVFN_CSE_2),
			ANY_PIRQ(PCI_DEVFN_CSE_IDER),
			ANY_PIRQ(PCI_DEVFN_CSE_KT),
			ANY_PIRQ(PCI_DEVFN_CSE_3),
			ANY_PIRQ(PCI_DEVFN_CSE_4),
		},
	},
	{
		.slot = PCI_DEV_SLOT_SATA,
		.fns = {
			ANY_PIRQ(PCI_DEVFN_SATA),
		},
	},
	{
		.slot = PCI_DEV_SLOT_SIO1,
		.fns = {
			DIRECT_IRQ(PCI_DEVFN_I2C4),
			DIRECT_IRQ(PCI_DEVFN_I2C5),
			DIRECT_IRQ(PCI_DEVFN_UART2),
		},
	},
	{
		.slot = PCI_DEV_SLOT_PCIE_1,
		.fns = {
			FIXED_INT_PIRQ(PCI_DEVFN_PCIE1, PCI_INT_A, PIRQ_A),
			FIXED_INT_PIRQ(PCI_DEVFN_PCIE2, PCI_INT_B, PIRQ_B),
			FIXED_INT_PIRQ(PCI_DEVFN_PCIE3, PCI_INT_C, PIRQ_C),
			FIXED_INT_PIRQ(PCI_DEVFN_PCIE4, PCI_INT_D, PIRQ_D),
			FIXED_INT_PIRQ(PCI_DEVFN_PCIE5, PCI_INT_A, PIRQ_A),
			FIXED_INT_PIRQ(PCI_DEVFN_PCIE6, PCI_INT_B, PIRQ_B),
			FIXED_INT_PIRQ(PCI_DEVFN_PCIE7, PCI_INT_C, PIRQ_C),
			FIXED_INT_PIRQ(PCI_DEVFN_PCIE8, PCI_INT_D, PIRQ_D),
		},
	},
	{
		.slot = PCI_DEV_SLOT_SIO2,
		.fns = {
			/* UART0 shares an interrupt line with TSN0, so must use
			   a PIRQ */
			FIXED_INT_ANY_PIRQ(PCI_DEVFN_UART0, PCI_INT_A),
			/* UART1 shares an interrupt line with TSN1, so must use
			   a PIRQ */
			FIXED_INT_ANY_PIRQ(PCI_DEVFN_UART1, PCI_INT_B),
			DIRECT_IRQ(PCI_DEVFN_GSPI0),
			DIRECT_IRQ(PCI_DEVFN_GSPI1),
		},
	},
	{
		.slot = PCI_DEV_SLOT_ESPI,
		.fns = {
			ANY_PIRQ(PCI_DEVFN_HDA),
			ANY_PIRQ(PCI_DEVFN_SMBUS),
			ANY_PIRQ(PCI_DEVFN_GBE),
			/* INTERRUPT_PIN is RO/0x01 */
			FIXED_INT_ANY_PIRQ(PCI_DEVFN_NPK, PCI_INT_A),
		},
	},
};

bool is_pch_slot(unsigned int devfn)
{
	if (PCI_SLOT(devfn) >= MIN_PCH_SLOT)
		return true;
	const struct pcie_rp_group *group;
	for (group = get_pcie_rp_table(); group->count; ++group) {
		if (PCI_SLOT(devfn) == group->slot)
			return true;
	}
	return false;
}

static const SI_PCH_DEVICE_INTERRUPT_CONFIG *pci_irq_to_fsp(size_t *out_count)
{
	const struct pci_irq_entry *entry = get_cached_pci_irqs();
	SI_PCH_DEVICE_INTERRUPT_CONFIG *config;
	size_t pch_total = 0;
	size_t cfg_count = 0;

	if (!entry)
		return NULL;

	/* Count PCH devices */
	while (entry) {
		if (is_pch_slot(entry->devfn))
			++pch_total;
		entry = entry->next;
	}

	/* Convert PCH device entries to FSP format */
	config = calloc(pch_total, sizeof(*config));
	entry = get_cached_pci_irqs();
	while (entry) {
		if (!is_pch_slot(entry->devfn)) {
			entry = entry->next;
			continue;
		}

		config[cfg_count].Device = PCI_SLOT(entry->devfn);
		config[cfg_count].Function = PCI_FUNC(entry->devfn);
		config[cfg_count].IntX = (SI_PCH_INT_PIN)entry->pin;
		config[cfg_count].Irq = entry->irq;
		++cfg_count;

		entry = entry->next;
	}

	*out_count = cfg_count;

	return config;
}

/*
 * ME End of Post configuration
 * 0 - Disable EOP.
 * 1 - Send in PEI (Applicable for FSP in API mode)
 * 2 - Send in DXE (Not applicable for FSP in API mode)
 */
enum fsp_end_of_post {
	EOP_DISABLE = 0,
	EOP_PEI = 1,
	EOP_DXE = 2,
};

static const pci_devfn_t i2c_dev[] = {
	PCI_DEVFN_I2C0,
	PCI_DEVFN_I2C1,
	PCI_DEVFN_I2C2,
	PCI_DEVFN_I2C3,
	PCI_DEVFN_I2C4,
	PCI_DEVFN_I2C5,
};

static const pci_devfn_t gspi_dev[] = {
	PCI_DEVFN_GSPI0,
	PCI_DEVFN_GSPI1,
	PCI_DEVFN_GSPI2,
};

static const pci_devfn_t uart_dev[] = {
	PCI_DEVFN_UART0,
	PCI_DEVFN_UART1,
	PCI_DEVFN_UART2
};

/*
 * Chip config parameter PcieRpL1Substates uses (UPD value + 1)
 * because UPD value of 0 for PcieRpL1Substates means disabled for FSP.
 * In order to ensure that mainboard setting does not disable L1 substates
 * incorrectly, chip config parameter values are offset by 1 with 0 meaning
 * use FSP UPD default. get_l1_substate_control() ensures that the right UPD
 * value is set in fsp_params.
 * 0: Use FSP UPD default
 * 1: Disable L1 substates
 * 2: Use L1.1
 * 3: Use L1.2 (FSP UPD default)
 */
static int get_l1_substate_control(enum L1_substates_control ctl)
{
	if (CONFIG(SOC_INTEL_COMPLIANCE_TEST_MODE))
		ctl = L1_SS_DISABLED;
	else if ((ctl > L1_SS_L1_2) || (ctl == L1_SS_FSP_DEFAULT))
		ctl = L1_SS_L1_2;
	return ctl - 1;
}

/*
 * Chip config parameter pcie_rp_aspm uses (UPD value + 1) because
 * a UPD value of 0 for pcie_rp_aspm means disabled. In order to ensure
 * that the mainboard setting does not disable ASPM incorrectly, chip
 * config parameter values are offset by 1 with 0 meaning use FSP UPD default.
 * get_aspm_control() ensures that the right UPD value is set in fsp_params.
 * 0: Use FSP UPD default
 * 1: Disable ASPM
 * 2: L0s only
 * 3: L1 only
 * 4: L0s and L1
 * 5: Auto configuration
 */
static unsigned int get_aspm_control(enum ASPM_control ctl)
{
	if ((ctl > ASPM_AUTO) || (ctl == ASPM_DEFAULT))
		ctl = ASPM_AUTO;
	return ctl - 1;
}

__weak void mainboard_update_soc_chip_config(struct soc_intel_meteorlake_config *config)
{
	/* Override settings per board. */
}

static void fill_fsps_lpss_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	int max_port, i;

	max_port = get_max_i2c_port();
	for (i = 0; i < max_port; i++) {
		s_cfg->SerialIoI2cMode[i] = is_devfn_enabled(i2c_dev[i]) ?
			config->serial_io_i2c_mode[i] : 0;
	}

	max_port = get_max_gspi_port();
	for (i = 0; i < max_port; i++) {
		s_cfg->SerialIoSpiCsMode[i] = config->serial_io_gspi_cs_mode[i];
		s_cfg->SerialIoSpiCsState[i] = config->serial_io_gspi_cs_state[i];
		s_cfg->SerialIoSpiMode[i] = is_devfn_enabled(gspi_dev[i]) ?
			config->serial_io_gspi_mode[i] : 0;
	}

	max_port = get_max_uart_port();
	for (i = 0; i < max_port; i++) {
		s_cfg->SerialIoUartMode[i] = is_devfn_enabled(uart_dev[i]) ?
			config->serial_io_uart_mode[i] : 0;
	}
}

static void fill_fsps_microcode_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	const struct microcode *microcode_file;
	size_t microcode_len;

	/* Locate microcode and pass to FSP-S for 2nd microcode loading */
	microcode_file = intel_microcode_find();

	if (microcode_file != NULL) {
		microcode_len = get_microcode_size(microcode_file);
		if (microcode_len != 0) {
			/* Update CPU Microcode patch base address/size */
			s_cfg->MicrocodeRegionBase = (uint32_t)(uintptr_t)microcode_file;
			s_cfg->MicrocodeRegionSize = (uint32_t)microcode_len;
		}
	}
}

static void fill_fsps_cpu_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	/*
	 * FIXME: FSP assumes ownership of the APs (Application Processors)
	 * upon passing `NULL` pointer to the CpuMpPpi FSP-S UPD.
	 * Hence, pass a valid pointer to the CpuMpPpi UPD unconditionally.
	 * This would avoid APs from getting hijacked by FSP while coreboot
	 * decides to set SkipMpInit UPD.
	 */
	s_cfg->CpuMpPpi = (uintptr_t)mp_fill_ppi_services_data();

	/*
	 * Fill `2nd microcode loading FSP UPD` if FSP is running CPU feature
	 * programming.
	 */
	if (CONFIG(USE_FSP_FEATURE_PROGRAM_ON_APS))
		fill_fsps_microcode_params(s_cfg, config);
}


static void fill_fsps_igd_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	/* Load VBT before devicetree-specific config. */
	s_cfg->GraphicsConfigPtr = (uintptr_t)vbt_get();

	/* Check if IGD is present and fill Graphics init param accordingly */
	s_cfg->PeiGraphicsPeimInit = CONFIG(RUN_FSP_GOP) && is_devfn_enabled(PCI_DEVFN_IGD);
	s_cfg->LidStatus = CONFIG(VBOOT_LID_SWITCH) ? get_lid_switch() : CONFIG(RUN_FSP_GOP);
	s_cfg->PavpEnable = CONFIG(PAVP);
}

static void fill_fsps_tcss_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	const struct device *tcss_port_arr[] = {
		DEV_PTR(tcss_usb3_port0),
		DEV_PTR(tcss_usb3_port1),
		DEV_PTR(tcss_usb3_port2),
		DEV_PTR(tcss_usb3_port3),
	};

	s_cfg->TcssAuxOri = config->tcss_aux_ori;

	/* Explicitly clear this field to avoid using defaults */
	memset(s_cfg->IomTypeCPortPadCfg, 0, sizeof(s_cfg->IomTypeCPortPadCfg));

	/* D3Hot and D3Cold for TCSS */
	s_cfg->D3HotEnable = !config->tcss_d3_hot_disable;
	s_cfg->D3ColdEnable = CONFIG(D3COLD_SUPPORT);
	s_cfg->UsbTcPortEn = 0;

	for (int i = 0; i < MAX_TYPE_C_PORTS; i++) {
		if (is_dev_enabled(tcss_port_arr[i]))
			s_cfg->UsbTcPortEn |= BIT(i);
	}
}

static void fill_fsps_chipset_lockdown_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	/* Chipset Lockdown */
	const bool lockdown_by_fsp = get_lockdown_config() == CHIPSET_LOCKDOWN_FSP;
	s_cfg->PchLockDownGlobalSmi = lockdown_by_fsp;
	s_cfg->PchLockDownBiosInterface = lockdown_by_fsp;
	s_cfg->PchUnlockGpioPads = !lockdown_by_fsp;
	s_cfg->RtcMemoryLock = lockdown_by_fsp;
	s_cfg->SkipPamLock = !lockdown_by_fsp;

	/* coreboot will send EOP before loading payload */
	s_cfg->EndOfPostMessage = EOP_DISABLE;
}

static void fill_fsps_xhci_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	int i, max_port;

	max_port = get_max_usb20_port();
	for (i = 0; i < max_port; i++) {
		s_cfg->PortUsb20Enable[i] = config->usb2_ports[i].enable;
		s_cfg->Usb2PhyPetxiset[i] = config->usb2_ports[i].pre_emp_bias;
		s_cfg->Usb2PhyTxiset[i] = config->usb2_ports[i].tx_bias;
		s_cfg->Usb2PhyPredeemp[i] = config->usb2_ports[i].tx_emp_enable;
		s_cfg->Usb2PhyPehalfbit[i] = config->usb2_ports[i].pre_emp_bit;

		if (config->usb2_ports[i].enable)
			s_cfg->Usb2OverCurrentPin[i] = config->usb2_ports[i].ocpin;
		else
			s_cfg->Usb2OverCurrentPin[i] = OC_SKIP;

		if (config->usb2_ports[i].type_c)
			s_cfg->PortResetMessageEnable[i] = 1;
	}

	max_port = get_max_usb30_port();
	for (i = 0; i < max_port; i++) {
		s_cfg->PortUsb30Enable[i] = config->usb3_ports[i].enable;
		if (config->usb3_ports[i].enable)
			s_cfg->Usb3OverCurrentPin[i] = config->usb3_ports[i].ocpin;
		else
			s_cfg->Usb3OverCurrentPin[i] = OC_SKIP;

		if (config->usb3_ports[i].tx_de_emp) {
			s_cfg->Usb3HsioTxDeEmphEnable[i] = 1;
			s_cfg->Usb3HsioTxDeEmph[i] = config->usb3_ports[i].tx_de_emp;
		}
		if (config->usb3_ports[i].tx_downscale_amp) {
			s_cfg->Usb3HsioTxDownscaleAmpEnable[i] = 1;
			s_cfg->Usb3HsioTxDownscaleAmp[i] =
				config->usb3_ports[i].tx_downscale_amp;
		}
	}

	max_port = get_max_tcss_port();
	for (i = 0; i < max_port; i++) {
		if (config->tcss_ports[i].enable)
			s_cfg->CpuUsb3OverCurrentPin[i] = config->tcss_ports[i].ocpin;
	}
}

static void fill_fsps_xdci_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	s_cfg->XdciEnable = xdci_can_enable(PCI_DEVFN_USBOTG);
}

static void fill_fsps_uart_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	ASSERT(ARRAY_SIZE(s_cfg->SerialIoUartAutoFlow) > CONFIG_UART_FOR_CONSOLE);
	s_cfg->SerialIoUartAutoFlow[CONFIG_UART_FOR_CONSOLE] = 0;
}

static void fill_fsps_sata_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	/* SATA */
	s_cfg->SataEnable = is_devfn_enabled(PCI_DEVFN_SATA);
	if (s_cfg->SataEnable) {
		s_cfg->SataMode = config->sata_mode;
		s_cfg->SataSalpSupport = config->sata_salp_support;
		memcpy(s_cfg->SataPortsEnable, config->sata_ports_enable,
			sizeof(s_cfg->SataPortsEnable));
		memcpy(s_cfg->SataPortsDevSlp, config->sata_ports_dev_slp,
			sizeof(s_cfg->SataPortsDevSlp));
	}

	/*
	 * Power Optimizer for SATA.
	 * SataPwrOptimizeDisable is default to 0.
	 * Boards not needing the optimizers explicitly disables them by setting
	 * these disable variables to 1 in devicetree overrides.
	 */
	s_cfg->SataPwrOptEnable = !(config->sata_pwr_optimize_disable);
	/*
	 *  Enable DEVSLP Idle Timeout settings DmVal and DitoVal.
	 *  SataPortsDmVal is the DITO multiplier. Default is 15.
	 *  SataPortsDitoVal is the DEVSLP Idle Timeout (DITO), Default is 625ms.
	 *  The default values can be changed from devicetree.
	 */
	for (size_t i = 0; i < ARRAY_SIZE(config->sata_ports_enable_dito_config); i++) {
		if (config->sata_ports_enable_dito_config[i]) {
			s_cfg->SataPortsDmVal[i] = config->sata_ports_dm_val[i];
			s_cfg->SataPortsDitoVal[i] = config->sata_ports_dito_val[i];
		}
	}
}

static void fill_fsps_thermal_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	/* Enable TCPU for processor thermal control */
	s_cfg->Device4Enable = is_devfn_enabled(PCI_DEVFN_DPTF);

	/* Set TccActivationOffset */
	s_cfg->TccActivationOffset = config->tcc_offset;
}

static void fill_fsps_lan_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	/* LAN */
	s_cfg->PchLanEnable = is_devfn_enabled(PCI_DEVFN_GBE);
}

static void fill_fsps_cnvi_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	/* CNVi */
	s_cfg->CnviMode = is_devfn_enabled(PCI_DEVFN_CNVI_WIFI);
	s_cfg->CnviWifiCore = config->cnvi_wifi_core;
	s_cfg->CnviBtCore = config->cnvi_bt_core;
	s_cfg->CnviBtAudioOffload = config->cnvi_bt_audio_offload;
	if (!s_cfg->CnviMode && s_cfg->CnviWifiCore) {
		printk(BIOS_ERR, "CNVi WiFi is enabled without CNVi being enabled\n");
		s_cfg->CnviWifiCore = 0;
	}
	if (!s_cfg->CnviBtCore && s_cfg->CnviBtAudioOffload) {
		printk(BIOS_ERR, "BT offload is enabled without CNVi BT being enabled\n");
		s_cfg->CnviBtAudioOffload = 0;
	}
	if (!s_cfg->CnviMode && s_cfg->CnviBtCore) {
		printk(BIOS_ERR, "CNVi BT is enabled without CNVi being enabled\n");
		s_cfg->CnviBtCore = 0;
		s_cfg->CnviBtAudioOffload = 0;
	}
}

static void fill_fsps_vmd_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	/* VMD */
	s_cfg->VmdEnable = is_devfn_enabled(PCI_DEVFN_VMD);
}

static void fill_fsps_tbt_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	for (int i = 0; i < ARRAY_SIZE(s_cfg->ITbtPcieRootPortEn); i++)
		s_cfg->ITbtPcieRootPortEn[i] = is_devfn_enabled(PCI_DEVFN_TBT(i));
}

static void fill_fsps_8254_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	/* Legacy 8254 timer support */
	bool use_8254 = get_uint_option("legacy_8254_timer", CONFIG(USE_LEGACY_8254_TIMER));
	s_cfg->Enable8254ClockGating = !use_8254;
	s_cfg->Enable8254ClockGatingOnS3 = !use_8254;
}

static void fill_fsps_pm_timer_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	/*
	 * Legacy PM ACPI Timer (and TCO Timer)
	 * This *must* be 1 in any case to keep FSP from
	 *  1) enabling PM ACPI Timer emulation in uCode.
	 *  2) disabling the PM ACPI Timer.
	 * We handle both by ourself!
	 */
	s_cfg->EnableTcoTimer = 1;
}

static void fill_fsps_pcie_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	int max_port = get_max_pcie_port();
	uint32_t enable_mask = pcie_rp_enable_mask(get_pcie_rp_table());
	for (int i = 0; i < max_port; i++) {
		if (!(enable_mask & BIT(i)))
			continue;
		const struct pcie_rp_config *rp_cfg = &config->pcie_rp[i];
		s_cfg->PcieRpL1Substates[i] =
				get_l1_substate_control(rp_cfg->PcieRpL1Substates);
		s_cfg->PcieRpLtrEnable[i] = !!(rp_cfg->flags & PCIE_RP_LTR);
		s_cfg->PcieRpAdvancedErrorReporting[i] = !!(rp_cfg->flags & PCIE_RP_AER);
		s_cfg->PcieRpHotPlug[i] = !!(rp_cfg->flags & PCIE_RP_HOTPLUG)
				|| CONFIG(SOC_INTEL_COMPLIANCE_TEST_MODE);
		s_cfg->PcieRpClkReqDetect[i] = !!(rp_cfg->flags & PCIE_RP_CLK_REQ_DETECT);
		if (rp_cfg->pcie_rp_aspm)
			s_cfg->PcieRpAspm[i] = get_aspm_control(rp_cfg->pcie_rp_aspm);
	}
	s_cfg->PcieComplianceTestMode = CONFIG(SOC_INTEL_COMPLIANCE_TEST_MODE);
}

static void fill_fsps_misc_power_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	/* Skip setting D0I3 bit for all HECI devices */
	s_cfg->DisableD0I3SettingForHeci = 1;

	s_cfg->Hwp = 1;
	s_cfg->Cx = 1;
	s_cfg->PsOnEnable = 1;
	s_cfg->PkgCStateLimit = LIMIT_AUTO;
	/* Enable the energy efficient turbo mode */
	s_cfg->EnergyEfficientTurbo = 1;
	s_cfg->PmcLpmS0ixSubStateEnableMask = get_supported_lpm_mask();
	/* Un-Demotion from Demoted C1 need to be disable when
	 * C1 auto demotion is disabled */
	s_cfg->C1StateUnDemotion = !config->disable_c1_state_auto_demotion;
	s_cfg->C1StateAutoDemotion = !config->disable_c1_state_auto_demotion;
	s_cfg->PkgCStateDemotion = !config->disable_package_c_state_demotion;
	s_cfg->PkgCStateUnDemotion = !config->disable_package_c_state_demotion;
	s_cfg->PmcV1p05PhyExtFetControlEn = 1;

	/* Enable PCH to CPU energy report feature. */
	s_cfg->PchPmDisableEnergyReport = !config->pch_pm_energy_report_enable;
}


static void fill_fsps_ufs_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	s_cfg->UfsEnable[0] = is_devfn_enabled(PCI_DEVFN_UFS);
}

static void fill_fsps_ai_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	s_cfg->GnaEnable = is_devfn_enabled(PCI_DEVFN_GNA);
	s_cfg->VpuEnable = is_devfn_enabled(PCI_DEVFN_VPU);
}

static void fill_fsps_irq_params(FSP_S_CONFIG *s_cfg,
				 const struct soc_intel_meteorlake_config *config)
{
	if (!assign_pci_irqs(irq_constraints, ARRAY_SIZE(irq_constraints)))
		die("ERROR: Unable to assign PCI IRQs, and no _PRT table available\n");

	size_t pch_count = 0;
	const SI_PCH_DEVICE_INTERRUPT_CONFIG *upd_irqs = pci_irq_to_fsp(&pch_count);

	s_cfg->DevIntConfigPtr = (UINT32)((uintptr_t)upd_irqs);
	s_cfg->NumOfDevIntConfig = pch_count;
	printk(BIOS_INFO, "IRQ: Using dynamically assigned PCI IO-APIC IRQs\n");
}

static void arch_silicon_init_params(FSPS_ARCH_UPD *s_arch_cfg)
{
	/*
	 * EnableMultiPhaseSiliconInit for running MultiPhaseSiInit
	 */
	s_arch_cfg->EnableMultiPhaseSiliconInit = 1;

	/* Assign FspEventHandler arch Upd to use coreboot debug event handler */
	if (CONFIG(FSP_USES_CB_DEBUG_EVENT_HANDLER) && CONFIG(CONSOLE_SERIAL) &&
			 CONFIG(FSP_ENABLE_SERIAL_DEBUG))
		s_arch_cfg->FspEventHandler = (FSP_EVENT_HANDLER)
				fsp_debug_event_handler;
}

static void evaluate_ssid(const struct device *dev, uint16_t *svid, uint16_t *ssid)
{
	if (!(dev && svid && ssid))
		return;

	*svid = CONFIG_SUBSYSTEM_VENDOR_ID ? : (dev->subsystem_vendor ? : 0x8086);
	*ssid = CONFIG_SUBSYSTEM_DEVICE_ID ? : (dev->subsystem_device ? : 0xfffe);
}

/*
 * Programming SSID before FSP-S is important because SSID registers of a few PCIE
 * devices (e.g. IPU, Crashlog, XHCI, TCSS_XHCI etc.) are locked after FSP-S hence
 * provide a custom SSID (same as DID by default) value via UPD.
 */
static void fill_fsps_pci_ssid_params(FSP_S_CONFIG *s_cfg,
		const struct soc_intel_meteorlake_config *config)
{
	struct svid_ssid_init_entry {
		union {
			struct {
				uint64_t reg:12;
				uint64_t function:3;
				uint64_t device:5;
				uint64_t bus:8;
				uint64_t ignore1:4;
				uint64_t segment:16;
				uint64_t ignore2:16;
			};
			uint64_t data;
		};
		struct {
			uint16_t svid;
			uint16_t ssid;
		};
		uint32_t ignore3;
	};

	static struct svid_ssid_init_entry ssid_table[MAX_ONBOARD_PCIE_DEVICES];
	const struct device *dev;
	int i = 0;

	for (dev = all_devices; dev; dev = dev->next) {
		if (!(is_dev_enabled(dev) && dev->path.type == DEVICE_PATH_PCI &&
		    dev->upstream->secondary == 0))
			continue;

		if (dev->path.pci.devfn == PCI_DEVFN_ROOT) {
			evaluate_ssid(dev, &s_cfg->SiCustomizedSvid, &s_cfg->SiCustomizedSsid);
		} else {
			ssid_table[i].reg	= PCI_SUBSYSTEM_VENDOR_ID;
			ssid_table[i].device	= PCI_SLOT(dev->path.pci.devfn);
			ssid_table[i].function	= PCI_FUNC(dev->path.pci.devfn);
			evaluate_ssid(dev, &ssid_table[i].svid, &ssid_table[i].ssid);
			i++;
		}
	}

	s_cfg->SiSsidTablePtr = (uintptr_t)ssid_table;
	s_cfg->SiNumberOfSsidTableEntry = i;

	/* Ensure FSP will program the registers */
	s_cfg->SiSkipSsidProgramming = 0;
}

static void soc_silicon_init_params(FSP_S_CONFIG *s_cfg,
		struct soc_intel_meteorlake_config *config)
{
	/* Override settings per board if required. */
	mainboard_update_soc_chip_config(config);

	 void (*fill_fsps_params[])(FSP_S_CONFIG *s_cfg,
			const struct soc_intel_meteorlake_config *config) = {
		fill_fsps_lpss_params,
		fill_fsps_cpu_params,
		fill_fsps_igd_params,
		fill_fsps_tcss_params,
		fill_fsps_chipset_lockdown_params,
		fill_fsps_xhci_params,
		fill_fsps_xdci_params,
		fill_fsps_uart_params,
		fill_fsps_sata_params,
		fill_fsps_thermal_params,
		fill_fsps_lan_params,
		fill_fsps_cnvi_params,
		fill_fsps_vmd_params,
		fill_fsps_tbt_params,
		fill_fsps_8254_params,
		fill_fsps_pm_timer_params,
		fill_fsps_pcie_params,
		fill_fsps_misc_power_params,
		fill_fsps_ufs_params,
		fill_fsps_ai_params,
		fill_fsps_irq_params,
		fill_fsps_pci_ssid_params,
	};

	for (size_t i = 0; i < ARRAY_SIZE(fill_fsps_params); i++)
		fill_fsps_params[i](s_cfg, config);
}

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	struct soc_intel_meteorlake_config *config;
	FSP_S_CONFIG *s_cfg = &supd->FspsConfig;
	FSPS_ARCH_UPD *s_arch_cfg = &supd->FspsArchUpd;

	config = config_of_soc();
	arch_silicon_init_params(s_arch_cfg);
	soc_silicon_init_params(s_cfg, config);
	mainboard_silicon_init_params(s_cfg);
}

/*
 * Callbacks for SoC/Mainboard specific overrides for FspMultiPhaseSiInit
 * This platform supports below MultiPhaseSIInit Phase(s):
 * Phase   |  FSP return point                                |  Purpose
 * ------- + ------------------------------------------------ + -------------------------------
 *   1     |  After TCSS initialization completed             |  for TCSS specific init
 *   2     |  Before BIOS Reset CPL is set by FSP-S           |  for CPU specific init
 */
void platform_fsp_multi_phase_init_cb(uint32_t phase_index)
{
	switch (phase_index) {
	case 1:
		/* TCSS specific initialization here */
		printk(BIOS_DEBUG, "FSP MultiPhaseSiInit %s/%s called\n",
			__FILE__, __func__);

		if (CONFIG(SOC_INTEL_COMMON_BLOCK_TCSS)) {
			const config_t *config = config_of_soc();
			tcss_configure(config->typec_aux_bias_pads);
		}
		break;
	case 2:
		/* CPU specific initialization here */
		printk(BIOS_DEBUG, "FSP MultiPhaseSiInit %s/%s called\n",
			__FILE__, __func__);
		before_post_cpus_init();
		/* Enable BIOS Reset CPL */
		enable_bios_reset_cpl();
		break;
	default:
		break;
	}
}

/* Mainboard GPIO Configuration */
__weak void mainboard_silicon_init_params(FSP_S_CONFIG *s_cfg)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}

/* Handle FSP logo params */
void soc_load_logo(FSPS_UPD *supd)
{
	fsp_convert_bmp_to_gop_blt(&supd->FspsConfig.LogoPtr,
			 &supd->FspsConfig.LogoSize,
			 &supd->FspsConfig.BltBufferAddress,
			 &supd->FspsConfig.BltBufferSize,
			 &supd->FspsConfig.LogoPixelHeight,
			 &supd->FspsConfig.LogoPixelWidth);
}
