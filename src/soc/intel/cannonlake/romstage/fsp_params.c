/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <device/pci_def.h>
#include <device/pci.h>
#include <cpu/x86/msr.h>
#include <console/console.h>
#include <fsp/util.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/pmclib.h>
#include <soc/iomap.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/romstage.h>
#include <vendorcode/google/chromeos/chromeos.h>

#include "../chip.h"

static void soc_memory_init_params(FSPM_UPD *mupd, const config_t *config)
{
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;
	FSP_M_TEST_CONFIG *tconfig = &mupd->FspmTestConfig;

	unsigned int i;
	uint32_t mask = 0;
	const struct device *dev = pcidev_path_on_root(SA_DEVFN_IGD);

	/*
	 * Probe for no IGD and disable InternalGfx and panel power to prevent a
	 * crash in FSP-M.
	 */
	if (dev && dev->enabled && pci_read_config16(SA_DEV_IGD, PCI_VENDOR_ID) != 0xffff) {
		/* Set IGD stolen size to 64MB. */
		m_cfg->InternalGfx = 1;
		m_cfg->IgdDvmt50PreAlloc = 2;
	} else {
		m_cfg->InternalGfx = 0;
		m_cfg->IgdDvmt50PreAlloc = 0;
		tconfig->PanelPowerEnable = 0;
	}
	m_cfg->TsegSize = CONFIG_SMM_TSEG_SIZE;
	m_cfg->IedSize = CONFIG_IED_REGION_SIZE;
	m_cfg->SaGv = config->SaGv;
	if (CONFIG(SOC_INTEL_CANNONLAKE_PCH_H))
		m_cfg->UserBd = BOARD_TYPE_DESKTOP;
	else
		m_cfg->UserBd = BOARD_TYPE_ULT_ULX;
	m_cfg->RMT = config->RMT;

	for (i = 0; i < ARRAY_SIZE(config->PcieRpEnable); i++) {
		if (config->PcieRpEnable[i])
			mask |= (1 << i);
	}
	m_cfg->PcieRpEnableMask = mask;
	m_cfg->PrmrrSize = get_valid_prmrr_size();
	m_cfg->EnableC6Dram = config->enable_c6dram;
#if CONFIG(SOC_INTEL_COMETLAKE)
	m_cfg->SerialIoUartDebugControllerNumber = CONFIG_UART_FOR_CONSOLE;
#else
	m_cfg->PcdSerialIoUartNumber = CONFIG_UART_FOR_CONSOLE;
#endif
	/*
	 * PcdDebugInterfaceFlags
	 * This config will allow coreboot to pass information to the FSP
	 * regarding which debug interface is being used.
	 * Debug Interfaces:
	 * BIT0-RAM, BIT1-Legacy Uart BIT3-USB3, BIT4-LPSS Uart, BIT5-TraceHub
	 * BIT2 - Not used.
	 */
	m_cfg->PcdDebugInterfaceFlags =
				CONFIG(DRIVERS_UART_8250IO) ? 0x02 : 0x10;

	/* Change VmxEnable UPD value according to ENABLE_VMX Kconfig */
	m_cfg->VmxEnable = CONFIG(ENABLE_VMX);

#if CONFIG(SOC_INTEL_CANNONLAKE_ALTERNATE_HEADERS)
	m_cfg->SkipMpInit = !CONFIG(USE_INTEL_FSP_MP_INIT);
#endif

	if (config->cpu_ratio_override) {
		m_cfg->CpuRatio = config->cpu_ratio_override;
	} else {
		/* Set CpuRatio to match existing MSR value */
		msr_t flex_ratio;
		flex_ratio = rdmsr(MSR_FLEX_RATIO);
		m_cfg->CpuRatio = (flex_ratio.lo >> 8) & 0xff;
	}

	dev = pcidev_path_on_root(PCH_DEVFN_ISH);
	/* If ISH is enabled, enable ISH elements */
	if (!dev)
		m_cfg->PchIshEnable = 0;
	else
		m_cfg->PchIshEnable = dev->enabled;

	/* If HDA is enabled, enable HDA elements */
	dev = pcidev_path_on_root(PCH_DEVFN_HDA);
	if (!dev)
		m_cfg->PchHdaEnable = 0;
	else
		m_cfg->PchHdaEnable = dev->enabled;

	/* Enable IPU only if the device is enabled */
	m_cfg->SaIpuEnable = 0;
	dev = pcidev_path_on_root(SA_DEVFN_IPU);
	if (dev)
		m_cfg->SaIpuEnable = dev->enabled;

	/* SATA Gen3 strength */
	for (i = 0; i < SOC_INTEL_CML_SATA_DEV_MAX; i++) {
		if (config->sata_port[i].RxGen3EqBoostMagEnable) {
			m_cfg->PchSataHsioRxGen3EqBoostMagEnable[i] =
				config->sata_port[i].RxGen3EqBoostMagEnable;
			m_cfg->PchSataHsioRxGen3EqBoostMag[i] =
				config->sata_port[i].RxGen3EqBoostMag;
		}
		if (config->sata_port[i].TxGen3DownscaleAmpEnable) {
			m_cfg->PchSataHsioTxGen3DownscaleAmpEnable[i] =
				config->sata_port[i].TxGen3DownscaleAmpEnable;
			m_cfg->PchSataHsioTxGen3DownscaleAmp[i] =
				config->sata_port[i].TxGen3DownscaleAmp;
		}
		if (config->sata_port[i].TxGen3DeEmphEnable) {
			m_cfg->PchSataHsioTxGen3DeEmphEnable[i] =
				config->sata_port[i].TxGen3DeEmphEnable;
			m_cfg->PchSataHsioTxGen3DeEmph[i] =
				config->sata_port[i].TxGen3DeEmph;
		}
	}
#if !CONFIG(SOC_INTEL_COMETLAKE)
	if (config->DisableHeciRetry)
		tconfig->DisableHeciRetry = config->DisableHeciRetry;
#endif
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	const struct device *dev = pcidev_path_on_root(PCH_DEVFN_LPC);
	const struct device *smbus = pcidev_path_on_root(PCH_DEVFN_SMBUS);
	assert(dev != NULL);
	const config_t *config = config_of(dev);
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;
	FSP_M_TEST_CONFIG *tconfig = &mupd->FspmTestConfig;

	soc_memory_init_params(mupd, config);

	/* Enable SMBus controller based on config */
	if (!smbus)
		m_cfg->SmbusEnable = 0;
	else
		m_cfg->SmbusEnable = smbus->enabled;

	/* Set debug probe type */
	m_cfg->PlatformDebugConsent =
		CONFIG_SOC_INTEL_CANNONLAKE_DEBUG_CONSENT;

	/* Configure VT-d */
	tconfig->VtdDisable = 0;

	/* Set HECI1 PCI BAR address */
	m_cfg->Heci1BarAddress = HECI1_BASE_ADDRESS;

	mainboard_memory_init_params(mupd);
}

__weak void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}

