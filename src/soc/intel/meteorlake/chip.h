/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_CHIP_H_
#define _SOC_CHIP_H_

#include <drivers/i2c/designware/dw_i2c.h>
#include <drivers/intel/gma/gma.h>
#include <device/pci_ids.h>
#include <gpio.h>
#include <intelblocks/cfg.h>
#include <intelblocks/gspi.h>
#include <intelblocks/power_limit.h>
#include <intelblocks/pcie_rp.h>
#include <intelblocks/tcss.h>
#include <soc/gpe.h>
#include <soc/pci_devs.h>
#include <soc/pmc.h>
#include <soc/serialio.h>
#include <soc/usb.h>
#include <stdbool.h>
#include <stdint.h>

/* Define config parameters for In-Band ECC (IBECC). */
#define MAX_IBECC_REGIONS 8

#define MAX_SAGV_POINTS 4
#define MAX_HD_AUDIO_SDI_LINKS 2

/* In-Band ECC Operation Mode */
enum ibecc_mode {
	IBECC_MODE_PER_REGION,
	IBECC_MODE_NONE,
	IBECC_MODE_ALL
};

struct ibecc_config {
	bool enable;
	bool parity_en;
	enum ibecc_mode mode;
	bool region_enable[MAX_IBECC_REGIONS];
	uint16_t region_base[MAX_IBECC_REGIONS];
	uint16_t region_mask[MAX_IBECC_REGIONS];
};

/* Types of different SKUs */
enum soc_intel_meteorlake_power_limits {
	MTL_P_282_242_CORE,
	MTL_P_682_482_CORE,
	MTL_POWER_LIMITS_COUNT
};

/* TDP values for different SKUs */
enum soc_intel_meteorlake_cpu_tdps {
	TDP_15W = 15,
	TDP_28W = 28
};

/* Mapping of different SKUs based on CPU ID and TDP values */
static const struct {
	unsigned int cpu_id;
	enum soc_intel_meteorlake_power_limits limits;
	enum soc_intel_meteorlake_cpu_tdps cpu_tdp;
} cpuid_to_mtl[] = {
	{ PCI_DID_INTEL_MTL_P_ID_5, MTL_P_282_242_CORE, TDP_15W },
	{ PCI_DID_INTEL_MTL_P_ID_2, MTL_P_282_242_CORE, TDP_15W },
	{ PCI_DID_INTEL_MTL_P_ID_3, MTL_P_682_482_CORE, TDP_28W },
	{ PCI_DID_INTEL_MTL_P_ID_1, MTL_P_682_482_CORE, TDP_28W },
};

/* Types of display ports */
enum ddi_ports {
	DDI_PORT_A,
	DDI_PORT_B,
	DDI_PORT_C,
	DDI_PORT_1,
	DDI_PORT_2,
	DDI_PORT_3,
	DDI_PORT_4,
	DDI_PORT_COUNT,
};

enum ddi_port_flags {
	DDI_ENABLE_DDC = 1 << 0, // Display Data Channel
	DDI_ENABLE_HPD = 1 << 1, // Hot Plug Detect
};

/*
 * The Max Pkg Cstate
 * Values 0 - C0/C1, 1 - C2, 2 - C3, 3 - C6, 4 - C7, 5 - C7S, 6 - C8, 7 - C9, 8 - C10,
 * 254 - CPU Default , 255 - Auto.
 */
enum pkgcstate_limit {
	LIMIT_C0_C1		= 0,
	LIMIT_C2		= 1,
	LIMIT_C3		= 2,
	LIMIT_C6		= 3,
	LIMIT_C7		= 4,
	LIMIT_C7S		= 5,
	LIMIT_C8		= 6,
	LIMIT_C9		= 7,
	LIMIT_C10		= 8,
	LIMIT_CPUDEFAULT	= 254,
	LIMIT_AUTO		= 255,
};

/* Bit values for use in LpmStateEnableMask. */
enum lpm_state_mask {
	LPM_S0i2_0 = BIT(0),
	LPM_S0i2_1 = BIT(1),
	LPM_S0i2_2 = BIT(2),
	LPM_S0i3_0 = BIT(3),
	LPM_S0i3_1 = BIT(4),
	LPM_S0i3_2 = BIT(5),
	LPM_S0i3_3 = BIT(6),
	LPM_S0i3_4 = BIT(7),
	LPM_S0iX_ALL = LPM_S0i2_0 | LPM_S0i2_1 | LPM_S0i2_2
		     | LPM_S0i3_0 | LPM_S0i3_1 | LPM_S0i3_2 | LPM_S0i3_3 | LPM_S0i3_4,
};

