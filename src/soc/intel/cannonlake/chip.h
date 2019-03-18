/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2017-2019 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_CHIP_H_
#define _SOC_CHIP_H_

#include <intelblocks/chip.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <intelblocks/gspi.h>
#include <stdint.h>
#include <soc/gpio.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/pmc.h>
#include <soc/serialio.h>
#include <soc/usb.h>
#include <soc/vr_config.h>
#if CONFIG(SOC_INTEL_CANNONLAKE_PCH_H)
#include <soc/gpio_defs_cnp_h.h>
#else
#include <soc/gpio_defs.h>
#endif

#define SOC_INTEL_CML_UART_DEV_MAX 3

struct soc_intel_cannonlake_config {

	/* Common struct containing soc config data required by common code */
	struct soc_intel_common_config common_soc_config;

	/* GPE configuration */
	uint32_t gpe0_en_1; /* GPE0_EN_31_0 */
	uint32_t gpe0_en_2; /* GPE0_EN_63_32 */
	uint32_t gpe0_en_3; /* GPE0_EN_95_64 */
	uint32_t gpe0_en_4; /* GPE0_EN_127_96 / GPE_STD */

	/* Gpio group routed to each dword of the GPE0 block. Values are
	 * of the form GPP_[A:G] or GPD. */
	uint8_t gpe0_dw0; /* GPE0_31_0 STS/EN */
	uint8_t gpe0_dw1; /* GPE0_63_32 STS/EN */
	uint8_t gpe0_dw2; /* GPE0_95_64 STS/EN */

	/* Generic IO decode ranges */
	uint32_t gen1_dec;
	uint32_t gen2_dec;
	uint32_t gen3_dec;
	uint32_t gen4_dec;

	/* Enable S0iX support */
	int s0ix_enable;
	/* Enable DPTF support */
	int dptf_enable;

	/* Deep SX enable for both AC and DC */
	int deep_s3_enable_ac;
	int deep_s3_enable_dc;
	int deep_s5_enable_ac;
	int deep_s5_enable_dc;

	/* Deep Sx Configuration
	 *  DSX_EN_WAKE_PIN       - Enable WAKE# pin
	 *  DSX_EN_LAN_WAKE_PIN   - Enable LAN_WAKE# pin
	 *  DSX_DIS_AC_PRESENT_PD - Disable pull-down on AC_PRESENT pin */
	uint32_t deep_sx_config;

	/* TCC activation offset */
	uint32_t tcc_offset;

	uint64_t PlatformMemorySize;
	uint8_t SmramMask;
	uint8_t MrcFastBoot;
	uint32_t TsegSize;
	uint16_t MmioSize;

	/* DDR Frequency Limit. Maximum Memory Frequency Selections in Mhz.
	 * Options : 1067, 1333, 1600, 1867, 2133, 2400, 2667, 2933, 0(Auto) */
	uint16_t DdrFreqLimit;

	/* SAGV Low Frequency Selections in Mhz.
	 * Options : 1067, 1333, 1600, 1867, 2133, 2400, 2667, 2933, 0(Auto) */
	uint16_t FreqSaGvLow;

	/* SAGV Mid Frequency Selections in Mhz.
	 * Options : 1067, 1333, 1600, 1867, 2133, 2400, 2667, 2933, 0(Auto) */
	uint16_t FreqSaGvMid;

	/* System Agent dynamic frequency support. Only effects ULX/ULT CPUs.
	 * For CNL, options are as following
	 * When enabled, memory will be training at three different frequencies.
	 * 0:Disabled, 1:FixedLow, 2:FixedMid, 3:FixedHigh, 4:Enabled
	 * For WHL/CFL/CML options are as following
	 * When enabled, memory will be training at two different frequencies.
	 * 0:Disabled, 1:FixedLow, 2:FixedHigh, 3:Enabled*/
	enum {
		SaGv_Disabled,
		SaGv_FixedLow,
#if !CONFIG(SOC_INTEL_COMMON_CANNONLAKE_BASE)
		SaGv_FixedMid,
#endif
		SaGv_FixedHigh,
		SaGv_Enabled,
	} SaGv;


	/* Rank Margin Tool. 1:Enable, 0:Disable */
	uint8_t RMT;

	/* USB related */
	struct usb2_port_config usb2_ports[16];
	struct usb3_port_config usb3_ports[10];
	uint8_t SsicPortEnable;
	/* Wake Enable Bitmap for USB2 ports */
	uint16_t usb2_wake_enable_bitmap;
	/* Wake Enable Bitmap for USB3 ports */
	uint16_t usb3_wake_enable_bitmap;

	/* SATA related */
	enum {
		Sata_AHCI,
		Sata_RAID,
	} SataMode;
	uint8_t SataSalpSupport;
	uint8_t SataPortsEnable[8];
	uint8_t SataPortsDevSlp[8];

