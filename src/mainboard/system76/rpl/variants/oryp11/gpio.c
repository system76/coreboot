/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/gpio.h>
#include <soc/gpio.h>

static const struct pad_config gpio_table[] = {
	/* ------- GPIO Group GPD ------- */
	PAD_CFG_NF(GPD0, UP_20K, PWROK, NF1), // BATLOW_N
	PAD_CFG_NF(GPD1, NATIVE, PWROK, NF1), // AC_PRESENT
	PAD_NC(GPD2, NONE),
	PAD_CFG_NF(GPD3, UP_20K, PWROK, NF1), // PWR_BTN#
	PAD_CFG_NF(GPD4, NONE, PWROK, NF1), // SUSB#_PCH
	PAD_CFG_NF(GPD5, NONE, PWROK, NF1), // SUSC#_PCH
	PAD_NC(GPD6, NONE),
	PAD_CFG_GPO(GPD7, 0, DEEP), // GPD_7 (Strap)
	PAD_CFG_NF(GPD8, NONE, PWROK, NF1), // CNVI_SUSCLK
	PAD_CFG_GPO(GPD9, 0, PWROK), // SLP_WLAN#
	PAD_NC(GPD10, NONE),
	PAD_NC(GPD11, NONE),
	PAD_NC(GPD12, NONE),

	/* ------- GPIO Group GPP_A ------- */
	PAD_CFG_NF(GPP_A0, UP_20K, DEEP, NF1), // ESPI_IO0_EC
	PAD_CFG_NF(GPP_A1, UP_20K, DEEP, NF1), // ESPI_IO1_EC
	PAD_CFG_NF(GPP_A2, UP_20K, DEEP, NF1), // ESPI_IO2_EC
	PAD_CFG_NF(GPP_A3, UP_20K, DEEP, NF1), // ESPI_IO3_EC
	PAD_CFG_NF(GPP_A4, UP_20K, DEEP, NF1), // ESPI_CS_EC#
	PAD_CFG_NF(GPP_A5, DN_20K, DEEP, NF1), // ESPI_CLK_EC
	PAD_CFG_NF(GPP_A6, NONE, DEEP, NF1), // ESPI_RESET#
	PAD_NC(GPP_A7, NONE),
	PAD_NC(GPP_A8, NONE),
	PAD_NC(GPP_A9, NONE),
	PAD_CFG_NF(GPP_A10, UP_20K, DEEP, NF1), // ESPI_ALRT0#
	PAD_CFG_GPI(GPP_A11, UP_20K, DEEP), // GPIO4_GC6_NVVDD_EN_R
	PAD_NC(GPP_A12, NONE),
	PAD_NC(GPP_A13, NONE),
	PAD_NC(GPP_A14, NONE),

	/* ------- GPIO Group GPP_B ------- */
	_PAD_CFG_STRUCT(GPP_B0, 0x40100100, 0x3000), // TPM_PIRQ#
	PAD_NC(GPP_B1, NONE),
	PAD_CFG_GPI(GPP_B2, NONE, DEEP), // CNVI_WAKE#
	PAD_CFG_GPO(GPP_B3, 1, PLTRST), // PCH_BT_EN
	PAD_NC(GPP_B4, NONE),
	PAD_NC(GPP_B5, NONE),
	PAD_NC(GPP_B6, NONE),
	PAD_NC(GPP_B7, NONE),
	PAD_NC(GPP_B8, NONE),
	PAD_NC(GPP_B9, NONE),
	PAD_NC(GPP_B10, NONE),
	PAD_NC(GPP_B11, NONE),
	PAD_NC(GPP_B12, NONE),
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1), // PLT_RST#
	PAD_CFG_NF(GPP_B14, NONE, DEEP, NF1), // HDA_SPKR
	PAD_NC(GPP_B15, NONE),
	PAD_NC(GPP_B16, NONE),
	PAD_NC(GPP_B17, NONE),
	PAD_CFG_NF(GPP_B18, NONE, PWROK, NF1), // GPP_B18_PMCALERT#
	PAD_NC(GPP_B19, NONE),
	PAD_CFG_GPO(GPP_B20, 0, DEEP), // GPIO_LANRTD3
	_PAD_CFG_STRUCT(GPP_B21, 0x42880100, 0x0000), // GPP_B21_TBT_WAKE#
	PAD_CFG_GPO(GPP_B22, 0, DEEP), // LAN_EC_RST#
	PAD_CFG_GPI(GPP_B23, NONE, DEEP), // Crystal frequency strap

	/* ------- GPIO Group GPP_C ------- */
	PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1), // SMB_CLK
	PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1), // SMB_DATA
	PAD_NC(GPP_C2, NONE), // PCH_PORT80_LED (TLS confidentiality strap)
	PAD_CFG_NF(GPP_C3, NONE, DEEP, NF3), // GPPB_I2C2_SDA (Pantone)
	PAD_CFG_NF(GPP_C4, NONE, DEEP, NF3), // GPPB_I2C2_SCL (Pantone)
	PAD_NC(GPP_C5, NONE), // eSPI disable strap
	PAD_CFG_GPI(GPP_C6, NONE, DEEP), // I2C3_SDA (PD controller)
	PAD_CFG_GPI(GPP_C7, NONE, DEEP), // I2C3_SCL (PD controller)
	PAD_CFG_GPI(GPP_C8, NONE, DEEP), // TPM_DET
	PAD_NC(GPP_C9, NONE),
	PAD_NC(GPP_C10, NONE),
	PAD_NC(GPP_C11, NONE),
	PAD_NC(GPP_C12, NONE),
	PAD_NC(GPP_C13, NONE),
	PAD_NC(GPP_C14, NONE),
	PAD_NC(GPP_C15, NONE),
	PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1), // I2C_SDA_TP
	PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1), // I2C_SCL_TP
	PAD_CFG_NF(GPP_C18, NONE, DEEP, NF1), // PCH_I2C_SDA
	PAD_CFG_NF(GPP_C19, NONE, DEEP, NF1), // PCH_I2C_SCL
	// GPP_C20 (UART2_RX) configured in bootblock
	// GPP_C21 (UART2_TX) configured in bootblock
	PAD_NC(GPP_C22, NONE),
	PAD_NC(GPP_C23, NONE),

	/* ------- GPIO Group GPP_D ------- */
	PAD_NC(GPP_D0, NONE),
	PAD_NC(GPP_D1, NONE),
	PAD_NC(GPP_D2, NONE),
	PAD_CFG_GPO(GPP_D3, 0, DEEP), // GFX_DETECT_STRAP
	PAD_NC(GPP_D4, NONE),
	// GPP_D5 (M.2_BT_PCMFRM_CRF_RST_N) configured by FSP
	// GPP_D6 (M.2_BT_PCMOUT_CLKREQ0) configured by FSP
	PAD_NC(GPP_D7, NONE), // M.2_BT_PCMIN
	PAD_NC(GPP_D8, NONE), // M.2_BT_PCMCLK
	PAD_NC(GPP_D9, NONE),
	PAD_NC(GPP_D10, NONE),
	PAD_NC(GPP_D11, NONE),
	// GPP_D12 (SSD_CLKREQ#) configured by FSP
	PAD_NC(GPP_D13, NONE),
	PAD_NC(GPP_D14, NONE),
	PAD_NC(GPP_D15, NONE),
	PAD_NC(GPP_D16, NONE),
	PAD_NC(GPP_D17, NONE),
	PAD_NC(GPP_D18, NONE),
	PAD_NC(GPP_D19, NONE),
	PAD_NC(GPP_D20, NONE),
	PAD_NC(GPP_D21, NONE),
	PAD_NC(GPP_D22, NONE),
	PAD_NC(GPP_D23, NONE),

	/* ------- GPIO Group GPP_E ------- */
	PAD_NC(GPP_E0, NONE),
	PAD_NC(GPP_E1, NONE),
	PAD_CFG_GPI(GPP_E2, NONE, DEEP), // SWI#
	PAD_CFG_GPI(GPP_E3, NONE, DEEP), // SMI#
	PAD_NC(GPP_E4, NONE),
	PAD_NC(GPP_E5, NONE),
	PAD_NC(GPP_E6, NONE),
	PAD_CFG_GPI_INT(GPP_E7, NONE, PLTRST, LEVEL), // TP_ATTN#
	PAD_CFG_NF(GPP_E8, NONE, DEEP, NF1), // SATA_LED#
	PAD_CFG_GPI(GPP_E9, NONE, DEEP), // USB_OC0#
	PAD_CFG_GPI(GPP_E10, NONE, DEEP), // USB_OC1#
	PAD_CFG_GPI(GPP_E11, NONE, DEEP), // USB_OC2#
	PAD_CFG_GPI(GPP_E12, NONE, DEEP), // USB_OC3#
	PAD_NC(GPP_E13, NONE),
	PAD_NC(GPP_E14, NONE),
	PAD_CFG_GPO(GPP_E15, 0, DEEP), // ROM_I2C_EN
	PAD_NC(GPP_E16, NONE),
	PAD_CFG_GPI(GPP_E17, NONE, DEEP), // SB_KBCRST#
	PAD_CFG_GPO(GPP_E18, 1, DEEP), // SB_BLON
	PAD_NC(GPP_E19, NONE),
	PAD_NC(GPP_E20, NONE),
	PAD_NC(GPP_E21, NONE),

	/* ------- GPIO Group GPP_F ------- */
	PAD_CFG_NF(GPP_F0, UP_20K, DEEP, NF1), // M.2_SSD_SATA_DET_N
	PAD_NC(GPP_F1, NONE),
	PAD_CFG_GPO(GPP_F2, 1, DEEP), // GPP_F2_TBT_RST#
	PAD_NC(GPP_F3, NONE),
	PAD_NC(GPP_F4, NONE),
	PAD_NC(GPP_F5, NONE), // SSD_SATA_DEVSLP
	PAD_NC(GPP_F6, NONE),
	PAD_CFG_GPI(GPP_F7, NONE, DEEP), // INTP_8851
	PAD_CFG_GPI(GPP_F8, NONE, PLTRST), // GC6_FB_EN_PCH
	// GPP_F9 (DGPU_PWR_EN) configured in bootblock
	PAD_CFG_GPI(GPP_F10, NONE, DEEP), // Recovery strap
	PAD_NC(GPP_F11, NONE),
	PAD_NC(GPP_F12, NONE),
	PAD_NC(GPP_F13, NONE),
	PAD_NC(GPP_F14, NONE),
	PAD_CFG_GPI(GPP_F15, NONE, DEEP), // K_SKTOCC_N
	PAD_NC(GPP_F16, NONE),
	PAD_NC(GPP_F17, NONE),
	PAD_CFG_GPO(GPP_F18, 0, DEEP), // CCD_WP#
	PAD_CFG_NF(GPP_F19, NONE, DEEP, NF1), // NV_ENAVDD
	PAD_CFG_NF(GPP_F20, NONE, DEEP, NF1), // BLON
	PAD_CFG_NF(GPP_F21, NONE, DEEP, NF1), // EDP_BRIGHTNESS
	PAD_NC(GPP_F22, NONE),
	PAD_NC(GPP_F23, NONE),

	/* ------- GPIO Group GPP_G ------- */
	PAD_NC(GPP_G0, NONE),
	PAD_NC(GPP_G1, NONE),
	PAD_NC(GPP_G2, NONE),
	PAD_CFG_GPI(GPP_G3, NONE, DEEP), // MB det (MB1/MB2)
	PAD_CFG_GPI(GPP_G4, NONE, DEEP), // MB det (15"/17")
	PAD_CFG_NF(GPP_G5, NONE, DEEP, NF1), // SLP_DRAM_N
	PAD_CFG_GPI(GPP_G6, NONE, DEEP), // MB det (G-SYNC)
	PAD_CFG_GPI(GPP_G7, NONE, DEEP), // MB det (Pantone)

	/* ------- GPIO Group GPP_H ------- */
	PAD_CFG_GPI(GPP_H0, NONE, DEEP), // VAL_SV_ADVANCE_STRAP
	PAD_NC(GPP_H1, NONE),
	PAD_CFG_GPI(GPP_H2, NONE, DEEP), // WLAN_GPIO_WAKE_N
	// GPP_H3 (NC) configured by FSP
	// GPP_H4 (SSD_SATA_CLKREQ#) configured by FSP
	// GPP_H5 (WLAN_CLKREQ#) configured by FSP
	// GPP_H6 (CARD_CLKREQ#) configured by FSP
	// GPP_H7 (GLAN_CLKREQ#) configured by FSP
	// GPP_H8 (PEG_CLKREQ#) configured by FSP
	// GPP_H9 (TBT_CLKREQ#) configured by FSP
	PAD_NC(GPP_H10, NONE),
	PAD_NC(GPP_H11, NONE),
	PAD_NC(GPP_H12, NONE), // eSPI flash sharing mode strap
	PAD_NC(GPP_H13, NONE),
	PAD_NC(GPP_H14, NONE),
	PAD_NC(GPP_H15, NONE), // JTAG ODT disable strap
	PAD_NC(GPP_H16, NONE),
	PAD_NC(GPP_H17, NONE),
	PAD_NC(GPP_H18, NONE), // 1.8V VCCPSPI strap
	PAD_NC(GPP_H19, NONE),
	PAD_NC(GPP_H20, NONE),
	PAD_NC(GPP_H21, NONE),
	PAD_NC(GPP_H22, NONE),
	PAD_NC(GPP_H23, NONE),

	/* ------- GPIO Group GPP_I ------- */
	_PAD_CFG_STRUCT(GPP_I0, 0x86880100, 0x0000), // DP_F_HPD
	PAD_CFG_NF(GPP_I1, NONE, DEEP, NF1), // CPU_DP_B_HPD
	_PAD_CFG_STRUCT(GPP_I2, 0x86880100, 0x0000), // HDMI_HPD
	PAD_CFG_NF(GPP_I3, NONE, DEEP, NF1), // CPU_DP_D_HPD
	_PAD_CFG_STRUCT(GPP_I4, 0x86880100, 0x0000), // G_DP_DHPD_E
	PAD_CFG_GPO(GPP_I5, 0, DEEP), // GPIO_TBT_RESET
	PAD_NC(GPP_I6, NONE),
	PAD_NC(GPP_I7, NONE),
	PAD_NC(GPP_I8, NONE),
	PAD_NC(GPP_I9, NONE),
	PAD_NC(GPP_I10, NONE),
	PAD_CFG_GPI(GPP_I11, NONE, DEEP), // USB_OC4#
	PAD_CFG_GPI(GPP_I12, NONE, DEEP), // USB_OC5#
	PAD_CFG_GPI(GPP_I13, NONE, DEEP), // USB_OC6#
	PAD_CFG_GPI(GPP_I14, NONE, DEEP), // USB_OC7#
	PAD_NC(GPP_I15, NONE),
	PAD_NC(GPP_I16, NONE),
	PAD_NC(GPP_I17, NONE),
	PAD_NC(GPP_I18, NONE), // NO REBOOT strap
	PAD_NC(GPP_I19, NONE),
	PAD_NC(GPP_I20, NONE),
	PAD_NC(GPP_I21, NONE),
	PAD_NC(GPP_I22, NONE), // BOOT BIOS strap

	/* ------- GPIO Group GPP_J ------- */
	PAD_CFG_NF(GPP_J0, NONE, DEEP, NF1), // CNVI_GNSS_PA_BLANKING
	PAD_CFG_NF(GPP_J1, NONE, DEEP, NF1), // CPU_C10_GATE_N
	PAD_CFG_NF(GPP_J2, NONE, DEEP, NF1), // CNVI_BRI_DT_R
	PAD_CFG_NF(GPP_J3, UP_20K, DEEP, NF1), // CNVI_BRI_RSP
	PAD_CFG_NF(GPP_J4, NONE, DEEP, NF1), // CNVI_RGI_DT_R
	PAD_CFG_NF(GPP_J5, UP_20K, DEEP, NF1), // CNVI_RGI_RSP
	PAD_CFG_NF(GPP_J6, NONE, DEEP, NF1), // CNVI_MFUART2_RXD
	PAD_CFG_NF(GPP_J7, NONE, DEEP, NF1), // CNVI_MFUART2_TXD
	PAD_CFG_GPI(GPP_J8, NONE, DEEP), // VAL_TEST_SETUP_MENU
	PAD_NC(GPP_J9, NONE),
	PAD_NC(GPP_J10, NONE),
	PAD_NC(GPP_J11, NONE),

	/* ------- GPIO Group GPP_K ------- */
	_PAD_CFG_STRUCT(GPP_K0, 0x42800100, 0x0000), // TBCIO_PLUG_EVENT#
	PAD_NC(GPP_K1, NONE),
	PAD_NC(GPP_K2, NONE),
	PAD_CFG_GPO(GPP_K3, 1, PLTRST), // TBT_RTD3_PWR_EN_R
	PAD_CFG_GPO(GPP_K4, 0, PWROK), // TBT_FORCE_PWR_R
	PAD_NC(GPP_K5, NONE),
	// GPP_K6 doesn't exist
	// GPP_K7 doesn't exist
	PAD_CFG_NF(GPP_K8, NONE, DEEP, NF1), // VCCIN_AUX_VID0
	PAD_CFG_NF(GPP_K9, NONE, DEEP, NF1), // VCCIN_AUX_VID1
	// GPP_K10 doesn't exist
	PAD_NC(GPP_K11, NONE),

	/* ------- GPIO Group GPP_R ------- */
	PAD_CFG_NF(GPP_R0, NONE, DEEP, NF1), // HDA_BITCLK
	PAD_CFG_NF(GPP_R1, NATIVE, DEEP, NF1), // HDA_SYNC
	PAD_CFG_NF(GPP_R2, NATIVE, DEEP, NF1), // HDA_SDOUT / ME_WE
	PAD_CFG_NF(GPP_R3, NATIVE, DEEP, NF1), // HDA_SDIN0
	PAD_CFG_NF(GPP_R4, NONE, DEEP, NF1), // HDA_RST#_R
	PAD_NC(GPP_R5, NONE),
	PAD_NC(GPP_R6, NONE),
	PAD_NC(GPP_R7, NONE),
	PAD_CFG_GPI(GPP_R8, NONE, DEEP), // DGPU_PWRGD_R
	PAD_CFG_NF(GPP_R9, NONE, DEEP, NF1), // EDP_HPD
	PAD_NC(GPP_R10, NONE),
	PAD_NC(GPP_R11, NONE),
	PAD_NC(GPP_R12, NONE),
	PAD_NC(GPP_R13, NONE),
	PAD_NC(GPP_R14, NONE),
	PAD_NC(GPP_R15, NONE),
	// GPP_R16 (DGPU_RST#_PCH) configured in bootblock
	PAD_NC(GPP_R17, NONE),
	PAD_NC(GPP_R18, NONE),
	PAD_CFG_GPI(GPP_R19, NONE, DEEP), // SCI#
	PAD_NC(GPP_R20, NONE),
	PAD_NC(GPP_R21, NONE),

	/* ------- GPIO Group GPP_S ------- */
	PAD_NC(GPP_S0, NONE),
	PAD_NC(GPP_S1, NONE),
	PAD_NC(GPP_S2, NONE),
	PAD_NC(GPP_S3, NONE),
	PAD_NC(GPP_S4, NONE), // GPPS_DMIC_CLK
	PAD_NC(GPP_S5, NONE), // GPPS_DMIC_DATA
	PAD_NC(GPP_S6, NONE),
	PAD_NC(GPP_S7, NONE),
};

void mainboard_configure_gpios(void)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