/*
 * As per definition from FSP header:
 * - [0] for IA
 * - [1] for GT
 * - [2] for SA
 * - [3] through [5] are reserved
 */
enum vr_domain {
	VR_DOMAIN_IA,
	VR_DOMAIN_GT,
	VR_DOMAIN_SA,
	NUM_VR_DOMAINS
};

/*
 * Slew Rate configuration for Deep Package C States for VR domain.
 * They are fast time divided by 2.
 * 0 - Fast/2
 * 1 - Fast/4
 * 2 - Fast/8
 * 3 - Fast/16
 */
enum slew_rate {
	SLEW_FAST_2,
	SLEW_FAST_4,
	SLEW_FAST_8,
	SLEW_FAST_16,
	SLEW_IGNORE = 0xff,
};

struct soc_intel_meteorlake_config {
	/* Common struct containing soc config data required by common code */
	struct soc_intel_common_config common_soc_config;

	/* Common struct containing power limits configuration information */
	struct soc_power_limits_config power_limits_config[MTL_POWER_LIMITS_COUNT];

	/* Gpio group routed to each dword of the GPE0 block. Values are
	 * of the form PMC_GPP_[A:U] or GPD. */
	uint8_t pmc_gpe0_dw0; /* GPE0_31_0 STS/EN */
	uint8_t pmc_gpe0_dw1; /* GPE0_63_32 STS/EN */
	uint8_t pmc_gpe0_dw2; /* GPE0_95_64 STS/EN */

	/* Generic IO decode ranges */
	uint32_t gen1_dec;
	uint32_t gen2_dec;
	uint32_t gen3_dec;
	uint32_t gen4_dec;

	/* Enable S0iX support */
	bool s0ix_enable;
	/* Support for TCSS xhci, xdci, TBT PCIe root ports and DMA controllers */
	bool tcss_d3_hot_disable;
	/* Enable DPTF support */
	bool dptf_enable;

	/* Deep SX enable for both AC and DC */
	bool deep_s3_enable_ac;
	bool deep_s3_enable_dc;
	bool deep_s5_enable_ac;
	bool deep_s5_enable_dc;

	/* Deep Sx Configuration
	 *  DSX_EN_WAKE_PIN       - Enable WAKE# pin
	 *  DSX_EN_LAN_WAKE_PIN   - Enable LAN_WAKE# pin
	 *  DSX_DIS_AC_PRESENT_PD - Disable pull-down on AC_PRESENT pin */
	uint32_t deep_sx_config;

	/* TCC activation offset */
	uint32_t tcc_offset;

	/* In-Band ECC (IBECC) configuration */
	struct ibecc_config ibecc;

	/* System Agent dynamic frequency support. Only effects ULX/ULT CPUs.
	 * When enabled memory will be training at two different frequencies.
	 * 0:Disabled, 1:Enabled
	 */
	enum {
		SAGV_DISABLED,
		SAGV_ENABLED,
	} sagv;

	/* System Agent dynamic frequency work points that memory will be training
	 * at the enabled frequencies. Possible work points are:
	 * 0x3:Points0_1, 0x7:Points0_1_2, 0xF:AllPoints0_1_2_3
	 */
	enum {
		SAGV_POINTS_0_1 = 0x03,
		SAGV_POINTS_0_1_2 = 0x07,
		SAGV_POINTS_0_1_2_3 = 0x0f,
	} sagv_wp_bitmap;

	/* Rank Margin Tool. */
	bool rmt;

	/* USB related */
	struct usb2_port_config usb2_ports[CONFIG_SOC_INTEL_USB2_DEV_MAX];
	struct usb3_port_config usb3_ports[CONFIG_SOC_INTEL_USB3_DEV_MAX];
	/* Wake Enable Bitmap for USB2 ports */
	uint16_t usb2_wake_enable_bitmap;
	/* Wake Enable Bitmap for USB3 ports */
	uint16_t usb3_wake_enable_bitmap;
	/* Program OC pins for TCSS */
	struct tcss_port_config tcss_ports[MAX_TYPE_C_PORTS];
	/* Validate TBT firmware authenticated and loaded into IMR */
	bool tbt_authentication;