	/* Audio related */
	uint8_t PchHdaDspEnable;

	/* Enable/Disable HD Audio Link. Muxed with SSP0/SSP1/SNDW1 */
	uint8_t PchHdaAudioLinkHda;
	uint8_t PchHdaAudioLinkDmic0;
	uint8_t PchHdaAudioLinkDmic1;
	uint8_t PchHdaAudioLinkSsp0;
	uint8_t PchHdaAudioLinkSsp1;
	uint8_t PchHdaAudioLinkSsp2;
	uint8_t PchHdaAudioLinkSndw1;
	uint8_t PchHdaAudioLinkSndw2;
	uint8_t PchHdaAudioLinkSndw3;
	uint8_t PchHdaAudioLinkSndw4;

	/* PCIe Root Ports */
	uint8_t PcieRpEnable[CONFIG_MAX_ROOT_PORTS];
	/* PCIe output clocks type to Pcie devices.
	 * 0-23: PCH rootport, 0x70: LAN, 0x80: unspecified but in use,
	 * 0xFF: not used */
	uint8_t PcieClkSrcUsage[CONFIG_MAX_ROOT_PORTS];
	/* PCIe ClkReq-to-ClkSrc mapping, number of clkreq signal assigned to
	 * clksrc. */
	uint8_t PcieClkSrcClkReq[CONFIG_MAX_ROOT_PORTS];
	/* PCIe LTR(Latency Tolerance Reporting) mechanism */
	uint8_t PcieRpLtrEnable[CONFIG_MAX_ROOT_PORTS];
	/* Enable/Disable HotPlug support for Root Port */
	uint8_t PcieRpHotPlug[CONFIG_MAX_ROOT_PORTS];

	/* eMMC and SD */
	uint8_t ScsEmmcHs400Enabled;
	/* Need to update DLL setting to get Emmc running at HS400 speed */
	uint8_t EmmcHs400DllNeed;
	/* 0-39: number of active delay for RX strobe, unit is 125 psec */
	uint8_t EmmcHs400RxStrobeDll1;
	/* 0-78: number of active delay for TX data, unit is 125 psec */
	uint8_t EmmcHs400TxDataDll;

	/* Integrated Sensor */
	uint8_t PchIshEnable;

	/* Heci related */
	uint8_t Heci3Enabled;

	/* Gfx related */
	uint8_t IgdDvmt50PreAlloc;
	uint8_t InternalGfx;
	uint8_t SkipExtGfxScan;

	uint32_t GraphicsConfigPtr;
	uint8_t Device4Enable;

	/* GPIO IRQ Select. The valid value is 14 or 15 */
	uint8_t GpioIrqRoute;
	/* SCI IRQ Select. The valid value is 9, 10, 11, 20, 21, 22, 23 */
	uint8_t SciIrqSelect;
	/* TCO IRQ Select. The valid value is 9, 10, 11, 20, 21, 22, 23 */
	uint8_t TcoIrqSelect;
	uint8_t TcoIrqEnable;

	/* VrConfig Settings for 5 domains
	 * 0 = System Agent, 1 = IA Core, 2 = Ring,
	 * 3 = GT unsliced,  4 = GT sliced */
	struct vr_config domain_vr_config[NUM_VR_DOMAINS];
	/* HeciEnabled decides the state of Heci1 at end of boot
	 * Setting to 0 (default) disables Heci1 and hides the device from OS */
	uint8_t HeciEnabled;

	/* PL1 Override value in Watts */
	uint32_t tdp_pl1_override;
	/* PL2 Override value in Watts */
	uint32_t tdp_pl2_override;
	/* SysPL2 Value in Watts */
	uint32_t tdp_psyspl2;
	/* SysPL3 Value in Watts */
	uint32_t tdp_psyspl3;
	/* SysPL3 window size */
	uint32_t tdp_psyspl3_time;
	/* SysPL3 duty cycle */
	uint32_t tdp_psyspl3_dutycycle;
	/* PL4 Value in Watts */
	uint32_t tdp_pl4;
	/* Estimated maximum platform power in Watts */
	uint16_t psys_pmax;

	/* Intel Speed Shift Technology */
	uint8_t speed_shift_enable;
	/* Enable VR specific mailbox command
	 * 00b - no VR specific cmd sent
	 * 01b - VR mailbox cmd specifically for the MPS IMPV8 VR will be sent
	 * 10b - VR specific cmd sent for PS4 exit issue
	 * 11b - Reserved */
	uint8_t SendVrMbxCmd;

	/* Enable/Disable EIST. 1b:Enabled, 0b:Disabled */
	uint8_t eist_enable;

	/* Statically clock gate 8254 PIT. */
	uint8_t clock_gate_8254;
	/* Enable C6 DRAM */
	uint8_t enable_c6dram;
	/*
	 * PRMRR size setting with below options
	 * 0x00100000 - 1MiB
	 * 0x02000000 - 32MiB and beyond
	 */
	uint32_t PrmrrSize;
	uint8_t PmTimerDisabled;