#if CONFIG(SOC_INTEL_COMETLAKE)
void soc_display_fspm_upd_params(const FSPM_UPD *fspm_old_upd,
	const FSPM_UPD *fspm_new_upd)
{
	const FSP_M_CONFIG *new;
	const FSP_M_CONFIG *old;

	old = &fspm_old_upd->FspmConfig;
	new = &fspm_new_upd->FspmConfig;

	/* Display the parameters for MemoryInit */
	printk(BIOS_SPEW, "UPD values for MemoryInit:\n");

	#define DISPLAY_UPD(field) \
		fsp_display_upd_value(#field, sizeof(old->field), \
			old->field, new->field)

	DISPLAY_UPD(PlatformMemorySize);
	DISPLAY_UPD(MemorySpdPtr00);
	DISPLAY_UPD(MemorySpdPtr01);
	DISPLAY_UPD(MemorySpdPtr10);
	DISPLAY_UPD(MemorySpdPtr11);
	DISPLAY_UPD(MemorySpdDataLen);
	// DqByteMapCh0[12]
	DISPLAY_UPD(DqByteMapCh0[0]);
	DISPLAY_UPD(DqByteMapCh0[1]);
	DISPLAY_UPD(DqByteMapCh0[2]);
	DISPLAY_UPD(DqByteMapCh0[3]);
	DISPLAY_UPD(DqByteMapCh0[4]);
	DISPLAY_UPD(DqByteMapCh0[5]);
	DISPLAY_UPD(DqByteMapCh0[6]);
	DISPLAY_UPD(DqByteMapCh0[7]);
	DISPLAY_UPD(DqByteMapCh0[8]);
	DISPLAY_UPD(DqByteMapCh0[9]);
	DISPLAY_UPD(DqByteMapCh0[10]);
	DISPLAY_UPD(DqByteMapCh0[11]);
	// DqByteMapCh1[12]
	DISPLAY_UPD(DqByteMapCh1[0]);
	DISPLAY_UPD(DqByteMapCh1[1]);
	DISPLAY_UPD(DqByteMapCh1[2]);
	DISPLAY_UPD(DqByteMapCh1[3]);
	DISPLAY_UPD(DqByteMapCh1[4]);
	DISPLAY_UPD(DqByteMapCh1[5]);
	DISPLAY_UPD(DqByteMapCh1[6]);
	DISPLAY_UPD(DqByteMapCh1[7]);
	DISPLAY_UPD(DqByteMapCh1[8]);
	DISPLAY_UPD(DqByteMapCh1[9]);
	DISPLAY_UPD(DqByteMapCh1[10]);
	DISPLAY_UPD(DqByteMapCh1[11]);
	// DqsMapCpu2DramCh0[8]
	DISPLAY_UPD(DqsMapCpu2DramCh0[0]);
	DISPLAY_UPD(DqsMapCpu2DramCh0[1]);
	DISPLAY_UPD(DqsMapCpu2DramCh0[2]);
	DISPLAY_UPD(DqsMapCpu2DramCh0[3]);
	DISPLAY_UPD(DqsMapCpu2DramCh0[4]);
	DISPLAY_UPD(DqsMapCpu2DramCh0[5]);
	DISPLAY_UPD(DqsMapCpu2DramCh0[6]);
	DISPLAY_UPD(DqsMapCpu2DramCh0[7]);
	// DqsMapCpu2DramCh1[8]
	DISPLAY_UPD(DqsMapCpu2DramCh1[0]);
	DISPLAY_UPD(DqsMapCpu2DramCh1[1]);
	DISPLAY_UPD(DqsMapCpu2DramCh1[2]);
	DISPLAY_UPD(DqsMapCpu2DramCh1[3]);
	DISPLAY_UPD(DqsMapCpu2DramCh1[4]);
	DISPLAY_UPD(DqsMapCpu2DramCh1[5]);
	DISPLAY_UPD(DqsMapCpu2DramCh1[6]);
	DISPLAY_UPD(DqsMapCpu2DramCh1[7]);
	// RcompResistor[3]
	DISPLAY_UPD(RcompResistor[0]);
	DISPLAY_UPD(RcompResistor[1]);
	DISPLAY_UPD(RcompResistor[2]);
	// RcompTarget[5]
	DISPLAY_UPD(RcompTarget[0]);
	DISPLAY_UPD(RcompTarget[1]);
	DISPLAY_UPD(RcompTarget[2]);
	DISPLAY_UPD(RcompTarget[3]);
	DISPLAY_UPD(RcompTarget[4]);
	DISPLAY_UPD(DqPinsInterleaved);
	DISPLAY_UPD(CaVrefConfig);
	DISPLAY_UPD(SmramMask);
	DISPLAY_UPD(MrcTimeMeasure);
	DISPLAY_UPD(MrcFastBoot);
	DISPLAY_UPD(RmtPerTask);
	DISPLAY_UPD(TrainTrace);
	DISPLAY_UPD(IedSize);
	DISPLAY_UPD(TsegSize);
	DISPLAY_UPD(MmioSize);
	DISPLAY_UPD(ProbelessTrace);
	DISPLAY_UPD(GdxcIotSize);
	DISPLAY_UPD(GdxcMotSize);
	// SpdAddressTable[4]
	DISPLAY_UPD(SpdAddressTable[0]);
	DISPLAY_UPD(SpdAddressTable[1]);
	DISPLAY_UPD(SpdAddressTable[2]);
	DISPLAY_UPD(SpdAddressTable[3]);
	DISPLAY_UPD(IgdDvmt50PreAlloc);
	DISPLAY_UPD(InternalGfx);
	DISPLAY_UPD(ApertureSize);
	DISPLAY_UPD(UserBd);
	DISPLAY_UPD(SaGv);
	DISPLAY_UPD(DdrFreqLimit);
	DISPLAY_UPD(FreqSaGvLow);
	DISPLAY_UPD(RMT);
	DISPLAY_UPD(DisableDimmChannel0);
	DISPLAY_UPD(DisableDimmChannel1);
	DISPLAY_UPD(ScramblerSupport);
	DISPLAY_UPD(SkipMpInit);
	DISPLAY_UPD(SpdProfileSelected);
	DISPLAY_UPD(RefClk);
	DISPLAY_UPD(VddVoltage);
	DISPLAY_UPD(Ratio);
	DISPLAY_UPD(OddRatioMode);
	DISPLAY_UPD(tCL);
	DISPLAY_UPD(tCWL);
	DISPLAY_UPD(tRCDtRP);
	DISPLAY_UPD(tRRD);
	DISPLAY_UPD(tFAW);
	DISPLAY_UPD(tRAS);
	DISPLAY_UPD(tREFI);
	DISPLAY_UPD(tRFC);
	DISPLAY_UPD(tRTP);
	DISPLAY_UPD(tWR);
	DISPLAY_UPD(tWTR);
	DISPLAY_UPD(NModeSupport);
	DISPLAY_UPD(DllBwEn0);
	DISPLAY_UPD(DllBwEn1);
	DISPLAY_UPD(DllBwEn2);
	DISPLAY_UPD(DllBwEn3);
	DISPLAY_UPD(IsvtIoPort);
	DISPLAY_UPD(MarginLimitCheck);
	DISPLAY_UPD(MarginLimitL2);
	DISPLAY_UPD(CpuTraceHubMode);
	DISPLAY_UPD(CpuTraceHubMemReg0Size);
	DISPLAY_UPD(CpuTraceHubMemReg1Size);
	DISPLAY_UPD(PeciC10Reset);
	DISPLAY_UPD(PeciSxReset);
	DISPLAY_UPD(HeciTimeouts);
	DISPLAY_UPD(Heci1BarAddress);
	DISPLAY_UPD(Heci2BarAddress);
	DISPLAY_UPD(Heci3BarAddress);
	DISPLAY_UPD(SgDelayAfterPwrEn);
	DISPLAY_UPD(SgDelayAfterHoldReset);
	DISPLAY_UPD(MmioSizeAdjustment);
	DISPLAY_UPD(DmiGen3ProgramStaticEq);
	DISPLAY_UPD(Peg0Enable);
	DISPLAY_UPD(Peg1Enable);
	DISPLAY_UPD(Peg2Enable);
	DISPLAY_UPD(Peg3Enable);
	DISPLAY_UPD(Peg0MaxLinkSpeed);
	DISPLAY_UPD(Peg1MaxLinkSpeed);
	DISPLAY_UPD(Peg2MaxLinkSpeed);
	DISPLAY_UPD(Peg3MaxLinkSpeed);
	DISPLAY_UPD(Peg0MaxLinkWidth);
	DISPLAY_UPD(Peg1MaxLinkWidth);
	DISPLAY_UPD(Peg2MaxLinkWidth);
	DISPLAY_UPD(Peg3MaxLinkWidth);
	DISPLAY_UPD(Peg0PowerDownUnusedLanes);
	DISPLAY_UPD(Peg1PowerDownUnusedLanes);
	DISPLAY_UPD(Peg2PowerDownUnusedLanes);
	DISPLAY_UPD(Peg3PowerDownUnusedLanes);
	DISPLAY_UPD(InitPcieAspmAfterOprom);
	DISPLAY_UPD(PegDisableSpreadSpectrumClocking);
	// DmiGen3RootPortPreset[8]
	DISPLAY_UPD(DmiGen3RootPortPreset[0]);
	DISPLAY_UPD(DmiGen3RootPortPreset[1]);
	DISPLAY_UPD(DmiGen3RootPortPreset[2]);
	DISPLAY_UPD(DmiGen3RootPortPreset[3]);
	DISPLAY_UPD(DmiGen3RootPortPreset[4]);
	DISPLAY_UPD(DmiGen3RootPortPreset[5]);
	DISPLAY_UPD(DmiGen3RootPortPreset[6]);
	DISPLAY_UPD(DmiGen3RootPortPreset[7]);
	// DmiGen3EndPointPreset[8]
	DISPLAY_UPD(DmiGen3EndPointPreset[0]);
	DISPLAY_UPD(DmiGen3EndPointPreset[1]);
	DISPLAY_UPD(DmiGen3EndPointPreset[2]);
	DISPLAY_UPD(DmiGen3EndPointPreset[3]);
	DISPLAY_UPD(DmiGen3EndPointPreset[4]);
	DISPLAY_UPD(DmiGen3EndPointPreset[5]);
	DISPLAY_UPD(DmiGen3EndPointPreset[6]);
	DISPLAY_UPD(DmiGen3EndPointPreset[7]);
	// DmiGen3EndPointHint[8]
	DISPLAY_UPD(DmiGen3EndPointHint[0]);
	DISPLAY_UPD(DmiGen3EndPointHint[1]);
	DISPLAY_UPD(DmiGen3EndPointHint[2]);
	DISPLAY_UPD(DmiGen3EndPointHint[3]);
	DISPLAY_UPD(DmiGen3EndPointHint[4]);
	DISPLAY_UPD(DmiGen3EndPointHint[5]);
	DISPLAY_UPD(DmiGen3EndPointHint[6]);
	DISPLAY_UPD(DmiGen3EndPointHint[7]);
	// DmiGen3RxCtlePeaking[4]
	DISPLAY_UPD(DmiGen3RxCtlePeaking[0]);
	DISPLAY_UPD(DmiGen3RxCtlePeaking[1]);
	DISPLAY_UPD(DmiGen3RxCtlePeaking[2]);
	DISPLAY_UPD(DmiGen3RxCtlePeaking[3]);
	DISPLAY_UPD(TvbRatioClipping);
	DISPLAY_UPD(TvbVoltageOptimization);
	// PegGen3RxCtlePeaking[10]
	DISPLAY_UPD(PegGen3RxCtlePeaking[0]);
	DISPLAY_UPD(PegGen3RxCtlePeaking[1]);
	DISPLAY_UPD(PegGen3RxCtlePeaking[2]);
	DISPLAY_UPD(PegGen3RxCtlePeaking[3]);
	DISPLAY_UPD(PegGen3RxCtlePeaking[4]);
	DISPLAY_UPD(PegGen3RxCtlePeaking[5]);
	DISPLAY_UPD(PegGen3RxCtlePeaking[6]);
	DISPLAY_UPD(PegGen3RxCtlePeaking[7]);
	DISPLAY_UPD(PegGen3RxCtlePeaking[8]);
	DISPLAY_UPD(PegGen3RxCtlePeaking[9]);
	DISPLAY_UPD(PegDataPtr);
	// PegGpioData[28]
	DISPLAY_UPD(PegGpioData[0]);
	DISPLAY_UPD(PegGpioData[1]);
	DISPLAY_UPD(PegGpioData[2]);
	DISPLAY_UPD(PegGpioData[3]);
	DISPLAY_UPD(PegGpioData[4]);
	DISPLAY_UPD(PegGpioData[5]);
	DISPLAY_UPD(PegGpioData[6]);
	DISPLAY_UPD(PegGpioData[7]);
	DISPLAY_UPD(PegGpioData[8]);
	DISPLAY_UPD(PegGpioData[9]);
	DISPLAY_UPD(PegGpioData[10]);
	DISPLAY_UPD(PegGpioData[11]);
	DISPLAY_UPD(PegGpioData[12]);
	DISPLAY_UPD(PegGpioData[13]);
	DISPLAY_UPD(PegGpioData[14]);
	DISPLAY_UPD(PegGpioData[15]);
	DISPLAY_UPD(PegGpioData[16]);
	DISPLAY_UPD(PegGpioData[17]);
	DISPLAY_UPD(PegGpioData[18]);
	DISPLAY_UPD(PegGpioData[19]);
	DISPLAY_UPD(PegGpioData[20]);
	DISPLAY_UPD(PegGpioData[21]);
	DISPLAY_UPD(PegGpioData[22]);
	DISPLAY_UPD(PegGpioData[23]);
	DISPLAY_UPD(PegGpioData[24]);
	DISPLAY_UPD(PegGpioData[25]);
	DISPLAY_UPD(PegGpioData[26]);
	DISPLAY_UPD(PegGpioData[27]);
	// PegRootPortHPE[4]
	DISPLAY_UPD(PegRootPortHPE[0]);
	DISPLAY_UPD(PegRootPortHPE[1]);
	DISPLAY_UPD(PegRootPortHPE[2]);
	DISPLAY_UPD(PegRootPortHPE[3]);
	DISPLAY_UPD(DmiDeEmphasis);
	DISPLAY_UPD(PrimaryDisplay);
	DISPLAY_UPD(GttSize);
	DISPLAY_UPD(GmAdr);
	DISPLAY_UPD(GttMmAdr);
	DISPLAY_UPD(PsmiRegionSize);
	// SaRtd3Pcie0Gpio[24]
	DISPLAY_UPD(SaRtd3Pcie0Gpio[0]);
	DISPLAY_UPD(SaRtd3Pcie0Gpio[1]);
	DISPLAY_UPD(SaRtd3Pcie0Gpio[2]);
	DISPLAY_UPD(SaRtd3Pcie0Gpio[3]);
	DISPLAY_UPD(SaRtd3Pcie0Gpio[4]);
	DISPLAY_UPD(SaRtd3Pcie0Gpio[5]);
	DISPLAY_UPD(SaRtd3Pcie0Gpio[6]);
	DISPLAY_UPD(SaRtd3Pcie0Gpio[7]);
	DISPLAY_UPD(SaRtd3Pcie0Gpio[8]);
	DISPLAY_UPD(SaRtd3Pcie0Gpio[9]);
	DISPLAY_UPD(SaRtd3Pcie0Gpio[10]);
	DISPLAY_UPD(SaRtd3Pcie0Gpio[11]);
	DISPLAY_UPD(SaRtd3Pcie0Gpio[12]);
	DISPLAY_UPD(SaRtd3Pcie0Gpio[13]);
	DISPLAY_UPD(SaRtd3Pcie0Gpio[14]);
	DISPLAY_UPD(SaRtd3Pcie0Gpio[15]);
	DISPLAY_UPD(SaRtd3Pcie0Gpio[16]);
	DISPLAY_UPD(SaRtd3Pcie0Gpio[17]);
	DISPLAY_UPD(SaRtd3Pcie0Gpio[18]);
	DISPLAY_UPD(SaRtd3Pcie0Gpio[19]);
	DISPLAY_UPD(SaRtd3Pcie0Gpio[20]);
	DISPLAY_UPD(SaRtd3Pcie0Gpio[21]);
	DISPLAY_UPD(SaRtd3Pcie0Gpio[22]);
	DISPLAY_UPD(SaRtd3Pcie0Gpio[23]);
	// SaRtd3Pcie1Gpio[24]
	DISPLAY_UPD(SaRtd3Pcie1Gpio[0]);
	DISPLAY_UPD(SaRtd3Pcie1Gpio[1]);
	DISPLAY_UPD(SaRtd3Pcie1Gpio[2]);
	DISPLAY_UPD(SaRtd3Pcie1Gpio[3]);
	DISPLAY_UPD(SaRtd3Pcie1Gpio[4]);
	DISPLAY_UPD(SaRtd3Pcie1Gpio[5]);
	DISPLAY_UPD(SaRtd3Pcie1Gpio[6]);
	DISPLAY_UPD(SaRtd3Pcie1Gpio[7]);
	DISPLAY_UPD(SaRtd3Pcie1Gpio[8]);
	DISPLAY_UPD(SaRtd3Pcie1Gpio[9]);
	DISPLAY_UPD(SaRtd3Pcie1Gpio[10]);
	DISPLAY_UPD(SaRtd3Pcie1Gpio[11]);
	DISPLAY_UPD(SaRtd3Pcie1Gpio[12]);
	DISPLAY_UPD(SaRtd3Pcie1Gpio[13]);
	DISPLAY_UPD(SaRtd3Pcie1Gpio[14]);
	DISPLAY_UPD(SaRtd3Pcie1Gpio[15]);
	DISPLAY_UPD(SaRtd3Pcie1Gpio[16]);
	DISPLAY_UPD(SaRtd3Pcie1Gpio[17]);
	DISPLAY_UPD(SaRtd3Pcie1Gpio[18]);
	DISPLAY_UPD(SaRtd3Pcie1Gpio[19]);
	DISPLAY_UPD(SaRtd3Pcie1Gpio[20]);
	DISPLAY_UPD(SaRtd3Pcie1Gpio[21]);
	DISPLAY_UPD(SaRtd3Pcie1Gpio[22]);
	DISPLAY_UPD(SaRtd3Pcie1Gpio[23]);
	// SaRtd3Pcie2Gpio[24]
	DISPLAY_UPD(SaRtd3Pcie2Gpio[0]);
	DISPLAY_UPD(SaRtd3Pcie2Gpio[1]);
	DISPLAY_UPD(SaRtd3Pcie2Gpio[2]);
	DISPLAY_UPD(SaRtd3Pcie2Gpio[3]);
	DISPLAY_UPD(SaRtd3Pcie2Gpio[4]);
	DISPLAY_UPD(SaRtd3Pcie2Gpio[5]);
	DISPLAY_UPD(SaRtd3Pcie2Gpio[6]);
	DISPLAY_UPD(SaRtd3Pcie2Gpio[7]);
	DISPLAY_UPD(SaRtd3Pcie2Gpio[8]);
	DISPLAY_UPD(SaRtd3Pcie2Gpio[9]);
	DISPLAY_UPD(SaRtd3Pcie2Gpio[10]);
	DISPLAY_UPD(SaRtd3Pcie2Gpio[11]);
	DISPLAY_UPD(SaRtd3Pcie2Gpio[12]);
	DISPLAY_UPD(SaRtd3Pcie2Gpio[13]);
	DISPLAY_UPD(SaRtd3Pcie2Gpio[14]);
	DISPLAY_UPD(SaRtd3Pcie2Gpio[15]);
	DISPLAY_UPD(SaRtd3Pcie2Gpio[16]);
	DISPLAY_UPD(SaRtd3Pcie2Gpio[17]);
	DISPLAY_UPD(SaRtd3Pcie2Gpio[18]);
	DISPLAY_UPD(SaRtd3Pcie2Gpio[19]);
	DISPLAY_UPD(SaRtd3Pcie2Gpio[20]);
	DISPLAY_UPD(SaRtd3Pcie2Gpio[21]);
	DISPLAY_UPD(SaRtd3Pcie2Gpio[22]);
	DISPLAY_UPD(SaRtd3Pcie2Gpio[23]);
	// SaRtd3Pcie3Gpio[24]
	DISPLAY_UPD(SaRtd3Pcie3Gpio[0]);
	DISPLAY_UPD(SaRtd3Pcie3Gpio[1]);
	DISPLAY_UPD(SaRtd3Pcie3Gpio[2]);
	DISPLAY_UPD(SaRtd3Pcie3Gpio[3]);
	DISPLAY_UPD(SaRtd3Pcie3Gpio[4]);
	DISPLAY_UPD(SaRtd3Pcie3Gpio[5]);
	DISPLAY_UPD(SaRtd3Pcie3Gpio[6]);
	DISPLAY_UPD(SaRtd3Pcie3Gpio[7]);
	DISPLAY_UPD(SaRtd3Pcie3Gpio[8]);
	DISPLAY_UPD(SaRtd3Pcie3Gpio[9]);
	DISPLAY_UPD(SaRtd3Pcie3Gpio[10]);
	DISPLAY_UPD(SaRtd3Pcie3Gpio[11]);
	DISPLAY_UPD(SaRtd3Pcie3Gpio[12]);
	DISPLAY_UPD(SaRtd3Pcie3Gpio[13]);
	DISPLAY_UPD(SaRtd3Pcie3Gpio[14]);
	DISPLAY_UPD(SaRtd3Pcie3Gpio[15]);
	DISPLAY_UPD(SaRtd3Pcie3Gpio[16]);
	DISPLAY_UPD(SaRtd3Pcie3Gpio[17]);
	DISPLAY_UPD(SaRtd3Pcie3Gpio[18]);
	DISPLAY_UPD(SaRtd3Pcie3Gpio[19]);
	DISPLAY_UPD(SaRtd3Pcie3Gpio[20]);
	DISPLAY_UPD(SaRtd3Pcie3Gpio[21]);
	DISPLAY_UPD(SaRtd3Pcie3Gpio[22]);
	DISPLAY_UPD(SaRtd3Pcie3Gpio[23]);
	DISPLAY_UPD(TxtImplemented);
	DISPLAY_UPD(SaOcSupport);
	DISPLAY_UPD(GtVoltageMode);
	DISPLAY_UPD(GtMaxOcRatio);
	DISPLAY_UPD(GtVoltageOffset);
	DISPLAY_UPD(GtVoltageOverride);
	DISPLAY_UPD(GtExtraTurboVoltage);
	DISPLAY_UPD(SaVoltageOffset);
	DISPLAY_UPD(RootPortIndex);
	DISPLAY_UPD(RealtimeMemoryTiming);
	DISPLAY_UPD(SaIpuEnable);
	DISPLAY_UPD(SaIpuImrConfiguration);
	DISPLAY_UPD(GtPsmiSupport);
	DISPLAY_UPD(GtusVoltageMode);
	DISPLAY_UPD(GtusVoltageOffset);
	DISPLAY_UPD(GtusVoltageOverride);
	DISPLAY_UPD(GtusExtraTurboVoltage);
	DISPLAY_UPD(GtusMaxOcRatio);
	// SaPreMemProductionRsvd[1]
	DISPLAY_UPD(SaPreMemProductionRsvd[0]);
	DISPLAY_UPD(PerCoreHtDisable);
	DISPLAY_UPD(BistOnReset);
	DISPLAY_UPD(SkipStopPbet);
	DISPLAY_UPD(EnableC6Dram);
	DISPLAY_UPD(OcSupport);
	DISPLAY_UPD(OcLock);
	DISPLAY_UPD(CoreMaxOcRatio);
	DISPLAY_UPD(CoreVoltageMode);
	DISPLAY_UPD(DisableMtrrProgram);
	DISPLAY_UPD(RingMaxOcRatio);
	DISPLAY_UPD(HyperThreading);
	DISPLAY_UPD(CpuRatio);
	DISPLAY_UPD(BootFrequency);
	DISPLAY_UPD(ActiveCoreCount);
	DISPLAY_UPD(FClkFrequency);
	DISPLAY_UPD(JtagC10PowerGateDisable);
	DISPLAY_UPD(VmxEnable);
	DISPLAY_UPD(Avx2RatioOffset);
	DISPLAY_UPD(Avx3RatioOffset);
	DISPLAY_UPD(BclkAdaptiveVoltage);
	DISPLAY_UPD(CorePllVoltageOffset);
	DISPLAY_UPD(CoreVoltageOverride);
	DISPLAY_UPD(CoreVoltageAdaptive);
	DISPLAY_UPD(CoreVoltageOffset);
	DISPLAY_UPD(RingDownBin);
	DISPLAY_UPD(RingVoltageMode);
	DISPLAY_UPD(RingVoltageOverride);
	DISPLAY_UPD(RingVoltageAdaptive);
	DISPLAY_UPD(RingVoltageOffset);
	DISPLAY_UPD(TjMaxOffset);
	DISPLAY_UPD(BiosGuard);
	DISPLAY_UPD(BiosGuardToolsInterface);
	DISPLAY_UPD(EnableSgx);
	DISPLAY_UPD(Txt);
	DISPLAY_UPD(PrmrrSize);
	DISPLAY_UPD(SinitMemorySize);
	DISPLAY_UPD(TxtHeapMemorySize);
	DISPLAY_UPD(TxtDprMemorySize);
	DISPLAY_UPD(TxtDprMemoryBase);
	DISPLAY_UPD(BiosAcmBase);
	DISPLAY_UPD(BiosAcmSize);
	DISPLAY_UPD(ApStartupBase);
	DISPLAY_UPD(TgaSize);
	DISPLAY_UPD(TxtLcpPdBase);
	DISPLAY_UPD(TxtLcpPdSize);
	DISPLAY_UPD(IsTPMPresence);
	DISPLAY_UPD(AutoEasyOverclock);
	// VtdBaseAddress[3]
	DISPLAY_UPD(VtdBaseAddress[0]);
	DISPLAY_UPD(VtdBaseAddress[1]);
	DISPLAY_UPD(VtdBaseAddress[2]);
	DISPLAY_UPD(SmbusEnable);
	DISPLAY_UPD(PlatformDebugConsent);
	DISPLAY_UPD(DciUsb3TypecUfpDbg);
	DISPLAY_UPD(PchTraceHubMode);
	DISPLAY_UPD(PchTraceHubMemReg0Size);
	DISPLAY_UPD(PchTraceHubMemReg1Size);
	DISPLAY_UPD(PchHdaEnable);
	DISPLAY_UPD(PchIshEnable);
	// PchPcieHsioRxSetCtleEnable[24]
	DISPLAY_UPD(PchPcieHsioRxSetCtleEnable[0]);
	DISPLAY_UPD(PchPcieHsioRxSetCtleEnable[1]);
	DISPLAY_UPD(PchPcieHsioRxSetCtleEnable[2]);
	DISPLAY_UPD(PchPcieHsioRxSetCtleEnable[3]);
	DISPLAY_UPD(PchPcieHsioRxSetCtleEnable[4]);
	DISPLAY_UPD(PchPcieHsioRxSetCtleEnable[5]);
	DISPLAY_UPD(PchPcieHsioRxSetCtleEnable[6]);
	DISPLAY_UPD(PchPcieHsioRxSetCtleEnable[7]);
	DISPLAY_UPD(PchPcieHsioRxSetCtleEnable[8]);
	DISPLAY_UPD(PchPcieHsioRxSetCtleEnable[9]);
	DISPLAY_UPD(PchPcieHsioRxSetCtleEnable[10]);
	DISPLAY_UPD(PchPcieHsioRxSetCtleEnable[11]);
	DISPLAY_UPD(PchPcieHsioRxSetCtleEnable[12]);
	DISPLAY_UPD(PchPcieHsioRxSetCtleEnable[13]);
	DISPLAY_UPD(PchPcieHsioRxSetCtleEnable[14]);
	DISPLAY_UPD(PchPcieHsioRxSetCtleEnable[15]);
	DISPLAY_UPD(PchPcieHsioRxSetCtleEnable[16]);
	DISPLAY_UPD(PchPcieHsioRxSetCtleEnable[17]);
	DISPLAY_UPD(PchPcieHsioRxSetCtleEnable[18]);
	DISPLAY_UPD(PchPcieHsioRxSetCtleEnable[19]);
	DISPLAY_UPD(PchPcieHsioRxSetCtleEnable[20]);
	DISPLAY_UPD(PchPcieHsioRxSetCtleEnable[21]);
	DISPLAY_UPD(PchPcieHsioRxSetCtleEnable[22]);
	DISPLAY_UPD(PchPcieHsioRxSetCtleEnable[23]);
	// PchPcieHsioRxSetCtle[24]
	DISPLAY_UPD(PchPcieHsioRxSetCtle[0]);
	DISPLAY_UPD(PchPcieHsioRxSetCtle[1]);
	DISPLAY_UPD(PchPcieHsioRxSetCtle[2]);
	DISPLAY_UPD(PchPcieHsioRxSetCtle[3]);
	DISPLAY_UPD(PchPcieHsioRxSetCtle[4]);
	DISPLAY_UPD(PchPcieHsioRxSetCtle[5]);
	DISPLAY_UPD(PchPcieHsioRxSetCtle[6]);
	DISPLAY_UPD(PchPcieHsioRxSetCtle[7]);
	DISPLAY_UPD(PchPcieHsioRxSetCtle[8]);
	DISPLAY_UPD(PchPcieHsioRxSetCtle[9]);
	DISPLAY_UPD(PchPcieHsioRxSetCtle[10]);
	DISPLAY_UPD(PchPcieHsioRxSetCtle[11]);
	DISPLAY_UPD(PchPcieHsioRxSetCtle[12]);
	DISPLAY_UPD(PchPcieHsioRxSetCtle[13]);
	DISPLAY_UPD(PchPcieHsioRxSetCtle[14]);
	DISPLAY_UPD(PchPcieHsioRxSetCtle[15]);
	DISPLAY_UPD(PchPcieHsioRxSetCtle[16]);
	DISPLAY_UPD(PchPcieHsioRxSetCtle[17]);
	DISPLAY_UPD(PchPcieHsioRxSetCtle[18]);
	DISPLAY_UPD(PchPcieHsioRxSetCtle[19]);
	DISPLAY_UPD(PchPcieHsioRxSetCtle[20]);
	DISPLAY_UPD(PchPcieHsioRxSetCtle[21]);
	DISPLAY_UPD(PchPcieHsioRxSetCtle[22]);
	DISPLAY_UPD(PchPcieHsioRxSetCtle[23]);
	// PchPcieHsioTxGen1DownscaleAmpEnable[24]
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmpEnable[0]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmpEnable[1]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmpEnable[2]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmpEnable[3]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmpEnable[4]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmpEnable[5]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmpEnable[6]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmpEnable[7]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmpEnable[8]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmpEnable[9]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmpEnable[10]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmpEnable[11]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmpEnable[12]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmpEnable[13]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmpEnable[14]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmpEnable[15]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmpEnable[16]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmpEnable[17]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmpEnable[18]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmpEnable[19]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmpEnable[20]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmpEnable[21]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmpEnable[22]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmpEnable[23]);
	// PchPcieHsioTxGen1DownscaleAmp[24]
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmp[0]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmp[1]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmp[2]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmp[3]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmp[4]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmp[5]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmp[6]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmp[7]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmp[8]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmp[9]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmp[10]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmp[11]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmp[12]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmp[13]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmp[14]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmp[15]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmp[16]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmp[17]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmp[18]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmp[19]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmp[20]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmp[21]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmp[22]);
	DISPLAY_UPD(PchPcieHsioTxGen1DownscaleAmp[23]);
	// PchPcieHsioTxGen2DownscaleAmpEnable[24]
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmpEnable[0]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmpEnable[1]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmpEnable[2]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmpEnable[3]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmpEnable[4]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmpEnable[5]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmpEnable[6]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmpEnable[7]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmpEnable[8]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmpEnable[9]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmpEnable[10]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmpEnable[11]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmpEnable[12]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmpEnable[13]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmpEnable[14]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmpEnable[15]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmpEnable[16]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmpEnable[17]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmpEnable[18]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmpEnable[19]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmpEnable[20]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmpEnable[21]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmpEnable[22]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmpEnable[23]);
	// PchPcieHsioTxGen2DownscaleAmp[24]
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmp[0]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmp[1]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmp[2]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmp[3]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmp[4]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmp[5]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmp[6]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmp[7]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmp[8]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmp[9]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmp[10]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmp[11]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmp[12]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmp[13]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmp[14]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmp[15]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmp[16]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmp[17]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmp[18]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmp[19]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmp[20]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmp[21]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmp[22]);
	DISPLAY_UPD(PchPcieHsioTxGen2DownscaleAmp[23]);
	// PchPcieHsioTxGen3DownscaleAmpEnable[24]
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmpEnable[0]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmpEnable[1]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmpEnable[2]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmpEnable[3]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmpEnable[4]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmpEnable[5]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmpEnable[6]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmpEnable[7]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmpEnable[8]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmpEnable[9]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmpEnable[10]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmpEnable[11]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmpEnable[12]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmpEnable[13]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmpEnable[14]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmpEnable[15]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmpEnable[16]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmpEnable[17]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmpEnable[18]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmpEnable[19]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmpEnable[20]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmpEnable[21]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmpEnable[22]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmpEnable[23]);
	// PchPcieHsioTxGen3DownscaleAmp[24]
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmp[0]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmp[1]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmp[2]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmp[3]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmp[4]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmp[5]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmp[6]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmp[7]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmp[8]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmp[9]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmp[10]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmp[11]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmp[12]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmp[13]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmp[14]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmp[15]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmp[16]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmp[17]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmp[18]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmp[19]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmp[20]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmp[21]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmp[22]);
	DISPLAY_UPD(PchPcieHsioTxGen3DownscaleAmp[23]);
	// PchPcieHsioTxGen1DeEmphEnable[24]
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmphEnable[0]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmphEnable[1]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmphEnable[2]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmphEnable[3]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmphEnable[4]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmphEnable[5]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmphEnable[6]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmphEnable[7]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmphEnable[8]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmphEnable[9]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmphEnable[10]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmphEnable[11]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmphEnable[12]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmphEnable[13]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmphEnable[14]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmphEnable[15]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmphEnable[16]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmphEnable[17]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmphEnable[18]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmphEnable[19]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmphEnable[20]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmphEnable[21]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmphEnable[22]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmphEnable[23]);
	// PchPcieHsioTxGen1DeEmph[24]
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmph[0]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmph[1]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmph[2]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmph[3]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmph[4]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmph[5]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmph[6]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmph[7]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmph[8]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmph[9]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmph[10]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmph[11]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmph[12]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmph[13]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmph[14]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmph[15]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmph[16]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmph[17]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmph[18]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmph[19]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmph[20]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmph[21]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmph[22]);
	DISPLAY_UPD(PchPcieHsioTxGen1DeEmph[23]);
	// PchPcieHsioTxGen2DeEmph3p5Enable[24]
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5Enable[0]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5Enable[1]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5Enable[2]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5Enable[3]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5Enable[4]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5Enable[5]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5Enable[6]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5Enable[7]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5Enable[8]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5Enable[9]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5Enable[10]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5Enable[11]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5Enable[12]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5Enable[13]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5Enable[14]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5Enable[15]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5Enable[16]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5Enable[17]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5Enable[18]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5Enable[19]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5Enable[20]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5Enable[21]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5Enable[22]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5Enable[23]);
	// PchPcieHsioTxGen2DeEmph3p5[24]
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5[0]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5[1]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5[2]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5[3]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5[4]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5[5]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5[6]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5[7]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5[8]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5[9]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5[10]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5[11]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5[12]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5[13]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5[14]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5[15]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5[16]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5[17]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5[18]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5[19]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5[20]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5[21]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5[22]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph3p5[23]);
	// PchPcieHsioTxGen2DeEmph6p0Enable[24]
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0Enable[0]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0Enable[1]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0Enable[2]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0Enable[3]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0Enable[4]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0Enable[5]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0Enable[6]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0Enable[7]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0Enable[8]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0Enable[9]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0Enable[10]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0Enable[11]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0Enable[12]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0Enable[13]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0Enable[14]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0Enable[15]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0Enable[16]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0Enable[17]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0Enable[18]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0Enable[19]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0Enable[20]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0Enable[21]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0Enable[22]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0Enable[23]);
	// PchPcieHsioTxGen2DeEmph6p0[24]
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0[0]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0[1]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0[2]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0[3]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0[4]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0[5]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0[6]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0[7]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0[8]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0[9]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0[10]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0[11]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0[12]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0[13]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0[14]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0[15]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0[16]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0[17]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0[18]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0[19]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0[20]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0[21]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0[22]);
	DISPLAY_UPD(PchPcieHsioTxGen2DeEmph6p0[23]);
	// PchSataHsioRxGen1EqBoostMagEnable[8]
	DISPLAY_UPD(PchSataHsioRxGen1EqBoostMagEnable[0]);
	DISPLAY_UPD(PchSataHsioRxGen1EqBoostMagEnable[1]);
	DISPLAY_UPD(PchSataHsioRxGen1EqBoostMagEnable[2]);
	DISPLAY_UPD(PchSataHsioRxGen1EqBoostMagEnable[3]);
	DISPLAY_UPD(PchSataHsioRxGen1EqBoostMagEnable[4]);
	DISPLAY_UPD(PchSataHsioRxGen1EqBoostMagEnable[5]);
	DISPLAY_UPD(PchSataHsioRxGen1EqBoostMagEnable[6]);
	DISPLAY_UPD(PchSataHsioRxGen1EqBoostMagEnable[7]);
	// PchSataHsioRxGen1EqBoostMag[8]
	DISPLAY_UPD(PchSataHsioRxGen1EqBoostMag[0]);
	DISPLAY_UPD(PchSataHsioRxGen1EqBoostMag[1]);
	DISPLAY_UPD(PchSataHsioRxGen1EqBoostMag[2]);
	DISPLAY_UPD(PchSataHsioRxGen1EqBoostMag[3]);
	DISPLAY_UPD(PchSataHsioRxGen1EqBoostMag[4]);
	DISPLAY_UPD(PchSataHsioRxGen1EqBoostMag[5]);
	DISPLAY_UPD(PchSataHsioRxGen1EqBoostMag[6]);
	DISPLAY_UPD(PchSataHsioRxGen1EqBoostMag[7]);
	// PchSataHsioRxGen2EqBoostMagEnable[8]
	DISPLAY_UPD(PchSataHsioRxGen2EqBoostMagEnable[0]);
	DISPLAY_UPD(PchSataHsioRxGen2EqBoostMagEnable[1]);
	DISPLAY_UPD(PchSataHsioRxGen2EqBoostMagEnable[2]);
	DISPLAY_UPD(PchSataHsioRxGen2EqBoostMagEnable[3]);
	DISPLAY_UPD(PchSataHsioRxGen2EqBoostMagEnable[4]);
	DISPLAY_UPD(PchSataHsioRxGen2EqBoostMagEnable[5]);
	DISPLAY_UPD(PchSataHsioRxGen2EqBoostMagEnable[6]);
	DISPLAY_UPD(PchSataHsioRxGen2EqBoostMagEnable[7]);
	// PchSataHsioRxGen2EqBoostMag[8]
	DISPLAY_UPD(PchSataHsioRxGen2EqBoostMag[0]);
	DISPLAY_UPD(PchSataHsioRxGen2EqBoostMag[1]);
	DISPLAY_UPD(PchSataHsioRxGen2EqBoostMag[2]);
	DISPLAY_UPD(PchSataHsioRxGen2EqBoostMag[3]);
	DISPLAY_UPD(PchSataHsioRxGen2EqBoostMag[4]);
	DISPLAY_UPD(PchSataHsioRxGen2EqBoostMag[5]);
	DISPLAY_UPD(PchSataHsioRxGen2EqBoostMag[6]);
	DISPLAY_UPD(PchSataHsioRxGen2EqBoostMag[7]);
	// PchSataHsioRxGen3EqBoostMagEnable[8]
	DISPLAY_UPD(PchSataHsioRxGen3EqBoostMagEnable[0]);
	DISPLAY_UPD(PchSataHsioRxGen3EqBoostMagEnable[1]);
	DISPLAY_UPD(PchSataHsioRxGen3EqBoostMagEnable[2]);
	DISPLAY_UPD(PchSataHsioRxGen3EqBoostMagEnable[3]);
	DISPLAY_UPD(PchSataHsioRxGen3EqBoostMagEnable[4]);
	DISPLAY_UPD(PchSataHsioRxGen3EqBoostMagEnable[5]);
	DISPLAY_UPD(PchSataHsioRxGen3EqBoostMagEnable[6]);
	DISPLAY_UPD(PchSataHsioRxGen3EqBoostMagEnable[7]);
	// PchSataHsioRxGen3EqBoostMag[8]
	DISPLAY_UPD(PchSataHsioRxGen3EqBoostMag[0]);
	DISPLAY_UPD(PchSataHsioRxGen3EqBoostMag[1]);
	DISPLAY_UPD(PchSataHsioRxGen3EqBoostMag[2]);
	DISPLAY_UPD(PchSataHsioRxGen3EqBoostMag[3]);
	DISPLAY_UPD(PchSataHsioRxGen3EqBoostMag[4]);
	DISPLAY_UPD(PchSataHsioRxGen3EqBoostMag[5]);
	DISPLAY_UPD(PchSataHsioRxGen3EqBoostMag[6]);
	DISPLAY_UPD(PchSataHsioRxGen3EqBoostMag[7]);
	// PchSataHsioTxGen1DownscaleAmpEnable[8]
	DISPLAY_UPD(PchSataHsioTxGen1DownscaleAmpEnable[0]);
	DISPLAY_UPD(PchSataHsioTxGen1DownscaleAmpEnable[1]);
	DISPLAY_UPD(PchSataHsioTxGen1DownscaleAmpEnable[2]);
	DISPLAY_UPD(PchSataHsioTxGen1DownscaleAmpEnable[3]);
	DISPLAY_UPD(PchSataHsioTxGen1DownscaleAmpEnable[4]);
	DISPLAY_UPD(PchSataHsioTxGen1DownscaleAmpEnable[5]);
	DISPLAY_UPD(PchSataHsioTxGen1DownscaleAmpEnable[6]);
	DISPLAY_UPD(PchSataHsioTxGen1DownscaleAmpEnable[7]);
	// PchSataHsioTxGen1DownscaleAmp[8]
	DISPLAY_UPD(PchSataHsioTxGen1DownscaleAmp[0]);
	DISPLAY_UPD(PchSataHsioTxGen1DownscaleAmp[1]);
	DISPLAY_UPD(PchSataHsioTxGen1DownscaleAmp[2]);
	DISPLAY_UPD(PchSataHsioTxGen1DownscaleAmp[3]);
	DISPLAY_UPD(PchSataHsioTxGen1DownscaleAmp[4]);
	DISPLAY_UPD(PchSataHsioTxGen1DownscaleAmp[5]);
	DISPLAY_UPD(PchSataHsioTxGen1DownscaleAmp[6]);
	DISPLAY_UPD(PchSataHsioTxGen1DownscaleAmp[7]);
	// PchSataHsioTxGen2DownscaleAmpEnable[8]
	DISPLAY_UPD(PchSataHsioTxGen2DownscaleAmpEnable[0]);
	DISPLAY_UPD(PchSataHsioTxGen2DownscaleAmpEnable[1]);
	DISPLAY_UPD(PchSataHsioTxGen2DownscaleAmpEnable[2]);
	DISPLAY_UPD(PchSataHsioTxGen2DownscaleAmpEnable[3]);
	DISPLAY_UPD(PchSataHsioTxGen2DownscaleAmpEnable[4]);
	DISPLAY_UPD(PchSataHsioTxGen2DownscaleAmpEnable[5]);
	DISPLAY_UPD(PchSataHsioTxGen2DownscaleAmpEnable[6]);
	DISPLAY_UPD(PchSataHsioTxGen2DownscaleAmpEnable[7]);
	// PchSataHsioTxGen2DownscaleAmp[8]
	DISPLAY_UPD(PchSataHsioTxGen2DownscaleAmp[0]);
	DISPLAY_UPD(PchSataHsioTxGen2DownscaleAmp[1]);
	DISPLAY_UPD(PchSataHsioTxGen2DownscaleAmp[2]);
	DISPLAY_UPD(PchSataHsioTxGen2DownscaleAmp[3]);
	DISPLAY_UPD(PchSataHsioTxGen2DownscaleAmp[4]);
	DISPLAY_UPD(PchSataHsioTxGen2DownscaleAmp[5]);
	DISPLAY_UPD(PchSataHsioTxGen2DownscaleAmp[6]);
	DISPLAY_UPD(PchSataHsioTxGen2DownscaleAmp[7]);
	// PchSataHsioTxGen3DownscaleAmpEnable[8]
	DISPLAY_UPD(PchSataHsioTxGen3DownscaleAmpEnable[0]);
	DISPLAY_UPD(PchSataHsioTxGen3DownscaleAmpEnable[1]);
	DISPLAY_UPD(PchSataHsioTxGen3DownscaleAmpEnable[2]);
	DISPLAY_UPD(PchSataHsioTxGen3DownscaleAmpEnable[3]);
	DISPLAY_UPD(PchSataHsioTxGen3DownscaleAmpEnable[4]);
	DISPLAY_UPD(PchSataHsioTxGen3DownscaleAmpEnable[5]);
	DISPLAY_UPD(PchSataHsioTxGen3DownscaleAmpEnable[6]);
	DISPLAY_UPD(PchSataHsioTxGen3DownscaleAmpEnable[7]);
	// PchSataHsioTxGen3DownscaleAmp[8]
	DISPLAY_UPD(PchSataHsioTxGen3DownscaleAmp[0]);
	DISPLAY_UPD(PchSataHsioTxGen3DownscaleAmp[1]);
	DISPLAY_UPD(PchSataHsioTxGen3DownscaleAmp[2]);
	DISPLAY_UPD(PchSataHsioTxGen3DownscaleAmp[3]);
	DISPLAY_UPD(PchSataHsioTxGen3DownscaleAmp[4]);
	DISPLAY_UPD(PchSataHsioTxGen3DownscaleAmp[5]);
	DISPLAY_UPD(PchSataHsioTxGen3DownscaleAmp[6]);
	DISPLAY_UPD(PchSataHsioTxGen3DownscaleAmp[7]);
	// PchSataHsioTxGen1DeEmphEnable[8]
	DISPLAY_UPD(PchSataHsioTxGen1DeEmphEnable[0]);
	DISPLAY_UPD(PchSataHsioTxGen1DeEmphEnable[1]);
	DISPLAY_UPD(PchSataHsioTxGen1DeEmphEnable[2]);
	DISPLAY_UPD(PchSataHsioTxGen1DeEmphEnable[3]);
	DISPLAY_UPD(PchSataHsioTxGen1DeEmphEnable[4]);
	DISPLAY_UPD(PchSataHsioTxGen1DeEmphEnable[5]);
	DISPLAY_UPD(PchSataHsioTxGen1DeEmphEnable[6]);
	DISPLAY_UPD(PchSataHsioTxGen1DeEmphEnable[7]);
	// PchSataHsioTxGen1DeEmph[8]
	DISPLAY_UPD(PchSataHsioTxGen1DeEmph[0]);
	DISPLAY_UPD(PchSataHsioTxGen1DeEmph[1]);
	DISPLAY_UPD(PchSataHsioTxGen1DeEmph[2]);
	DISPLAY_UPD(PchSataHsioTxGen1DeEmph[3]);
	DISPLAY_UPD(PchSataHsioTxGen1DeEmph[4]);
	DISPLAY_UPD(PchSataHsioTxGen1DeEmph[5]);
	DISPLAY_UPD(PchSataHsioTxGen1DeEmph[6]);
	DISPLAY_UPD(PchSataHsioTxGen1DeEmph[7]);
	// PchSataHsioTxGen2DeEmphEnable[8]
	DISPLAY_UPD(PchSataHsioTxGen2DeEmphEnable[0]);
	DISPLAY_UPD(PchSataHsioTxGen2DeEmphEnable[1]);
	DISPLAY_UPD(PchSataHsioTxGen2DeEmphEnable[2]);
	DISPLAY_UPD(PchSataHsioTxGen2DeEmphEnable[3]);
	DISPLAY_UPD(PchSataHsioTxGen2DeEmphEnable[4]);
	DISPLAY_UPD(PchSataHsioTxGen2DeEmphEnable[5]);
	DISPLAY_UPD(PchSataHsioTxGen2DeEmphEnable[6]);
	DISPLAY_UPD(PchSataHsioTxGen2DeEmphEnable[7]);
	// PchSataHsioTxGen2DeEmph[8]
	DISPLAY_UPD(PchSataHsioTxGen2DeEmph[0]);
	DISPLAY_UPD(PchSataHsioTxGen2DeEmph[1]);
	DISPLAY_UPD(PchSataHsioTxGen2DeEmph[2]);
	DISPLAY_UPD(PchSataHsioTxGen2DeEmph[3]);
	DISPLAY_UPD(PchSataHsioTxGen2DeEmph[4]);
	DISPLAY_UPD(PchSataHsioTxGen2DeEmph[5]);
	DISPLAY_UPD(PchSataHsioTxGen2DeEmph[6]);
	DISPLAY_UPD(PchSataHsioTxGen2DeEmph[7]);
	// PchSataHsioTxGen3DeEmphEnable[8]
	DISPLAY_UPD(PchSataHsioTxGen3DeEmphEnable[0]);
	DISPLAY_UPD(PchSataHsioTxGen3DeEmphEnable[1]);
	DISPLAY_UPD(PchSataHsioTxGen3DeEmphEnable[2]);
	DISPLAY_UPD(PchSataHsioTxGen3DeEmphEnable[3]);
	DISPLAY_UPD(PchSataHsioTxGen3DeEmphEnable[4]);
	DISPLAY_UPD(PchSataHsioTxGen3DeEmphEnable[5]);
	DISPLAY_UPD(PchSataHsioTxGen3DeEmphEnable[6]);
	DISPLAY_UPD(PchSataHsioTxGen3DeEmphEnable[7]);
	// PchSataHsioTxGen3DeEmph[8]
	DISPLAY_UPD(PchSataHsioTxGen3DeEmph[0]);
	DISPLAY_UPD(PchSataHsioTxGen3DeEmph[1]);
	DISPLAY_UPD(PchSataHsioTxGen3DeEmph[2]);
	DISPLAY_UPD(PchSataHsioTxGen3DeEmph[3]);
	DISPLAY_UPD(PchSataHsioTxGen3DeEmph[4]);
	DISPLAY_UPD(PchSataHsioTxGen3DeEmph[5]);
	DISPLAY_UPD(PchSataHsioTxGen3DeEmph[6]);
	DISPLAY_UPD(PchSataHsioTxGen3DeEmph[7]);
	DISPLAY_UPD(PchLpcEnhancePort8xhDecoding);
	DISPLAY_UPD(PchPort80Route);
	DISPLAY_UPD(SmbusArpEnable);
	DISPLAY_UPD(PchNumRsvdSmbusAddresses);
	DISPLAY_UPD(PchSmbusIoBase);
	DISPLAY_UPD(PcieImrSize);
	DISPLAY_UPD(RsvdSmbusAddressTablePtr);
	DISPLAY_UPD(PcieRpEnableMask);
	DISPLAY_UPD(PcieImrEnabled);
	DISPLAY_UPD(ImrRpSelection);
	DISPLAY_UPD(PchSmbAlertEnable);
	DISPLAY_UPD(PcdDebugInterfaceFlags);
	DISPLAY_UPD(SerialIoUartDebugControllerNumber);
	DISPLAY_UPD(SerialIoUartDebugAutoFlow);
	DISPLAY_UPD(SerialIoUartDebugBaudRate);
	DISPLAY_UPD(SerialIoUartDebugParity);
	DISPLAY_UPD(SerialIoUartDebugStopBits);
	DISPLAY_UPD(SerialIoUartDebugDataBits);
	DISPLAY_UPD(PchHdaDspEnable);
	DISPLAY_UPD(PchHdaVcType);
	DISPLAY_UPD(PchHdaDspUaaCompliance);
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
	DISPLAY_UPD(PcdIsaSerialUartBase);
	DISPLAY_UPD(GtPllVoltageOffset);
	DISPLAY_UPD(RingPllVoltageOffset);
	DISPLAY_UPD(SaPllVoltageOffset);
	DISPLAY_UPD(McPllVoltageOffset);
	DISPLAY_UPD(MrcSafeConfig);
	DISPLAY_UPD(PcdSerialDebugBaudRate);
	DISPLAY_UPD(HobBufferSize);
	DISPLAY_UPD(ECT);
	DISPLAY_UPD(SOT);
	DISPLAY_UPD(ERDMPRTC2D);
	DISPLAY_UPD(RDMPRT);
	DISPLAY_UPD(RCVET);
	DISPLAY_UPD(JWRL);
	DISPLAY_UPD(EWRTC2D);
	DISPLAY_UPD(ERDTC2D);
	DISPLAY_UPD(WRTC1D);
	DISPLAY_UPD(WRVC1D);
	DISPLAY_UPD(RDTC1D);
	DISPLAY_UPD(DIMMODTT);
	DISPLAY_UPD(DIMMRONT);
	DISPLAY_UPD(WRDSEQT);
	DISPLAY_UPD(WRSRT);
	DISPLAY_UPD(RDODTT);
	DISPLAY_UPD(RDEQT);
	DISPLAY_UPD(RDAPT);
	DISPLAY_UPD(WRTC2D);
	DISPLAY_UPD(RDTC2D);
	DISPLAY_UPD(WRVC2D);
	DISPLAY_UPD(RDVC2D);
	DISPLAY_UPD(CMDVC);
	DISPLAY_UPD(LCT);
	DISPLAY_UPD(RTL);
	DISPLAY_UPD(TAT);
	DISPLAY_UPD(MEMTST);
	DISPLAY_UPD(ALIASCHK);
	DISPLAY_UPD(RCVENC1D);
	DISPLAY_UPD(RMC);
	DISPLAY_UPD(WRDSUDT);
	DISPLAY_UPD(EccSupport);
	DISPLAY_UPD(RemapEnable);
	DISPLAY_UPD(RankInterleave);
	DISPLAY_UPD(EnhancedInterleave);
	DISPLAY_UPD(MemoryTrace);
	DISPLAY_UPD(ChHashEnable);
	DISPLAY_UPD(EnableExtts);
	DISPLAY_UPD(EnableCltm);
	DISPLAY_UPD(EnableOltm);
	DISPLAY_UPD(EnablePwrDn);
	DISPLAY_UPD(EnablePwrDnLpddr);
	DISPLAY_UPD(UserPowerWeightsEn);
	DISPLAY_UPD(RaplLim2Lock);
	DISPLAY_UPD(RaplLim2Ena);
	DISPLAY_UPD(RaplLim1Ena);
	DISPLAY_UPD(SrefCfgEna);
	DISPLAY_UPD(ThrtCkeMinDefeatLpddr);
	DISPLAY_UPD(ThrtCkeMinDefeat);
	DISPLAY_UPD(RhPrevention);
	DISPLAY_UPD(ExitOnFailure);
	DISPLAY_UPD(DdrThermalSensor);
	DISPLAY_UPD(Ddr4DdpSharedClock);
	DISPLAY_UPD(Ddr4DdpSharedZq);
	DISPLAY_UPD(ChHashMask);
	DISPLAY_UPD(BClkFrequency);
	DISPLAY_UPD(ChHashInterleaveBit);
	DISPLAY_UPD(EnergyScaleFact);
	DISPLAY_UPD(Idd3n);
	DISPLAY_UPD(Idd3p);
	DISPLAY_UPD(CMDSR);
	DISPLAY_UPD(CMDDSEQ);
	DISPLAY_UPD(CMDNORM);
	DISPLAY_UPD(EWRDSEQ);
	DISPLAY_UPD(RhActProbability);
	DISPLAY_UPD(RaplLim2WindX);
	DISPLAY_UPD(RaplLim2WindY);
	DISPLAY_UPD(RaplLim1WindX);
	DISPLAY_UPD(RaplLim1WindY);
	DISPLAY_UPD(RaplLim2Pwr);
	DISPLAY_UPD(RaplLim1Pwr);
	DISPLAY_UPD(WarmThresholdCh0Dimm0);
	DISPLAY_UPD(WarmThresholdCh0Dimm1);
	DISPLAY_UPD(WarmThresholdCh1Dimm0);
	DISPLAY_UPD(WarmThresholdCh1Dimm1);
	DISPLAY_UPD(HotThresholdCh0Dimm0);
	DISPLAY_UPD(HotThresholdCh0Dimm1);
	DISPLAY_UPD(HotThresholdCh1Dimm0);
	DISPLAY_UPD(HotThresholdCh1Dimm1);
	DISPLAY_UPD(WarmBudgetCh0Dimm0);
	DISPLAY_UPD(WarmBudgetCh0Dimm1);
	DISPLAY_UPD(WarmBudgetCh1Dimm0);
	DISPLAY_UPD(WarmBudgetCh1Dimm1);
	DISPLAY_UPD(HotBudgetCh0Dimm0);
	DISPLAY_UPD(HotBudgetCh0Dimm1);
	DISPLAY_UPD(HotBudgetCh1Dimm0);
	DISPLAY_UPD(HotBudgetCh1Dimm1);
	DISPLAY_UPD(IdleEnergyCh0Dimm0);
	DISPLAY_UPD(IdleEnergyCh0Dimm1);
	DISPLAY_UPD(IdleEnergyCh1Dimm0);
	DISPLAY_UPD(IdleEnergyCh1Dimm1);
	DISPLAY_UPD(PdEnergyCh0Dimm0);
	DISPLAY_UPD(PdEnergyCh0Dimm1);
	DISPLAY_UPD(PdEnergyCh1Dimm0);
	DISPLAY_UPD(PdEnergyCh1Dimm1);
	DISPLAY_UPD(ActEnergyCh0Dimm0);
	DISPLAY_UPD(ActEnergyCh0Dimm1);
	DISPLAY_UPD(ActEnergyCh1Dimm0);
	DISPLAY_UPD(ActEnergyCh1Dimm1);
	DISPLAY_UPD(RdEnergyCh0Dimm0);
	DISPLAY_UPD(RdEnergyCh0Dimm1);
	DISPLAY_UPD(RdEnergyCh1Dimm0);
	DISPLAY_UPD(RdEnergyCh1Dimm1);
	DISPLAY_UPD(WrEnergyCh0Dimm0);
	DISPLAY_UPD(WrEnergyCh0Dimm1);
	DISPLAY_UPD(WrEnergyCh1Dimm0);
	DISPLAY_UPD(WrEnergyCh1Dimm1);
	DISPLAY_UPD(ThrtCkeMinTmr);
	DISPLAY_UPD(CkeRankMapping);
	DISPLAY_UPD(RaplPwrFlCh0);
	DISPLAY_UPD(RaplPwrFlCh1);
	DISPLAY_UPD(EnCmdRate);
	DISPLAY_UPD(Refresh2X);
	DISPLAY_UPD(EpgEnable);
	DISPLAY_UPD(RhSolution);
	DISPLAY_UPD(UserThresholdEnable);
	DISPLAY_UPD(UserBudgetEnable);
	DISPLAY_UPD(TsodTcritMax);
	DISPLAY_UPD(TsodEventMode);
	DISPLAY_UPD(TsodEventPolarity);
	DISPLAY_UPD(TsodCriticalEventOnly);
	DISPLAY_UPD(TsodEventOutputControl);
	DISPLAY_UPD(TsodAlarmwindowLockBit);
	DISPLAY_UPD(TsodCriticaltripLockBit);
	DISPLAY_UPD(TsodShutdownMode);
	DISPLAY_UPD(TsodThigMax);
	DISPLAY_UPD(TsodManualEnable);
	DISPLAY_UPD(ForceOltmOrRefresh2x);
	DISPLAY_UPD(PwdwnIdleCounter);
	DISPLAY_UPD(CmdRanksTerminated);
	DISPLAY_UPD(GdxcEnable);
	DISPLAY_UPD(PcdSerialDebugLevel);
	DISPLAY_UPD(FivrFaults);
	DISPLAY_UPD(FivrEfficiency);
	DISPLAY_UPD(SafeMode);
	DISPLAY_UPD(CleanMemory);
	DISPLAY_UPD(LpDdrDqDqsReTraining);
	DISPLAY_UPD(PostCodeOutputPort);
	DISPLAY_UPD(RMTLoopCount);
	DISPLAY_UPD(EnBER);
	DISPLAY_UPD(DualDimmPerChannelBoardType);
	DISPLAY_UPD(Ddr4Mixed2DpcLimit);
	DISPLAY_UPD(FastBootRmt);
	DISPLAY_UPD(MemTestOnWarmBoot);
	DISPLAY_UPD(ThrtCkeMinTmrLpddr);
	DISPLAY_UPD(X2ApicOptOut);
	DISPLAY_UPD(MrcTrainOnWarm);
	DISPLAY_UPD(LpddrDramOdt);
	DISPLAY_UPD(Ddr4SkipRefreshEn);
	DISPLAY_UPD(SerialDebugMrcLevel);
	// PchHdaSndwLinkIoControlEnabled[4]
	DISPLAY_UPD(PchHdaSndwLinkIoControlEnabled[0]);
	DISPLAY_UPD(PchHdaSndwLinkIoControlEnabled[1]);
	DISPLAY_UPD(PchHdaSndwLinkIoControlEnabled[2]);
	DISPLAY_UPD(PchHdaSndwLinkIoControlEnabled[3]);
	DISPLAY_UPD(CoreVfPointOffsetMode);
	// CoreVfPointOffset[15]
	DISPLAY_UPD(CoreVfPointOffset[0]);
	DISPLAY_UPD(CoreVfPointOffset[1]);
	DISPLAY_UPD(CoreVfPointOffset[2]);
	DISPLAY_UPD(CoreVfPointOffset[3]);
	DISPLAY_UPD(CoreVfPointOffset[4]);
	DISPLAY_UPD(CoreVfPointOffset[5]);
	DISPLAY_UPD(CoreVfPointOffset[6]);
	DISPLAY_UPD(CoreVfPointOffset[7]);
	DISPLAY_UPD(CoreVfPointOffset[8]);
	DISPLAY_UPD(CoreVfPointOffset[9]);
	DISPLAY_UPD(CoreVfPointOffset[10]);
	DISPLAY_UPD(CoreVfPointOffset[11]);
	DISPLAY_UPD(CoreVfPointOffset[12]);
	DISPLAY_UPD(CoreVfPointOffset[13]);
	DISPLAY_UPD(CoreVfPointOffset[14]);
	// CoreVfPointOffsetPrefix[15]
	DISPLAY_UPD(CoreVfPointOffsetPrefix[0]);
	DISPLAY_UPD(CoreVfPointOffsetPrefix[1]);
	DISPLAY_UPD(CoreVfPointOffsetPrefix[2]);
	DISPLAY_UPD(CoreVfPointOffsetPrefix[3]);
	DISPLAY_UPD(CoreVfPointOffsetPrefix[4]);
	DISPLAY_UPD(CoreVfPointOffsetPrefix[5]);
	DISPLAY_UPD(CoreVfPointOffsetPrefix[6]);
	DISPLAY_UPD(CoreVfPointOffsetPrefix[7]);
	DISPLAY_UPD(CoreVfPointOffsetPrefix[8]);
	DISPLAY_UPD(CoreVfPointOffsetPrefix[9]);
	DISPLAY_UPD(CoreVfPointOffsetPrefix[10]);
	DISPLAY_UPD(CoreVfPointOffsetPrefix[11]);
	DISPLAY_UPD(CoreVfPointOffsetPrefix[12]);
	DISPLAY_UPD(CoreVfPointOffsetPrefix[13]);
	DISPLAY_UPD(CoreVfPointOffsetPrefix[14]);
	// CoreVfPointRatio[15]
	DISPLAY_UPD(CoreVfPointRatio[0]);
	DISPLAY_UPD(CoreVfPointRatio[1]);
	DISPLAY_UPD(CoreVfPointRatio[2]);
	DISPLAY_UPD(CoreVfPointRatio[3]);
	DISPLAY_UPD(CoreVfPointRatio[4]);
	DISPLAY_UPD(CoreVfPointRatio[5]);
	DISPLAY_UPD(CoreVfPointRatio[6]);
	DISPLAY_UPD(CoreVfPointRatio[7]);
	DISPLAY_UPD(CoreVfPointRatio[8]);
	DISPLAY_UPD(CoreVfPointRatio[9]);
	DISPLAY_UPD(CoreVfPointRatio[10]);
	DISPLAY_UPD(CoreVfPointRatio[11]);
	DISPLAY_UPD(CoreVfPointRatio[12]);
	DISPLAY_UPD(CoreVfPointRatio[13]);
	DISPLAY_UPD(CoreVfPointRatio[14]);
	DISPLAY_UPD(CoreVfPointCount);

	#undef DISPLAY_UPD
}
#endif // CONFIG(SOC_INTEL_COMETLAKE)