	/* SATA related */
	uint8_t sata_mode;
	bool sata_salp_support;
	bool sata_ports_enable[8];
	bool sata_ports_dev_slp[8];

	/*
	 * Enable(false)/Disable(true) SATA Power Optimizer on PCH side.
	 * Default false. Setting this to true disables the SATA Power Optimizer.
	 */
	bool sata_pwr_optimize_disable;

	/*
	 * SATA Port Enable Dito Config.
	 * Enable DEVSLP Idle Timeout settings (DmVal, DitoVal).
	 */
	bool sata_ports_enable_dito_config[8];

	/* SataPortsDmVal is the DITO multiplier. Default is 15. */
	uint8_t sata_ports_dm_val[8];
	/* SataPortsDitoVal is the DEVSLP Idle Timeout, default is 625ms */
	uint16_t sata_ports_dito_val[8];

	/* Audio related */
	bool pch_hda_audio_link_hda_enable;
	bool pch_hda_dsp_enable;

	bool pch_hda_sdi_enable[MAX_HD_AUDIO_SDI_LINKS];

	/* iDisp-Link T-Mode 0: 2T, 2: 4T, 3: 8T, 4: 16T */
	enum {
		HDA_TMODE_2T = 0,
		HDA_TMODE_4T = 2,
		HDA_TMODE_8T = 3,
		HDA_TMODE_16T = 4,
	} pch_hda_idisp_link_tmode;

	/* iDisp-Link Freq 4: 96MHz, 3: 48MHz. */
	enum {
		HDA_LINKFREQ_48MHZ = 3,
		HDA_LINKFREQ_96MHZ = 4,
	} pch_hda_idisp_link_frequency;

	bool pch_hda_idisp_codec_enable;

	struct pcie_rp_config pcie_rp[CONFIG_MAX_ROOT_PORTS];
	uint8_t pcie_clk_config_flag[CONFIG_MAX_PCIE_CLOCK_SRC];

	/* Gfx related */
	enum {
		IGD_SM_0MB = 0x00,
		IGD_SM_32MB = 0x01,
		IGD_SM_64MB = 0x02,
		IGD_SM_96MB = 0x03,
		IGD_SM_128MB = 0x04,
		IGD_SM_160MB = 0x05,
		IGD_SM_4MB = 0xF0,
		IGD_SM_8MB = 0xF1,
		IGD_SM_12MB = 0xF2,
		IGD_SM_16MB = 0xF3,
		IGD_SM_20MB = 0xF4,
		IGD_SM_24MB = 0xF5,
		IGD_SM_28MB = 0xF6,
		IGD_SM_36MB = 0xF8,
		IGD_SM_40MB = 0xF9,
		IGD_SM_44MB = 0xFA,
		IGD_SM_48MB = 0xFB,
		IGD_SM_52MB = 0xFC,
		IGD_SM_56MB = 0xFD,
		IGD_SM_60MB = 0xFE,
	} igd_dvmt50_pre_alloc;

	bool skip_ext_gfx_scan;
	bool eist_enable;

	/*
	 * When enabled, this feature makes the SoC throttle when the power
	 * consumption exceeds the I_TRIP threshold.
	 *
	 * FSPs sets a by default I_TRIP threshold adapted to the current SoC
	 * and assuming a Voltage Regulator error accuracy of 6.5%.
	 */
	bool enable_fast_vmode[NUM_VR_DOMAINS];

	/*
	 * Current Excursion Protection needs to be set for each VR domain
	 * in order to be able to enable fast Vmode.
	 */
	bool cep_enable[NUM_VR_DOMAINS];

	/*
	 * VR Fast Vmode I_TRIP threshold.
	 * 0-255A in 1/4 A units. Example: 400 = 100A
	 * This setting overrides the default value set by FSPs when Fast VMode
	 * is enabled.
	 */
	uint16_t fast_vmode_i_trip[NUM_VR_DOMAINS];

