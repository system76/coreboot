/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_GPIO_H
#define MAINBOARD_GPIO_H

#include <soc/gpe.h>
#include <soc/gpio.h>

#ifndef __ACPI__

static const struct pad_config early_gpio_table[] = {
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1), // UART2_RXD
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1), // UART2_TXD
};

static const struct pad_config gpio_table[] = {
	/* ------- GPIO Group GPD ------- */
	PAD_NC(GPD0, NONE), // PM_BATLOW#
	PAD_CFG_NF(GPD1, NATIVE, DEEP, NF1), // AC_PRESENT
	PAD_CFG_GPI(GPD2, NATIVE, PWROK), // LAN_WAKEUP#
	PAD_CFG_NF(GPD3, UP_20K, DEEP, NF1), // PWR_BTN#
	PAD_CFG_NF(GPD4, NONE, DEEP, NF1), // SUSB#_PCH
	PAD_CFG_NF(GPD5, NONE, DEEP, NF1), // SUSC#_PCH
	PAD_NC(GPD6, NONE), // SLP_A#
	PAD_CFG_GPI(GPD7, NONE, PWROK), // GPD_7 (crystal input)
	PAD_CFG_NF(GPD8, NONE, DEEP, NF1), // SUS_CLK
	PAD_NC(GPD9, NONE), // PCH_SLP_WLAN#
	PAD_NC(GPD10, NONE),
	PAD_NC(GPD11, NONE), // LAN_DISABLE_N

	/* ------- GPIO Group GPP_A ------- */
	PAD_CFG_NF(GPP_A0, NONE, DEEP, NF1), // SB_KBCRST#
	PAD_CFG_NF(GPP_A1, NONE, DEEP, NF1), // LPC_AD0
	PAD_CFG_NF(GPP_A2, NONE, DEEP, NF1), // LPC_AD1
	PAD_CFG_NF(GPP_A3, NONE, DEEP, NF1), // LPC_AD2
	PAD_CFG_NF(GPP_A4, NONE, DEEP, NF1), // LPC_AD3
	PAD_CFG_NF(GPP_A5, NONE, DEEP, NF1), // LPC_FRAME#
	PAD_CFG_NF(GPP_A6, NONE, DEEP, NF1), // SERIRQ
	PAD_CFG_GPI_APIC_HIGH(GPP_A7, NONE, PLTRST), // INTP_OUT
	PAD_NC(GPP_A8, NONE), // PM_CLKRUN#
	PAD_CFG_NF(GPP_A9, NONE, DEEP, NF1), // PCLK_KBC
	PAD_NC(GPP_A10, NONE),
	PAD_CFG_GPI(GPP_A11, NONE, DEEP), // LAN_WAKEUP#
	PAD_NC(GPP_A12, NONE), // ISH_GP_6_R
	PAD_CFG_NF(GPP_A13, NONE, DEEP, NF1), // SUSWARN#
	PAD_NC(GPP_A14, NONE), // S4_STATE#
	PAD_CFG_NF(GPP_A15, NONE, DEEP, NF1), // SUS_PWR_ACK#
	PAD_NC(GPP_A16, NONE),
	PAD_NC(GPP_A17, NONE),
	PAD_CFG_GPO(GPP_A18, 1, DEEP), // SB_BLON
	PAD_NC(GPP_A19, NONE),
	PAD_NC(GPP_A20, NONE),
	PAD_NC(GPP_A21, NONE), // 3G_CONFIG2
	PAD_CFG_GPO(GPP_A22, 0, DEEP), // SATA_PWR_EN
	PAD_NC(GPP_A23, NONE), // DGPU_PWM_SELECT#

	/* ------- GPIO Group GPP_B ------- */
	PAD_CFG_GPI(GPP_B0, NONE, DEEP), // TPM_PIRQ#
	PAD_NC(GPP_B1, NONE),
	PAD_NC(GPP_B2, NONE), // VRALERTB#
	PAD_NC(GPP_B3, NONE),
	PAD_NC(GPP_B4, NONE), // EXTTS_SNI_DRV1
	PAD_NC(GPP_B5, NONE),
	PAD_NC(GPP_B6, NONE),
	PAD_NC(GPP_B7, NONE),
	PAD_NC(GPP_B8, NONE),
	PAD_NC(GPP_B9, NONE),
	PAD_CFG_NF(GPP_B10, NONE, DEEP, NF1), // LAN_CLKREQ#
	PAD_NC(GPP_B11, NONE),
	_PAD_CFG_STRUCT(GPP_B12, 0x44000601, 0x0000), // SLP_S0#
	_PAD_CFG_STRUCT(GPP_B13, 0x44000601, 0x1000), // PLT_RST#
	PAD_CFG_NF(GPP_B14, NONE, DEEP, NF1), // PCH_SPKR
	PAD_NC(GPP_B15, NONE),
	PAD_NC(GPP_B16, NONE),
	PAD_NC(GPP_B17, NONE),
	PAD_CFG_NF(GPP_B18, NONE, DEEP, NF1), // NO REBOOT strap
	PAD_NC(GPP_B19, NONE),
	PAD_NC(GPP_B20, NONE),
	PAD_NC(GPP_B21, NONE),
	PAD_CFG_NF(GPP_B22, NONE, DEEP, NF1), // LPC/SPI BOOT strap
	PAD_CFG_GPI(GPP_B23, NONE, DEEP), // DCI BSSB mode strap

	/* ------- GPIO Group GPP_C ------- */
	PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1), // SMB_CLK
	PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1), // SMB_DATA
	PAD_CFG_GPI(GPP_C2, NONE, DEEP), // SKIN_THRM_SNSR_ALERT_N
	PAD_NC(GPP_C3, NONE), // SML0_CLK
	PAD_NC(GPP_C4, NONE), // SML0_DATA
	PAD_NC(GPP_C5, NONE),
	PAD_NC(GPP_C6, NONE), // SMC_CPU_THERM
	PAD_NC(GPP_C7, NONE), // SMD_CPU_THERM
	PAD_NC(GPP_C8, NONE),
	PAD_NC(GPP_C9, NONE),
	PAD_NC(GPP_C10, NONE),
	PAD_NC(GPP_C11, NONE),
	PAD_NC(GPP_C12, NONE),
	PAD_NC(GPP_C13, NONE),
	PAD_NC(GPP_C14, NONE),
	PAD_NC(GPP_C15, NONE),
	PAD_CFG_NF(GPP_C16, NONE, PLTRST, NF1), // I2C_SDA_TP
	PAD_CFG_NF(GPP_C17, NONE, PLTRST, NF1), // I2C_SCL_TP
	PAD_CFG_NF(GPP_C18, NONE, PLTRST, NF1), // SMD_7411
	PAD_CFG_NF(GPP_C19, NONE, PLTRST, NF1), // SMC_7411
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1), // UART2_RXD
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1), // UART2_TXD
	PAD_NC(GPP_C22, NONE),
	PAD_NC(GPP_C23, NONE),

	/* ------- GPIO Group GPP_D ------- */
	PAD_NC(GPP_D0, NONE),
	PAD_NC(GPP_D1, NONE),
	PAD_NC(GPP_D2, NONE),
	PAD_NC(GPP_D3, NONE),
	PAD_NC(GPP_D4, NONE),
	PAD_CFG_NF(GPP_D5, NONE, DEEP, NF3), // CNVI_RF_RST#
	PAD_CFG_NF(GPP_D6, NONE, DEEP, NF3), // XTAL_CLKREQ
	PAD_NC(GPP_D7, NONE),
	PAD_NC(GPP_D8, NONE),
	PAD_NC(GPP_D9, NONE),
	PAD_NC(GPP_D10, NONE),
	PAD_NC(GPP_D11, NONE),
	PAD_NC(GPP_D12, NONE),
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
	PAD_NC(GPP_E0, NONE), // SATAGP0
	PAD_CFG_NF(GPP_E1, UP_20K, DEEP, NF1), // SATAGP1
	PAD_NC(GPP_E2, NONE), // SATAGP2
	PAD_NC(GPP_E3, NONE), // EXTTS_SNI_DRV0
	PAD_CFG_GPI(GPP_E4, NONE, DEEP), // DEVSLP0
	PAD_CFG_GPI(GPP_E5, NONE, DEEP), // DEVSLP1
	PAD_NC(GPP_E6, NONE), // PCH_MUTE#
	_PAD_CFG_STRUCT(GPP_E7, 0x80800100, 0x0000), // TP_ATTN#
	PAD_CFG_NF(GPP_E8, NONE, DEEP, NF1), // SATA_LED#
	PAD_NC(GPP_E9, NONE),
	PAD_NC(GPP_E10, NONE),
	PAD_NC(GPP_E11, NONE),
	PAD_NC(GPP_E12, NONE),

	/* ------- GPIO Group GPP_F ------- */
	PAD_NC(GPP_F0, NONE), // SATAGP3
	PAD_NC(GPP_F1, NONE),
	PAD_NC(GPP_F2, NONE),
	PAD_NC(GPP_F3, NONE),
	PAD_NC(GPP_F4, NONE),
	PAD_CFG_GPI(GPP_F5, NONE, DEEP), // KBLED_DET
	PAD_CFG_GPI(GPP_F6, NONE, DEEP), // LIGHT_KB_DET#
	PAD_NC(GPP_F7, NONE),
	PAD_NC(GPP_F8, NONE),
	PAD_NC(GPP_F9, NONE),
	PAD_NC(GPP_F10, NONE), // BIOS_REC - BIOS RECOVERY ENABLE strap
	PAD_NC(GPP_F11, NONE), // PCH_RSVD - unused strap
	PAD_NC(GPP_F12, NONE), // MFG_MODE - Manufacturing mode strap
	PAD_NC(GPP_F13, NONE), // GP39_GFX_CRB_DETECT - 0 = normal gfx, 1 = customer gfx
	PAD_CFG_GPI(GPP_F14, NONE, DEEP), // H_SKTOCC_N
	PAD_NC(GPP_F15, NONE),
	PAD_NC(GPP_F16, NONE),
	PAD_NC(GPP_F17, NONE),
	PAD_NC(GPP_F18, NONE),
	PAD_CFG_NF(GPP_F19, NONE, DEEP, NF1), // NB_ENAVDD
	PAD_CFG_NF(GPP_F20, NONE, DEEP, NF1), // BLON
	PAD_CFG_NF(GPP_F21, NONE, DEEP, NF1), // EDP_BRIGHTNESS
	PAD_CFG_TERM_GPO(GPP_F22, 1, NONE, DEEP), // DGPU_RST#_PCH
	PAD_CFG_TERM_GPO(GPP_F23, 1, NONE, DEEP), // DGPU_PWR_EN

	/* ------- GPIO Group GPP_G ------- */
	PAD_CFG_GPI(GPP_G0, NONE, DEEP), // BOARD_ID1
	PAD_CFG_GPI(GPP_G1, NONE, DEEP), // BOARD_ID2
	PAD_CFG_GPI(GPP_G2, NONE, DEEP), // TPM_DET
	PAD_CFG_GPI(GPP_G3, UP_20K, DEEP), // TODO: GPIO4_1V8_MAIN_EN_R
	PAD_NC(GPP_G4, NONE),
	PAD_NC(GPP_G5, NONE),
	PAD_NC(GPP_G6, NONE),
	PAD_NC(GPP_G7, NONE),

	/* ------- GPIO Group GPP_H ------- */
	PAD_CFG_NF(GPP_H0, NONE, DEEP, NF1), // WLAN_CLKREQ#
	PAD_NC(GPP_H1, NONE),
	PAD_CFG_NF(GPP_H2, NONE, DEEP, NF1), // PEG_CLKREQ
	PAD_NC(GPP_H3, NONE),
	PAD_CFG_NF(GPP_H4, NONE, DEEP, NF1), // SSD_CLKREQ#
	PAD_CFG_NF(GPP_H5, NONE, DEEP, NF1), // SSD2_CLKREQ#
	PAD_NC(GPP_H6, NONE),
	PAD_NC(GPP_H7, NONE),
	PAD_NC(GPP_H8, NONE),
	PAD_NC(GPP_H9, NONE),
	PAD_NC(GPP_H10, NONE),
	PAD_NC(GPP_H11, NONE),
	PAD_CFG_GPI(GPP_H12, NONE, DEEP), // ESPI FLASH SHARING MODE strap
	PAD_NC(GPP_H13, NONE),
	PAD_NC(GPP_H14, NONE),
	PAD_NC(GPP_H15, NONE),
	PAD_NC(GPP_H16, NONE),
	PAD_NC(GPP_H17, NONE),
	PAD_NC(GPP_H18, NONE),
	PAD_NC(GPP_H19, NONE),
	PAD_NC(GPP_H20, NONE),
	PAD_NC(GPP_H21, NONE),
	PAD_NC(GPP_H22, NONE),
	PAD_NC(GPP_H23, NONE), // DGPU_SELECT#

	/* ------- GPIO Group GPP_I ------- */
	PAD_CFG_NF(GPP_I0, NONE, DEEP, NF1), // I_MDP_HPD
	_PAD_CFG_STRUCT(GPP_I1, 0x46880100, 0x0000), // HDMI_HPD
	PAD_CFG_NF(GPP_I2, NONE, DEEP, NF1),
	_PAD_CFG_STRUCT(GPP_I3, 0x46880100, 0x0000), // MDP_E_HPD
	PAD_CFG_NF(GPP_I4, NONE, DEEP, NF1), // EDP_HPD
	PAD_CFG_NF(GPP_I5, NONE, DEEP, NF1), // I_MDP_CLK
	PAD_CFG_NF(GPP_I6, NONE, DEEP, NF1), // I_MDP_DATA
	PAD_NC(GPP_I7, NONE),
	PAD_NC(GPP_I8, NONE),
	PAD_NC(GPP_I9, NONE),
	PAD_NC(GPP_I10, NONE),
	PAD_CFG_GPI(GPP_I11, NONE, DEEP), // H_SKTOCC_N
	PAD_NC(GPP_I12, NONE),
	PAD_NC(GPP_I13, NONE),
	PAD_NC(GPP_I14, NONE),

	/* ------- GPIO Group GPP_J ------- */
	PAD_CFG_NF(GPP_J0, NONE, DEEP, NF1), // CNVI_GNSS_PA_BLANKING
	PAD_NC(GPP_J1, NONE),
	PAD_NC(GPP_J2, NONE),
	PAD_NC(GPP_J3, NONE),
	PAD_CFG_NF(GPP_J4, NONE, DEEP, NF1), // CNVI_BRI_DT
	PAD_CFG_NF(GPP_J5, NONE, DEEP, NF1), // CNVI_BRI_RSP
	PAD_CFG_NF(GPP_J6, NONE, DEEP, NF1), // CNVI_RGI_DT
	PAD_CFG_NF(GPP_J7, NONE, DEEP, NF1), // CNVI_RGI_RSP
	PAD_CFG_NF(GPP_J8, NONE, DEEP, NF1), // CNVI_MFUART2_RXD
	PAD_CFG_NF(GPP_J9, NONE, DEEP, NF1), // CNVI_MFUART2_TXD
	PAD_NC(GPP_J10, NONE),
	PAD_NC(GPP_J11, NONE),

	/* ------- GPIO Group GPP_K ------- */
	PAD_NC(GPP_K0, NONE),
	PAD_NC(GPP_K1, NONE),
	PAD_NC(GPP_K2, NONE),
	_PAD_CFG_STRUCT(GPP_K3, 0x40880100, 0x0000), // SCI#
	PAD_NC(GPP_K4, NONE),
	PAD_NC(GPP_K5, NONE),
	_PAD_CFG_STRUCT(GPP_K6, 0x40880100, 0x0000), // SWI#
	PAD_NC(GPP_K7, NONE),
	PAD_CFG_GPO(GPP_K8, 1, DEEP), // SATA_M2_PWR_EN1
	PAD_CFG_GPO(GPP_K9, 1, DEEP), // SATA_M2_PWR_EN2
	PAD_NC(GPP_K10, NONE),
	PAD_NC(GPP_K11, NONE),
	PAD_NC(GPP_K12, NONE),
	PAD_NC(GPP_K13, NONE),
	PAD_NC(GPP_K14, NONE), // GPP_K14_TEST_R
	PAD_NC(GPP_K15, NONE),
	PAD_NC(GPP_K16, NONE),
	PAD_NC(GPP_K17, NONE),
	PAD_NC(GPP_K18, NONE),
	_PAD_CFG_STRUCT(GPP_K19, 0x42800101, 0x0000), // SMI#
	PAD_CFG_GPI(GPP_K20, NONE, DEEP), // GPU_EVENT#
	PAD_CFG_GPI(GPP_K21, NONE, DEEP), // GC6_FB_EN_PCH
	PAD_CFG_GPI(GPP_K22, NONE, DEEP), // DGPU_PWRGD_R
	PAD_NC(GPP_K23, NONE), // DGPU_PRSNT#
};

#endif

#endif