	/*
	 * SLP_S3 Minimum Assertion Width Policy
	 *  1 = 60us
	 *  2 = 1ms (default)
	 *  3 = 50ms
	 *  4 = 2s
	 */
	uint8_t PchPmSlpS3MinAssert;

	/*
	 * SLP_S4 Minimum Assertion Width Policy
	 *  1 = 1s
	 *  2 = 2s
	 *  3 = 3s
	 *  4 = 4s (default)
	 */
	uint8_t PchPmSlpS4MinAssert;

	/*
	 * SLP_SUS Minimum Assertion Width Policy
	 *  1 = 0ms
	 *  2 = 500ms
	 *  3 = 1s (default)
	 *  4 = 4s
	 */
	uint8_t PchPmSlpSusMinAssert;

	/*
	 * SLP_A Minimum Assertion Width Policy
	 *  1 = 0ms
	 *  2 = 4s
	 *  3 = 98ms (default)
	 *  4 = 2s
	 */
	uint8_t PchPmSlpAMinAssert;

	/* Desired platform debug type. */
	enum {
		DebugConsent_Disabled,
		DebugConsent_DCI_DBC,
		DebugConsent_DCI,
		DebugConsent_USB3_DBC,
		DebugConsent_XDP, /* XDP/Mipi60 */
		DebugConsent_USB2_DBC,
	} DebugConsent;
	/*
	 * SerialIO device mode selection:
	 *
	 * Device index:
	 * PchSerialIoIndexI2C0
	 * PchSerialIoIndexI2C1
	 * PchSerialIoIndexI2C2
	 * PchSerialIoIndexI2C3
	 * PchSerialIoIndexI2C4
	 * PchSerialIoIndexI2C5
	 * PchSerialIoIndexSPI0
	 * PchSerialIoIndexSPI1
	 * PchSerialIoIndexSPI2
	 * PchSerialIoIndexUART0
	 * PchSerialIoIndexUART1
	 * PchSerialIoIndexUART2
	 *
	 * Mode select:
	 * For Cannonlake PCH following values are supported:
	 * PchSerialIoNotInitialized
	 * PchSerialIoDisabled
	 * PchSerialIoPci
	 * PchSerialIoAcpi
	 * PchSerialIoHidden
	 * PchSerialIoMax
	 *
	 * For Cometlake following values are supported:
	 * PchSerialIoNotInitialized
	 * PchSerialIoDisabled,
	 * PchSerialIoPci,
	 * PchSerialIoHidden,
	 * PchSerialIoLegacyUart,
	 * PchSerialIoSkipInit,
	 * PchSerialIoMax
	 *
	 * NOTE:
	 * PchSerialIoNotInitialized is not an option provided by FSP, this
	 * option is default selected in case devicetree doesn't fill this param
	 * In case PchSerialIoNotInitialized is selected or an invalid value is
	 * provided from devicetree, coreboot will configure device into PCI
	 * mode by default.
	 *
	 */
	uint8_t SerialIoDevMode[PchSerialIoIndexMAX];

	enum {
		SERIAL_IRQ_QUIET_MODE      = 0,
		SERIAL_IRQ_CONTINUOUS_MODE = 1,
	} SerialIrqConfigSirqMode;

	/* GPIO SD card detect pin */
	unsigned int sdcard_cd_gpio;

	/* Enable Pch iSCLK */
	uint8_t pch_isclk;

	/* Intel VT configuration */
	uint8_t VtdDisable;
	uint8_t VmxEnable;

	/*
	 * Acoustic Noise Mitigation
	 * 0b - Disable
	 * 1b - Enable noise mitigation
	 */
	uint8_t AcousticNoiseMitigation;

	/*
	 * Disable Fast Package C-state ramping
	 * Need to set AcousticNoiseMitigation = '1' first
	 * 0b - Enabled
	 * 1b - Disabled
	 */
	uint8_t FastPkgCRampDisableIa;
	uint8_t FastPkgCRampDisableGt;
	uint8_t FastPkgCRampDisableSa;
	uint8_t FastPkgCRampDisableFivr;

	/*
	 * Adjust the VR slew rates
	 * Need to set AcousticNoiseMitigation = '1' first
	 * 000b - Fast/2
	 * 001b - Fast/4
	 * 010b - Fast/8
	 * 011b - Fast/16
	 */
	uint8_t SlowSlewRateForIa;
	uint8_t SlowSlewRateForGt;
	uint8_t SlowSlewRateForSa;
	uint8_t SlowSlewRateForFivr;

	/* DMI Power Optimizer */
	uint8_t dmipwroptimize;

	/* SATA Power Optimizer */
	uint8_t satapwroptimize;
};

typedef struct soc_intel_cannonlake_config config_t;

#endif
