/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_GPIO_H
#define MAINBOARD_GPIO_H

#define DGPU_RST_N GPP_F22
#define DGPU_PWR_EN GPP_F23
#define DGPU_GC6 GPP_K21

#ifndef __ACPI__

#include <soc/gpe.h>
#include <soc/gpio.h>

/* Pad configuration in romstage. */
static const struct pad_config early_gpio_table[] = {
	PAD_CFG_TERM_GPO(GPP_C14, 1, NONE, RSMRST), // M.2_PLT_RST_CNTRL1#
	PAD_CFG_TERM_GPO(GPP_C15, 1, NONE, RSMRST), // M.2_PLT_RST_CNTRL2#
	PAD_CFG_TERM_GPO(GPP_F0, 1, NONE, RSMRST), // TBT_PERST_N
	PAD_CFG_TERM_GPO(GPP_F22, 0, NONE, DEEP), // DGPU_RST_N
	PAD_CFG_TERM_GPO(GPP_F23, 0, NONE, DEEP), // DGPU_PWR_EN
	PAD_CFG_TERM_GPO(GPP_K8, 1, NONE, RSMRST), // SATA_M2_PWR_EN1
	PAD_CFG_TERM_GPO(GPP_K9, 1, NONE, RSMRST), // SATA_M2_PWR_EN2
	PAD_CFG_TERM_GPO(GPP_H16, 1, NONE, RSMRST), // TBT_RTD3_PWR_EN_R
	PAD_CFG_TERM_GPO(GPP_K11, 1, NONE, RSMRST), // GPIO_LANRTD3
};

