/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <intelblocks/lpss.h>
#include <intelblocks/power_limit.h>
#include <intelblocks/xdci.h>
#include <intelpch/lockdown.h>
#include <soc/intel/common/vbt.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <string.h>

#include "chip.h"

static const pci_devfn_t serial_io_dev[] = {
	PCH_DEVFN_I2C0,
	PCH_DEVFN_I2C1,
	PCH_DEVFN_I2C2,
	PCH_DEVFN_I2C3,
	PCH_DEVFN_I2C4,
	PCH_DEVFN_I2C5,
	PCH_DEVFN_GSPI0,
	PCH_DEVFN_GSPI1,
	PCH_DEVFN_GSPI2,
	PCH_DEVFN_UART0,
	PCH_DEVFN_UART1,
	PCH_DEVFN_UART2
};

/* List of Minimum Assertion durations in microseconds */
enum min_assrt_dur {
	MinAssrtDur0s		= 0,
	MinAssrtDur60us		= 60,
	MinAssrtDur1ms		= 1000,
	MinAssrtDur50ms		= 50000,
	MinAssrtDur98ms		= 98000,
	MinAssrtDur500ms	= 500000,
	MinAssrtDur1s		= 1000000,
	MinAssrtDur2s		= 2000000,
	MinAssrtDur3s		= 3000000,
	MinAssrtDur4s		= 4000000,
};


/* Signal Assertion duration values */
struct cfg_assrt_dur {
	/* Minimum assertion duration of SLP_A signal */
	enum min_assrt_dur slp_a;

	/* Minimum assertion duration of SLP_4 signal */
	enum min_assrt_dur slp_s4;

	/* Minimum assertion duration of SLP_3 signal */
	enum min_assrt_dur slp_s3;

	/* PCH PM Power Cycle duration */
	enum min_assrt_dur pm_pwr_cyc_dur;
};

/* Default value of PchPmPwrCycDur */
#define PCH_PM_PWR_CYC_DUR	4

/*
 * Given an enum for PCH_SERIAL_IO_MODE, 1 needs to be subtracted to get the FSP
 * UPD expected value for Serial IO since valid enum index starts from 1.
 */
#define PCH_SERIAL_IO_INDEX(x)		((x) - 1)

static uint8_t get_param_value(const config_t *config, uint32_t dev_offset)
{
	struct device *dev;

	dev = pcidev_path_on_root(serial_io_dev[dev_offset]);
	if (!dev || !dev->enabled)
		return PCH_SERIAL_IO_INDEX(PchSerialIoDisabled);

	if ((config->SerialIoDevMode[dev_offset] >= PchSerialIoMax) ||
	    (config->SerialIoDevMode[dev_offset] == PchSerialIoNotInitialized))
		return PCH_SERIAL_IO_INDEX(PchSerialIoPci);

	/*
	 * Correct Enum index starts from 1, so subtract 1 while returning value
	 */
	return PCH_SERIAL_IO_INDEX(config->SerialIoDevMode[dev_offset]);
}

#if CONFIG(SOC_INTEL_COMETLAKE)
static enum min_assrt_dur get_high_asst_width(const struct cfg_assrt_dur *cfg_assrt_dur)
{
	enum min_assrt_dur max_assert_dur = cfg_assrt_dur->slp_s4;

	if (max_assert_dur < cfg_assrt_dur->slp_s3)
		max_assert_dur = cfg_assrt_dur->slp_s3;

	if (max_assert_dur < cfg_assrt_dur->slp_a)
		max_assert_dur = cfg_assrt_dur->slp_a;

	return max_assert_dur;
}

static void get_min_assrt_dur(uint8_t slp_s4_min_asst, uint8_t slp_s3_min_asst,
		uint8_t slp_a_min_asst, uint8_t pm_pwr_cyc_dur,
		struct cfg_assrt_dur *cfg_assrt_dur)
{
	/*
	 * Ensure slp_x_dur_list[] elements are in sync with devicetree config to FSP encoded
	 * values.
	 * slp_s4_asst_dur_list : 1s, 1s, 2s, 3s, 4s(Default)
	 */
	const enum min_assrt_dur slp_s4_asst_dur_list[] = {
		MinAssrtDur1s, MinAssrtDur1s, MinAssrtDur2s, MinAssrtDur3s, MinAssrtDur4s
	};

	/* slp_s3_asst_dur_list: 50ms, 60us, 50ms (Default), 2s */
	const enum min_assrt_dur slp_s3_asst_dur_list[] = {
		MinAssrtDur50ms, MinAssrtDur60us, MinAssrtDur50ms, MinAssrtDur2s
	};

	/* slp_a_asst_dur_list: 2s, 0s, 4s, 98ms, 2s(Default) */
	const enum min_assrt_dur slp_a_asst_dur_list[] = {
		MinAssrtDur2s, MinAssrtDur0s, MinAssrtDur4s, MinAssrtDur98ms, MinAssrtDur2s
	};

	/* pm_pwr_cyc_dur_list: 4s(Default), 1s, 2s, 3s, 4s */
	const enum min_assrt_dur pm_pwr_cyc_dur_list[] = {
		MinAssrtDur4s, MinAssrtDur1s, MinAssrtDur2s, MinAssrtDur3s, MinAssrtDur4s
	};

	/* Get signal assertion width */
	if (slp_s4_min_asst < ARRAY_SIZE(slp_s4_asst_dur_list))
		cfg_assrt_dur->slp_s4 = slp_s4_asst_dur_list[slp_s4_min_asst];

	if (slp_s3_min_asst < ARRAY_SIZE(slp_s3_asst_dur_list))
		cfg_assrt_dur->slp_s3 = slp_s3_asst_dur_list[slp_s3_min_asst];

	if (slp_a_min_asst < ARRAY_SIZE(slp_a_asst_dur_list))
		cfg_assrt_dur->slp_a = slp_a_asst_dur_list[slp_a_min_asst];

	if (pm_pwr_cyc_dur < ARRAY_SIZE(pm_pwr_cyc_dur_list))
		cfg_assrt_dur->pm_pwr_cyc_dur = pm_pwr_cyc_dur_list[pm_pwr_cyc_dur];
}


static uint8_t get_pm_pwr_cyc_dur(uint8_t slp_s4_min_asst, uint8_t slp_s3_min_asst,
		uint8_t slp_a_min_asst, uint8_t pm_pwr_cyc_dur)
{
	/* Sets default minimum asserton duration values */
	struct cfg_assrt_dur cfg_assrt_dur = {
		.slp_a		= MinAssrtDur2s,
		.slp_s4		= MinAssrtDur4s,
		.slp_s3		= MinAssrtDur50ms,
		.pm_pwr_cyc_dur	= MinAssrtDur4s
	};

	enum min_assrt_dur high_asst_width;

	/* Convert assertion durations from register-encoded to microseconds */
	get_min_assrt_dur(slp_s4_min_asst, slp_s3_min_asst, slp_a_min_asst, pm_pwr_cyc_dur,
			&cfg_assrt_dur);

	/* Get the higher assertion duration among PCH EDS specified signals for pwr_cyc_dur */
	high_asst_width = get_high_asst_width(&cfg_assrt_dur);

	if (cfg_assrt_dur.pm_pwr_cyc_dur >= high_asst_width)
		return pm_pwr_cyc_dur;

	printk(BIOS_DEBUG,
			"Set PmPwrCycDur to 4s as configured PmPwrCycDur(%d) violates PCH EDS "
			"spec\n", pm_pwr_cyc_dur);

	return PCH_PM_PWR_CYC_DUR;
}


static void parse_devicetree_param(const config_t *config, FSP_S_CONFIG *params)
{
	uint32_t dev_offset = 0;
	uint32_t i = 0;

	for (i = 0; i < CONFIG_SOC_INTEL_I2C_DEV_MAX; i++, dev_offset++) {
		params->SerialIoI2cMode[i] =
				get_param_value(config, dev_offset);
	}

	for (i = 0; i < CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX; i++,
	     dev_offset++) {
		params->SerialIoSpiMode[i] =
				get_param_value(config, dev_offset);
	}

	for (i = 0; i < SOC_INTEL_CML_UART_DEV_MAX; i++, dev_offset++) {
		params->SerialIoUartMode[i] =
				get_param_value(config, dev_offset);
	}
}
#else
static void parse_devicetree_param(const config_t *config, FSP_S_CONFIG *params)
{
	for (int i = 0; i < ARRAY_SIZE(serial_io_dev); i++)
		params->SerialIoDevMode[i] = get_param_value(config, i);
}
#endif

static void parse_devicetree(FSP_S_CONFIG *params)
{
	const config_t *config = config_of_soc();

	parse_devicetree_param(config, params);
}

/* Ignore LTR value for GBE devices */
static void ignore_gbe_ltr(void)
{
	uint8_t reg8;
	uint8_t *pmcbase = pmc_mmio_regs();

	reg8 = read8(pmcbase + LTR_IGN);
	reg8 |= IGN_GBE;
	write8(pmcbase + LTR_IGN, reg8);
}

static void configure_gspi_cs(int idx, const config_t *config,
			      uint8_t *polarity, uint8_t *enable,
			      uint8_t *defaultcs)
{
	struct spi_cfg cfg;

	/* If speed_mhz is set, infer that the port should be configured */
	if (config->common_soc_config.gspi[idx].speed_mhz != 0) {
		if (gspi_get_soc_spi_cfg(idx, &cfg) == 0) {
			if (cfg.cs_polarity == SPI_POLARITY_LOW)
				*polarity = 0;
			else
				*polarity = 1;

			if (defaultcs != NULL)
				*defaultcs = 0;
			if (enable != NULL)
				*enable = 1;
		}
	}
}

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	int i;
	FSP_S_CONFIG *params = &supd->FspsConfig;
	FSP_S_TEST_CONFIG *tconfig = &supd->FspsTestConfig;
	struct device *dev;

	config_t *config = config_of_soc();

	/* Parse device tree and enable/disable devices */
	parse_devicetree(params);

	/* Load VBT before devicetree-specific config. */
	params->GraphicsConfigPtr = (uintptr_t)vbt_get();

	/* Set USB OC pin to 0 first */
	for (i = 0; i < ARRAY_SIZE(params->Usb2OverCurrentPin); i++) {
		params->Usb2OverCurrentPin[i] = 0;
	}

	for (i = 0; i < ARRAY_SIZE(params->Usb3OverCurrentPin); i++) {
		params->Usb3OverCurrentPin[i] = 0;
	}

	mainboard_silicon_init_params(params);

	const struct soc_power_limits_config *soc_config;
	soc_config = &config->power_limits_config;
	/* Set PsysPmax if it is available from DT */
	if (soc_config->psys_pmax) {
		printk(BIOS_DEBUG, "psys_pmax = %dW\n", soc_config->psys_pmax);
		/* PsysPmax is in unit of 1/8 Watt */
		tconfig->PsysPmax = soc_config->psys_pmax * 8;
	}

	/* Unlock upper 8 bytes of RTC RAM */
	params->PchLockDownRtcMemoryLock = 0;

	/* SATA */
	dev = pcidev_path_on_root(PCH_DEVFN_SATA);
	if (!dev)
		params->SataEnable = 0;
	else {
		params->SataEnable = dev->enabled;
		params->SataMode = config->SataMode;
		params->SataSalpSupport = config->SataSalpSupport;
		memcpy(params->SataPortsEnable, config->SataPortsEnable,
			sizeof(params->SataPortsEnable));
		memcpy(params->SataPortsDevSlp, config->SataPortsDevSlp,
			sizeof(params->SataPortsDevSlp));

		memcpy(params->SataPortsHotPlug, config->SataPortsHotPlug,
			sizeof(params->SataPortsHotPlug));

#if CONFIG(SOC_INTEL_COMETLAKE)
		memcpy(params->SataPortsDevSlpResetConfig,
			config->SataPortsDevSlpResetConfig,
			sizeof(params->SataPortsDevSlpResetConfig));
#endif
	}
	params->SlpS0WithGbeSupport = 0;
	params->PchPmSlpS0VmRuntimeControl = config->PchPmSlpS0VmRuntimeControl;
	params->PchPmSlpS0Vm070VSupport = config->PchPmSlpS0Vm070VSupport;
	params->PchPmSlpS0Vm075VSupport = config->PchPmSlpS0Vm075VSupport;

	/* Lan */
	dev = pcidev_path_on_root(PCH_DEVFN_GBE);
	if (!dev)
		params->PchLanEnable = 0;
	else {
		params->PchLanEnable = dev->enabled;
		if (config->s0ix_enable && params->PchLanEnable) {
			/*
			 * The VmControl UPDs need to be set as per board
			 * design to allow voltage margining in S0ix to lower
			 * power consumption.
			 * But if GbE is enabled, voltage magining cannot be
			 * enabled, so the Vm control UPDs need to be set to 0.
			 */
			params->SlpS0WithGbeSupport = 1;
			params->PchPmSlpS0VmRuntimeControl = 0;
			params->PchPmSlpS0Vm070VSupport = 0;
			params->PchPmSlpS0Vm075VSupport = 0;
			ignore_gbe_ltr();
		}
	}

	/* Audio */
	params->PchHdaDspEnable = config->PchHdaDspEnable;
	params->PchHdaIDispCodecDisconnect = config->PchHdaIDispCodecDisconnect;
	params->PchHdaAudioLinkHda = config->PchHdaAudioLinkHda;
	params->PchHdaAudioLinkDmic0 = config->PchHdaAudioLinkDmic0;
	params->PchHdaAudioLinkDmic1 = config->PchHdaAudioLinkDmic1;
	params->PchHdaAudioLinkSsp0 = config->PchHdaAudioLinkSsp0;
	params->PchHdaAudioLinkSsp1 = config->PchHdaAudioLinkSsp1;
	params->PchHdaAudioLinkSsp2 = config->PchHdaAudioLinkSsp2;
	params->PchHdaAudioLinkSndw1 = config->PchHdaAudioLinkSndw1;
	params->PchHdaAudioLinkSndw2 = config->PchHdaAudioLinkSndw2;
	params->PchHdaAudioLinkSndw3 = config->PchHdaAudioLinkSndw3;
	params->PchHdaAudioLinkSndw4 = config->PchHdaAudioLinkSndw4;

	/* eDP device */
	params->DdiPortEdp = config->DdiPortEdp;

	/* HPD of DDI ports */
	params->DdiPortBHpd = config->DdiPortBHpd;
	params->DdiPortCHpd = config->DdiPortCHpd;
	params->DdiPortDHpd = config->DdiPortDHpd;
	params->DdiPortFHpd = config->DdiPortFHpd;

	/* DDC of DDI ports */
	params->DdiPortBDdc = config->DdiPortBDdc;
	params->DdiPortCDdc = config->DdiPortCDdc;
	params->DdiPortDDdc = config->DdiPortDDdc;
	params->DdiPortFDdc = config->DdiPortFDdc;

	/* WOL */
	params->PchPmPcieWakeFromDeepSx = config->LanWakeFromDeepSx;
	params->PchPmWolEnableOverride = config->WolEnableOverride;

	/* S0ix */
	params->PchPmSlpS0Enable = config->s0ix_enable;

	/* disable Legacy PME */
	memset(params->PcieRpPmSci, 0, sizeof(params->PcieRpPmSci));

	/* Legacy 8254 timer support */
	params->Enable8254ClockGating = !CONFIG_USE_LEGACY_8254_TIMER;
	params->Enable8254ClockGatingOnS3 = !CONFIG_USE_LEGACY_8254_TIMER;

	/* USB */
	for (i = 0; i < ARRAY_SIZE(config->usb2_ports); i++) {
		params->PortUsb20Enable[i] = config->usb2_ports[i].enable;
		params->Usb2OverCurrentPin[i] = config->usb2_ports[i].ocpin;
		params->Usb2AfePetxiset[i] = config->usb2_ports[i].pre_emp_bias;
		params->Usb2AfeTxiset[i] = config->usb2_ports[i].tx_bias;
		params->Usb2AfePredeemp[i] =
			config->usb2_ports[i].tx_emp_enable;
		params->Usb2AfePehalfbit[i] = config->usb2_ports[i].pre_emp_bit;
	}

	if (config->PchUsb2PhySusPgDisable)
		params->PchUsb2PhySusPgEnable = 0;

	for (i = 0; i < ARRAY_SIZE(config->usb3_ports); i++) {
		params->PortUsb30Enable[i] = config->usb3_ports[i].enable;
		params->Usb3OverCurrentPin[i] = config->usb3_ports[i].ocpin;
		if (config->usb3_ports[i].tx_de_emp) {
			params->Usb3HsioTxDeEmphEnable[i] = 1;
			params->Usb3HsioTxDeEmph[i] =
				config->usb3_ports[i].tx_de_emp;
		}
		if (config->usb3_ports[i].tx_downscale_amp) {
			params->Usb3HsioTxDownscaleAmpEnable[i] = 1;
			params->Usb3HsioTxDownscaleAmp[i] =
				config->usb3_ports[i].tx_downscale_amp;
		}
#if CONFIG(SOC_INTEL_COMETLAKE)
		if (config->usb3_ports[i].gen2_tx_rate0_uniq_tran_enable) {
			params->Usb3HsioTxRate0UniqTranEnable[i] = 1;
			params->Usb3HsioTxRate0UniqTran[i] =
				config->usb3_ports[i].gen2_tx_rate0_uniq_tran;
		}
		if (config->usb3_ports[i].gen2_tx_rate1_uniq_tran_enable) {
			params->Usb3HsioTxRate1UniqTranEnable[i] = 1;
			params->Usb3HsioTxRate1UniqTran[i] =
				config->usb3_ports[i].gen2_tx_rate1_uniq_tran;
		}
		if (config->usb3_ports[i].gen2_tx_rate2_uniq_tran_enable) {
			params->Usb3HsioTxRate2UniqTranEnable[i] = 1;
			params->Usb3HsioTxRate2UniqTran[i] =
				config->usb3_ports[i].gen2_tx_rate2_uniq_tran;
		}
		if (config->usb3_ports[i].gen2_tx_rate3_uniq_tran_enable) {
			params->Usb3HsioTxRate3UniqTranEnable[i] = 1;
			params->Usb3HsioTxRate3UniqTran[i] =
				config->usb3_ports[i].gen2_tx_rate3_uniq_tran;
		}
#endif
		if (config->usb3_ports[i].gen2_rx_tuning_enable) {
			params->PchUsbHsioRxTuningEnable[i] =
				config->usb3_ports[i].gen2_rx_tuning_enable;
			params->PchUsbHsioRxTuningParameters[i] =
				config->usb3_ports[i].gen2_rx_tuning_params;
			params->PchUsbHsioFilterSel[i] =
				config->usb3_ports[i].gen2_rx_filter_sel;
		}
	}

	/* Enable xDCI controller if enabled in devicetree and allowed */
	dev = pcidev_path_on_root(PCH_DEVFN_USBOTG);
	if (dev) {
		if (!xdci_can_enable())
			dev->enabled = 0;
		params->XdciEnable = dev->enabled;
	} else
		params->XdciEnable = 0;

	/* Set Debug serial port */
	params->SerialIoDebugUartNumber = CONFIG_UART_FOR_CONSOLE;
