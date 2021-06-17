/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <device/device.h>
#include <fsp/util.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/mp_init.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>
#include <soc/soc_chip.h>
#include <string.h>

static void soc_memory_init_params(FSP_M_CONFIG *m_cfg,
		const struct soc_intel_tigerlake_config *config)
{
	unsigned int i;
	uint32_t cpu_id, mask = 0;
	const struct device *dev;

	/*
	 * If IGD is enabled, set IGD stolen size to 60MB.
	 * Otherwise, skip IGD init in FSP.
	 */
	dev = pcidev_path_on_root(SA_DEVFN_IGD);
	m_cfg->InternalGfx = is_dev_enabled(dev);
	m_cfg->IgdDvmt50PreAlloc = m_cfg->InternalGfx ? 0xFE : 0;

	m_cfg->TsegSize = CONFIG_SMM_TSEG_SIZE;
	m_cfg->IedSize = CONFIG_IED_REGION_SIZE;
	m_cfg->SaGv = config->SaGv;
	m_cfg->UserBd = BOARD_TYPE_ULT_ULX;
	m_cfg->RMT = config->RMT;

	/* CpuRatio Settings */
	if (config->cpu_ratio_override) {
		m_cfg->CpuRatio = config->cpu_ratio_override;
	} else {
		/* Set CpuRatio to match existing MSR value */
		msr_t flex_ratio;
		flex_ratio = rdmsr(MSR_FLEX_RATIO);
		m_cfg->CpuRatio = (flex_ratio.lo >> 8) & 0xff;
	}

	for (i = 0; i < ARRAY_SIZE(config->PcieRpEnable); i++) {
		if (config->PcieRpEnable[i])
			mask |= (1 << i);
	}
	m_cfg->PcieRpEnableMask = mask;

	memcpy(m_cfg->PcieClkSrcUsage, config->PcieClkSrcUsage,
		sizeof(config->PcieClkSrcUsage));

	for (i = 0; i < CONFIG_MAX_PCIE_CLOCKS; i++) {
		if (config->PcieClkSrcUsage[i] == 0)
			m_cfg->PcieClkSrcUsage[i] = 0xff;
	}

	memcpy(m_cfg->PcieClkSrcClkReq, config->PcieClkSrcClkReq,
		sizeof(config->PcieClkSrcClkReq));

	m_cfg->PrmrrSize = get_valid_prmrr_size();
	m_cfg->EnableC6Dram = config->enable_c6dram;
	/* Disable BIOS Guard */
	m_cfg->BiosGuard = 0;

	/* Set debug interface flags */
	m_cfg->PcdDebugInterfaceFlags = CONFIG(DRIVERS_UART_8250IO) ?
		DEBUG_INTERFACE_UART_8250IO : DEBUG_INTERFACE_LPSS_SERIAL_IO;

	/* TraceHub configuration */
	dev = pcidev_path_on_root(PCH_DEVFN_TRACEHUB);
	if (is_dev_enabled(dev) && config->TraceHubMode) {
		m_cfg->PcdDebugInterfaceFlags |= DEBUG_INTERFACE_TRACEHUB;
		m_cfg->PchTraceHubMode = config->TraceHubMode;
		m_cfg->CpuTraceHubMode = config->TraceHubMode;
	}

	m_cfg->SerialIoUartDebugControllerNumber = CONFIG_UART_FOR_CONSOLE;
	m_cfg->SerialIoUartDebugMode = PchSerialIoSkipInit;

	/* ISH */
	dev = pcidev_path_on_root(PCH_DEVFN_ISH);
	m_cfg->PchIshEnable = is_dev_enabled(dev);

	/* Skip GPIO configuration from FSP */
	m_cfg->GpioOverride = 0x1;

	/* DP port config */
	m_cfg->DdiPortAConfig = config->DdiPortAConfig;
	m_cfg->DdiPortBConfig = config->DdiPortBConfig;
	m_cfg->DdiPortAHpd = config->DdiPortAHpd;
	m_cfg->DdiPortBHpd = config->DdiPortBHpd;
	m_cfg->DdiPortCHpd = config->DdiPortCHpd;
	m_cfg->DdiPort1Hpd = config->DdiPort1Hpd;
	m_cfg->DdiPort2Hpd = config->DdiPort2Hpd;
	m_cfg->DdiPort3Hpd = config->DdiPort3Hpd;
	m_cfg->DdiPort4Hpd = config->DdiPort4Hpd;
	m_cfg->DdiPortADdc = config->DdiPortADdc;
	m_cfg->DdiPortBDdc = config->DdiPortBDdc;
	m_cfg->DdiPortCDdc = config->DdiPortCDdc;
	m_cfg->DdiPort1Ddc = config->DdiPort1Ddc;
	m_cfg->DdiPort2Ddc = config->DdiPort2Ddc;
	m_cfg->DdiPort3Ddc = config->DdiPort3Ddc;
	m_cfg->DdiPort4Ddc = config->DdiPort4Ddc;

	/* Image clock: disable all clocks for bypassing FSP pin mux */
	memset(m_cfg->ImguClkOutEn, 0, sizeof(m_cfg->ImguClkOutEn));

	/* Tcss USB */
	m_cfg->TcssXhciEn = config->TcssXhciEn;
	m_cfg->TcssXdciEn = config->TcssXdciEn;

	/* TCSS DMA */
	dev = pcidev_path_on_root(SA_DEVFN_TCSS_DMA0);
	m_cfg->TcssDma0En = is_dev_enabled(dev);

	dev = pcidev_path_on_root(SA_DEVFN_TCSS_DMA1);
	m_cfg->TcssDma1En = is_dev_enabled(dev);

	/* USB4/TBT */
	dev = pcidev_path_on_root(SA_DEVFN_TBT0);
	m_cfg->TcssItbtPcie0En = is_dev_enabled(dev);

	dev = pcidev_path_on_root(SA_DEVFN_TBT1);
	m_cfg->TcssItbtPcie1En = is_dev_enabled(dev);

	dev = pcidev_path_on_root(SA_DEVFN_TBT2);
	m_cfg->TcssItbtPcie2En = is_dev_enabled(dev);

	dev = pcidev_path_on_root(SA_DEVFN_TBT3);
	m_cfg->TcssItbtPcie3En = is_dev_enabled(dev);

	/* Hyper Threading */
	m_cfg->HyperThreading = !config->HyperThreadingDisable;

	/* Disable Lock PCU Thermal Management registers */
	m_cfg->LockPTMregs = 0;
	/* Channel Hash Mask:0x0001=BIT6 set(Minimal), 0x3FFF=BIT[19:6] set(Maximum) */
	m_cfg->ChHashMask = 0x30CC;
	/* Enable SMBus controller based on config */
	m_cfg->SmbusEnable = config->SmbusEnable;
	/* Set debug probe type */
	m_cfg->PlatformDebugConsent = CONFIG_SOC_INTEL_TIGERLAKE_DEBUG_CONSENT;

	/* Audio: HDAUDIO_LINK_MODE I2S/SNDW */
	dev = pcidev_path_on_root(PCH_DEVFN_HDA);
	m_cfg->PchHdaEnable = is_dev_enabled(dev);

	m_cfg->PchHdaDspEnable = config->PchHdaDspEnable;
	m_cfg->PchHdaAudioLinkHdaEnable = config->PchHdaAudioLinkHdaEnable;
	m_cfg->PchHdaIDispCodecDisconnect = config->PchHdaIDispCodecDisconnect;
	memcpy(m_cfg->PchHdaAudioLinkDmicEnable, config->PchHdaAudioLinkDmicEnable,
			sizeof(m_cfg->PchHdaAudioLinkDmicEnable));
	memcpy(m_cfg->PchHdaAudioLinkSspEnable, config->PchHdaAudioLinkSspEnable,
			sizeof(m_cfg->PchHdaAudioLinkSspEnable));
	memcpy(m_cfg->PchHdaAudioLinkSndwEnable, config->PchHdaAudioLinkSndwEnable,
			sizeof(m_cfg->PchHdaAudioLinkSndwEnable));

	/* IPU configuration */
	dev = pcidev_path_on_root(SA_DEVFN_IPU);
	m_cfg->SaIpuEnable = is_dev_enabled(dev);

	/* Vt-D config */
	cpu_id = cpu_get_cpuid();
	if (cpu_id == CPUID_TIGERLAKE_A0) {
		/* Disable VT-d support for pre-QS platform */
		m_cfg->VtdDisable = 1;
	} else {
		/* Enable VT-d support for QS platform */
		m_cfg->VtdDisable = 0;
		m_cfg->VtdIopEnable = 0x1;

		if (m_cfg->InternalGfx) {
			m_cfg->VtdIgdEnable = 0x1;
			m_cfg->VtdBaseAddress[0] = GFXVT_BASE_ADDRESS;
		}

		if (m_cfg->SaIpuEnable) {
			m_cfg->VtdIpuEnable = 0x1;
			m_cfg->VtdBaseAddress[1] = IPUVT_BASE_ADDRESS;
		}

		m_cfg->VtdBaseAddress[2] = VTVC0_BASE_ADDRESS;

		if (m_cfg->TcssDma0En || m_cfg->TcssDma1En)
			m_cfg->VtdItbtEnable = 0x1;
		if (m_cfg->TcssItbtPcie0En)
			m_cfg->VtdBaseAddress[3] = TBT0_BASE_ADDRESS;
		if (m_cfg->TcssItbtPcie1En)
			m_cfg->VtdBaseAddress[4] = TBT1_BASE_ADDRESS;
		if (m_cfg->TcssItbtPcie2En)
			m_cfg->VtdBaseAddress[5] = TBT2_BASE_ADDRESS;
		if (m_cfg->TcssItbtPcie3En)
			m_cfg->VtdBaseAddress[6] = TBT3_BASE_ADDRESS;
	}

	/* Change VmxEnable UPD value according to ENABLE_VMX Kconfig */
	m_cfg->VmxEnable = CONFIG(ENABLE_VMX);

	/* Command Pins Mirrored */
	m_cfg->CmdMirror[0] = config->CmdMirror;

	/* Skip CPU replacement check */
	m_cfg->SkipCpuReplacementCheck = !config->CpuReplacementCheck;

	/* Skip CPU side PCIe enablement in FSP if device is disabled in devicetree */
	m_cfg->CpuPcieRpEnableMask = 0;
	dev = pcidev_path_on_root(SA_DEVFN_CPU_PCIE);
	if (is_dev_enabled(dev)) {
		m_cfg->CpuPcieRpEnableMask |= (1 << 0);
	}
	dev = pcidev_path_on_root(SA_DEVFN_PEG1);
	if (is_dev_enabled(dev)) {
		m_cfg->CpuPcieRpEnableMask |= (1 << 1);
	}
	dev = pcidev_path_on_root(SA_DEVFN_PEG2);
	if (is_dev_enabled(dev)) {
		m_cfg->CpuPcieRpEnableMask |= (1 << 2);
	}
	dev = pcidev_path_on_root(SA_DEVFN_PEG3);
	if (is_dev_enabled(dev)) {
		m_cfg->CpuPcieRpEnableMask |= (1 << 3);
	}

	/* Change TmeEnable UPD value according to INTEL_TME Kconfig */
	m_cfg->TmeEnable = CONFIG(INTEL_TME);
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	const struct soc_intel_tigerlake_config *config;
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;

	config = config_of_soc();

	soc_memory_init_params(m_cfg, config);
	mainboard_memory_init_params(mupd);
}

__weak void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}