	/*
	 * Power state current threshold 1.
	 * Defined in 1/4 A increments. A value of 400 = 100A. Range 0-512,
	 * which translates to 0-128A. 0 = AUTO. [0] for IA, [1] for GT, [2] for
	 * SA, [3] through [5] are Reserved.
	 */
	uint16_t ps_cur_1_threshold[NUM_VR_DOMAINS];

	/*
	 * Power state current threshold 2.
	 * Defined in 1/4 A increments. A value of 400 = 100A. Range 0-512,
	 * which translates to 0-128A. 0 = AUTO. [0] for IA, [1] for GT, [2] for
	 * SA, [3] through [5] are Reserved.
	 */
	uint16_t ps_cur_2_threshold[NUM_VR_DOMAINS];

	/*
	 * Power state current threshold 3.
	 * Defined in 1/4 A increments. A value of 400 = 100A. Range 0-512,
	 * which translates to 0-128A. 0 = AUTO. [0] for IA, [1] for GT, [2] for
	 * SA, [3] through [5] are Reserved.
	 */
	uint16_t ps_cur_3_threshold[NUM_VR_DOMAINS];

	/*
	 * SerialIO device mode selection:
	 * PchSerialIoDisabled,
	 * PchSerialIoPci,
	 * PchSerialIoHidden,
	 * PchSerialIoLegacyUart,
	 * PchSerialIoSkipInit
	 */
	uint8_t serial_io_i2c_mode[CONFIG_SOC_INTEL_I2C_DEV_MAX];
	uint8_t serial_io_gspi_mode[CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX];
	uint8_t serial_io_uart_mode[CONFIG_SOC_INTEL_UART_DEV_MAX];
	/*
	 * GSPIn Default Chip Select Mode:
	 * 0:Hardware Mode,
	 * 1:Software Mode
	 */
	uint8_t serial_io_gspi_cs_mode[CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX];
	/*
	 * GSPIn Default Chip Select State:
	 * 0: Low,
	 * 1: High
	 */
	uint8_t serial_io_gspi_cs_state[CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX];

	/* CNVi WiFi Core Enable/Disable */
	bool cnvi_wifi_core;

	/* CNVi BT Core Enable/Disable */
	bool cnvi_bt_core;

	/* CNVi BT Audio Offload: Enable/Disable BT Audio Offload. */
	bool cnvi_bt_audio_offload;

	/*
	 * These GPIOs will be programmed by the IOM to handle biasing of the
	 * Type-C aux (SBU) signals when certain alternate modes are used.
	 * `pad_auxn_dc` should be assigned to the GPIO pad providing negative
	 * bias (name usually contains `AUXN_DC` or `AUX_N`); similarly,
	 * `pad_auxp_dc` should be assigned to the GPIO providing positive bias
	 * (name often contains `AUXP_DC` or `_AUX_P`).
	 */
	struct typec_aux_bias_pads typec_aux_bias_pads[MAX_TYPE_C_PORTS];

	/*
	 * SOC Aux orientation override:
	 * This is a bitfield that corresponds to up to 4 TCSS ports on MTL.
	 * Even numbered bits (0, 2, 4, 6) control the retimer being handled by SOC.
	 * Odd numbered bits (1, 3, 5, 7) control the orientation of the physical aux lines
	 * on the motherboard.
	 */
	uint16_t tcss_aux_ori;

	/* Connect Topology Command timeout value */
	uint16_t itbt_connect_topology_timeout_in_ms;

	/*
	 * Override GPIO PM configuration:
	 * 0: Use FSP default GPIO PM program,
	 * 1: coreboot to override GPIO PM program
	 */
	uint8_t gpio_override_pm;

	/*
	 * GPIO PM configuration: 0 to disable, 1 to enable power gating
	 * Bit 6-7: Reserved
	 * Bit 5: MISCCFG_GPSIDEDPCGEN
	 * Bit 4: MISCCFG_GPRCOMPCDLCGEN
	 * Bit 3: MISCCFG_GPRTCDLCGEN
	 * Bit 2: MISCCFG_GSXLCGEN
	 * Bit 1: MISCCFG_GPDPCGEN
	 * Bit 0: MISCCFG_GPDLCGEN
	 */
	uint8_t gpio_pm[TOTAL_GPIO_COMM];