#if !CONFIG(SOC_INTEL_COMETLAKE)
	params->SerialIoEnableDebugUartAfterPost = CONFIG_INTEL_LPSS_UART_FOR_CONSOLE;
#endif

	/* Enable CNVi Wifi if enabled in device tree */
	dev = pcidev_path_on_root(PCH_DEVFN_CNViWIFI);
#if CONFIG(SOC_INTEL_COMETLAKE)
	if (dev)
		params->CnviMode = dev->enabled;
	else
		params->CnviMode = 0;
#else
	if (dev)
		params->PchCnviMode = dev->enabled;
	else
		params->PchCnviMode = 0;
#endif
	/* PCI Express */
	for (i = 0; i < ARRAY_SIZE(config->PcieClkSrcUsage); i++) {
		if (config->PcieClkSrcUsage[i] == 0)
			config->PcieClkSrcUsage[i] = PCIE_CLK_NOTUSED;
		else if (config->PcieClkSrcUsage[i] == PCIE_CLK_RP0)
			config->PcieClkSrcUsage[i] = 0;
	}
	memcpy(params->PcieClkSrcUsage, config->PcieClkSrcUsage,
	       sizeof(config->PcieClkSrcUsage));
	memcpy(params->PcieClkSrcClkReq, config->PcieClkSrcClkReq,
	       sizeof(config->PcieClkSrcClkReq));

	memcpy(params->PcieRpAdvancedErrorReporting,
		config->PcieRpAdvancedErrorReporting,
		sizeof(params->PcieRpAdvancedErrorReporting));

	memcpy(params->PcieRpLtrEnable, config->PcieRpLtrEnable,
	       sizeof(config->PcieRpLtrEnable));
	memcpy(params->PcieRpHotPlug, config->PcieRpHotPlug,
	       sizeof(params->PcieRpHotPlug));

	for (i = 0; i < CONFIG_MAX_ROOT_PORTS; i++) {
		params->PcieRpMaxPayload[i] = config->PcieRpMaxPayload[i];
		if (config->PcieRpAspm[i])
			params->PcieRpAspm[i] = config->PcieRpAspm[i] - 1;
	};


	/* eMMC and SD */
	dev = pcidev_path_on_root(PCH_DEVFN_EMMC);
	if (!dev)
		params->ScsEmmcEnabled = 0;
	else {
		params->ScsEmmcEnabled = dev->enabled;
		params->ScsEmmcHs400Enabled = config->ScsEmmcHs400Enabled;
		params->PchScsEmmcHs400DllDataValid = config->EmmcHs400DllNeed;
		if (config->EmmcHs400DllNeed == 1) {
			params->PchScsEmmcHs400RxStrobeDll1 =
				config->EmmcHs400RxStrobeDll1;
			params->PchScsEmmcHs400TxDataDll =
				config->EmmcHs400TxDataDll;
		}
	}

	dev = pcidev_path_on_root(PCH_DEVFN_SDCARD);
	if (!dev) {
		params->ScsSdCardEnabled = 0;
	} else {
		params->ScsSdCardEnabled = dev->enabled;
		params->SdCardPowerEnableActiveHigh =
			CONFIG(MB_HAS_ACTIVE_HIGH_SD_PWR_ENABLE);
#if CONFIG(SOC_INTEL_COMETLAKE)
		params->ScsSdCardWpPinEnabled = config->ScsSdCardWpPinEnabled;
#endif
	}

	dev = pcidev_path_on_root(PCH_DEVFN_UFS);
	if (!dev)
		params->ScsUfsEnabled = 0;
	else
		params->ScsUfsEnabled = dev->enabled;

	params->Heci3Enabled = config->Heci3Enabled;
#if !CONFIG(HECI_DISABLE_USING_SMM)
	params->Heci1Disabled = !config->HeciEnabled;
#endif
	params->Device4Enable = config->Device4Enable;

	/* Teton Glacier hybrid storage support */
	params->TetonGlacierMode = config->TetonGlacierMode;

	/* VrConfig Settings for 5 domains
	 * 0 = System Agent, 1 = IA Core, 2 = Ring,
	 * 3 = GT unsliced,  4 = GT sliced */
	for (i = 0; i < ARRAY_SIZE(config->domain_vr_config); i++)
		fill_vr_domain_config(params, i, &config->domain_vr_config[i]);

	/* Acoustic Noise Mitigation */
	params->AcousticNoiseMitigation = config->AcousticNoiseMitigation;
	params->SlowSlewRateForIa = config->SlowSlewRateForIa;
	params->SlowSlewRateForGt = config->SlowSlewRateForGt;
	params->SlowSlewRateForSa = config->SlowSlewRateForSa;
	params->SlowSlewRateForFivr = config->SlowSlewRateForFivr;
	params->FastPkgCRampDisableIa = config->FastPkgCRampDisableIa;
	params->FastPkgCRampDisableGt = config->FastPkgCRampDisableGt;
	params->FastPkgCRampDisableSa = config->FastPkgCRampDisableSa;
	params->FastPkgCRampDisableFivr = config->FastPkgCRampDisableFivr;

	/* Power Optimizer */
	params->SataPwrOptEnable = config->satapwroptimize;

	/* Disable PCH ACPI timer */
	params->EnableTcoTimer = !config->PmTimerDisabled;

	/* Apply minimum assertion width settings if non-zero */
	if (config->PchPmSlpS3MinAssert)
		params->PchPmSlpS3MinAssert = config->PchPmSlpS3MinAssert;
	if (config->PchPmSlpS4MinAssert)
		params->PchPmSlpS4MinAssert = config->PchPmSlpS4MinAssert;
	if (config->PchPmSlpSusMinAssert)
		params->PchPmSlpSusMinAssert = config->PchPmSlpSusMinAssert;
	if (config->PchPmSlpAMinAssert)
		params->PchPmSlpAMinAssert = config->PchPmSlpAMinAssert;

#if CONFIG(SOC_INTEL_COMETLAKE)
	if (config->PchPmPwrCycDur)
		params->PchPmPwrCycDur = get_pm_pwr_cyc_dur(config->PchPmSlpS4MinAssert,
				config->PchPmSlpS3MinAssert, config->PchPmSlpAMinAssert,
				config->PchPmPwrCycDur);
#endif

	/* Set TccActivationOffset */
	tconfig->TccActivationOffset = config->tcc_offset;

	/* Unlock all GPIO pads */
	tconfig->PchUnlockGpioPads = config->PchUnlockGpioPads;

	/* Set correct Sirq mode based on config */
	params->PchSirqEnable = config->serirq_mode != SERIRQ_OFF;
	params->PchSirqMode = config->serirq_mode == SERIRQ_CONTINUOUS;

	/*
	 * GSPI Chip Select parameters
	 * The GSPI driver assumes that CS0 is the used chip-select line,
	 * therefore only CS0 is configured below.
	 */
#if CONFIG(SOC_INTEL_COMETLAKE)
	configure_gspi_cs(0, config, &params->SerialIoSpi0CsPolarity[0],
			&params->SerialIoSpi0CsEnable[0],
			&params->SerialIoSpiDefaultCsOutput[0]);
	configure_gspi_cs(1, config, &params->SerialIoSpi1CsPolarity[0],
			&params->SerialIoSpi1CsEnable[0],
			&params->SerialIoSpiDefaultCsOutput[1]);
	configure_gspi_cs(2, config, &params->SerialIoSpi2CsPolarity[0],
			&params->SerialIoSpi2CsEnable[0],
			&params->SerialIoSpiDefaultCsOutput[2]);
#else
	for (i = 0; i < CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX; i++)
		configure_gspi_cs(i, config,
				&params->SerialIoSpiCsPolarity[0], NULL, NULL);
#endif

	/* Chipset Lockdown */
	if (get_lockdown_config() == CHIPSET_LOCKDOWN_COREBOOT) {
		tconfig->PchLockDownGlobalSmi = 0;
		tconfig->PchLockDownBiosInterface = 0;
		params->PchLockDownBiosLock = 0;
		params->PchLockDownRtcMemoryLock = 0;
#if CONFIG(SOC_INTEL_COMETLAKE)
		/*
		 * Skip SPI Flash Lockdown from inside FSP.
		 * Making this config "0" means FSP won't set the FLOCKDN bit
		 * of SPIBAR + 0x04 (i.e., Bit 15 of BIOS_HSFSTS_CTL).
		 * So, it becomes coreboot's responsibility to set this bit
		 * before end of POST for security concerns.
		 */
		params->SpiFlashCfgLockDown = 0;
#endif
	} else {
		tconfig->PchLockDownGlobalSmi = 1;
		tconfig->PchLockDownBiosInterface = 1;
		params->PchLockDownBiosLock = 1;
		params->PchLockDownRtcMemoryLock = 1;
#if CONFIG(SOC_INTEL_COMETLAKE)
		/*
		 * Enable SPI Flash Lockdown from inside FSP.
		 * Making this config "1" means FSP will set the FLOCKDN bit
		 * of SPIBAR + 0x04 (i.e., Bit 15 of BIOS_HSFSTS_CTL).
		 */
		params->SpiFlashCfgLockDown = 1;
#endif
	}

#if !CONFIG(SOC_INTEL_COMETLAKE)
	params->VrPowerDeliveryDesign = config->VrPowerDeliveryDesign;
#endif

	dev = pcidev_path_on_root(SA_DEVFN_IGD);
	if (CONFIG(RUN_FSP_GOP) && dev && dev->enabled)
		params->PeiGraphicsPeimInit = 1;
	else
		params->PeiGraphicsPeimInit = 0;
}

/* Mainboard GPIO Configuration */
__weak void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}

/* Return list of SOC LPSS controllers */
const pci_devfn_t *soc_lpss_controllers_list(size_t *size)
{
	*size = ARRAY_SIZE(serial_io_dev);
	return serial_io_dev;
}

/* Handle FSP logo params */
const struct cbmem_entry *soc_load_logo(FSPS_UPD *supd)
{
	return fsp_load_logo(&supd->FspsConfig.LogoPtr, &supd->FspsConfig.LogoSize);
}

