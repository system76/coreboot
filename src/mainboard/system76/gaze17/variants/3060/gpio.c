/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/gpio.h>
#include <variant/gpio.h>

static const struct pad_config gpio_table[] = {
	/* ------- GPIO Group GPD ------- */
	PAD_CFG_NF(GPD0, UP_20K, PWROK, NF1), // PM_BATLOW#
	PAD_CFG_NF(GPD1, NATIVE, PWROK, NF1), // AC_PRESENT
	PAD_CFG_NF(GPD2, NATIVE, PWROK, NF1), // LAN_WAKEUP#
	PAD_CFG_NF(GPD3, UP_20K, PWROK, NF1), // PWR_BTN#
	PAD_CFG_NF(GPD4, NONE, PWROK, NF1), // SUSB#_PCH
	PAD_CFG_NF(GPD5, NONE, PWROK, NF1), // SUSC#_PCH
	PAD_CFG_NF(GPD6, NONE, PWROK, NF1), // SLP_A#
	PAD_CFG_GPO(GPD7, 0, DEEP), // GPD_7 (Strap 16)
	PAD_CFG_NF(GPD8, NONE, PWROK, NF1), // SUS_CLK
	PAD_CFG_GPO(GPD9, 0, PWROK), // SLP_WLAN#
	PAD_CFG_NF(GPD10, NONE, PWROK, NF1), // SLP_S5#
	PAD_CFG_NF(GPD11, NONE, PWROK, NF1), // LAN_DISABLE#

	/* ------- GPIO Group GPP_A ------- */
	PAD_CFG_NF(GPP_A0, UP_20K, DEEP, NF1), // ESPI_IO0_EC
	PAD_CFG_NF(GPP_A1, UP_20K, DEEP, NF1), // ESPI_IO1_EC
	PAD_CFG_NF(GPP_A2, UP_20K, DEEP, NF1), // ESPI_IO2_EC
	PAD_CFG_NF(GPP_A3, UP_20K, DEEP, NF1), // ESPI_IO3_EC
	PAD_CFG_NF(GPP_A4, UP_20K, DEEP, NF1), // ESPI_CS_EC#
	PAD_CFG_NF(GPP_A5, UP_20K, DEEP, NF1), // SERIRQ_ESPI_ALERT0
	PAD_NC(GPP_A6, NONE),
	PAD_NC(GPP_A7, NONE),
	PAD_NC(GPP_A8, NONE),
	PAD_CFG_NF(GPP_A9, DN_20K, DEEP, NF1), // ESPI_CLK_EC
	PAD_CFG_NF(GPP_A10, NONE, DEEP, NF1), // ESPI_RESET#
	PAD_NC(GPP_A11, NONE),
	PAD_CFG_NF(GPP_A12, UP_20K, DEEP, NF1), // SATAGP1
	PAD_CFG_GPO(GPP_A13, 1, PLTRST), // BT_EN
	//PAD_CFG_GPO(GPP_A14, 0, DEEP), // DGPU_PWR_EN
	_PAD_CFG_STRUCT(GPP_A15, 0x46880100, 0x0000), // DP_E_HPD
	PAD_NC(GPP_A16, NONE), // USB_OC3#
	PAD_CFG_GPI_INT(GPP_A17, NONE, PLTRST, LEVEL), // TP_ATTN#
	_PAD_CFG_STRUCT(GPP_A18, 0x46880100, 0x0000), // HDMI_HPD
	PAD_CFG_GPI(GPP_A19, NONE, DEEP), // DGPU_PWRGD_R
	PAD_CFG_GPI(GPP_A20, NONE, PLTRST), // PEG_WAKE#
	PAD_NC(GPP_A21, NONE),
	PAD_NC(GPP_A22, NONE),
	PAD_NC(GPP_A23, NONE),

	/* ------- GPIO Group GPP_B ------- */
	PAD_CFG_NF(GPP_B0, NONE, DEEP, NF1), // VCCIN_AUX_VID0
	PAD_CFG_NF(GPP_B1, NONE, DEEP, NF1), // VCCIN_AUX_VID1
	//PAD_CFG_GPO(GPP_B2, 0, DEEP), // DGPU_RST#_PCH
	PAD_CFG_GPI(GPP_B3, NONE, DEEP), // SCI#
	PAD_CFG_GPI(GPP_B4, NONE, DEEP), // SWI#
	PAD_NC(GPP_B5, NONE),
	PAD_NC(GPP_B6, NONE),
	PAD_NC(GPP_B7, NONE),
	PAD_NC(GPP_B8, NONE),
	//PAD_CFG_GPO(GPP_B9, 0, DEEP),
	//PAD_CFG_GPO(GPP_B10, 0, DEEP),
	PAD_CFG_NF(GPP_B11, NONE, PWROK, NF1), // TBTA-PCH_I2C_INT
	PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1), // SLP_S0#
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1), // PLT_RST#
	_PAD_CFG_STRUCT(GPP_B14, 0x44001100, 0x0000), // SATA_LED#
	PAD_NC(GPP_B15, NONE),
	PAD_NC(GPP_B16, NONE),
	PAD_NC(GPP_B17, NONE),
	PAD_CFG_GPO(GPP_B18, 0, DEEP), // Strap 2 of 24
	//PAD_CFG_GPO(GPP_B19, 0, DEEP),
	//PAD_CFG_GPO(GPP_B20, 0, DEEP),
	//PAD_CFG_GPO(GPP_B21, 0, DEEP),
	//PAD_CFG_GPO(GPP_B22, 0, DEEP),
	PAD_CFG_GPO(GPP_B23, 0, DEEP), // Strap 8 of 24

	/* ------- GPIO Group GPP_C ------- */
	PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1), // SMB_CLK
	PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1), // SMB_DATA
	PAD_CFG_GPO(GPP_C2, 1, PLTRST), // SATA_M2_PWR_EN1
	PAD_CFG_NF(GPP_C3, NONE, DEEP, NF1), // SML0_CLK
	PAD_CFG_NF(GPP_C4, NONE, DEEP, NF1), // SML0_DATA
	PAD_CFG_GPO(GPP_C5, 0, DEEP), // Strap 4 of 24
	PAD_CFG_NF(GPP_C6, NONE, DEEP, NF1), // SML1_CLK
	PAD_CFG_NF(GPP_C7, NONE, DEEP, NF1), // SML1_DATA
	//PAD_NC(GPP_C8, NONE),
	//PAD_NC(GPP_C9, NONE),
	//PAD_NC(GPP_C10, NONE),
	//PAD_NC(GPP_C11, NONE),
	//PAD_NC(GPP_C12, NONE),
	//PAD_NC(GPP_C13, NONE),
	//PAD_NC(GPP_C14, NONE),
	//PAD_NC(GPP_C15, NONE),
	//PAD_NC(GPP_C16, NONE),
	//PAD_NC(GPP_C17, NONE),
	//PAD_NC(GPP_C18, NONE),
	//PAD_NC(GPP_C19, NONE),
	//PAD_NC(GPP_C20, NONE),
	//PAD_NC(GPP_C21, NONE),
	//PAD_NC(GPP_C22, NONE),
	//PAD_NC(GPP_C23, NONE),

	/* ------- GPIO Group GPP_D ------- */
	PAD_CFG_GPO(GPP_D0, 1, DEEP), // SB_BLON
	PAD_CFG_GPI(GPP_D1, NONE, DEEP), // SB_KBCRST#
	PAD_CFG_GPO(GPP_D2, 0, DEEP), // ROM_I2C_EN
	PAD_NC(GPP_D3, NONE),
	PAD_CFG_GPI(GPP_D4, NONE, DEEP), // PS8461_SW
	//PAD_CFG_NF(GPP_D5, NONE, DEEP, NF1), // PEX4_SSD_CLKREQ#
	//PAD_CFG_NF(GPP_D6, NONE, DEEP, NF1), // SSD_CLKREQ#
	//PAD_CFG_NF(GPP_D7, NONE, DEEP, NF1), // WLAN_CLKREQ#
	//PAD_CFG_NF(GPP_D8, NONE, DEEP, NF1), // PEQ_CLKREQ#
	PAD_NC(GPP_D9, NONE),
	PAD_CFG_GPO(GPP_D10, 0, DEEP), // Strap 6 of 24
	PAD_NC(GPP_D11, NONE),
	PAD_CFG_GPI(GPP_D12, NATIVE, DEEP), // Strap 7 of 24
	PAD_CFG_GPI(GPP_D13, NONE, DEEP), // WLAN_WAKEUP#
	PAD_CFG_GPO(GPP_D14, 1, PLTRST), // SATA_M2_PWR_EN2
	PAD_NC(GPP_D15, NONE),
	PAD_NC(GPP_D16, NONE),
	PAD_NC(GPP_D17, NONE),
	PAD_NC(GPP_D18, NONE),
	PAD_NC(GPP_D19, NONE),

	/* ------- GPIO Group GPP_E ------- */
	PAD_CFG_GPI(GPP_E0, NONE, DEEP), // CNVI_WAKE#
	//_PAD_CFG_STRUCT(GPP_E1, 0x40100100, 0x3000), // TPM_PIRQ#
	PAD_CFG_GPI(GPP_E2, NONE, DEEP), // BOARD_ID2
	PAD_CFG_GPO(GPP_E3, 1, PLTRST), // PCH_WLAN_EN
	PAD_CFG_GPO(GPP_E4, 0, PLTRST), // TBT_FORCE_PWR
	PAD_NC(GPP_E5, NONE),
	PAD_CFG_GPO(GPP_E6, 0, DEEP), // Strap 12 of 24
	PAD_CFG_GPI(GPP_E7, NONE, DEEP), // SMI#
	PAD_CFG_GPO(GPP_E8, 0, DEEP), // SLP_DRAM#
	PAD_CFG_NF(GPP_E9, NONE, DEEP, NF1), // USB_OC0#
	PAD_NC(GPP_E10, NONE),
	PAD_CFG_GPI(GPP_E11, NONE, DEEP), // BOARD_ID1
	PAD_CFG_GPO(GPP_E12, 0, DEEP), // TP_ATTN#
	PAD_CFG_GPI(GPP_E13, NONE, DEEP), // BOARD_ID4
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1), // PCH_EDP_HPD
	PAD_NC(GPP_E15, NONE),
	PAD_NC(GPP_E16, NONE),
	PAD_CFG_GPI(GPP_E17, NONE, DEEP), // BOARD_ID3
	PAD_NC(GPP_E18, NATIVE), // TBT_LSX0_TXD
	PAD_NC(GPP_E19, NATIVE), // TBT_LSX0_RXD
	PAD_NC(GPP_E20, NONE),
	PAD_CFG_GPO(GPP_E21, 0, DEEP), // Strap 14 of 24
	PAD_NC(GPP_E22, NONE),
	PAD_NC(GPP_E23, NONE),

	/* ------- GPIO Group GPP_F ------- */
	PAD_CFG_NF(GPP_F0, NONE, DEEP, NF1), // CNVI_BRI_DT
	PAD_CFG_NF(GPP_F1, UP_20K, DEEP, NF1), // CNVI_BRI_RSP
	PAD_CFG_NF(GPP_F2, NONE, DEEP, NF1), // CNVI_RGI_DT
	PAD_CFG_NF(GPP_F3, UP_20K, DEEP, NF1), // CNVI_RGI_RSP
	PAD_CFG_NF(GPP_F4, NONE, DEEP, NF1), // CNVI_RF_RST#
	//PAD_CFG_NF(GPP_F5, NONE, DEEP, NF2), // XTAL_CLKREQ
	PAD_CFG_NF(GPP_F6, NONE, DEEP, NF1), // CNVI_GNSS_PA_BLANKING
	PAD_CFG_GPO(GPP_F7, 1, DEEP), // LAN_PLT_RST#
	//PAD_CFG_GPO(GPP_F8, 0, DEEP),
	PAD_NC(GPP_F9, NONE),
	PAD_CFG_GPO(GPP_F10, 1, DEEP), // CARD_RTD3_RST#
	PAD_NC(GPP_F11, NONE),
	PAD_CFG_GPI(GPP_F12, NONE, PLTRST), // GPIO4_GC6_NVVDD_EN
	PAD_CFG_GPI(GPP_F13, NONE, PLTRST), // GC6_FB_EN_PCH
	PAD_NC(GPP_F14, NONE),
	PAD_NC(GPP_F15, NONE),
	PAD_CFG_GPI(GPP_F16, NONE, PLTRST), // GPU_EVENT#
	PAD_NC(GPP_F17, NONE),
	PAD_NC(GPP_F18, NONE),
	//PAD_CFG_NF(GPP_F19, NONE, DEEP, NF1), // LAN_CLKREQ#
	PAD_CFG_GPO(GPP_F20, 1, PLTRST), // M2_CPU_SSD1_RST#
	PAD_NC(GPP_F21, NONE),
	PAD_NC(GPP_F22, NONE),
	PAD_NC(GPP_F23, NONE),

	/* ------- GPIO Group GPP_H ------- */
	PAD_CFG_GPO(GPP_H0, 1, DEEP), // M2_PCH_SSD_RST#
	PAD_CFG_GPO(GPP_H1, 0, DEEP), // Strap 22 of 24
	PAD_CFG_GPO(GPP_H2, 1, DEEP), // M2_WLAN_RST#
	PAD_CFG_GPI(GPP_H3, NONE, DEEP), // TPM_DET
	PAD_CFG_NF(GPP_H4, NONE, DEEP, NF1), // I2C_SDA_TP
	PAD_CFG_NF(GPP_H5, NONE, DEEP, NF1), // I2C_SCL_TP
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1), // PCH_I2C_SDA
	PAD_CFG_NF(GPP_H7, NONE, DEEP, NF1), // PCH_I2C_SCL
	PAD_CFG_GPO(GPP_H8, 0, DEEP), // CNVI_MFUART2_RXD
	PAD_CFG_GPO(GPP_H9, 0, DEEP), // CNVI_MFUART2_TXD
	PAD_CFG_GPI(GPP_H10, NONE, DEEP), // UART0_RXD
	PAD_CFG_GPI(GPP_H11, NONE, DEEP), // UART0_TXD
	PAD_NC(GPP_H12, NONE),
	PAD_CFG_GPI(GPP_H13, NONE, DEEP), // DEVSLP1
	//PAD_NC(GPP_H14, NONE),
	PAD_NC(GPP_H15, NONE),
	//PAD_NC(GPP_H16, NONE),
	PAD_NC(GPP_H17, NONE),
	PAD_CFG_NF(GPP_H18, NONE, DEEP, NF1), // CPU_C10_GATE#
	PAD_NC(GPP_H19, NONE),
	PAD_CFG_GPI(GPP_H20, NONE, DEEP), // BL_PWW_EN_EC
	PAD_CFG_GPI(GPP_H21, NONE, DEEP), // PLVDD_RST_EC
	PAD_CFG_GPO(GPP_H22, 0, DEEP), // MUX_CTRL_BIOS
	//PAD_CFG_NF(GPP_H23, NONE, DEEP, NF2), // CARD_CLKREQ#

	/* ------- GPIO Group GPP_R ------- */
	PAD_CFG_NF(GPP_R0, NONE, DEEP, NF1), // HDA_BITCLK
	PAD_CFG_NF(GPP_R1, NATIVE, DEEP, NF1), // HDA_SYNC
	PAD_CFG_NF(GPP_R2, NATIVE, DEEP, NF1), // HDA_SDOUT
	PAD_CFG_NF(GPP_R3, NATIVE, DEEP, NF1), // HDA_SDIN0
	PAD_CFG_NF(GPP_R4, NONE, DEEP, NF1), // AZ_RST#_R
	PAD_NC(GPP_R5, NONE),
	PAD_CFG_GPO(GPP_R6, 0, DEEP), // DMIC_CLK1
	PAD_CFG_GPO(GPP_R7, 0, DEEP), // DMIC_DATA1

	/* ------- GPIO Group GPP_S ------- */
	PAD_NC(GPP_S0, NONE),
	PAD_NC(GPP_S1, NONE),
	PAD_NC(GPP_S2, NONE),
	PAD_NC(GPP_S3, NONE),
	PAD_NC(GPP_S4, NONE),
	PAD_NC(GPP_S5, NONE),
	PAD_NC(GPP_S6, NONE),
	PAD_NC(GPP_S7, NONE),

	/* ------- GPIO Group GPP_T ------- */
	PAD_NC(GPP_T2, NONE),
	PAD_NC(GPP_T3, NONE),
};

void variant_configure_gpios(void)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