/* Pad configuration in ramstage. */
static const struct pad_config gpio_table[] = {
	/* ------- GPIO Group GPD ------- */
	PAD_CFG_NF(GPD0, NONE, DEEP, NF1), // PM_BATLOW#
	PAD_CFG_NF(GPD1, NATIVE, DEEP, NF1), // AC_PRESENT
	PAD_CFG_GPI(GPD2, NATIVE, PWROK), // LAN_WAKEUP#
	PAD_CFG_NF(GPD3, UP_20K, DEEP, NF1), // PWR_BTN#
	PAD_CFG_NF(GPD4, NONE, DEEP, NF1), // SUSB#_PCH
	PAD_CFG_NF(GPD5, NONE, DEEP, NF1), // SUSC#_PCH
	PAD_NC(GPD6, UP_20K),
	PAD_CFG_GPI(GPD7, UP_20K, PWROK), // GPD_7 (crystal input, low = single ended, high = differential)
	PAD_CFG_NF(GPD8, NONE, DEEP, NF1), // SUS_CLK
	PAD_NC(GPD9, UP_20K),
	PAD_CFG_NF(GPD10, NONE, DEEP, NF1), // SLP_S5#
	PAD_NC(GPD11, UP_20K),

	/* ------- GPIO Group GPP_A ------- */
	PAD_CFG_NF(GPP_A0, NONE, DEEP, NF1), // SB_KBCRST#
	PAD_CFG_NF(GPP_A1, NONE, DEEP, NF1), // LPC_AD0
	PAD_CFG_NF(GPP_A2, NONE, DEEP, NF1), // LPC_AD1
	PAD_CFG_NF(GPP_A3, NONE, DEEP, NF1), // LPC_AD2
	PAD_CFG_NF(GPP_A4, NONE, DEEP, NF1), // LPC_AD3
	PAD_CFG_NF(GPP_A5, NONE, DEEP, NF1), // LPC_FRAME#
	PAD_CFG_NF(GPP_A6, NONE, DEEP, NF1), // SERIRQ
	PAD_CFG_NF(GPP_A7, NONE, DEEP, NF1), // LPC_PIRQA#
	PAD_CFG_NF(GPP_A8, NONE, DEEP, NF1), // PM_CLKRUN#
	PAD_CFG_NF(GPP_A9, NONE, DEEP, NF1), // PCLK_KBC
	PAD_NC(GPP_A10, UP_20K),
	PAD_CFG_GPI(GPP_A11, UP_20K, DEEP), // LAN_WAKEUP#
	PAD_NC(GPP_A12, UP_20K),
	PAD_NC(GPP_A13, NONE), // SUSWARN# (test point)
	PAD_NC(GPP_A14, NONE),
	PAD_NC(GPP_A15, NONE), // SUS_PWR_ACK# (test point)
	PAD_NC(GPP_A16, UP_20K),
	PAD_NC(GPP_A17, UP_20K),
	PAD_NC(GPP_A18, UP_20K),
	PAD_CFG_TERM_GPO(GPP_A19, 1, NONE, DEEP), // SB_BLON
	PAD_CFG_GPI(GPP_A20, NONE, DEEP), // PEX_WAKE#
	PAD_CFG_GPI(GPP_A21, UP_20K, DEEP), // EAPD_MODE
	PAD_CFG_TERM_GPO(GPP_A22, 1, NONE, DEEP), // WLAN_SSD2_GPIO1
	PAD_CFG_TERM_GPO(GPP_A23, 1, NONE, DEEP), // WLAN_SSD2_GPIO

	/* ------- GPIO Group GPP_B ------- */
	_PAD_CFG_STRUCT(GPP_B0, 0x42080100, 0x3000), // TPM_PIRQ#
	PAD_NC(GPP_B1, UP_20K),
	PAD_NC(GPP_B2, UP_20K),
	PAD_CFG_TERM_GPO(GPP_B3, 1, NONE, DEEP), // BT_EN
	PAD_CFG_TERM_GPO(GPP_B4, 1, NONE, DEEP), // WLAN_EN
	PAD_CFG_NF(GPP_B5, NONE, DEEP, NF1), // TBT_CLKREQ#
	PAD_NC(GPP_B6, UP_20K),
	PAD_CFG_TERM_GPO(GPP_B7, 1, NONE, PLTRST), // CR_GPIO_RST#
	PAD_CFG_TERM_GPO(GPP_B8, 1, NONE, PLTRST), // CR_GPIO_WAKE#
	PAD_NC(GPP_B9, UP_20K),
	PAD_NC(GPP_B10, UP_20K),
	PAD_NC(GPP_B11, UP_20K),
	PAD_CFG_GPI(GPP_B12, UP_20K, DEEP), // SLP_S0#
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1), // PLT_RST#
	PAD_CFG_NF(GPP_B14, NONE, DEEP, NF1), // PCH_SPKR
	PAD_NC(GPP_B15, UP_20K),
	PAD_NC(GPP_B16, UP_20K),
	PAD_NC(GPP_B17, UP_20K),
	PAD_NC(GPP_B18, UP_20K), // LPSS_GSPI0_MOSI (test point)
	PAD_NC(GPP_B19, UP_20K),
	PAD_NC(GPP_B20, UP_20K),
	PAD_NC(GPP_B21, UP_20K),
	PAD_CFG_GPI(GPP_B22, UP_20K, DEEP), // LPSS_GSPI1_MOSI (boot strap)
	PAD_CFG_NF(GPP_B23, NONE, DEEP, NF2), // PCH_HOT_GNSS_DISABLE (boot strap)

	/* ------- GPIO Group GPP_C ------- */
	PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1), // SMB_CLK
	PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1), // SMB_DATA
	PAD_CFG_GPI(GPP_C2, UP_20K, DEEP), // CNVI_WAKE#
	PAD_NC(GPP_C3, UP_20K),
	PAD_NC(GPP_C4, UP_20K),
	PAD_CFG_GPI(GPP_C5, UP_20K, DEEP), // WLAN_WAKEUP#
	PAD_CFG_GPI(GPP_C6, UP_20K, DEEP), // SMC_CPU_THERM
	PAD_CFG_GPI(GPP_C7, UP_20K, DEEP), // SMD_CPU_THERM
	PAD_CFG_GPI(GPP_C8, NONE, PLTRST), // TPM_DET
	PAD_CFG_GPI(GPP_C9, NONE, DEEP), // CNVI_DET#
	PAD_NC(GPP_C10, UP_20K),
	PAD_NC(GPP_C11, UP_20K),
	PAD_NC(GPP_C12, UP_20K),
	PAD_NC(GPP_C13, UP_20K),
	PAD_CFG_TERM_GPO(GPP_C14, 1, NONE, RSMRST), // M.2_PLT_RST_CNTRL1#
	PAD_CFG_TERM_GPO(GPP_C15, 1, NONE, RSMRST), // M.2_PLT_RST_CNTRL2#
	PAD_CFG_NF(GPP_C16, NONE, PLTRST, NF1), // I2C_SDA_TP
	PAD_CFG_NF(GPP_C17, NONE, PLTRST, NF1), // I2C_SCL_TP
	PAD_CFG_NF(GPP_C18, NONE, PLTRST, NF1), // I2C_SDA_Pantone
	PAD_CFG_NF(GPP_C19, NONE, PLTRST, NF1), // I2C_SCL_Pantone
	PAD_CFG_GPI(GPP_C20, UP_20K, DEEP), // CNVI_MFUART2_RXD
	PAD_CFG_GPI(GPP_C21, UP_20K, DEEP), // CNVI_MFUART2_TXD
	PAD_CFG_GPI(GPP_C22, UP_20K, DEEP), // LAN_PLT_RST#
	PAD_NC(GPP_C23, UP_20K),

	/* ------- GPIO Group GPP_D ------- */
	PAD_NC(GPP_D0, UP_20K),
	PAD_NC(GPP_D1, UP_20K),
	PAD_NC(GPP_D2, UP_20K),
	PAD_NC(GPP_D3, UP_20K),
	PAD_NC(GPP_D4, UP_20K),
	PAD_CFG_NF(GPP_D5, NONE, DEEP, NF3), // CNVI_RST#
	PAD_CFG_NF(GPP_D6, NONE, DEEP, NF3),  // CNVI_CLKREQ
	PAD_NC(GPP_D7, UP_20K), // M.2_BT_PCMIN (test point)
	PAD_NC(GPP_D8, UP_20K), // M.2_BT_PCMCLK (test point)
	PAD_NC(GPP_D9, UP_20K),
	PAD_NC(GPP_D10, UP_20K),
	PAD_NC(GPP_D11, UP_20K),
	PAD_NC(GPP_D12, UP_20K),
	PAD_NC(GPP_D13, UP_20K),
	PAD_NC(GPP_D14, UP_20K),
	PAD_NC(GPP_D15, UP_20K),
	PAD_NC(GPP_D16, UP_20K),
	PAD_NC(GPP_D17, UP_20K),
	PAD_NC(GPP_D18, UP_20K),
	PAD_NC(GPP_D19, UP_20K),
	PAD_NC(GPP_D20, UP_20K),
	PAD_NC(GPP_D21, UP_20K),
	PAD_NC(GPP_D22, UP_20K),
	PAD_NC(GPP_D23, UP_20K),

	/* ------- GPIO Group GPP_E ------- */
	PAD_NC(GPP_E0, UP_20K),
	PAD_CFG_NF(GPP_E1, NONE, DEEP, NF1), // M.2_SSD1_PEDET
	PAD_NC(GPP_E2, UP_20K),
	PAD_CFG_GPI(GPP_E3, NONE, DEEP), // 10k pull up
	PAD_NC(GPP_E4, UP_20K),
	PAD_CFG_NF(GPP_E5, NONE, DEEP, NF1), // M2_P1_SATA_DEVSLP
	_PAD_CFG_STRUCT(GPP_E6, 0x82040100, 0x0000), // SMI#
	PAD_CFG_GPI_APIC(GPP_E7, NONE, PLTRST, EDGE_SINGLE, INVERT), // TP_ATTN#
	PAD_CFG_NF(GPP_E8, NONE, DEEP, NF1), // PCH_SATAHDD_LED#
	PAD_NC(GPP_E9, UP_20K), // USB_OC0# (test point)
	PAD_NC(GPP_E10, UP_20K), // USB_OC1# (test point)
	PAD_NC(GPP_E11, UP_20K), // USB_OC2# (test point)
	PAD_NC(GPP_E12, UP_20K), // USB_OC3# (test point)

	/* ------- GPIO Group GPP_F ------- */
	PAD_CFG_TERM_GPO(GPP_F0, 1, NONE, RSMRST), // TBT_PERST_N
	PAD_CFG_GPI(GPP_F1, NONE, DEEP), // M.2_SSD2_PEDET (board error)
	PAD_CFG_GPI(GPP_F2, NONE, DEEP), // TBTA_HRESET
	PAD_NC(GPP_F3, UP_20K),
	PAD_NC(GPP_F4, UP_20K),
	PAD_NC(GPP_F5, UP_20K),
	PAD_CFG_NF(GPP_F6, NONE, DEEP, NF1), // M2_P4_SATA_DEVSLP (board error)
	PAD_NC(GPP_F7, UP_20K),
	PAD_NC(GPP_F8, UP_20K),
	PAD_NC(GPP_F9, UP_20K),
	PAD_CFG_GPI(GPP_F10, UP_20K, DEEP), // BIOS_REC
	PAD_CFG_GPI(GPP_F11, UP_20K, DEEP), // PCH_RSVD
	PAD_NC(GPP_F12, UP_20K),
	PAD_CFG_GPI(GPP_F13, UP_20K, DEEP), // GP39_GFX_CRB_DETECT
	PAD_CFG_GPI(GPP_F14, UP_20K, DEEP), // 10k pull to H_SKTOCC_N
	PAD_NC(GPP_F15, UP_20K), // USB_OC4# (test point)
	PAD_NC(GPP_F16, UP_20K),
	PAD_NC(GPP_F17, UP_20K),
	PAD_NC(GPP_F18, UP_20K), // USB_OC7# (test point)
	PAD_CFG_NF(GPP_F19, NONE, DEEP, NF1), // NB_ENAVDD
	PAD_CFG_NF(GPP_F20, NONE, DEEP, NF1), // BLON
	PAD_CFG_NF(GPP_F21, NONE, DEEP, NF1), // EDP_BRIGHTNESS
	PAD_CFG_TERM_GPO(GPP_F22, 1, NONE, DEEP), // DGPU_RST#_PCH
	PAD_CFG_TERM_GPO(GPP_F23, 1, NONE, DEEP), // DGPU_PWR_EN

	/* ------- GPIO Group GPP_G ------- */
	PAD_CFG_GPI(GPP_G0, DN_20K, DEEP), // BOARD_ID1
	PAD_CFG_GPI(GPP_G1, DN_20K, DEEP), // BOARD_ID2
	PAD_CFG_GPI(GPP_G2, DN_20K, DEEP), // BOARD_ID3
	PAD_CFG_GPI(GPP_G3, DN_20K, DEEP), // BOARD_ID4
	PAD_CFG_GPI(GPP_G4, UP_20K, DEEP), // GPIO4_1V8_MAIN_EN_R
	PAD_NC(GPP_G5, UP_20K),
	PAD_NC(GPP_G6, UP_20K),
	PAD_NC(GPP_G7, UP_20K),

	/* ------- GPIO Group GPP_H ------- */
	PAD_CFG_NF(GPP_H0, NONE, DEEP, NF1), // WLAN_CLKREQ#
	PAD_CFG_NF(GPP_H1, NONE, DEEP, NF1), // CLK_REQ7_LAN#
	PAD_CFG_NF(GPP_H2, NONE, DEEP, NF1), // CLK_REQ8_PEG#
	PAD_CFG_NF(GPP_H3, NONE, DEEP, NF1), // CLK_REQ9_CARD#
	_PAD_CFG_STRUCT(GPP_H4, 0x40880100, 0x3000), // RTD3_PCIE_WAKE#
	PAD_CFG_NF(GPP_H5, NONE, DEEP, NF1), // CLK_REQ11_SSD2#
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1), // CLK_REQ12_SSD1#
	PAD_NC(GPP_H7, UP_20K),
	PAD_NC(GPP_H8, UP_20K),
	PAD_NC(GPP_H9, UP_20K),
	PAD_NC(GPP_H10, UP_20K),
	PAD_NC(GPP_H11, UP_20K),
	PAD_NC(GPP_H12, NONE), // eSPI flash sharing mode strap
	PAD_CFG_GPI(GPP_H13, NONE, DEEP), // 100k pull up
	PAD_NC(GPP_H14, UP_20K),
	PAD_CFG_GPI(GPP_H15, NONE, DEEP), // 20k pull up
	PAD_CFG_TERM_GPO(GPP_H16, 1, NONE, RSMRST), // TBT_RTD3_PWR_EN_R
	PAD_CFG_TERM_GPO(GPP_H17, 1, NONE, PLTRST), // TBT_FORCE_PWR_R
	PAD_NC(GPP_H18, UP_20K),
	PAD_CFG_TERM_GPO(GPP_H19, 1, NONE, DEEP), // GPIO_CARD_AUX
	PAD_CFG_TERM_GPO(GPP_H20, 1, NONE, DEEP), // GPIO_CARD
	PAD_CFG_GPI(GPP_H21, UP_20K, DEEP), // 20k pull down, 4.7k pull up
	PAD_NC(GPP_H22, UP_20K),
	PAD_CFG_GPI(GPP_H23, NONE, DEEP), // DGPU_SELECT#

	/* ------- GPIO Group GPP_I ------- */
	PAD_NC(GPP_I0, UP_20K),
	_PAD_CFG_STRUCT(GPP_I1, 0x46880100, 0x0000), // HDMI_HPD
	_PAD_CFG_STRUCT(GPP_I2, 0x46880100, 0x0000), // G_DP_DHPD_E
	_PAD_CFG_STRUCT(GPP_I3, 0x46880100, 0x0000), // OUT2_HPD
	PAD_CFG_NF(GPP_I4, NONE, DEEP, NF1), // EDP_HPD
	PAD_NC(GPP_I5, UP_20K),
	PAD_NC(GPP_I6, UP_20K),
	PAD_NC(GPP_I7, UP_20K),
	PAD_NC(GPP_I8, UP_20K),
	PAD_NC(GPP_I9, UP_20K),
	_PAD_CFG_STRUCT(GPP_I10, 0x82880100, 0x0000), // TBCIO_PLUG_EVENT
	PAD_CFG_GPI(GPP_I11, UP_20K, DEEP), // 10k pull to H_SKTOCC_N, 10k pull up
	PAD_CFG_GPI(GPP_I12, DN_20K, DEEP), // D02C_BOARD_ID (10k pull up)
	PAD_NC(GPP_I13, UP_20K),
	PAD_NC(GPP_I14, UP_20K),

	/* ------- GPIO Group GPP_J ------- */
	PAD_CFG_GPI(GPP_J0, NONE, DEEP), // CNVI_GNSS_PA_BLANKING
	PAD_CFG_NF(GPP_J1, NONE, DEEP, NF1), // CPU_VCCIO_PWR_GATE
	PAD_CFG_GPI(GPP_J2, UP_20K, DEEP), // 100k pull down
	PAD_NC(GPP_J3, UP_20K),
	PAD_CFG_NF(GPP_J4, NONE, DEEP, NF1), // M.2_CNV_BRI_DT_BT_UART0_RTS
	PAD_CFG_NF(GPP_J5, UP_20K, DEEP, NF1), // M.2_CNV_BRI_RSP
	PAD_CFG_NF(GPP_J6, NONE, DEEP, NF1), // M.2_CNV_RGI_DT_BT_UART0_TX
	PAD_CFG_NF(GPP_J7, UP_20K, DEEP, NF1), // M.2_CNV_RGI_RSP
	PAD_NC(GPP_J8, UP_20K),
	PAD_CFG_GPI(GPP_J9, UP_20K, DEEP), // 100k pull up, 100k pull down
	PAD_CFG_GPI(GPP_J10, UP_20K, DEEP), // 100k pull down
	PAD_CFG_GPI(GPP_J11, UP_20K, DEEP), // 75k pull down

	/* ------- GPIO Group GPP_K ------- */
	PAD_NC(GPP_K0, UP_20K),
	PAD_NC(GPP_K1, UP_20K),
	PAD_NC(GPP_K2, UP_20K),
	_PAD_CFG_STRUCT(GPP_K3, 0x80880100, 0x3000), // SCI#
	PAD_NC(GPP_K4, UP_20K),
	PAD_NC(GPP_K5, UP_20K),
	_PAD_CFG_STRUCT(GPP_K6, 0x40880100, 0x0000), // SWI#
	PAD_NC(GPP_K7, UP_20K),
	PAD_CFG_TERM_GPO(GPP_K8, 1, NONE, RSMRST), // SATA_M2_PWR_EN1
	PAD_CFG_TERM_GPO(GPP_K9, 1, NONE, RSMRST), // SATA_M2_PWR_EN2
	PAD_CFG_TERM_GPO(GPP_K10, 1, NONE, DEEP), // LANRTD3_WAKE#
	PAD_CFG_TERM_GPO(GPP_K11, 1, NONE, RSMRST), // GPIO_LANRTD3
	PAD_NC(GPP_K12, UP_20K),
	PAD_NC(GPP_K13, UP_20K),
	PAD_CFG_GPI(GPP_K14, UP_20K, DEEP), // GPP_K_14_GSXDIN (test point)
	PAD_NC(GPP_K15, UP_20K),
	PAD_NC(GPP_K16, UP_20K),
	PAD_NC(GPP_K17, UP_20K),
	PAD_NC(GPP_K18, UP_20K),
	PAD_CFG_GPI(GPP_K19, UP_20K, DEEP), // SMI#
	PAD_CFG_GPI(GPP_K20, NONE, DEEP), // GPU_EVENT#
	PAD_CFG_GPI(GPP_K21, NONE, DEEP), // GC6_FB_EN_PCH
	PAD_CFG_TERM_GPO(GPP_K22, 0, NONE, DEEP), // OVRM
	PAD_CFG_GPI(GPP_K23, NONE, DEEP), // DGPU_PWRGD_R
};

#endif

#endif