	/* DP config */
	/*
	 * Port config
	 * 0:Disabled, 1:eDP, 2:MIPI DSI
	 */
	uint8_t ddi_port_A_config;
	uint8_t ddi_port_B_config;

	/* Enable(1)/Disable(0) HPD/DDC */
	uint8_t ddi_ports_config[DDI_PORT_COUNT];

	/*
	 * Override CPU flex ratio value:
	 * CPU ratio value controls the maximum processor non-turbo ratio.
	 * Valid Range 0 to 63.
	 *
	 * In general descriptor provides option to set default cpu flex ratio.
	 * Default cpu flex ratio is 0 ensures booting with non-turbo max frequency.
	 * That's the reason FSP skips cpu_ratio override if cpu_ratio is 0.
	 *
	 * Only override CPU flex ratio if don't want to boot with non-turbo max.
	 */
	uint8_t cpu_ratio_override;

	/*
	 * Enable(true)/Disable(false) DMI Power Optimizer on PCH side.
	 * Default false. Setting this to true disables the DMI Power Optimizer.
	 */
	bool dmi_pwr_optimize_disable;

	/*
	 * Enable(true)/Disable(false) CPU Replacement check.
	 * Default false. Setting this to true to check CPU replacement.
	 */
	bool cpu_replacement_check;

	/* ISA Serial Base selection. */
	enum {
		ISA_SERIAL_BASE_ADDR_3F8,
		ISA_SERIAL_BASE_ADDR_2F8,
	} isa_serial_uart_base;

	/*
	 * Assign clock source port for GbE. 0: Disable, N-1: port number
	 * Default 0.
	 */
	uint8_t lan_clk;

	/*
	 * Enable or Disable C1 C-state Auto Demotion & un-demotion
	 * The algorithm looks at the behavior of the wake up tracker, how
	 * often it is waking up, and based on that it demote the c-state.
	 * Default false. Set this to true in order to disable C1-state auto
	 * demotion.
	 * NOTE: Un-Demotion from Demoted C1 needs to be disabled when
	 *       C1 C-state Auto Demotion is disabled.
	 */
	bool disable_c1_state_auto_demotion;

	/*
	 * Enable or Disable Package C-state Demotion.
	 * Default is set to false.
	 * Set this to true in order to disable Package C-state demotion.
	 * NOTE: Un-Demotion from demoted Package C-state needs to be disabled
	 *       when auto demotion is disabled.
	 */
	bool disable_package_c_state_demotion;

	/* Enable PCH to CPU energy report feature. */
	bool pch_pm_energy_report_enable;

	/* Energy-Performance Preference (HWP feature) */
	bool enable_energy_perf_pref;
	uint8_t energy_perf_pref_value;

	bool disable_vmx;

	/*
	 * SAGV Frequency per point in Mhz. 0 is Auto, otherwise holds the
	 * frequency value expressed as an integer. For example: 1867
	 */
	uint16_t sagv_freq_mhz[MAX_SAGV_POINTS];

	/* Gear Selection for SAGV points. 0: Auto, 1: Gear 1, 2: Gear 2, 4: Gear 4 */
	uint8_t sagv_gear[MAX_SAGV_POINTS];

	/*
	 * Enable or Disable Reduced BasicMemoryTest size.
	 * Default is set to false.
	 * Set this to true in order to reduce BasicMemoryTest size
	 */
	bool lower_basic_mem_test_size;

	/* Platform Power Pmax in Watts. Zero means automatic. */
	uint16_t psys_pmax_watts;

	/* Platform Power Limit 2 in Watts. */
	uint16_t psys_pl2_watts;

	/* Enable or Disable Acoustic Noise Mitigation feature */
	bool enable_acoustic_noise_mitigation;
	/* Disable Fast Slew Rate for Deep Package C States for VR domains */
	bool disable_fast_pkgc_ramp[NUM_VR_DOMAINS];
	/*
	 * Slew Rate configuration for Deep Package C States for VR domains
	 * as per `enum slew_rate` data type.
	 */
	uint8_t slow_slew_rate_config[NUM_VR_DOMAINS];

	/* i915 struct for GMA backlight control */
	struct i915_gpu_controller_info gfx;
};

typedef struct soc_intel_meteorlake_config config_t;

#endif