#if CONFIG(SOC_INTEL_COMETLAKE)
void soc_display_fsps_upd_params(
	const FSPS_UPD *fsps_old_upd,
	const FSPS_UPD *fsps_new_upd)
{
	const FSP_S_CONFIG *new;
	const FSP_S_CONFIG *old;

	old = &fsps_old_upd->FspsConfig;
	new = &fsps_new_upd->FspsConfig;

	printk(BIOS_SPEW, "UPD values for SiliconInit:\n");

	#define DISPLAY_UPD(field) \
		fsp_display_upd_value(#field, sizeof(old->field), \
			old->field, new->field)

	DISPLAY_UPD(LogoPtr);
	DISPLAY_UPD(LogoSize);
	DISPLAY_UPD(GraphicsConfigPtr);
	DISPLAY_UPD(Device4Enable);
	DISPLAY_UPD(MicrocodeRegionBase);
	DISPLAY_UPD(MicrocodeRegionSize);
	DISPLAY_UPD(TurboMode);
	DISPLAY_UPD(PchDmiCwbEnable);
	DISPLAY_UPD(Heci3Enabled);
	DISPLAY_UPD(Heci1Disabled);
	DISPLAY_UPD(AmtEnabled);
	DISPLAY_UPD(WatchDogEnabled);
	DISPLAY_UPD(ManageabilityMode);
	DISPLAY_UPD(FwProgress);
	DISPLAY_UPD(AmtSolEnabled);
	DISPLAY_UPD(WatchDogTimerOs);
	DISPLAY_UPD(WatchDogTimerBios);
	DISPLAY_UPD(RemoteAssistance);
	DISPLAY_UPD(AmtKvmEnabled);
	DISPLAY_UPD(ForcMebxSyncUp);
	DISPLAY_UPD(CridEnable);
	DISPLAY_UPD(DmiAspm);
	// PegDeEmphasis[4]
	DISPLAY_UPD(PegDeEmphasis[0]);
	DISPLAY_UPD(PegDeEmphasis[1]);
	DISPLAY_UPD(PegDeEmphasis[2]);
	DISPLAY_UPD(PegDeEmphasis[3]);
	// PegSlotPowerLimitValue[4]
	DISPLAY_UPD(PegSlotPowerLimitValue[0]);
	DISPLAY_UPD(PegSlotPowerLimitValue[1]);
	DISPLAY_UPD(PegSlotPowerLimitValue[2]);
	DISPLAY_UPD(PegSlotPowerLimitValue[3]);
	// PegSlotPowerLimitScale[4]
	DISPLAY_UPD(PegSlotPowerLimitScale[0]);
	DISPLAY_UPD(PegSlotPowerLimitScale[1]);
	DISPLAY_UPD(PegSlotPowerLimitScale[2]);
	DISPLAY_UPD(PegSlotPowerLimitScale[3]);
	// PegPhysicalSlotNumber[4]
	DISPLAY_UPD(PegPhysicalSlotNumber[0]);
	DISPLAY_UPD(PegPhysicalSlotNumber[1]);
	DISPLAY_UPD(PegPhysicalSlotNumber[2]);
	DISPLAY_UPD(PegPhysicalSlotNumber[3]);
	DISPLAY_UPD(PavpEnable);
	DISPLAY_UPD(CdClock);
	DISPLAY_UPD(PeiGraphicsPeimInit);
	DISPLAY_UPD(GnaEnable);
	DISPLAY_UPD(X2ApicOptOutDeprecated);
	// VtdBaseAddressDeprecated[3]
	DISPLAY_UPD(VtdBaseAddressDeprecated[0]);
	DISPLAY_UPD(VtdBaseAddressDeprecated[1]);
	DISPLAY_UPD(VtdBaseAddressDeprecated[2]);
	DISPLAY_UPD(DdiPortEdp);
	DISPLAY_UPD(DdiPortBHpd);
	DISPLAY_UPD(DdiPortCHpd);
	DISPLAY_UPD(DdiPortDHpd);
	DISPLAY_UPD(DdiPortFHpd);
	DISPLAY_UPD(DdiPortBDdc);
	DISPLAY_UPD(DdiPortCDdc);
	DISPLAY_UPD(DdiPortDDdc);
	DISPLAY_UPD(DdiPortFDdc);
	DISPLAY_UPD(SkipS3CdClockInit);
	DISPLAY_UPD(DeltaT12PowerCycleDelay);
	DISPLAY_UPD(BltBufferAddress);
	DISPLAY_UPD(BltBufferSize);
	DISPLAY_UPD(ProgramGtChickenBits);
	// SaPostMemProductionRsvd[34]
	DISPLAY_UPD(SaPostMemProductionRsvd[0]);
	DISPLAY_UPD(SaPostMemProductionRsvd[1]);
	DISPLAY_UPD(SaPostMemProductionRsvd[2]);
	DISPLAY_UPD(SaPostMemProductionRsvd[3]);
	DISPLAY_UPD(SaPostMemProductionRsvd[4]);
	DISPLAY_UPD(SaPostMemProductionRsvd[5]);
	DISPLAY_UPD(SaPostMemProductionRsvd[6]);
	DISPLAY_UPD(SaPostMemProductionRsvd[7]);
	DISPLAY_UPD(SaPostMemProductionRsvd[8]);
	DISPLAY_UPD(SaPostMemProductionRsvd[9]);
	DISPLAY_UPD(SaPostMemProductionRsvd[10]);
	DISPLAY_UPD(SaPostMemProductionRsvd[11]);
	DISPLAY_UPD(SaPostMemProductionRsvd[12]);
	DISPLAY_UPD(SaPostMemProductionRsvd[13]);
	DISPLAY_UPD(SaPostMemProductionRsvd[14]);
	DISPLAY_UPD(SaPostMemProductionRsvd[15]);
	DISPLAY_UPD(SaPostMemProductionRsvd[16]);
	DISPLAY_UPD(SaPostMemProductionRsvd[17]);
	DISPLAY_UPD(SaPostMemProductionRsvd[18]);
	DISPLAY_UPD(SaPostMemProductionRsvd[19]);
	DISPLAY_UPD(SaPostMemProductionRsvd[20]);
	DISPLAY_UPD(SaPostMemProductionRsvd[21]);
	DISPLAY_UPD(SaPostMemProductionRsvd[22]);
	DISPLAY_UPD(SaPostMemProductionRsvd[23]);
	DISPLAY_UPD(SaPostMemProductionRsvd[24]);
	DISPLAY_UPD(SaPostMemProductionRsvd[25]);
	DISPLAY_UPD(SaPostMemProductionRsvd[26]);
	DISPLAY_UPD(SaPostMemProductionRsvd[27]);
	DISPLAY_UPD(SaPostMemProductionRsvd[28]);
	DISPLAY_UPD(SaPostMemProductionRsvd[29]);
	DISPLAY_UPD(SaPostMemProductionRsvd[30]);
	DISPLAY_UPD(SaPostMemProductionRsvd[31]);
	DISPLAY_UPD(SaPostMemProductionRsvd[32]);
	DISPLAY_UPD(SaPostMemProductionRsvd[33]);
	// PcieRootPortGen2PllL1CgDisable[24]
	DISPLAY_UPD(PcieRootPortGen2PllL1CgDisable[0]);
	DISPLAY_UPD(PcieRootPortGen2PllL1CgDisable[1]);
	DISPLAY_UPD(PcieRootPortGen2PllL1CgDisable[2]);
	DISPLAY_UPD(PcieRootPortGen2PllL1CgDisable[3]);
	DISPLAY_UPD(PcieRootPortGen2PllL1CgDisable[4]);
	DISPLAY_UPD(PcieRootPortGen2PllL1CgDisable[5]);
	DISPLAY_UPD(PcieRootPortGen2PllL1CgDisable[6]);
	DISPLAY_UPD(PcieRootPortGen2PllL1CgDisable[7]);
	DISPLAY_UPD(PcieRootPortGen2PllL1CgDisable[8]);
	DISPLAY_UPD(PcieRootPortGen2PllL1CgDisable[9]);
	DISPLAY_UPD(PcieRootPortGen2PllL1CgDisable[10]);
	DISPLAY_UPD(PcieRootPortGen2PllL1CgDisable[11]);
	DISPLAY_UPD(PcieRootPortGen2PllL1CgDisable[12]);
	DISPLAY_UPD(PcieRootPortGen2PllL1CgDisable[13]);
	DISPLAY_UPD(PcieRootPortGen2PllL1CgDisable[14]);
	DISPLAY_UPD(PcieRootPortGen2PllL1CgDisable[15]);
	DISPLAY_UPD(PcieRootPortGen2PllL1CgDisable[16]);
	DISPLAY_UPD(PcieRootPortGen2PllL1CgDisable[17]);
	DISPLAY_UPD(PcieRootPortGen2PllL1CgDisable[18]);
	DISPLAY_UPD(PcieRootPortGen2PllL1CgDisable[19]);
	DISPLAY_UPD(PcieRootPortGen2PllL1CgDisable[20]);
	DISPLAY_UPD(PcieRootPortGen2PllL1CgDisable[21]);
	DISPLAY_UPD(PcieRootPortGen2PllL1CgDisable[22]);
	DISPLAY_UPD(PcieRootPortGen2PllL1CgDisable[23]);
	DISPLAY_UPD(AesEnable);
	// Psi3Enable[5]
	DISPLAY_UPD(Psi3Enable[0]);
	DISPLAY_UPD(Psi3Enable[1]);
	DISPLAY_UPD(Psi3Enable[2]);
	DISPLAY_UPD(Psi3Enable[3]);
	DISPLAY_UPD(Psi3Enable[4]);
	// Psi4Enable[5]
	DISPLAY_UPD(Psi4Enable[0]);
	DISPLAY_UPD(Psi4Enable[1]);
	DISPLAY_UPD(Psi4Enable[2]);
	DISPLAY_UPD(Psi4Enable[3]);
	DISPLAY_UPD(Psi4Enable[4]);
	// ImonSlope[5]
	DISPLAY_UPD(ImonSlope[0]);
	DISPLAY_UPD(ImonSlope[1]);
	DISPLAY_UPD(ImonSlope[2]);
	DISPLAY_UPD(ImonSlope[3]);
	DISPLAY_UPD(ImonSlope[4]);
	// ImonOffset[5]
	DISPLAY_UPD(ImonOffset[0]);
	DISPLAY_UPD(ImonOffset[1]);
	DISPLAY_UPD(ImonOffset[2]);
	DISPLAY_UPD(ImonOffset[3]);
	DISPLAY_UPD(ImonOffset[4]);
	// VrConfigEnable[5]
	DISPLAY_UPD(VrConfigEnable[0]);
	DISPLAY_UPD(VrConfigEnable[1]);
	DISPLAY_UPD(VrConfigEnable[2]);
	DISPLAY_UPD(VrConfigEnable[3]);
	DISPLAY_UPD(VrConfigEnable[4]);
	// TdcEnable[5]
	DISPLAY_UPD(TdcEnable[0]);
	DISPLAY_UPD(TdcEnable[1]);
	DISPLAY_UPD(TdcEnable[2]);
	DISPLAY_UPD(TdcEnable[3]);
	DISPLAY_UPD(TdcEnable[4]);
	// TdcTimeWindow[5]
	DISPLAY_UPD(TdcTimeWindow[0]);
	DISPLAY_UPD(TdcTimeWindow[1]);
	DISPLAY_UPD(TdcTimeWindow[2]);
	DISPLAY_UPD(TdcTimeWindow[3]);
	DISPLAY_UPD(TdcTimeWindow[4]);
	// TdcLock[5]
	DISPLAY_UPD(TdcLock[0]);
	DISPLAY_UPD(TdcLock[1]);
	DISPLAY_UPD(TdcLock[2]);
	DISPLAY_UPD(TdcLock[3]);
	DISPLAY_UPD(TdcLock[4]);
	DISPLAY_UPD(PsysSlope);
	DISPLAY_UPD(PsysOffset);
	DISPLAY_UPD(AcousticNoiseMitigation);
	DISPLAY_UPD(FastPkgCRampDisableIa);
	DISPLAY_UPD(SlowSlewRateForIa);
	DISPLAY_UPD(SlowSlewRateForGt);
	DISPLAY_UPD(SlowSlewRateForSa);
	// TdcPowerLimit[5]
	DISPLAY_UPD(TdcPowerLimit[0]);
	DISPLAY_UPD(TdcPowerLimit[1]);
	DISPLAY_UPD(TdcPowerLimit[2]);
	DISPLAY_UPD(TdcPowerLimit[3]);
	DISPLAY_UPD(TdcPowerLimit[4]);
	// AcLoadline[5]
	DISPLAY_UPD(AcLoadline[0]);
	DISPLAY_UPD(AcLoadline[1]);
	DISPLAY_UPD(AcLoadline[2]);
	DISPLAY_UPD(AcLoadline[3]);
	DISPLAY_UPD(AcLoadline[4]);
	// DcLoadline[5]
	DISPLAY_UPD(DcLoadline[0]);
	DISPLAY_UPD(DcLoadline[1]);
	DISPLAY_UPD(DcLoadline[2]);
	DISPLAY_UPD(DcLoadline[3]);
	DISPLAY_UPD(DcLoadline[4]);
	// Psi1Threshold[5]
	DISPLAY_UPD(Psi1Threshold[0]);
	DISPLAY_UPD(Psi1Threshold[1]);
	DISPLAY_UPD(Psi1Threshold[2]);
	DISPLAY_UPD(Psi1Threshold[3]);
	DISPLAY_UPD(Psi1Threshold[4]);
	// Psi2Threshold[5]
	DISPLAY_UPD(Psi2Threshold[0]);
	DISPLAY_UPD(Psi2Threshold[1]);
	DISPLAY_UPD(Psi2Threshold[2]);
	DISPLAY_UPD(Psi2Threshold[3]);
	DISPLAY_UPD(Psi2Threshold[4]);
	// Psi3Threshold[5]
	DISPLAY_UPD(Psi3Threshold[0]);
	DISPLAY_UPD(Psi3Threshold[1]);
	DISPLAY_UPD(Psi3Threshold[2]);
	DISPLAY_UPD(Psi3Threshold[3]);
	DISPLAY_UPD(Psi3Threshold[4]);
	// IccMax[5]
	DISPLAY_UPD(IccMax[0]);
	DISPLAY_UPD(IccMax[1]);
	DISPLAY_UPD(IccMax[2]);
	DISPLAY_UPD(IccMax[3]);
	DISPLAY_UPD(IccMax[4]);
	// VrVoltageLimit[5]
	DISPLAY_UPD(VrVoltageLimit[0]);
	DISPLAY_UPD(VrVoltageLimit[1]);
	DISPLAY_UPD(VrVoltageLimit[2]);
	DISPLAY_UPD(VrVoltageLimit[3]);
	DISPLAY_UPD(VrVoltageLimit[4]);
	DISPLAY_UPD(FastPkgCRampDisableGt);
	DISPLAY_UPD(FastPkgCRampDisableSa);
	DISPLAY_UPD(SendVrMbxCmd);
	DISPLAY_UPD(TxtEnable);
	DISPLAY_UPD(SkipMpInitDeprecated);
	DISPLAY_UPD(McivrRfiFrequencyPrefix);
	DISPLAY_UPD(McivrRfiFrequencyAdjust);
	DISPLAY_UPD(FivrRfiFrequency);
	DISPLAY_UPD(McivrSpreadSpectrum);
	DISPLAY_UPD(FivrSpreadSpectrum);
	DISPLAY_UPD(FastPkgCRampDisableFivr);
	DISPLAY_UPD(SlowSlewRateForFivr);
	DISPLAY_UPD(CpuBistData);
	DISPLAY_UPD(IslVrCmd);
	// ImonSlope1[5]
	DISPLAY_UPD(ImonSlope1[0]);
	DISPLAY_UPD(ImonSlope1[1]);
	DISPLAY_UPD(ImonSlope1[2]);
	DISPLAY_UPD(ImonSlope1[3]);
	DISPLAY_UPD(ImonSlope1[4]);
	DISPLAY_UPD(VrPowerDeliveryDesign);
	DISPLAY_UPD(PreWake);
	DISPLAY_UPD(RampUp);
	DISPLAY_UPD(RampDown);
	DISPLAY_UPD(CpuMpPpi);
	DISPLAY_UPD(CpuMpHob);
	DISPLAY_UPD(DebugInterfaceEnable);
	// ImonOffset1[5]
	DISPLAY_UPD(ImonOffset1[0]);
	DISPLAY_UPD(ImonOffset1[1]);
	DISPLAY_UPD(ImonOffset1[2]);
	DISPLAY_UPD(ImonOffset1[3]);
	DISPLAY_UPD(ImonOffset1[4]);
	DISPLAY_UPD(PchHdaDspEnable);
	// SerialIoSpi0CsPolarity[2]
	DISPLAY_UPD(SerialIoSpi0CsPolarity[0]);
	DISPLAY_UPD(SerialIoSpi0CsPolarity[1]);
	// SerialIoSpi1CsPolarity[2]
	DISPLAY_UPD(SerialIoSpi1CsPolarity[0]);
	DISPLAY_UPD(SerialIoSpi1CsPolarity[1]);
	// SerialIoSpi2CsPolarity[2]
	DISPLAY_UPD(SerialIoSpi2CsPolarity[0]);
	DISPLAY_UPD(SerialIoSpi2CsPolarity[1]);
	// SerialIoSpi0CsEnable[2]
	DISPLAY_UPD(SerialIoSpi0CsEnable[0]);
	DISPLAY_UPD(SerialIoSpi0CsEnable[1]);
	// SerialIoSpi1CsEnable[2]
	DISPLAY_UPD(SerialIoSpi1CsEnable[0]);
	DISPLAY_UPD(SerialIoSpi1CsEnable[1]);
	// SerialIoSpi2CsEnable[2]
	DISPLAY_UPD(SerialIoSpi2CsEnable[0]);
	DISPLAY_UPD(SerialIoSpi2CsEnable[1]);
	// SerialIoSpiMode[3]
	DISPLAY_UPD(SerialIoSpiMode[0]);
	DISPLAY_UPD(SerialIoSpiMode[1]);
	DISPLAY_UPD(SerialIoSpiMode[2]);
	// SerialIoSpiDefaultCsOutput[3]
	DISPLAY_UPD(SerialIoSpiDefaultCsOutput[0]);
	DISPLAY_UPD(SerialIoSpiDefaultCsOutput[1]);
	DISPLAY_UPD(SerialIoSpiDefaultCsOutput[2]);
	// PchSerialIoI2cPadsTermination[6]
	DISPLAY_UPD(PchSerialIoI2cPadsTermination[0]);
	DISPLAY_UPD(PchSerialIoI2cPadsTermination[1]);
	DISPLAY_UPD(PchSerialIoI2cPadsTermination[2]);
	DISPLAY_UPD(PchSerialIoI2cPadsTermination[3]);
	DISPLAY_UPD(PchSerialIoI2cPadsTermination[4]);
	DISPLAY_UPD(PchSerialIoI2cPadsTermination[5]);
	// SerialIoI2cMode[6]
	DISPLAY_UPD(SerialIoI2cMode[0]);
	DISPLAY_UPD(SerialIoI2cMode[1]);
	DISPLAY_UPD(SerialIoI2cMode[2]);
	DISPLAY_UPD(SerialIoI2cMode[3]);
	DISPLAY_UPD(SerialIoI2cMode[4]);
	DISPLAY_UPD(SerialIoI2cMode[5]);
	// SerialIoUartMode[3]
	DISPLAY_UPD(SerialIoUartMode[0]);
	DISPLAY_UPD(SerialIoUartMode[1]);
	DISPLAY_UPD(SerialIoUartMode[2]);
	// SerialIoUartBaudRate[3]
	DISPLAY_UPD(SerialIoUartBaudRate[0]);
	DISPLAY_UPD(SerialIoUartBaudRate[1]);
	DISPLAY_UPD(SerialIoUartBaudRate[2]);
	// SerialIoUartParity[3]
	DISPLAY_UPD(SerialIoUartParity[0]);
	DISPLAY_UPD(SerialIoUartParity[1]);
	DISPLAY_UPD(SerialIoUartParity[2]);
	// SerialIoUartDataBits[3]
	DISPLAY_UPD(SerialIoUartDataBits[0]);
	DISPLAY_UPD(SerialIoUartDataBits[1]);
	DISPLAY_UPD(SerialIoUartDataBits[2]);
	// SerialIoUartStopBits[3]
	DISPLAY_UPD(SerialIoUartStopBits[0]);
	DISPLAY_UPD(SerialIoUartStopBits[1]);
	DISPLAY_UPD(SerialIoUartStopBits[2]);
	// SerialIoUartPowerGating[3]
	DISPLAY_UPD(SerialIoUartPowerGating[0]);
	DISPLAY_UPD(SerialIoUartPowerGating[1]);
	DISPLAY_UPD(SerialIoUartPowerGating[2]);
	// SerialIoUartDmaEnable[3]
	DISPLAY_UPD(SerialIoUartDmaEnable[0]);
	DISPLAY_UPD(SerialIoUartDmaEnable[1]);
	DISPLAY_UPD(SerialIoUartDmaEnable[2]);
	// SerialIoUartAutoFlow[3]
	DISPLAY_UPD(SerialIoUartAutoFlow[0]);
	DISPLAY_UPD(SerialIoUartAutoFlow[1]);
	DISPLAY_UPD(SerialIoUartAutoFlow[2]);
	// SerialIoUartPinMux[3]
	DISPLAY_UPD(SerialIoUartPinMux[0]);
	DISPLAY_UPD(SerialIoUartPinMux[1]);
	DISPLAY_UPD(SerialIoUartPinMux[2]);
	DISPLAY_UPD(SerialIoDebugUartNumber);
	// SerialIoUartDbg2[3]
	DISPLAY_UPD(SerialIoUartDbg2[0]);
	DISPLAY_UPD(SerialIoUartDbg2[1]);
	DISPLAY_UPD(SerialIoUartDbg2[2]);
	DISPLAY_UPD(ScsEmmcEnabled);
	DISPLAY_UPD(ScsEmmcHs400Enabled);
	DISPLAY_UPD(ScsSdCardEnabled);
	DISPLAY_UPD(ShowSpiController);
	DISPLAY_UPD(SataSalpSupport);
	// SataPortsEnable[8]
	DISPLAY_UPD(SataPortsEnable[0]);
	DISPLAY_UPD(SataPortsEnable[1]);
	DISPLAY_UPD(SataPortsEnable[2]);
	DISPLAY_UPD(SataPortsEnable[3]);
	DISPLAY_UPD(SataPortsEnable[4]);
	DISPLAY_UPD(SataPortsEnable[5]);
	DISPLAY_UPD(SataPortsEnable[6]);
	DISPLAY_UPD(SataPortsEnable[7]);
	// SataPortsDevSlp[8]
	DISPLAY_UPD(SataPortsDevSlp[0]);
	DISPLAY_UPD(SataPortsDevSlp[1]);
	DISPLAY_UPD(SataPortsDevSlp[2]);
	DISPLAY_UPD(SataPortsDevSlp[3]);
	DISPLAY_UPD(SataPortsDevSlp[4]);
	DISPLAY_UPD(SataPortsDevSlp[5]);
	DISPLAY_UPD(SataPortsDevSlp[6]);
	DISPLAY_UPD(SataPortsDevSlp[7]);
	// PortUsb20Enable[16]
	DISPLAY_UPD(PortUsb20Enable[0]);
	DISPLAY_UPD(PortUsb20Enable[1]);
	DISPLAY_UPD(PortUsb20Enable[2]);
	DISPLAY_UPD(PortUsb20Enable[3]);
	DISPLAY_UPD(PortUsb20Enable[4]);
	DISPLAY_UPD(PortUsb20Enable[5]);
	DISPLAY_UPD(PortUsb20Enable[6]);
	DISPLAY_UPD(PortUsb20Enable[7]);
	DISPLAY_UPD(PortUsb20Enable[8]);
	DISPLAY_UPD(PortUsb20Enable[9]);
	DISPLAY_UPD(PortUsb20Enable[10]);
	DISPLAY_UPD(PortUsb20Enable[11]);
	DISPLAY_UPD(PortUsb20Enable[12]);
	DISPLAY_UPD(PortUsb20Enable[13]);
	DISPLAY_UPD(PortUsb20Enable[14]);
	DISPLAY_UPD(PortUsb20Enable[15]);
	// PortUsb30Enable[10]
	DISPLAY_UPD(PortUsb30Enable[0]);
	DISPLAY_UPD(PortUsb30Enable[1]);
	DISPLAY_UPD(PortUsb30Enable[2]);
	DISPLAY_UPD(PortUsb30Enable[3]);
	DISPLAY_UPD(PortUsb30Enable[4]);
	DISPLAY_UPD(PortUsb30Enable[5]);
	DISPLAY_UPD(PortUsb30Enable[6]);
	DISPLAY_UPD(PortUsb30Enable[7]);
	DISPLAY_UPD(PortUsb30Enable[8]);
	DISPLAY_UPD(PortUsb30Enable[9]);
	DISPLAY_UPD(XdciEnable);
	DISPLAY_UPD(DevIntConfigPtr);
	DISPLAY_UPD(NumOfDevIntConfig);
	// PxRcConfig[8]
	DISPLAY_UPD(PxRcConfig[0]);
	DISPLAY_UPD(PxRcConfig[1]);
	DISPLAY_UPD(PxRcConfig[2]);
	DISPLAY_UPD(PxRcConfig[3]);
	DISPLAY_UPD(PxRcConfig[4]);
	DISPLAY_UPD(PxRcConfig[5]);
	DISPLAY_UPD(PxRcConfig[6]);
	DISPLAY_UPD(PxRcConfig[7]);
	DISPLAY_UPD(GpioIrqRoute);
	DISPLAY_UPD(SciIrqSelect);
	DISPLAY_UPD(TcoIrqSelect);
	DISPLAY_UPD(TcoIrqEnable);
	DISPLAY_UPD(PchHdaVerbTableEntryNum);
	DISPLAY_UPD(PchHdaVerbTablePtr);
	DISPLAY_UPD(PchHdaCodecSxWakeCapability);
	DISPLAY_UPD(SataEnable);
	DISPLAY_UPD(SataMode);
	// Usb2AfePetxiset[16]
	DISPLAY_UPD(Usb2AfePetxiset[0]);
	DISPLAY_UPD(Usb2AfePetxiset[1]);
	DISPLAY_UPD(Usb2AfePetxiset[2]);
	DISPLAY_UPD(Usb2AfePetxiset[3]);
	DISPLAY_UPD(Usb2AfePetxiset[4]);
	DISPLAY_UPD(Usb2AfePetxiset[5]);
	DISPLAY_UPD(Usb2AfePetxiset[6]);
	DISPLAY_UPD(Usb2AfePetxiset[7]);
	DISPLAY_UPD(Usb2AfePetxiset[8]);
	DISPLAY_UPD(Usb2AfePetxiset[9]);
	DISPLAY_UPD(Usb2AfePetxiset[10]);
	DISPLAY_UPD(Usb2AfePetxiset[11]);
	DISPLAY_UPD(Usb2AfePetxiset[12]);
	DISPLAY_UPD(Usb2AfePetxiset[13]);
	DISPLAY_UPD(Usb2AfePetxiset[14]);
	DISPLAY_UPD(Usb2AfePetxiset[15]);
	// Usb2AfeTxiset[16]
	DISPLAY_UPD(Usb2AfeTxiset[0]);
	DISPLAY_UPD(Usb2AfeTxiset[1]);
	DISPLAY_UPD(Usb2AfeTxiset[2]);
	DISPLAY_UPD(Usb2AfeTxiset[3]);
	DISPLAY_UPD(Usb2AfeTxiset[4]);
	DISPLAY_UPD(Usb2AfeTxiset[5]);
	DISPLAY_UPD(Usb2AfeTxiset[6]);
	DISPLAY_UPD(Usb2AfeTxiset[7]);
	DISPLAY_UPD(Usb2AfeTxiset[8]);
	DISPLAY_UPD(Usb2AfeTxiset[9]);
	DISPLAY_UPD(Usb2AfeTxiset[10]);
	DISPLAY_UPD(Usb2AfeTxiset[11]);
	DISPLAY_UPD(Usb2AfeTxiset[12]);
	DISPLAY_UPD(Usb2AfeTxiset[13]);
	DISPLAY_UPD(Usb2AfeTxiset[14]);
	DISPLAY_UPD(Usb2AfeTxiset[15]);
	// Usb2AfePredeemp[16]
	DISPLAY_UPD(Usb2AfePredeemp[0]);
	DISPLAY_UPD(Usb2AfePredeemp[1]);
	DISPLAY_UPD(Usb2AfePredeemp[2]);
	DISPLAY_UPD(Usb2AfePredeemp[3]);
	DISPLAY_UPD(Usb2AfePredeemp[4]);
	DISPLAY_UPD(Usb2AfePredeemp[5]);
	DISPLAY_UPD(Usb2AfePredeemp[6]);
	DISPLAY_UPD(Usb2AfePredeemp[7]);
	DISPLAY_UPD(Usb2AfePredeemp[8]);
	DISPLAY_UPD(Usb2AfePredeemp[9]);
	DISPLAY_UPD(Usb2AfePredeemp[10]);
	DISPLAY_UPD(Usb2AfePredeemp[11]);
	DISPLAY_UPD(Usb2AfePredeemp[12]);
	DISPLAY_UPD(Usb2AfePredeemp[13]);
	DISPLAY_UPD(Usb2AfePredeemp[14]);
	DISPLAY_UPD(Usb2AfePredeemp[15]);
	// Usb2AfePehalfbit[16]
	DISPLAY_UPD(Usb2AfePehalfbit[0]);
	DISPLAY_UPD(Usb2AfePehalfbit[1]);
	DISPLAY_UPD(Usb2AfePehalfbit[2]);
	DISPLAY_UPD(Usb2AfePehalfbit[3]);
	DISPLAY_UPD(Usb2AfePehalfbit[4]);
	DISPLAY_UPD(Usb2AfePehalfbit[5]);
	DISPLAY_UPD(Usb2AfePehalfbit[6]);
	DISPLAY_UPD(Usb2AfePehalfbit[7]);
	DISPLAY_UPD(Usb2AfePehalfbit[8]);
	DISPLAY_UPD(Usb2AfePehalfbit[9]);
	DISPLAY_UPD(Usb2AfePehalfbit[10]);
	DISPLAY_UPD(Usb2AfePehalfbit[11]);
	DISPLAY_UPD(Usb2AfePehalfbit[12]);
	DISPLAY_UPD(Usb2AfePehalfbit[13]);
	DISPLAY_UPD(Usb2AfePehalfbit[14]);
	DISPLAY_UPD(Usb2AfePehalfbit[15]);
	// Usb3HsioTxDeEmphEnable[10]
	DISPLAY_UPD(Usb3HsioTxDeEmphEnable[0]);
	DISPLAY_UPD(Usb3HsioTxDeEmphEnable[1]);
	DISPLAY_UPD(Usb3HsioTxDeEmphEnable[2]);
	DISPLAY_UPD(Usb3HsioTxDeEmphEnable[3]);
	DISPLAY_UPD(Usb3HsioTxDeEmphEnable[4]);
	DISPLAY_UPD(Usb3HsioTxDeEmphEnable[5]);
	DISPLAY_UPD(Usb3HsioTxDeEmphEnable[6]);
	DISPLAY_UPD(Usb3HsioTxDeEmphEnable[7]);
	DISPLAY_UPD(Usb3HsioTxDeEmphEnable[8]);
	DISPLAY_UPD(Usb3HsioTxDeEmphEnable[9]);
	// Usb3HsioTxDeEmph[10]
	DISPLAY_UPD(Usb3HsioTxDeEmph[0]);
	DISPLAY_UPD(Usb3HsioTxDeEmph[1]);
	DISPLAY_UPD(Usb3HsioTxDeEmph[2]);
	DISPLAY_UPD(Usb3HsioTxDeEmph[3]);
	DISPLAY_UPD(Usb3HsioTxDeEmph[4]);
	DISPLAY_UPD(Usb3HsioTxDeEmph[5]);
	DISPLAY_UPD(Usb3HsioTxDeEmph[6]);
	DISPLAY_UPD(Usb3HsioTxDeEmph[7]);
	DISPLAY_UPD(Usb3HsioTxDeEmph[8]);
	DISPLAY_UPD(Usb3HsioTxDeEmph[9]);
	// Usb3HsioTxDownscaleAmpEnable[10]
	DISPLAY_UPD(Usb3HsioTxDownscaleAmpEnable[0]);
	DISPLAY_UPD(Usb3HsioTxDownscaleAmpEnable[1]);
	DISPLAY_UPD(Usb3HsioTxDownscaleAmpEnable[2]);
	DISPLAY_UPD(Usb3HsioTxDownscaleAmpEnable[3]);
	DISPLAY_UPD(Usb3HsioTxDownscaleAmpEnable[4]);
	DISPLAY_UPD(Usb3HsioTxDownscaleAmpEnable[5]);
	DISPLAY_UPD(Usb3HsioTxDownscaleAmpEnable[6]);
	DISPLAY_UPD(Usb3HsioTxDownscaleAmpEnable[7]);
	DISPLAY_UPD(Usb3HsioTxDownscaleAmpEnable[8]);
	DISPLAY_UPD(Usb3HsioTxDownscaleAmpEnable[9]);
	// Usb3HsioTxDownscaleAmp[10]
	DISPLAY_UPD(Usb3HsioTxDownscaleAmp[0]);
	DISPLAY_UPD(Usb3HsioTxDownscaleAmp[1]);
	DISPLAY_UPD(Usb3HsioTxDownscaleAmp[2]);
	DISPLAY_UPD(Usb3HsioTxDownscaleAmp[3]);
	DISPLAY_UPD(Usb3HsioTxDownscaleAmp[4]);
	DISPLAY_UPD(Usb3HsioTxDownscaleAmp[5]);
	DISPLAY_UPD(Usb3HsioTxDownscaleAmp[6]);
	DISPLAY_UPD(Usb3HsioTxDownscaleAmp[7]);
	DISPLAY_UPD(Usb3HsioTxDownscaleAmp[8]);
	DISPLAY_UPD(Usb3HsioTxDownscaleAmp[9]);
	DISPLAY_UPD(PchUsbLtrOverrideEnable);
	DISPLAY_UPD(PchUsbLtrHighIdleTimeOverride);
	DISPLAY_UPD(PchUsbLtrMediumIdleTimeOverride);
	DISPLAY_UPD(PchUsbLtrLowIdleTimeOverride);
	DISPLAY_UPD(PchLanEnable);
	DISPLAY_UPD(PchHdaAudioLinkHda);
	DISPLAY_UPD(PchHdaAudioLinkDmic0);
	DISPLAY_UPD(PchHdaAudioLinkDmic1);
	DISPLAY_UPD(PchHdaAudioLinkSsp0);
	DISPLAY_UPD(PchHdaAudioLinkSsp1);
	DISPLAY_UPD(PchHdaAudioLinkSsp2);
	DISPLAY_UPD(PchHdaAudioLinkSndw1);
	DISPLAY_UPD(PchHdaAudioLinkSndw2);
	DISPLAY_UPD(PchHdaAudioLinkSndw3);
	DISPLAY_UPD(PchHdaAudioLinkSndw4);
	DISPLAY_UPD(PchHdaSndwBufferRcomp);
	DISPLAY_UPD(PcieRpPtmMask);
	DISPLAY_UPD(PcieRpDpcMask);
	DISPLAY_UPD(PcieRpDpcExtensionsMask);
	DISPLAY_UPD(UsbPdoProgramming);
	DISPLAY_UPD(PmcPowerButtonDebounce);
	DISPLAY_UPD(PchEspiBmeMasterSlaveEnabled);
	DISPLAY_UPD(SataRstLegacyOrom);
	DISPLAY_UPD(TraceHubMemBase);
	DISPLAY_UPD(PmcDbgMsgEn);
	DISPLAY_UPD(ChipsetInitBinPtr);
	DISPLAY_UPD(ChipsetInitBinLen);
	DISPLAY_UPD(ScsUfsEnabled);
	DISPLAY_UPD(CnviMode);
	DISPLAY_UPD(CnviBtCore);
	DISPLAY_UPD(CnviBtAudioOffload);
	DISPLAY_UPD(SdCardPowerEnableActiveHigh);
	DISPLAY_UPD(PchUsb2PhySusPgEnable);
	DISPLAY_UPD(PchUsbOverCurrentEnable);
	DISPLAY_UPD(PchEspiLgmrEnable);
	DISPLAY_UPD(PchHotEnable);
	DISPLAY_UPD(SataLedEnable);
	DISPLAY_UPD(PchPmVrAlert);
	DISPLAY_UPD(PchPmSlpS0VmRuntimeControl);
	DISPLAY_UPD(PchPmSlpS0Vm070VSupport);
	DISPLAY_UPD(PchPmSlpS0Vm075VSupport);
	// PcieRpSlotImplemented[24]
	DISPLAY_UPD(PcieRpSlotImplemented[0]);
	DISPLAY_UPD(PcieRpSlotImplemented[1]);
	DISPLAY_UPD(PcieRpSlotImplemented[2]);
	DISPLAY_UPD(PcieRpSlotImplemented[3]);
	DISPLAY_UPD(PcieRpSlotImplemented[4]);
	DISPLAY_UPD(PcieRpSlotImplemented[5]);
	DISPLAY_UPD(PcieRpSlotImplemented[6]);
	DISPLAY_UPD(PcieRpSlotImplemented[7]);
	DISPLAY_UPD(PcieRpSlotImplemented[8]);
	DISPLAY_UPD(PcieRpSlotImplemented[9]);
	DISPLAY_UPD(PcieRpSlotImplemented[10]);
	DISPLAY_UPD(PcieRpSlotImplemented[11]);
	DISPLAY_UPD(PcieRpSlotImplemented[12]);
	DISPLAY_UPD(PcieRpSlotImplemented[13]);
	DISPLAY_UPD(PcieRpSlotImplemented[14]);
	DISPLAY_UPD(PcieRpSlotImplemented[15]);
	DISPLAY_UPD(PcieRpSlotImplemented[16]);
	DISPLAY_UPD(PcieRpSlotImplemented[17]);
	DISPLAY_UPD(PcieRpSlotImplemented[18]);
	DISPLAY_UPD(PcieRpSlotImplemented[19]);
	DISPLAY_UPD(PcieRpSlotImplemented[20]);
	DISPLAY_UPD(PcieRpSlotImplemented[21]);
	DISPLAY_UPD(PcieRpSlotImplemented[22]);
	DISPLAY_UPD(PcieRpSlotImplemented[23]);
	// PcieClkSrcUsage[16]
	DISPLAY_UPD(PcieClkSrcUsage[0]);
	DISPLAY_UPD(PcieClkSrcUsage[1]);
	DISPLAY_UPD(PcieClkSrcUsage[2]);
	DISPLAY_UPD(PcieClkSrcUsage[3]);
	DISPLAY_UPD(PcieClkSrcUsage[4]);
	DISPLAY_UPD(PcieClkSrcUsage[5]);
	DISPLAY_UPD(PcieClkSrcUsage[6]);
	DISPLAY_UPD(PcieClkSrcUsage[7]);
	DISPLAY_UPD(PcieClkSrcUsage[8]);
	DISPLAY_UPD(PcieClkSrcUsage[9]);
	DISPLAY_UPD(PcieClkSrcUsage[10]);
	DISPLAY_UPD(PcieClkSrcUsage[11]);
	DISPLAY_UPD(PcieClkSrcUsage[12]);
	DISPLAY_UPD(PcieClkSrcUsage[13]);
	DISPLAY_UPD(PcieClkSrcUsage[14]);
	DISPLAY_UPD(PcieClkSrcUsage[15]);
	// PcieClkSrcClkReq[16]
	DISPLAY_UPD(PcieClkSrcClkReq[0]);
	DISPLAY_UPD(PcieClkSrcClkReq[1]);
	DISPLAY_UPD(PcieClkSrcClkReq[2]);
	DISPLAY_UPD(PcieClkSrcClkReq[3]);
	DISPLAY_UPD(PcieClkSrcClkReq[4]);
	DISPLAY_UPD(PcieClkSrcClkReq[5]);
	DISPLAY_UPD(PcieClkSrcClkReq[6]);
	DISPLAY_UPD(PcieClkSrcClkReq[7]);
	DISPLAY_UPD(PcieClkSrcClkReq[8]);
	DISPLAY_UPD(PcieClkSrcClkReq[9]);
	DISPLAY_UPD(PcieClkSrcClkReq[10]);
	DISPLAY_UPD(PcieClkSrcClkReq[11]);
	DISPLAY_UPD(PcieClkSrcClkReq[12]);
	DISPLAY_UPD(PcieClkSrcClkReq[13]);
	DISPLAY_UPD(PcieClkSrcClkReq[14]);
	DISPLAY_UPD(PcieClkSrcClkReq[15]);
	// PcieRpAcsEnabled[24]
	DISPLAY_UPD(PcieRpAcsEnabled[0]);
	DISPLAY_UPD(PcieRpAcsEnabled[1]);
	DISPLAY_UPD(PcieRpAcsEnabled[2]);
	DISPLAY_UPD(PcieRpAcsEnabled[3]);
	DISPLAY_UPD(PcieRpAcsEnabled[4]);
	DISPLAY_UPD(PcieRpAcsEnabled[5]);
	DISPLAY_UPD(PcieRpAcsEnabled[6]);
	DISPLAY_UPD(PcieRpAcsEnabled[7]);
	DISPLAY_UPD(PcieRpAcsEnabled[8]);
	DISPLAY_UPD(PcieRpAcsEnabled[9]);
	DISPLAY_UPD(PcieRpAcsEnabled[10]);
	DISPLAY_UPD(PcieRpAcsEnabled[11]);
	DISPLAY_UPD(PcieRpAcsEnabled[12]);
	DISPLAY_UPD(PcieRpAcsEnabled[13]);
	DISPLAY_UPD(PcieRpAcsEnabled[14]);
	DISPLAY_UPD(PcieRpAcsEnabled[15]);
	DISPLAY_UPD(PcieRpAcsEnabled[16]);
	DISPLAY_UPD(PcieRpAcsEnabled[17]);
	DISPLAY_UPD(PcieRpAcsEnabled[18]);
	DISPLAY_UPD(PcieRpAcsEnabled[19]);
	DISPLAY_UPD(PcieRpAcsEnabled[20]);
	DISPLAY_UPD(PcieRpAcsEnabled[21]);
	DISPLAY_UPD(PcieRpAcsEnabled[22]);
	DISPLAY_UPD(PcieRpAcsEnabled[23]);
	// PcieRpEnableCpm[24]
	DISPLAY_UPD(PcieRpEnableCpm[0]);
	DISPLAY_UPD(PcieRpEnableCpm[1]);
	DISPLAY_UPD(PcieRpEnableCpm[2]);
	DISPLAY_UPD(PcieRpEnableCpm[3]);
	DISPLAY_UPD(PcieRpEnableCpm[4]);
	DISPLAY_UPD(PcieRpEnableCpm[5]);
	DISPLAY_UPD(PcieRpEnableCpm[6]);
	DISPLAY_UPD(PcieRpEnableCpm[7]);
	DISPLAY_UPD(PcieRpEnableCpm[8]);
	DISPLAY_UPD(PcieRpEnableCpm[9]);
	DISPLAY_UPD(PcieRpEnableCpm[10]);
	DISPLAY_UPD(PcieRpEnableCpm[11]);
	DISPLAY_UPD(PcieRpEnableCpm[12]);
	DISPLAY_UPD(PcieRpEnableCpm[13]);
	DISPLAY_UPD(PcieRpEnableCpm[14]);
	DISPLAY_UPD(PcieRpEnableCpm[15]);
	DISPLAY_UPD(PcieRpEnableCpm[16]);
	DISPLAY_UPD(PcieRpEnableCpm[17]);
	DISPLAY_UPD(PcieRpEnableCpm[18]);
	DISPLAY_UPD(PcieRpEnableCpm[19]);
	DISPLAY_UPD(PcieRpEnableCpm[20]);
	DISPLAY_UPD(PcieRpEnableCpm[21]);
	DISPLAY_UPD(PcieRpEnableCpm[22]);
	DISPLAY_UPD(PcieRpEnableCpm[23]);
	// PcieRpDetectTimeoutMs[24]
	DISPLAY_UPD(PcieRpDetectTimeoutMs[0]);
	DISPLAY_UPD(PcieRpDetectTimeoutMs[1]);
	DISPLAY_UPD(PcieRpDetectTimeoutMs[2]);
	DISPLAY_UPD(PcieRpDetectTimeoutMs[3]);
	DISPLAY_UPD(PcieRpDetectTimeoutMs[4]);
	DISPLAY_UPD(PcieRpDetectTimeoutMs[5]);
	DISPLAY_UPD(PcieRpDetectTimeoutMs[6]);
	DISPLAY_UPD(PcieRpDetectTimeoutMs[7]);
	DISPLAY_UPD(PcieRpDetectTimeoutMs[8]);
	DISPLAY_UPD(PcieRpDetectTimeoutMs[9]);
	DISPLAY_UPD(PcieRpDetectTimeoutMs[10]);
	DISPLAY_UPD(PcieRpDetectTimeoutMs[11]);
	DISPLAY_UPD(PcieRpDetectTimeoutMs[12]);
	DISPLAY_UPD(PcieRpDetectTimeoutMs[13]);
	DISPLAY_UPD(PcieRpDetectTimeoutMs[14]);
	DISPLAY_UPD(PcieRpDetectTimeoutMs[15]);
	DISPLAY_UPD(PcieRpDetectTimeoutMs[16]);
	DISPLAY_UPD(PcieRpDetectTimeoutMs[17]);
	DISPLAY_UPD(PcieRpDetectTimeoutMs[18]);
	DISPLAY_UPD(PcieRpDetectTimeoutMs[19]);
	DISPLAY_UPD(PcieRpDetectTimeoutMs[20]);
	DISPLAY_UPD(PcieRpDetectTimeoutMs[21]);
	DISPLAY_UPD(PcieRpDetectTimeoutMs[22]);
	DISPLAY_UPD(PcieRpDetectTimeoutMs[23]);
	DISPLAY_UPD(PmcModPhySusPgEnable);
	DISPLAY_UPD(SlpS0WithGbeSupport);
	DISPLAY_UPD(PchPwrOptEnable);
	// PchWriteProtectionEnable[5]
	DISPLAY_UPD(PchWriteProtectionEnable[0]);
	DISPLAY_UPD(PchWriteProtectionEnable[1]);
	DISPLAY_UPD(PchWriteProtectionEnable[2]);
	DISPLAY_UPD(PchWriteProtectionEnable[3]);
	DISPLAY_UPD(PchWriteProtectionEnable[4]);
	// PchReadProtectionEnable[5]
	DISPLAY_UPD(PchReadProtectionEnable[0]);
	DISPLAY_UPD(PchReadProtectionEnable[1]);
	DISPLAY_UPD(PchReadProtectionEnable[2]);
	DISPLAY_UPD(PchReadProtectionEnable[3]);
	DISPLAY_UPD(PchReadProtectionEnable[4]);
	// PchProtectedRangeLimit[5]
	DISPLAY_UPD(PchProtectedRangeLimit[0]);
	DISPLAY_UPD(PchProtectedRangeLimit[1]);
	DISPLAY_UPD(PchProtectedRangeLimit[2]);
	DISPLAY_UPD(PchProtectedRangeLimit[3]);
	DISPLAY_UPD(PchProtectedRangeLimit[4]);
	// PchProtectedRangeBase[5]
	DISPLAY_UPD(PchProtectedRangeBase[0]);
	DISPLAY_UPD(PchProtectedRangeBase[1]);
	DISPLAY_UPD(PchProtectedRangeBase[2]);
	DISPLAY_UPD(PchProtectedRangeBase[3]);
	DISPLAY_UPD(PchProtectedRangeBase[4]);
	DISPLAY_UPD(PchHdaPme);
	DISPLAY_UPD(PchHdaVcType);
	DISPLAY_UPD(PchHdaLinkFrequency);
	DISPLAY_UPD(PchHdaIDispLinkFrequency);
	DISPLAY_UPD(PchHdaIDispLinkTmode);
	DISPLAY_UPD(PchHdaDspUaaCompliance);
	DISPLAY_UPD(PchHdaIDispCodecDisconnect);
	// PchUsbHsioFilterSel[10]
	DISPLAY_UPD(PchUsbHsioFilterSel[0]);
	DISPLAY_UPD(PchUsbHsioFilterSel[1]);
	DISPLAY_UPD(PchUsbHsioFilterSel[2]);
	DISPLAY_UPD(PchUsbHsioFilterSel[3]);
	DISPLAY_UPD(PchUsbHsioFilterSel[4]);
	DISPLAY_UPD(PchUsbHsioFilterSel[5]);
	DISPLAY_UPD(PchUsbHsioFilterSel[6]);
	DISPLAY_UPD(PchUsbHsioFilterSel[7]);
	DISPLAY_UPD(PchUsbHsioFilterSel[8]);
	DISPLAY_UPD(PchUsbHsioFilterSel[9]);
	DISPLAY_UPD(PchIoApicEntry24_119);
	DISPLAY_UPD(PchIoApicId);
	DISPLAY_UPD(PchIshSpiGpioAssign);
	DISPLAY_UPD(PchIshUart0GpioAssign);
	DISPLAY_UPD(PchIshUart1GpioAssign);
	DISPLAY_UPD(PchIshI2c0GpioAssign);
	DISPLAY_UPD(PchIshI2c1GpioAssign);
	DISPLAY_UPD(PchIshI2c2GpioAssign);
	DISPLAY_UPD(PchIshGp0GpioAssign);
	DISPLAY_UPD(PchIshGp1GpioAssign);
	DISPLAY_UPD(PchIshGp2GpioAssign);
	DISPLAY_UPD(PchIshGp3GpioAssign);
	DISPLAY_UPD(PchIshGp4GpioAssign);
	DISPLAY_UPD(PchIshGp5GpioAssign);
	DISPLAY_UPD(PchIshGp6GpioAssign);
	DISPLAY_UPD(PchIshGp7GpioAssign);
	DISPLAY_UPD(PchIshPdtUnlock);
	DISPLAY_UPD(PchLanLtrEnable);
	DISPLAY_UPD(PchLockDownBiosLock);
	DISPLAY_UPD(PchCrid);
	DISPLAY_UPD(PchLockDownRtcMemoryLock);
	// PcieRpHotPlug[24]
	DISPLAY_UPD(PcieRpHotPlug[0]);
	DISPLAY_UPD(PcieRpHotPlug[1]);
	DISPLAY_UPD(PcieRpHotPlug[2]);
	DISPLAY_UPD(PcieRpHotPlug[3]);
	DISPLAY_UPD(PcieRpHotPlug[4]);
	DISPLAY_UPD(PcieRpHotPlug[5]);
	DISPLAY_UPD(PcieRpHotPlug[6]);
	DISPLAY_UPD(PcieRpHotPlug[7]);
	DISPLAY_UPD(PcieRpHotPlug[8]);
	DISPLAY_UPD(PcieRpHotPlug[9]);
	DISPLAY_UPD(PcieRpHotPlug[10]);
	DISPLAY_UPD(PcieRpHotPlug[11]);
	DISPLAY_UPD(PcieRpHotPlug[12]);
	DISPLAY_UPD(PcieRpHotPlug[13]);
	DISPLAY_UPD(PcieRpHotPlug[14]);
	DISPLAY_UPD(PcieRpHotPlug[15]);
	DISPLAY_UPD(PcieRpHotPlug[16]);
	DISPLAY_UPD(PcieRpHotPlug[17]);
	DISPLAY_UPD(PcieRpHotPlug[18]);
	DISPLAY_UPD(PcieRpHotPlug[19]);
	DISPLAY_UPD(PcieRpHotPlug[20]);
	DISPLAY_UPD(PcieRpHotPlug[21]);
	DISPLAY_UPD(PcieRpHotPlug[22]);
	DISPLAY_UPD(PcieRpHotPlug[23]);
	// PcieRpPmSci[24]
	DISPLAY_UPD(PcieRpPmSci[0]);
	DISPLAY_UPD(PcieRpPmSci[1]);
	DISPLAY_UPD(PcieRpPmSci[2]);
	DISPLAY_UPD(PcieRpPmSci[3]);
	DISPLAY_UPD(PcieRpPmSci[4]);
	DISPLAY_UPD(PcieRpPmSci[5]);
	DISPLAY_UPD(PcieRpPmSci[6]);
	DISPLAY_UPD(PcieRpPmSci[7]);
	DISPLAY_UPD(PcieRpPmSci[8]);
	DISPLAY_UPD(PcieRpPmSci[9]);
	DISPLAY_UPD(PcieRpPmSci[10]);
	DISPLAY_UPD(PcieRpPmSci[11]);
	DISPLAY_UPD(PcieRpPmSci[12]);
	DISPLAY_UPD(PcieRpPmSci[13]);
	DISPLAY_UPD(PcieRpPmSci[14]);
	DISPLAY_UPD(PcieRpPmSci[15]);
	DISPLAY_UPD(PcieRpPmSci[16]);
	DISPLAY_UPD(PcieRpPmSci[17]);
	DISPLAY_UPD(PcieRpPmSci[18]);
	DISPLAY_UPD(PcieRpPmSci[19]);
	DISPLAY_UPD(PcieRpPmSci[20]);
	DISPLAY_UPD(PcieRpPmSci[21]);
	DISPLAY_UPD(PcieRpPmSci[22]);
	DISPLAY_UPD(PcieRpPmSci[23]);
	// PcieRpExtSync[24]
	DISPLAY_UPD(PcieRpExtSync[0]);
	DISPLAY_UPD(PcieRpExtSync[1]);
	DISPLAY_UPD(PcieRpExtSync[2]);
	DISPLAY_UPD(PcieRpExtSync[3]);
	DISPLAY_UPD(PcieRpExtSync[4]);
	DISPLAY_UPD(PcieRpExtSync[5]);
	DISPLAY_UPD(PcieRpExtSync[6]);
	DISPLAY_UPD(PcieRpExtSync[7]);
	DISPLAY_UPD(PcieRpExtSync[8]);
	DISPLAY_UPD(PcieRpExtSync[9]);
	DISPLAY_UPD(PcieRpExtSync[10]);
	DISPLAY_UPD(PcieRpExtSync[11]);
	DISPLAY_UPD(PcieRpExtSync[12]);
	DISPLAY_UPD(PcieRpExtSync[13]);
	DISPLAY_UPD(PcieRpExtSync[14]);
	DISPLAY_UPD(PcieRpExtSync[15]);
	DISPLAY_UPD(PcieRpExtSync[16]);
	DISPLAY_UPD(PcieRpExtSync[17]);
	DISPLAY_UPD(PcieRpExtSync[18]);
	DISPLAY_UPD(PcieRpExtSync[19]);
	DISPLAY_UPD(PcieRpExtSync[20]);
	DISPLAY_UPD(PcieRpExtSync[21]);
	DISPLAY_UPD(PcieRpExtSync[22]);
	DISPLAY_UPD(PcieRpExtSync[23]);
	// PcieRpTransmitterHalfSwing[24]
	DISPLAY_UPD(PcieRpTransmitterHalfSwing[0]);
	DISPLAY_UPD(PcieRpTransmitterHalfSwing[1]);
	DISPLAY_UPD(PcieRpTransmitterHalfSwing[2]);
	DISPLAY_UPD(PcieRpTransmitterHalfSwing[3]);
	DISPLAY_UPD(PcieRpTransmitterHalfSwing[4]);
	DISPLAY_UPD(PcieRpTransmitterHalfSwing[5]);
	DISPLAY_UPD(PcieRpTransmitterHalfSwing[6]);
	DISPLAY_UPD(PcieRpTransmitterHalfSwing[7]);
	DISPLAY_UPD(PcieRpTransmitterHalfSwing[8]);
	DISPLAY_UPD(PcieRpTransmitterHalfSwing[9]);
	DISPLAY_UPD(PcieRpTransmitterHalfSwing[10]);
	DISPLAY_UPD(PcieRpTransmitterHalfSwing[11]);
	DISPLAY_UPD(PcieRpTransmitterHalfSwing[12]);
	DISPLAY_UPD(PcieRpTransmitterHalfSwing[13]);
	DISPLAY_UPD(PcieRpTransmitterHalfSwing[14]);
	DISPLAY_UPD(PcieRpTransmitterHalfSwing[15]);
	DISPLAY_UPD(PcieRpTransmitterHalfSwing[16]);
	DISPLAY_UPD(PcieRpTransmitterHalfSwing[17]);
	DISPLAY_UPD(PcieRpTransmitterHalfSwing[18]);
	DISPLAY_UPD(PcieRpTransmitterHalfSwing[19]);
	DISPLAY_UPD(PcieRpTransmitterHalfSwing[20]);
	DISPLAY_UPD(PcieRpTransmitterHalfSwing[21]);
	DISPLAY_UPD(PcieRpTransmitterHalfSwing[22]);
	DISPLAY_UPD(PcieRpTransmitterHalfSwing[23]);
	// PcieRpClkReqDetect[24]
	DISPLAY_UPD(PcieRpClkReqDetect[0]);
	DISPLAY_UPD(PcieRpClkReqDetect[1]);
	DISPLAY_UPD(PcieRpClkReqDetect[2]);
	DISPLAY_UPD(PcieRpClkReqDetect[3]);
	DISPLAY_UPD(PcieRpClkReqDetect[4]);
	DISPLAY_UPD(PcieRpClkReqDetect[5]);
	DISPLAY_UPD(PcieRpClkReqDetect[6]);
	DISPLAY_UPD(PcieRpClkReqDetect[7]);
	DISPLAY_UPD(PcieRpClkReqDetect[8]);
	DISPLAY_UPD(PcieRpClkReqDetect[9]);
	DISPLAY_UPD(PcieRpClkReqDetect[10]);
	DISPLAY_UPD(PcieRpClkReqDetect[11]);
	DISPLAY_UPD(PcieRpClkReqDetect[12]);
	DISPLAY_UPD(PcieRpClkReqDetect[13]);
	DISPLAY_UPD(PcieRpClkReqDetect[14]);
	DISPLAY_UPD(PcieRpClkReqDetect[15]);
	DISPLAY_UPD(PcieRpClkReqDetect[16]);
	DISPLAY_UPD(PcieRpClkReqDetect[17]);
	DISPLAY_UPD(PcieRpClkReqDetect[18]);
	DISPLAY_UPD(PcieRpClkReqDetect[19]);
	DISPLAY_UPD(PcieRpClkReqDetect[20]);
	DISPLAY_UPD(PcieRpClkReqDetect[21]);
	DISPLAY_UPD(PcieRpClkReqDetect[22]);
	DISPLAY_UPD(PcieRpClkReqDetect[23]);
	// PcieRpAdvancedErrorReporting[24]
	DISPLAY_UPD(PcieRpAdvancedErrorReporting[0]);
	DISPLAY_UPD(PcieRpAdvancedErrorReporting[1]);
	DISPLAY_UPD(PcieRpAdvancedErrorReporting[2]);
	DISPLAY_UPD(PcieRpAdvancedErrorReporting[3]);
	DISPLAY_UPD(PcieRpAdvancedErrorReporting[4]);
	DISPLAY_UPD(PcieRpAdvancedErrorReporting[5]);
	DISPLAY_UPD(PcieRpAdvancedErrorReporting[6]);
	DISPLAY_UPD(PcieRpAdvancedErrorReporting[7]);
	DISPLAY_UPD(PcieRpAdvancedErrorReporting[8]);
	DISPLAY_UPD(PcieRpAdvancedErrorReporting[9]);
	DISPLAY_UPD(PcieRpAdvancedErrorReporting[10]);
	DISPLAY_UPD(PcieRpAdvancedErrorReporting[11]);
	DISPLAY_UPD(PcieRpAdvancedErrorReporting[12]);
	DISPLAY_UPD(PcieRpAdvancedErrorReporting[13]);
	DISPLAY_UPD(PcieRpAdvancedErrorReporting[14]);
	DISPLAY_UPD(PcieRpAdvancedErrorReporting[15]);
	DISPLAY_UPD(PcieRpAdvancedErrorReporting[16]);
	DISPLAY_UPD(PcieRpAdvancedErrorReporting[17]);
	DISPLAY_UPD(PcieRpAdvancedErrorReporting[18]);
	DISPLAY_UPD(PcieRpAdvancedErrorReporting[19]);
	DISPLAY_UPD(PcieRpAdvancedErrorReporting[20]);
	DISPLAY_UPD(PcieRpAdvancedErrorReporting[21]);
	DISPLAY_UPD(PcieRpAdvancedErrorReporting[22]);
	DISPLAY_UPD(PcieRpAdvancedErrorReporting[23]);
	// PcieRpUnsupportedRequestReport[24]
	DISPLAY_UPD(PcieRpUnsupportedRequestReport[0]);
	DISPLAY_UPD(PcieRpUnsupportedRequestReport[1]);
	DISPLAY_UPD(PcieRpUnsupportedRequestReport[2]);
	DISPLAY_UPD(PcieRpUnsupportedRequestReport[3]);
	DISPLAY_UPD(PcieRpUnsupportedRequestReport[4]);
	DISPLAY_UPD(PcieRpUnsupportedRequestReport[5]);
	DISPLAY_UPD(PcieRpUnsupportedRequestReport[6]);
	DISPLAY_UPD(PcieRpUnsupportedRequestReport[7]);
	DISPLAY_UPD(PcieRpUnsupportedRequestReport[8]);
	DISPLAY_UPD(PcieRpUnsupportedRequestReport[9]);
	DISPLAY_UPD(PcieRpUnsupportedRequestReport[10]);
	DISPLAY_UPD(PcieRpUnsupportedRequestReport[11]);
	DISPLAY_UPD(PcieRpUnsupportedRequestReport[12]);
	DISPLAY_UPD(PcieRpUnsupportedRequestReport[13]);
	DISPLAY_UPD(PcieRpUnsupportedRequestReport[14]);
	DISPLAY_UPD(PcieRpUnsupportedRequestReport[15]);
	DISPLAY_UPD(PcieRpUnsupportedRequestReport[16]);
	DISPLAY_UPD(PcieRpUnsupportedRequestReport[17]);
	DISPLAY_UPD(PcieRpUnsupportedRequestReport[18]);
	DISPLAY_UPD(PcieRpUnsupportedRequestReport[19]);
	DISPLAY_UPD(PcieRpUnsupportedRequestReport[20]);
	DISPLAY_UPD(PcieRpUnsupportedRequestReport[21]);
	DISPLAY_UPD(PcieRpUnsupportedRequestReport[22]);
	DISPLAY_UPD(PcieRpUnsupportedRequestReport[23]);
	// PcieRpFatalErrorReport[24]
	DISPLAY_UPD(PcieRpFatalErrorReport[0]);
	DISPLAY_UPD(PcieRpFatalErrorReport[1]);
	DISPLAY_UPD(PcieRpFatalErrorReport[2]);
	DISPLAY_UPD(PcieRpFatalErrorReport[3]);
	DISPLAY_UPD(PcieRpFatalErrorReport[4]);
	DISPLAY_UPD(PcieRpFatalErrorReport[5]);
	DISPLAY_UPD(PcieRpFatalErrorReport[6]);
	DISPLAY_UPD(PcieRpFatalErrorReport[7]);
	DISPLAY_UPD(PcieRpFatalErrorReport[8]);
	DISPLAY_UPD(PcieRpFatalErrorReport[9]);
	DISPLAY_UPD(PcieRpFatalErrorReport[10]);
	DISPLAY_UPD(PcieRpFatalErrorReport[11]);
	DISPLAY_UPD(PcieRpFatalErrorReport[12]);
	DISPLAY_UPD(PcieRpFatalErrorReport[13]);
	DISPLAY_UPD(PcieRpFatalErrorReport[14]);
	DISPLAY_UPD(PcieRpFatalErrorReport[15]);
	DISPLAY_UPD(PcieRpFatalErrorReport[16]);
	DISPLAY_UPD(PcieRpFatalErrorReport[17]);
	DISPLAY_UPD(PcieRpFatalErrorReport[18]);
	DISPLAY_UPD(PcieRpFatalErrorReport[19]);
	DISPLAY_UPD(PcieRpFatalErrorReport[20]);
	DISPLAY_UPD(PcieRpFatalErrorReport[21]);
	DISPLAY_UPD(PcieRpFatalErrorReport[22]);
	DISPLAY_UPD(PcieRpFatalErrorReport[23]);
	// PcieRpNoFatalErrorReport[24]
	DISPLAY_UPD(PcieRpNoFatalErrorReport[0]);
	DISPLAY_UPD(PcieRpNoFatalErrorReport[1]);
	DISPLAY_UPD(PcieRpNoFatalErrorReport[2]);
	DISPLAY_UPD(PcieRpNoFatalErrorReport[3]);
	DISPLAY_UPD(PcieRpNoFatalErrorReport[4]);
	DISPLAY_UPD(PcieRpNoFatalErrorReport[5]);
	DISPLAY_UPD(PcieRpNoFatalErrorReport[6]);
	DISPLAY_UPD(PcieRpNoFatalErrorReport[7]);
	DISPLAY_UPD(PcieRpNoFatalErrorReport[8]);
	DISPLAY_UPD(PcieRpNoFatalErrorReport[9]);
	DISPLAY_UPD(PcieRpNoFatalErrorReport[10]);
	DISPLAY_UPD(PcieRpNoFatalErrorReport[11]);
	DISPLAY_UPD(PcieRpNoFatalErrorReport[12]);
	DISPLAY_UPD(PcieRpNoFatalErrorReport[13]);
	DISPLAY_UPD(PcieRpNoFatalErrorReport[14]);
	DISPLAY_UPD(PcieRpNoFatalErrorReport[15]);
	DISPLAY_UPD(PcieRpNoFatalErrorReport[16]);
	DISPLAY_UPD(PcieRpNoFatalErrorReport[17]);
	DISPLAY_UPD(PcieRpNoFatalErrorReport[18]);
	DISPLAY_UPD(PcieRpNoFatalErrorReport[19]);
	DISPLAY_UPD(PcieRpNoFatalErrorReport[20]);
	DISPLAY_UPD(PcieRpNoFatalErrorReport[21]);
	DISPLAY_UPD(PcieRpNoFatalErrorReport[22]);
	DISPLAY_UPD(PcieRpNoFatalErrorReport[23]);
	// PcieRpCorrectableErrorReport[24]
	DISPLAY_UPD(PcieRpCorrectableErrorReport[0]);
	DISPLAY_UPD(PcieRpCorrectableErrorReport[1]);
	DISPLAY_UPD(PcieRpCorrectableErrorReport[2]);
	DISPLAY_UPD(PcieRpCorrectableErrorReport[3]);
	DISPLAY_UPD(PcieRpCorrectableErrorReport[4]);
	DISPLAY_UPD(PcieRpCorrectableErrorReport[5]);
	DISPLAY_UPD(PcieRpCorrectableErrorReport[6]);
	DISPLAY_UPD(PcieRpCorrectableErrorReport[7]);
	DISPLAY_UPD(PcieRpCorrectableErrorReport[8]);
	DISPLAY_UPD(PcieRpCorrectableErrorReport[9]);
	DISPLAY_UPD(PcieRpCorrectableErrorReport[10]);
	DISPLAY_UPD(PcieRpCorrectableErrorReport[11]);
	DISPLAY_UPD(PcieRpCorrectableErrorReport[12]);
	DISPLAY_UPD(PcieRpCorrectableErrorReport[13]);
	DISPLAY_UPD(PcieRpCorrectableErrorReport[14]);
	DISPLAY_UPD(PcieRpCorrectableErrorReport[15]);
	DISPLAY_UPD(PcieRpCorrectableErrorReport[16]);
	DISPLAY_UPD(PcieRpCorrectableErrorReport[17]);
	DISPLAY_UPD(PcieRpCorrectableErrorReport[18]);
	DISPLAY_UPD(PcieRpCorrectableErrorReport[19]);
	DISPLAY_UPD(PcieRpCorrectableErrorReport[20]);
	DISPLAY_UPD(PcieRpCorrectableErrorReport[21]);
	DISPLAY_UPD(PcieRpCorrectableErrorReport[22]);
	DISPLAY_UPD(PcieRpCorrectableErrorReport[23]);
	// PcieRpSystemErrorOnFatalError[24]
	DISPLAY_UPD(PcieRpSystemErrorOnFatalError[0]);
	DISPLAY_UPD(PcieRpSystemErrorOnFatalError[1]);
	DISPLAY_UPD(PcieRpSystemErrorOnFatalError[2]);
	DISPLAY_UPD(PcieRpSystemErrorOnFatalError[3]);
	DISPLAY_UPD(PcieRpSystemErrorOnFatalError[4]);
	DISPLAY_UPD(PcieRpSystemErrorOnFatalError[5]);
	DISPLAY_UPD(PcieRpSystemErrorOnFatalError[6]);
	DISPLAY_UPD(PcieRpSystemErrorOnFatalError[7]);
	DISPLAY_UPD(PcieRpSystemErrorOnFatalError[8]);
	DISPLAY_UPD(PcieRpSystemErrorOnFatalError[9]);
	DISPLAY_UPD(PcieRpSystemErrorOnFatalError[10]);
	DISPLAY_UPD(PcieRpSystemErrorOnFatalError[11]);
	DISPLAY_UPD(PcieRpSystemErrorOnFatalError[12]);
	DISPLAY_UPD(PcieRpSystemErrorOnFatalError[13]);
	DISPLAY_UPD(PcieRpSystemErrorOnFatalError[14]);
	DISPLAY_UPD(PcieRpSystemErrorOnFatalError[15]);
	DISPLAY_UPD(PcieRpSystemErrorOnFatalError[16]);
	DISPLAY_UPD(PcieRpSystemErrorOnFatalError[17]);
	DISPLAY_UPD(PcieRpSystemErrorOnFatalError[18]);
	DISPLAY_UPD(PcieRpSystemErrorOnFatalError[19]);
	DISPLAY_UPD(PcieRpSystemErrorOnFatalError[20]);
	DISPLAY_UPD(PcieRpSystemErrorOnFatalError[21]);
	DISPLAY_UPD(PcieRpSystemErrorOnFatalError[22]);
	DISPLAY_UPD(PcieRpSystemErrorOnFatalError[23]);
	// PcieRpSystemErrorOnNonFatalError[24]
	DISPLAY_UPD(PcieRpSystemErrorOnNonFatalError[0]);
	DISPLAY_UPD(PcieRpSystemErrorOnNonFatalError[1]);
	DISPLAY_UPD(PcieRpSystemErrorOnNonFatalError[2]);
	DISPLAY_UPD(PcieRpSystemErrorOnNonFatalError[3]);
	DISPLAY_UPD(PcieRpSystemErrorOnNonFatalError[4]);
	DISPLAY_UPD(PcieRpSystemErrorOnNonFatalError[5]);
	DISPLAY_UPD(PcieRpSystemErrorOnNonFatalError[6]);
	DISPLAY_UPD(PcieRpSystemErrorOnNonFatalError[7]);
	DISPLAY_UPD(PcieRpSystemErrorOnNonFatalError[8]);
	DISPLAY_UPD(PcieRpSystemErrorOnNonFatalError[9]);
	DISPLAY_UPD(PcieRpSystemErrorOnNonFatalError[10]);
	DISPLAY_UPD(PcieRpSystemErrorOnNonFatalError[11]);
	DISPLAY_UPD(PcieRpSystemErrorOnNonFatalError[12]);
	DISPLAY_UPD(PcieRpSystemErrorOnNonFatalError[13]);
	DISPLAY_UPD(PcieRpSystemErrorOnNonFatalError[14]);
	DISPLAY_UPD(PcieRpSystemErrorOnNonFatalError[15]);
	DISPLAY_UPD(PcieRpSystemErrorOnNonFatalError[16]);
	DISPLAY_UPD(PcieRpSystemErrorOnNonFatalError[17]);
	DISPLAY_UPD(PcieRpSystemErrorOnNonFatalError[18]);
	DISPLAY_UPD(PcieRpSystemErrorOnNonFatalError[19]);
	DISPLAY_UPD(PcieRpSystemErrorOnNonFatalError[20]);
	DISPLAY_UPD(PcieRpSystemErrorOnNonFatalError[21]);
	DISPLAY_UPD(PcieRpSystemErrorOnNonFatalError[22]);
	DISPLAY_UPD(PcieRpSystemErrorOnNonFatalError[23]);
	// PcieRpSystemErrorOnCorrectableError[24]
	DISPLAY_UPD(PcieRpSystemErrorOnCorrectableError[0]);
	DISPLAY_UPD(PcieRpSystemErrorOnCorrectableError[1]);
	DISPLAY_UPD(PcieRpSystemErrorOnCorrectableError[2]);
	DISPLAY_UPD(PcieRpSystemErrorOnCorrectableError[3]);
	DISPLAY_UPD(PcieRpSystemErrorOnCorrectableError[4]);
	DISPLAY_UPD(PcieRpSystemErrorOnCorrectableError[5]);
	DISPLAY_UPD(PcieRpSystemErrorOnCorrectableError[6]);
	DISPLAY_UPD(PcieRpSystemErrorOnCorrectableError[7]);
	DISPLAY_UPD(PcieRpSystemErrorOnCorrectableError[8]);
	DISPLAY_UPD(PcieRpSystemErrorOnCorrectableError[9]);
	DISPLAY_UPD(PcieRpSystemErrorOnCorrectableError[10]);
	DISPLAY_UPD(PcieRpSystemErrorOnCorrectableError[11]);
	DISPLAY_UPD(PcieRpSystemErrorOnCorrectableError[12]);
	DISPLAY_UPD(PcieRpSystemErrorOnCorrectableError[13]);
	DISPLAY_UPD(PcieRpSystemErrorOnCorrectableError[14]);
	DISPLAY_UPD(PcieRpSystemErrorOnCorrectableError[15]);
	DISPLAY_UPD(PcieRpSystemErrorOnCorrectableError[16]);
	DISPLAY_UPD(PcieRpSystemErrorOnCorrectableError[17]);
	DISPLAY_UPD(PcieRpSystemErrorOnCorrectableError[18]);
	DISPLAY_UPD(PcieRpSystemErrorOnCorrectableError[19]);
	DISPLAY_UPD(PcieRpSystemErrorOnCorrectableError[20]);
	DISPLAY_UPD(PcieRpSystemErrorOnCorrectableError[21]);
	DISPLAY_UPD(PcieRpSystemErrorOnCorrectableError[22]);
	DISPLAY_UPD(PcieRpSystemErrorOnCorrectableError[23]);
	// PcieRpMaxPayload[24]
	DISPLAY_UPD(PcieRpMaxPayload[0]);
	DISPLAY_UPD(PcieRpMaxPayload[1]);
	DISPLAY_UPD(PcieRpMaxPayload[2]);
	DISPLAY_UPD(PcieRpMaxPayload[3]);
	DISPLAY_UPD(PcieRpMaxPayload[4]);
	DISPLAY_UPD(PcieRpMaxPayload[5]);
	DISPLAY_UPD(PcieRpMaxPayload[6]);
	DISPLAY_UPD(PcieRpMaxPayload[7]);
	DISPLAY_UPD(PcieRpMaxPayload[8]);
	DISPLAY_UPD(PcieRpMaxPayload[9]);
	DISPLAY_UPD(PcieRpMaxPayload[10]);
	DISPLAY_UPD(PcieRpMaxPayload[11]);
	DISPLAY_UPD(PcieRpMaxPayload[12]);
	DISPLAY_UPD(PcieRpMaxPayload[13]);
	DISPLAY_UPD(PcieRpMaxPayload[14]);
	DISPLAY_UPD(PcieRpMaxPayload[15]);
	DISPLAY_UPD(PcieRpMaxPayload[16]);
	DISPLAY_UPD(PcieRpMaxPayload[17]);
	DISPLAY_UPD(PcieRpMaxPayload[18]);
	DISPLAY_UPD(PcieRpMaxPayload[19]);
	DISPLAY_UPD(PcieRpMaxPayload[20]);
	DISPLAY_UPD(PcieRpMaxPayload[21]);
	DISPLAY_UPD(PcieRpMaxPayload[22]);
	DISPLAY_UPD(PcieRpMaxPayload[23]);
	// PchUsbHsioRxTuningParameters[10]
	DISPLAY_UPD(PchUsbHsioRxTuningParameters[0]);
	DISPLAY_UPD(PchUsbHsioRxTuningParameters[1]);
	DISPLAY_UPD(PchUsbHsioRxTuningParameters[2]);
	DISPLAY_UPD(PchUsbHsioRxTuningParameters[3]);
	DISPLAY_UPD(PchUsbHsioRxTuningParameters[4]);
	DISPLAY_UPD(PchUsbHsioRxTuningParameters[5]);
	DISPLAY_UPD(PchUsbHsioRxTuningParameters[6]);
	DISPLAY_UPD(PchUsbHsioRxTuningParameters[7]);
	DISPLAY_UPD(PchUsbHsioRxTuningParameters[8]);
	DISPLAY_UPD(PchUsbHsioRxTuningParameters[9]);
	// PchUsbHsioRxTuningEnable[10]
	DISPLAY_UPD(PchUsbHsioRxTuningEnable[0]);
	DISPLAY_UPD(PchUsbHsioRxTuningEnable[1]);
	DISPLAY_UPD(PchUsbHsioRxTuningEnable[2]);
	DISPLAY_UPD(PchUsbHsioRxTuningEnable[3]);
	DISPLAY_UPD(PchUsbHsioRxTuningEnable[4]);
	DISPLAY_UPD(PchUsbHsioRxTuningEnable[5]);
	DISPLAY_UPD(PchUsbHsioRxTuningEnable[6]);
	DISPLAY_UPD(PchUsbHsioRxTuningEnable[7]);
	DISPLAY_UPD(PchUsbHsioRxTuningEnable[8]);
	DISPLAY_UPD(PchUsbHsioRxTuningEnable[9]);
	// PcieRpPcieSpeed[24]
	DISPLAY_UPD(PcieRpPcieSpeed[0]);
	DISPLAY_UPD(PcieRpPcieSpeed[1]);
	DISPLAY_UPD(PcieRpPcieSpeed[2]);
	DISPLAY_UPD(PcieRpPcieSpeed[3]);
	DISPLAY_UPD(PcieRpPcieSpeed[4]);
	DISPLAY_UPD(PcieRpPcieSpeed[5]);
	DISPLAY_UPD(PcieRpPcieSpeed[6]);
	DISPLAY_UPD(PcieRpPcieSpeed[7]);
	DISPLAY_UPD(PcieRpPcieSpeed[8]);
	DISPLAY_UPD(PcieRpPcieSpeed[9]);
	DISPLAY_UPD(PcieRpPcieSpeed[10]);
	DISPLAY_UPD(PcieRpPcieSpeed[11]);
	DISPLAY_UPD(PcieRpPcieSpeed[12]);
	DISPLAY_UPD(PcieRpPcieSpeed[13]);
	DISPLAY_UPD(PcieRpPcieSpeed[14]);
	DISPLAY_UPD(PcieRpPcieSpeed[15]);
	DISPLAY_UPD(PcieRpPcieSpeed[16]);
	DISPLAY_UPD(PcieRpPcieSpeed[17]);
	DISPLAY_UPD(PcieRpPcieSpeed[18]);
	DISPLAY_UPD(PcieRpPcieSpeed[19]);
	DISPLAY_UPD(PcieRpPcieSpeed[20]);
	DISPLAY_UPD(PcieRpPcieSpeed[21]);
	DISPLAY_UPD(PcieRpPcieSpeed[22]);
	DISPLAY_UPD(PcieRpPcieSpeed[23]);
	// PcieRpGen3EqPh3Method[24]
	DISPLAY_UPD(PcieRpGen3EqPh3Method[0]);
	DISPLAY_UPD(PcieRpGen3EqPh3Method[1]);
	DISPLAY_UPD(PcieRpGen3EqPh3Method[2]);
	DISPLAY_UPD(PcieRpGen3EqPh3Method[3]);
	DISPLAY_UPD(PcieRpGen3EqPh3Method[4]);
	DISPLAY_UPD(PcieRpGen3EqPh3Method[5]);
	DISPLAY_UPD(PcieRpGen3EqPh3Method[6]);
	DISPLAY_UPD(PcieRpGen3EqPh3Method[7]);
	DISPLAY_UPD(PcieRpGen3EqPh3Method[8]);
	DISPLAY_UPD(PcieRpGen3EqPh3Method[9]);
	DISPLAY_UPD(PcieRpGen3EqPh3Method[10]);
	DISPLAY_UPD(PcieRpGen3EqPh3Method[11]);
	DISPLAY_UPD(PcieRpGen3EqPh3Method[12]);
	DISPLAY_UPD(PcieRpGen3EqPh3Method[13]);
	DISPLAY_UPD(PcieRpGen3EqPh3Method[14]);
	DISPLAY_UPD(PcieRpGen3EqPh3Method[15]);
	DISPLAY_UPD(PcieRpGen3EqPh3Method[16]);
	DISPLAY_UPD(PcieRpGen3EqPh3Method[17]);
	DISPLAY_UPD(PcieRpGen3EqPh3Method[18]);
	DISPLAY_UPD(PcieRpGen3EqPh3Method[19]);
	DISPLAY_UPD(PcieRpGen3EqPh3Method[20]);
	DISPLAY_UPD(PcieRpGen3EqPh3Method[21]);
	DISPLAY_UPD(PcieRpGen3EqPh3Method[22]);
	DISPLAY_UPD(PcieRpGen3EqPh3Method[23]);
	// PcieRpPhysicalSlotNumber[24]
	DISPLAY_UPD(PcieRpPhysicalSlotNumber[0]);
	DISPLAY_UPD(PcieRpPhysicalSlotNumber[1]);
	DISPLAY_UPD(PcieRpPhysicalSlotNumber[2]);
	DISPLAY_UPD(PcieRpPhysicalSlotNumber[3]);
	DISPLAY_UPD(PcieRpPhysicalSlotNumber[4]);
	DISPLAY_UPD(PcieRpPhysicalSlotNumber[5]);
	DISPLAY_UPD(PcieRpPhysicalSlotNumber[6]);
	DISPLAY_UPD(PcieRpPhysicalSlotNumber[7]);
	DISPLAY_UPD(PcieRpPhysicalSlotNumber[8]);
	DISPLAY_UPD(PcieRpPhysicalSlotNumber[9]);
	DISPLAY_UPD(PcieRpPhysicalSlotNumber[10]);
	DISPLAY_UPD(PcieRpPhysicalSlotNumber[11]);
	DISPLAY_UPD(PcieRpPhysicalSlotNumber[12]);
	DISPLAY_UPD(PcieRpPhysicalSlotNumber[13]);
	DISPLAY_UPD(PcieRpPhysicalSlotNumber[14]);
	DISPLAY_UPD(PcieRpPhysicalSlotNumber[15]);
	DISPLAY_UPD(PcieRpPhysicalSlotNumber[16]);
	DISPLAY_UPD(PcieRpPhysicalSlotNumber[17]);
	DISPLAY_UPD(PcieRpPhysicalSlotNumber[18]);
	DISPLAY_UPD(PcieRpPhysicalSlotNumber[19]);
	DISPLAY_UPD(PcieRpPhysicalSlotNumber[20]);
	DISPLAY_UPD(PcieRpPhysicalSlotNumber[21]);
	DISPLAY_UPD(PcieRpPhysicalSlotNumber[22]);
	DISPLAY_UPD(PcieRpPhysicalSlotNumber[23]);
	// PcieRpCompletionTimeout[24]
	DISPLAY_UPD(PcieRpCompletionTimeout[0]);
	DISPLAY_UPD(PcieRpCompletionTimeout[1]);
	DISPLAY_UPD(PcieRpCompletionTimeout[2]);
	DISPLAY_UPD(PcieRpCompletionTimeout[3]);
	DISPLAY_UPD(PcieRpCompletionTimeout[4]);
	DISPLAY_UPD(PcieRpCompletionTimeout[5]);
	DISPLAY_UPD(PcieRpCompletionTimeout[6]);
	DISPLAY_UPD(PcieRpCompletionTimeout[7]);
	DISPLAY_UPD(PcieRpCompletionTimeout[8]);
	DISPLAY_UPD(PcieRpCompletionTimeout[9]);
	DISPLAY_UPD(PcieRpCompletionTimeout[10]);
	DISPLAY_UPD(PcieRpCompletionTimeout[11]);
	DISPLAY_UPD(PcieRpCompletionTimeout[12]);
	DISPLAY_UPD(PcieRpCompletionTimeout[13]);
	DISPLAY_UPD(PcieRpCompletionTimeout[14]);
	DISPLAY_UPD(PcieRpCompletionTimeout[15]);
	DISPLAY_UPD(PcieRpCompletionTimeout[16]);
	DISPLAY_UPD(PcieRpCompletionTimeout[17]);
	DISPLAY_UPD(PcieRpCompletionTimeout[18]);
	DISPLAY_UPD(PcieRpCompletionTimeout[19]);
	DISPLAY_UPD(PcieRpCompletionTimeout[20]);
	DISPLAY_UPD(PcieRpCompletionTimeout[21]);
	DISPLAY_UPD(PcieRpCompletionTimeout[22]);
	DISPLAY_UPD(PcieRpCompletionTimeout[23]);
	// PcieRpAspm[24]
	DISPLAY_UPD(PcieRpAspm[0]);
	DISPLAY_UPD(PcieRpAspm[1]);
	DISPLAY_UPD(PcieRpAspm[2]);
	DISPLAY_UPD(PcieRpAspm[3]);
	DISPLAY_UPD(PcieRpAspm[4]);
	DISPLAY_UPD(PcieRpAspm[5]);
	DISPLAY_UPD(PcieRpAspm[6]);
	DISPLAY_UPD(PcieRpAspm[7]);
	DISPLAY_UPD(PcieRpAspm[8]);
	DISPLAY_UPD(PcieRpAspm[9]);
	DISPLAY_UPD(PcieRpAspm[10]);
	DISPLAY_UPD(PcieRpAspm[11]);
	DISPLAY_UPD(PcieRpAspm[12]);
	DISPLAY_UPD(PcieRpAspm[13]);
	DISPLAY_UPD(PcieRpAspm[14]);
	DISPLAY_UPD(PcieRpAspm[15]);
	DISPLAY_UPD(PcieRpAspm[16]);
	DISPLAY_UPD(PcieRpAspm[17]);
	DISPLAY_UPD(PcieRpAspm[18]);
	DISPLAY_UPD(PcieRpAspm[19]);
	DISPLAY_UPD(PcieRpAspm[20]);
	DISPLAY_UPD(PcieRpAspm[21]);
	DISPLAY_UPD(PcieRpAspm[22]);
	DISPLAY_UPD(PcieRpAspm[23]);
	// PcieRpL1Substates[24]
	DISPLAY_UPD(PcieRpL1Substates[0]);
	DISPLAY_UPD(PcieRpL1Substates[1]);
	DISPLAY_UPD(PcieRpL1Substates[2]);
	DISPLAY_UPD(PcieRpL1Substates[3]);
	DISPLAY_UPD(PcieRpL1Substates[4]);
	DISPLAY_UPD(PcieRpL1Substates[5]);
	DISPLAY_UPD(PcieRpL1Substates[6]);
	DISPLAY_UPD(PcieRpL1Substates[7]);
	DISPLAY_UPD(PcieRpL1Substates[8]);
	DISPLAY_UPD(PcieRpL1Substates[9]);
	DISPLAY_UPD(PcieRpL1Substates[10]);
	DISPLAY_UPD(PcieRpL1Substates[11]);
	DISPLAY_UPD(PcieRpL1Substates[12]);
	DISPLAY_UPD(PcieRpL1Substates[13]);
	DISPLAY_UPD(PcieRpL1Substates[14]);
	DISPLAY_UPD(PcieRpL1Substates[15]);
	DISPLAY_UPD(PcieRpL1Substates[16]);
	DISPLAY_UPD(PcieRpL1Substates[17]);
	DISPLAY_UPD(PcieRpL1Substates[18]);
	DISPLAY_UPD(PcieRpL1Substates[19]);
	DISPLAY_UPD(PcieRpL1Substates[20]);
	DISPLAY_UPD(PcieRpL1Substates[21]);
	DISPLAY_UPD(PcieRpL1Substates[22]);
	DISPLAY_UPD(PcieRpL1Substates[23]);
	// PcieRpLtrEnable[24]
	DISPLAY_UPD(PcieRpLtrEnable[0]);
	DISPLAY_UPD(PcieRpLtrEnable[1]);
	DISPLAY_UPD(PcieRpLtrEnable[2]);
	DISPLAY_UPD(PcieRpLtrEnable[3]);
	DISPLAY_UPD(PcieRpLtrEnable[4]);
	DISPLAY_UPD(PcieRpLtrEnable[5]);
	DISPLAY_UPD(PcieRpLtrEnable[6]);
	DISPLAY_UPD(PcieRpLtrEnable[7]);
	DISPLAY_UPD(PcieRpLtrEnable[8]);
	DISPLAY_UPD(PcieRpLtrEnable[9]);
	DISPLAY_UPD(PcieRpLtrEnable[10]);
	DISPLAY_UPD(PcieRpLtrEnable[11]);
	DISPLAY_UPD(PcieRpLtrEnable[12]);
	DISPLAY_UPD(PcieRpLtrEnable[13]);
	DISPLAY_UPD(PcieRpLtrEnable[14]);
	DISPLAY_UPD(PcieRpLtrEnable[15]);
	DISPLAY_UPD(PcieRpLtrEnable[16]);
	DISPLAY_UPD(PcieRpLtrEnable[17]);
	DISPLAY_UPD(PcieRpLtrEnable[18]);
	DISPLAY_UPD(PcieRpLtrEnable[19]);
	DISPLAY_UPD(PcieRpLtrEnable[20]);
	DISPLAY_UPD(PcieRpLtrEnable[21]);
	DISPLAY_UPD(PcieRpLtrEnable[22]);
	DISPLAY_UPD(PcieRpLtrEnable[23]);
	// PcieRpLtrConfigLock[24]
	DISPLAY_UPD(PcieRpLtrConfigLock[0]);
	DISPLAY_UPD(PcieRpLtrConfigLock[1]);
	DISPLAY_UPD(PcieRpLtrConfigLock[2]);
	DISPLAY_UPD(PcieRpLtrConfigLock[3]);
	DISPLAY_UPD(PcieRpLtrConfigLock[4]);
	DISPLAY_UPD(PcieRpLtrConfigLock[5]);
	DISPLAY_UPD(PcieRpLtrConfigLock[6]);
	DISPLAY_UPD(PcieRpLtrConfigLock[7]);
	DISPLAY_UPD(PcieRpLtrConfigLock[8]);
	DISPLAY_UPD(PcieRpLtrConfigLock[9]);
	DISPLAY_UPD(PcieRpLtrConfigLock[10]);
	DISPLAY_UPD(PcieRpLtrConfigLock[11]);
	DISPLAY_UPD(PcieRpLtrConfigLock[12]);
	DISPLAY_UPD(PcieRpLtrConfigLock[13]);
	DISPLAY_UPD(PcieRpLtrConfigLock[14]);
	DISPLAY_UPD(PcieRpLtrConfigLock[15]);
	DISPLAY_UPD(PcieRpLtrConfigLock[16]);
	DISPLAY_UPD(PcieRpLtrConfigLock[17]);
	DISPLAY_UPD(PcieRpLtrConfigLock[18]);
	DISPLAY_UPD(PcieRpLtrConfigLock[19]);
	DISPLAY_UPD(PcieRpLtrConfigLock[20]);
	DISPLAY_UPD(PcieRpLtrConfigLock[21]);
	DISPLAY_UPD(PcieRpLtrConfigLock[22]);
	DISPLAY_UPD(PcieRpLtrConfigLock[23]);
	// PcieEqPh3LaneParamCm[24]
	DISPLAY_UPD(PcieEqPh3LaneParamCm[0]);
	DISPLAY_UPD(PcieEqPh3LaneParamCm[1]);
	DISPLAY_UPD(PcieEqPh3LaneParamCm[2]);
	DISPLAY_UPD(PcieEqPh3LaneParamCm[3]);
	DISPLAY_UPD(PcieEqPh3LaneParamCm[4]);
	DISPLAY_UPD(PcieEqPh3LaneParamCm[5]);
	DISPLAY_UPD(PcieEqPh3LaneParamCm[6]);
	DISPLAY_UPD(PcieEqPh3LaneParamCm[7]);
	DISPLAY_UPD(PcieEqPh3LaneParamCm[8]);
	DISPLAY_UPD(PcieEqPh3LaneParamCm[9]);
	DISPLAY_UPD(PcieEqPh3LaneParamCm[10]);
	DISPLAY_UPD(PcieEqPh3LaneParamCm[11]);
	DISPLAY_UPD(PcieEqPh3LaneParamCm[12]);
	DISPLAY_UPD(PcieEqPh3LaneParamCm[13]);
	DISPLAY_UPD(PcieEqPh3LaneParamCm[14]);
	DISPLAY_UPD(PcieEqPh3LaneParamCm[15]);
	DISPLAY_UPD(PcieEqPh3LaneParamCm[16]);
	DISPLAY_UPD(PcieEqPh3LaneParamCm[17]);
	DISPLAY_UPD(PcieEqPh3LaneParamCm[18]);
	DISPLAY_UPD(PcieEqPh3LaneParamCm[19]);
	DISPLAY_UPD(PcieEqPh3LaneParamCm[20]);
	DISPLAY_UPD(PcieEqPh3LaneParamCm[21]);
	DISPLAY_UPD(PcieEqPh3LaneParamCm[22]);
	DISPLAY_UPD(PcieEqPh3LaneParamCm[23]);
	// PcieEqPh3LaneParamCp[24]
	DISPLAY_UPD(PcieEqPh3LaneParamCp[0]);
	DISPLAY_UPD(PcieEqPh3LaneParamCp[1]);
	DISPLAY_UPD(PcieEqPh3LaneParamCp[2]);
	DISPLAY_UPD(PcieEqPh3LaneParamCp[3]);
	DISPLAY_UPD(PcieEqPh3LaneParamCp[4]);
	DISPLAY_UPD(PcieEqPh3LaneParamCp[5]);
	DISPLAY_UPD(PcieEqPh3LaneParamCp[6]);
	DISPLAY_UPD(PcieEqPh3LaneParamCp[7]);
	DISPLAY_UPD(PcieEqPh3LaneParamCp[8]);
	DISPLAY_UPD(PcieEqPh3LaneParamCp[9]);
	DISPLAY_UPD(PcieEqPh3LaneParamCp[10]);
	DISPLAY_UPD(PcieEqPh3LaneParamCp[11]);
	DISPLAY_UPD(PcieEqPh3LaneParamCp[12]);
	DISPLAY_UPD(PcieEqPh3LaneParamCp[13]);
	DISPLAY_UPD(PcieEqPh3LaneParamCp[14]);
	DISPLAY_UPD(PcieEqPh3LaneParamCp[15]);
	DISPLAY_UPD(PcieEqPh3LaneParamCp[16]);
	DISPLAY_UPD(PcieEqPh3LaneParamCp[17]);
	DISPLAY_UPD(PcieEqPh3LaneParamCp[18]);
	DISPLAY_UPD(PcieEqPh3LaneParamCp[19]);
	DISPLAY_UPD(PcieEqPh3LaneParamCp[20]);
	DISPLAY_UPD(PcieEqPh3LaneParamCp[21]);
	DISPLAY_UPD(PcieEqPh3LaneParamCp[22]);
	DISPLAY_UPD(PcieEqPh3LaneParamCp[23]);
	// PcieSwEqCoeffListCm[5]
	DISPLAY_UPD(PcieSwEqCoeffListCm[0]);
	DISPLAY_UPD(PcieSwEqCoeffListCm[1]);
	DISPLAY_UPD(PcieSwEqCoeffListCm[2]);
	DISPLAY_UPD(PcieSwEqCoeffListCm[3]);
	DISPLAY_UPD(PcieSwEqCoeffListCm[4]);
	// PcieSwEqCoeffListCp[5]
	DISPLAY_UPD(PcieSwEqCoeffListCp[0]);
	DISPLAY_UPD(PcieSwEqCoeffListCp[1]);
	DISPLAY_UPD(PcieSwEqCoeffListCp[2]);
	DISPLAY_UPD(PcieSwEqCoeffListCp[3]);
	DISPLAY_UPD(PcieSwEqCoeffListCp[4]);
	DISPLAY_UPD(PcieDisableRootPortClockGating);
	DISPLAY_UPD(PcieEnablePeerMemoryWrite);
	DISPLAY_UPD(PcieComplianceTestMode);
	DISPLAY_UPD(PcieRpFunctionSwap);
	DISPLAY_UPD(TetonGlacierCR);
	DISPLAY_UPD(PchPmPmeB0S5Dis);
	DISPLAY_UPD(PcieRpImrEnabled);
	DISPLAY_UPD(PcieRpImrSelection);
	DISPLAY_UPD(TetonGlacierMode);
	DISPLAY_UPD(PchPmWolEnableOverride);
	DISPLAY_UPD(PchPmPcieWakeFromDeepSx);
	DISPLAY_UPD(PchPmWoWlanEnable);
	DISPLAY_UPD(PchPmWoWlanDeepSxEnable);
	DISPLAY_UPD(PchPmLanWakeFromDeepSx);
	DISPLAY_UPD(PchPmDeepSxPol);
	DISPLAY_UPD(PchPmSlpS3MinAssert);
	DISPLAY_UPD(PchPmSlpS4MinAssert);
	DISPLAY_UPD(PchPmSlpSusMinAssert);
	DISPLAY_UPD(PchPmSlpAMinAssert);
	DISPLAY_UPD(SlpS0Override);
	DISPLAY_UPD(SlpS0DisQForDebug);
	DISPLAY_UPD(PchEnableDbcObs);
	DISPLAY_UPD(PchLegacyIoLowLatency);
	DISPLAY_UPD(PchPmLpcClockRun);
	DISPLAY_UPD(PchPmSlpStrchSusUp);
	DISPLAY_UPD(PchPmSlpLanLowDc);
	DISPLAY_UPD(PchPmPwrBtnOverridePeriod);
	DISPLAY_UPD(PchPmDisableDsxAcPresentPulldown);
	DISPLAY_UPD(PchPmDisableNativePowerButton);
	DISPLAY_UPD(PchPmSlpS0Enable);
	DISPLAY_UPD(PchPmMeWakeSts);
	DISPLAY_UPD(PchPmWolOvrWkSts);
	DISPLAY_UPD(PchPmPwrCycDur);
	DISPLAY_UPD(PchPmPciePllSsc);
	DISPLAY_UPD(SataPwrOptEnable);
	DISPLAY_UPD(EsataSpeedLimit);
	DISPLAY_UPD(SataSpeedLimit);
	// SataPortsHotPlug[8]
	DISPLAY_UPD(SataPortsHotPlug[0]);
	DISPLAY_UPD(SataPortsHotPlug[1]);
	DISPLAY_UPD(SataPortsHotPlug[2]);
	DISPLAY_UPD(SataPortsHotPlug[3]);
	DISPLAY_UPD(SataPortsHotPlug[4]);
	DISPLAY_UPD(SataPortsHotPlug[5]);
	DISPLAY_UPD(SataPortsHotPlug[6]);
	DISPLAY_UPD(SataPortsHotPlug[7]);
	// SataPortsInterlockSw[8]
	DISPLAY_UPD(SataPortsInterlockSw[0]);
	DISPLAY_UPD(SataPortsInterlockSw[1]);
	DISPLAY_UPD(SataPortsInterlockSw[2]);
	DISPLAY_UPD(SataPortsInterlockSw[3]);
	DISPLAY_UPD(SataPortsInterlockSw[4]);
	DISPLAY_UPD(SataPortsInterlockSw[5]);
	DISPLAY_UPD(SataPortsInterlockSw[6]);
	DISPLAY_UPD(SataPortsInterlockSw[7]);
	// SataPortsExternal[8]
	DISPLAY_UPD(SataPortsExternal[0]);
	DISPLAY_UPD(SataPortsExternal[1]);
	DISPLAY_UPD(SataPortsExternal[2]);
	DISPLAY_UPD(SataPortsExternal[3]);
	DISPLAY_UPD(SataPortsExternal[4]);
	DISPLAY_UPD(SataPortsExternal[5]);
	DISPLAY_UPD(SataPortsExternal[6]);
	DISPLAY_UPD(SataPortsExternal[7]);
	// SataPortsSpinUp[8]
	DISPLAY_UPD(SataPortsSpinUp[0]);
	DISPLAY_UPD(SataPortsSpinUp[1]);
	DISPLAY_UPD(SataPortsSpinUp[2]);
	DISPLAY_UPD(SataPortsSpinUp[3]);
	DISPLAY_UPD(SataPortsSpinUp[4]);
	DISPLAY_UPD(SataPortsSpinUp[5]);
	DISPLAY_UPD(SataPortsSpinUp[6]);
	DISPLAY_UPD(SataPortsSpinUp[7]);
	// SataPortsSolidStateDrive[8]
	DISPLAY_UPD(SataPortsSolidStateDrive[0]);
	DISPLAY_UPD(SataPortsSolidStateDrive[1]);
	DISPLAY_UPD(SataPortsSolidStateDrive[2]);
	DISPLAY_UPD(SataPortsSolidStateDrive[3]);
	DISPLAY_UPD(SataPortsSolidStateDrive[4]);
	DISPLAY_UPD(SataPortsSolidStateDrive[5]);
	DISPLAY_UPD(SataPortsSolidStateDrive[6]);
	DISPLAY_UPD(SataPortsSolidStateDrive[7]);
	// SataPortsEnableDitoConfig[8]
	DISPLAY_UPD(SataPortsEnableDitoConfig[0]);
	DISPLAY_UPD(SataPortsEnableDitoConfig[1]);
	DISPLAY_UPD(SataPortsEnableDitoConfig[2]);
	DISPLAY_UPD(SataPortsEnableDitoConfig[3]);
	DISPLAY_UPD(SataPortsEnableDitoConfig[4]);
	DISPLAY_UPD(SataPortsEnableDitoConfig[5]);
	DISPLAY_UPD(SataPortsEnableDitoConfig[6]);
	DISPLAY_UPD(SataPortsEnableDitoConfig[7]);
	// SataPortsDmVal[8]
	DISPLAY_UPD(SataPortsDmVal[0]);
	DISPLAY_UPD(SataPortsDmVal[1]);
	DISPLAY_UPD(SataPortsDmVal[2]);
	DISPLAY_UPD(SataPortsDmVal[3]);
	DISPLAY_UPD(SataPortsDmVal[4]);
	DISPLAY_UPD(SataPortsDmVal[5]);
	DISPLAY_UPD(SataPortsDmVal[6]);
	DISPLAY_UPD(SataPortsDmVal[7]);
	// SataPortsDitoVal[8]
	DISPLAY_UPD(SataPortsDitoVal[0]);
	DISPLAY_UPD(SataPortsDitoVal[1]);
	DISPLAY_UPD(SataPortsDitoVal[2]);
	DISPLAY_UPD(SataPortsDitoVal[3]);
	DISPLAY_UPD(SataPortsDitoVal[4]);
	DISPLAY_UPD(SataPortsDitoVal[5]);
	DISPLAY_UPD(SataPortsDitoVal[6]);
	DISPLAY_UPD(SataPortsDitoVal[7]);
	// SataPortsZpOdd[8]
	DISPLAY_UPD(SataPortsZpOdd[0]);
	DISPLAY_UPD(SataPortsZpOdd[1]);
	DISPLAY_UPD(SataPortsZpOdd[2]);
	DISPLAY_UPD(SataPortsZpOdd[3]);
	DISPLAY_UPD(SataPortsZpOdd[4]);
	DISPLAY_UPD(SataPortsZpOdd[5]);
	DISPLAY_UPD(SataPortsZpOdd[6]);
	DISPLAY_UPD(SataPortsZpOdd[7]);
	DISPLAY_UPD(SataRstRaidDeviceId);
	DISPLAY_UPD(SataRstRaid0);
	DISPLAY_UPD(SataRstRaid1);
	DISPLAY_UPD(SataRstRaid10);
	DISPLAY_UPD(SataRstRaid5);
	DISPLAY_UPD(SataRstIrrt);
	DISPLAY_UPD(SataRstOromUiBanner);
	DISPLAY_UPD(SataRstOromUiDelay);
	DISPLAY_UPD(SataRstHddUnlock);
	DISPLAY_UPD(SataRstLedLocate);
	DISPLAY_UPD(SataRstIrrtOnly);
	DISPLAY_UPD(SataRstSmartStorage);
	// SataRstPcieEnable[3]
	DISPLAY_UPD(SataRstPcieEnable[0]);
	DISPLAY_UPD(SataRstPcieEnable[1]);
	DISPLAY_UPD(SataRstPcieEnable[2]);
	// SataRstPcieStoragePort[3]
	DISPLAY_UPD(SataRstPcieStoragePort[0]);
	DISPLAY_UPD(SataRstPcieStoragePort[1]);
	DISPLAY_UPD(SataRstPcieStoragePort[2]);
	// SataRstPcieDeviceResetDelay[3]
	DISPLAY_UPD(SataRstPcieDeviceResetDelay[0]);
	DISPLAY_UPD(SataRstPcieDeviceResetDelay[1]);
	DISPLAY_UPD(SataRstPcieDeviceResetDelay[2]);
	DISPLAY_UPD(PchScsEmmcHs400TuningRequired);
	DISPLAY_UPD(PchScsEmmcHs400DllDataValid);
	DISPLAY_UPD(PchScsEmmcHs400RxStrobeDll1);
	DISPLAY_UPD(PchScsEmmcHs400TxDataDll);
	DISPLAY_UPD(PchScsEmmcHs400DriverStrength);
	DISPLAY_UPD(PchSirqEnable);
	DISPLAY_UPD(PchSirqMode);
	DISPLAY_UPD(PchStartFramePulse);
	DISPLAY_UPD(PchEspiLockLinkConfiguration);
	DISPLAY_UPD(PchTsmicLock);
	DISPLAY_UPD(PchT0Level);
	DISPLAY_UPD(PchT1Level);
	DISPLAY_UPD(PchT2Level);
	DISPLAY_UPD(PchTTEnable);
	DISPLAY_UPD(PchTTState13Enable);
	DISPLAY_UPD(PchTTLock);
	DISPLAY_UPD(TTSuggestedSetting);
	DISPLAY_UPD(TTCrossThrottling);
	DISPLAY_UPD(PchDmiTsawEn);
	DISPLAY_UPD(DmiSuggestedSetting);
	DISPLAY_UPD(DmiTS0TW);
	DISPLAY_UPD(DmiTS1TW);
	DISPLAY_UPD(DmiTS2TW);
	DISPLAY_UPD(DmiTS3TW);
	DISPLAY_UPD(SataP0T1M);
	DISPLAY_UPD(SataP0T2M);
	DISPLAY_UPD(SataP0T3M);
	DISPLAY_UPD(SataP0TDisp);
	DISPLAY_UPD(SataP1T1M);
	DISPLAY_UPD(SataP1T2M);
	DISPLAY_UPD(SataP1T3M);
	DISPLAY_UPD(SataP1TDisp);
	DISPLAY_UPD(SataP0Tinact);
	DISPLAY_UPD(SataP0TDispFinit);
	DISPLAY_UPD(SataP1Tinact);
	DISPLAY_UPD(SataP1TDispFinit);
	DISPLAY_UPD(SataThermalSuggestedSetting);
	DISPLAY_UPD(PchMemoryThrottlingEnable);
	// PchMemoryPmsyncEnable[2]
	DISPLAY_UPD(PchMemoryPmsyncEnable[0]);
	DISPLAY_UPD(PchMemoryPmsyncEnable[1]);
	// PchMemoryC0TransmitEnable[2]
	DISPLAY_UPD(PchMemoryC0TransmitEnable[0]);
	DISPLAY_UPD(PchMemoryC0TransmitEnable[1]);
	// PchMemoryPinSelection[2]
	DISPLAY_UPD(PchMemoryPinSelection[0]);
	DISPLAY_UPD(PchMemoryPinSelection[1]);
	DISPLAY_UPD(PchTemperatureHotLevel);
	DISPLAY_UPD(PchEnableComplianceMode);
	// Usb2OverCurrentPin[16]
	DISPLAY_UPD(Usb2OverCurrentPin[0]);
	DISPLAY_UPD(Usb2OverCurrentPin[1]);
	DISPLAY_UPD(Usb2OverCurrentPin[2]);
	DISPLAY_UPD(Usb2OverCurrentPin[3]);
	DISPLAY_UPD(Usb2OverCurrentPin[4]);
	DISPLAY_UPD(Usb2OverCurrentPin[5]);
	DISPLAY_UPD(Usb2OverCurrentPin[6]);
	DISPLAY_UPD(Usb2OverCurrentPin[7]);
	DISPLAY_UPD(Usb2OverCurrentPin[8]);
	DISPLAY_UPD(Usb2OverCurrentPin[9]);
	DISPLAY_UPD(Usb2OverCurrentPin[10]);
	DISPLAY_UPD(Usb2OverCurrentPin[11]);
	DISPLAY_UPD(Usb2OverCurrentPin[12]);
	DISPLAY_UPD(Usb2OverCurrentPin[13]);
	DISPLAY_UPD(Usb2OverCurrentPin[14]);
	DISPLAY_UPD(Usb2OverCurrentPin[15]);
	// Usb3OverCurrentPin[10]
	DISPLAY_UPD(Usb3OverCurrentPin[0]);
	DISPLAY_UPD(Usb3OverCurrentPin[1]);
	DISPLAY_UPD(Usb3OverCurrentPin[2]);
	DISPLAY_UPD(Usb3OverCurrentPin[3]);
	DISPLAY_UPD(Usb3OverCurrentPin[4]);
	DISPLAY_UPD(Usb3OverCurrentPin[5]);
	DISPLAY_UPD(Usb3OverCurrentPin[6]);
	DISPLAY_UPD(Usb3OverCurrentPin[7]);
	DISPLAY_UPD(Usb3OverCurrentPin[8]);
	DISPLAY_UPD(Usb3OverCurrentPin[9]);
	DISPLAY_UPD(Enable8254ClockGating);
	DISPLAY_UPD(SataRstOptaneMemory);
	DISPLAY_UPD(SataRstCpuAttachedStorage);
	DISPLAY_UPD(Enable8254ClockGatingOnS3);
	DISPLAY_UPD(PchPcieDeviceOverrideTablePtr);
	DISPLAY_UPD(EnableTcoTimer);
	DISPLAY_UPD(PsOnEnable);
	DISPLAY_UPD(PmcCpuC10GatePinEnable);
	DISPLAY_UPD(PchDmiAspmCtrl);
	// Usb3HsioTxRate3UniqTranEnable[10]
	DISPLAY_UPD(Usb3HsioTxRate3UniqTranEnable[0]);
	DISPLAY_UPD(Usb3HsioTxRate3UniqTranEnable[1]);
	DISPLAY_UPD(Usb3HsioTxRate3UniqTranEnable[2]);
	DISPLAY_UPD(Usb3HsioTxRate3UniqTranEnable[3]);
	DISPLAY_UPD(Usb3HsioTxRate3UniqTranEnable[4]);
	DISPLAY_UPD(Usb3HsioTxRate3UniqTranEnable[5]);
	DISPLAY_UPD(Usb3HsioTxRate3UniqTranEnable[6]);
	DISPLAY_UPD(Usb3HsioTxRate3UniqTranEnable[7]);
	DISPLAY_UPD(Usb3HsioTxRate3UniqTranEnable[8]);
	DISPLAY_UPD(Usb3HsioTxRate3UniqTranEnable[9]);
	// Usb3HsioTxRate3UniqTran[10]
	DISPLAY_UPD(Usb3HsioTxRate3UniqTran[0]);
	DISPLAY_UPD(Usb3HsioTxRate3UniqTran[1]);
	DISPLAY_UPD(Usb3HsioTxRate3UniqTran[2]);
	DISPLAY_UPD(Usb3HsioTxRate3UniqTran[3]);
	DISPLAY_UPD(Usb3HsioTxRate3UniqTran[4]);
	DISPLAY_UPD(Usb3HsioTxRate3UniqTran[5]);
	DISPLAY_UPD(Usb3HsioTxRate3UniqTran[6]);
	DISPLAY_UPD(Usb3HsioTxRate3UniqTran[7]);
	DISPLAY_UPD(Usb3HsioTxRate3UniqTran[8]);
	DISPLAY_UPD(Usb3HsioTxRate3UniqTran[9]);
	// Usb3HsioTxRate2UniqTranEnable[10]
	DISPLAY_UPD(Usb3HsioTxRate2UniqTranEnable[0]);
	DISPLAY_UPD(Usb3HsioTxRate2UniqTranEnable[1]);
	DISPLAY_UPD(Usb3HsioTxRate2UniqTranEnable[2]);
	DISPLAY_UPD(Usb3HsioTxRate2UniqTranEnable[3]);
	DISPLAY_UPD(Usb3HsioTxRate2UniqTranEnable[4]);
	DISPLAY_UPD(Usb3HsioTxRate2UniqTranEnable[5]);
	DISPLAY_UPD(Usb3HsioTxRate2UniqTranEnable[6]);
	DISPLAY_UPD(Usb3HsioTxRate2UniqTranEnable[7]);
	DISPLAY_UPD(Usb3HsioTxRate2UniqTranEnable[8]);
	DISPLAY_UPD(Usb3HsioTxRate2UniqTranEnable[9]);
	// Usb3HsioTxRate2UniqTran[10]
	DISPLAY_UPD(Usb3HsioTxRate2UniqTran[0]);
	DISPLAY_UPD(Usb3HsioTxRate2UniqTran[1]);
	DISPLAY_UPD(Usb3HsioTxRate2UniqTran[2]);
	DISPLAY_UPD(Usb3HsioTxRate2UniqTran[3]);
	DISPLAY_UPD(Usb3HsioTxRate2UniqTran[4]);
	DISPLAY_UPD(Usb3HsioTxRate2UniqTran[5]);
	DISPLAY_UPD(Usb3HsioTxRate2UniqTran[6]);
	DISPLAY_UPD(Usb3HsioTxRate2UniqTran[7]);
	DISPLAY_UPD(Usb3HsioTxRate2UniqTran[8]);
	DISPLAY_UPD(Usb3HsioTxRate2UniqTran[9]);
	// Usb3HsioTxRate1UniqTranEnable[10]
	DISPLAY_UPD(Usb3HsioTxRate1UniqTranEnable[0]);
	DISPLAY_UPD(Usb3HsioTxRate1UniqTranEnable[1]);
	DISPLAY_UPD(Usb3HsioTxRate1UniqTranEnable[2]);
	DISPLAY_UPD(Usb3HsioTxRate1UniqTranEnable[3]);
	DISPLAY_UPD(Usb3HsioTxRate1UniqTranEnable[4]);
	DISPLAY_UPD(Usb3HsioTxRate1UniqTranEnable[5]);
	DISPLAY_UPD(Usb3HsioTxRate1UniqTranEnable[6]);
	DISPLAY_UPD(Usb3HsioTxRate1UniqTranEnable[7]);
	DISPLAY_UPD(Usb3HsioTxRate1UniqTranEnable[8]);
	DISPLAY_UPD(Usb3HsioTxRate1UniqTranEnable[9]);
	// Usb3HsioTxRate1UniqTran[10]
	DISPLAY_UPD(Usb3HsioTxRate1UniqTran[0]);
	DISPLAY_UPD(Usb3HsioTxRate1UniqTran[1]);
	DISPLAY_UPD(Usb3HsioTxRate1UniqTran[2]);
	DISPLAY_UPD(Usb3HsioTxRate1UniqTran[3]);
	DISPLAY_UPD(Usb3HsioTxRate1UniqTran[4]);
	DISPLAY_UPD(Usb3HsioTxRate1UniqTran[5]);
	DISPLAY_UPD(Usb3HsioTxRate1UniqTran[6]);
	DISPLAY_UPD(Usb3HsioTxRate1UniqTran[7]);
	DISPLAY_UPD(Usb3HsioTxRate1UniqTran[8]);
	DISPLAY_UPD(Usb3HsioTxRate1UniqTran[9]);
	// Usb3HsioTxRate0UniqTranEnable[10]
	DISPLAY_UPD(Usb3HsioTxRate0UniqTranEnable[0]);
	DISPLAY_UPD(Usb3HsioTxRate0UniqTranEnable[1]);
	DISPLAY_UPD(Usb3HsioTxRate0UniqTranEnable[2]);
	DISPLAY_UPD(Usb3HsioTxRate0UniqTranEnable[3]);
	DISPLAY_UPD(Usb3HsioTxRate0UniqTranEnable[4]);
	DISPLAY_UPD(Usb3HsioTxRate0UniqTranEnable[5]);
	DISPLAY_UPD(Usb3HsioTxRate0UniqTranEnable[6]);
	DISPLAY_UPD(Usb3HsioTxRate0UniqTranEnable[7]);
	DISPLAY_UPD(Usb3HsioTxRate0UniqTranEnable[8]);
	DISPLAY_UPD(Usb3HsioTxRate0UniqTranEnable[9]);
	// Usb3HsioTxRate0UniqTran[10]
	DISPLAY_UPD(Usb3HsioTxRate0UniqTran[0]);
	DISPLAY_UPD(Usb3HsioTxRate0UniqTran[1]);
	DISPLAY_UPD(Usb3HsioTxRate0UniqTran[2]);
	DISPLAY_UPD(Usb3HsioTxRate0UniqTran[3]);
	DISPLAY_UPD(Usb3HsioTxRate0UniqTran[4]);
	DISPLAY_UPD(Usb3HsioTxRate0UniqTran[5]);
	DISPLAY_UPD(Usb3HsioTxRate0UniqTran[6]);
	DISPLAY_UPD(Usb3HsioTxRate0UniqTran[7]);
	DISPLAY_UPD(Usb3HsioTxRate0UniqTran[8]);
	DISPLAY_UPD(Usb3HsioTxRate0UniqTran[9]);
	DISPLAY_UPD(PcieNumOfCoefficients);
	DISPLAY_UPD(GpioPmRcompCommunityLocalClockGating);
	DISPLAY_UPD(ScsSdCardWpPinEnabled);
	// SataPortsDevSlpResetConfig[8]
	DISPLAY_UPD(SataPortsDevSlpResetConfig[0]);
	DISPLAY_UPD(SataPortsDevSlpResetConfig[1]);
	DISPLAY_UPD(SataPortsDevSlpResetConfig[2]);
	DISPLAY_UPD(SataPortsDevSlpResetConfig[3]);
	DISPLAY_UPD(SataPortsDevSlpResetConfig[4]);
	DISPLAY_UPD(SataPortsDevSlpResetConfig[5]);
	DISPLAY_UPD(SataPortsDevSlpResetConfig[6]);
	DISPLAY_UPD(SataPortsDevSlpResetConfig[7]);
	DISPLAY_UPD(SpiFlashCfgLockDown);
	// PchHdaSndwLinkIoControlEnabled[4]
	DISPLAY_UPD(PchHdaSndwLinkIoControlEnabled[0]);
	DISPLAY_UPD(PchHdaSndwLinkIoControlEnabled[1]);
	DISPLAY_UPD(PchHdaSndwLinkIoControlEnabled[2]);
	DISPLAY_UPD(PchHdaSndwLinkIoControlEnabled[3]);
	// BgpdtHash[4]
	DISPLAY_UPD(BgpdtHash[0]);
	DISPLAY_UPD(BgpdtHash[1]);
	DISPLAY_UPD(BgpdtHash[2]);
	DISPLAY_UPD(BgpdtHash[3]);
	DISPLAY_UPD(BiosGuardAttr);
	DISPLAY_UPD(BiosGuardModulePtr);
	DISPLAY_UPD(SendEcCmd);
	DISPLAY_UPD(EcCmdProvisionEav);
	DISPLAY_UPD(EcCmdLock);
	DISPLAY_UPD(SgxEpoch0);
	DISPLAY_UPD(SgxEpoch1);
	DISPLAY_UPD(SgxSinitNvsData);
	DISPLAY_UPD(SiCsmFlag);
	DISPLAY_UPD(SiSsidTablePtr);
	DISPLAY_UPD(SiNumberOfSsidTableEntry);
	DISPLAY_UPD(SataRstInterrupt);
	DISPLAY_UPD(MeUnconfigOnRtcClear);

	#undef DISPLAY_UPD
}
#endif // CONFIG(SOC_INTEL_COMETLAKE)
