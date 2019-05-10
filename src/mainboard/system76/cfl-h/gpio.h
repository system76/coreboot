/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 System76
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 */

#ifndef MAINBOARD_GPIO_H
#define MAINBOARD_GPIO_H

#include <soc/gpe.h>
#include <soc/gpio.h>

#ifndef __ACPI__

/* Pad configuration in ramstage. */
static const struct pad_config gpio_table[] = {
// GPD
	// Power Management
		// NC
		PAD_NC(GPD0, NONE),
		// AC_PRESENT
		PAD_CFG_NF(GPD1, NATIVE, DEEP, NF1),
		// NC
		PAD_NC(GPD2, NONE),
		// PWR_BTN#
		PAD_CFG_NF(GPD3, UP_20K, DEEP, NF1),
		// SUSB#_PCH
		PAD_CFG_NF(GPD4, NONE, DEEP, NF1),
		// SUSC#_PCH
		PAD_CFG_NF(GPD5, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPD6, NONE),

	// GPIO
		// NC
		PAD_NC(GPD7, NONE),

	// Power Management
		// SUS_CLK_R
		PAD_CFG_NF(GPD8, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPD9, NONE),
		// NC
		PAD_NC(GPD10, NONE),
		// NC
		PAD_NC(GPD11, NONE),

// GPP_A
	// LPC
		// SB_KBCRST#
		PAD_CFG_NF(GPP_A0, NONE, DEEP, NF1),
		// LPC_AD0
		PAD_CFG_NF(GPP_A1, NONE, DEEP, NF1),
		// LPC_AD1
		PAD_CFG_NF(GPP_A2, NONE, DEEP, NF1),
		// LPC_AD2
		PAD_CFG_NF(GPP_A3, NONE, DEEP, NF1),
		// LPC_AD3
		PAD_CFG_NF(GPP_A4, NONE, DEEP, NF1),
		// LPC_FRAME#
		PAD_CFG_NF(GPP_A5, NONE, DEEP, NF1),
		// SERIRQ
		PAD_CFG_NF(GPP_A6, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPP_A7, NONE),
		// PM_CLKRUN#
		PAD_CFG_NF(GPP_A8, NONE, DEEP, NF1),
		// PCLK_KBC
		PAD_CFG_NF(GPP_A9, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPP_A10, NONE),

	// Power Management
		// TODO: LAN_WAKEUP#
		PAD_CFG_GPI(GPP_A11, NONE, DEEP),
		// NC
		PAD_NC(GPP_A12, NONE),
		// SUSWARN#
		PAD_CFG_NF(GPP_A13, DN_20K, DEEP, NF1),

	// LPC
		// NC
		PAD_NC(GPP_A14, NONE),

	// Power Management
		// SUS_PWR_ACK
		PAD_CFG_GPI(GPP_A15, NONE, DEEP),

	// Clock Signals
		// NC
		PAD_NC(GPP_A16, NONE),

	// ISH
		// NC
		PAD_NC(GPP_A17, NONE),
		// SB_BLON
		PAD_CFG_TERM_GPO(GPP_A18, 1, NONE, DEEP),
		// NC
		PAD_NC(GPP_A19, NONE),
		// NC
		PAD_NC(GPP_A20, NONE),
		// NC
		PAD_NC(GPP_A21, NONE),
		// SATA_PWR_EN
		PAD_CFG_TERM_GPO(GPP_A22, 0, NONE, DEEP),
		// NC
		PAD_NC(GPP_A23, NONE),

// GPP_B
	// GSPI
		// TODO: TPM_PIRQ#
		PAD_NC(GPP_B0, NONE),
		// NC
		PAD_NC(GPP_B1, NONE),

	// Power Management
		// NC
		PAD_NC(GPP_B2, NONE),

	// CPU Misc
		// NC
		PAD_NC(GPP_B3, NONE),
		// TODO: EXTTS_SNI_DRV1
		PAD_NC(GPP_B4, NONE),

	// Clock Signals
		// NC
		PAD_NC(GPP_B5, NONE),
		// NC
		PAD_NC(GPP_B6, NONE),
		// NC
		PAD_NC(GPP_B7, NONE),
		// NC
		PAD_NC(GPP_B8, NONE),
		// NC
		PAD_NC(GPP_B9, NONE),
		// LAN_CLKREQ#
		PAD_CFG_NF(GPP_B10, NONE, DEEP, NF1),

	// Audio
		// TODO: GPP_B11: DDR Voltage select - 0 = 1.2V, 1 = 1.35V
		PAD_CFG_GPO(GPP_B11, 0, DEEP),

	// Power Management
		// SLP_S0#
		PAD_CFG_NF(GPP_B12, UP_20K, DEEP, NF1),
		// PLT_RST#
		PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),

	// Audio
		// PCH_SPKR
		PAD_CFG_NF(GPP_B14, NONE, DEEP, NF1),

	// GSPI
		// NC
		PAD_NC(GPP_B15, NONE),
		// NC
		PAD_NC(GPP_B16, NONE),
		// NC
		PAD_NC(GPP_B17, NONE),
		// LPSS_GSPI0_MOSI - strap for no reboot mode
		PAD_NC(GPP_B18, NONE),
		// NC
		PAD_NC(GPP_B19, NONE),
		// NC
		PAD_NC(GPP_B20, NONE),
		// NC
		PAD_NC(GPP_B21, NONE),
		// LPSS_GSPI1_MOSI - strap for booting from SPI or LPC
		PAD_NC(GPP_B22, NONE),

	// SMBUS
		// PCH_HOT_GNSS_DISABLE - strap for DCI BSSB mode
		PAD_NC(GPP_B23, NONE),

// GPP_C
	// SMBUS
		// SMB_CLK
		PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),
		// SMB_DATA
		PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPP_C2, NONE),
		// NC
		PAD_NC(GPP_C3, NONE),
		// NC
		PAD_NC(GPP_C4, NONE),
		// NC
		PAD_NC(GPP_C5, NONE),
		// NC
		PAD_NC(GPP_C6, NONE),
		// NC
		PAD_NC(GPP_C7, NONE),

	// UART
		// NC
		PAD_NC(GPP_C8, NONE),
		// TODO: CNVI_DET#
		PAD_NC(GPP_C9, NONE),
		// NC
		PAD_NC(GPP_C10, NONE),
		// NC
		PAD_NC(GPP_C11, NONE),
		// NC
		PAD_NC(GPP_C12, NONE),
		// NC
		PAD_NC(GPP_C13, NONE),
		// NC
		PAD_NC(GPP_C14, NONE),
		// NC
		PAD_NC(GPP_C15, NONE),

	// I2C
		// I2C_SCL_TP
		PAD_CFG_NF(GPP_C16, NONE, PLTRST, NF1),
		// I2C_SDA_TP
		PAD_CFG_NF(GPP_C17, NONE, PLTRST, NF1),
		// NC
		PAD_NC(GPP_C18, NONE),
		// NC
		PAD_NC(GPP_C19, NONE),

	// UART
		// UART2_RXD
		PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
		// UART2_TXD
		PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPP_C22, NONE),
		// NC
		PAD_NC(GPP_C23, NONE),

// GPP_D
	// SPI
		// NC
		PAD_NC(GPP_D0, NONE),
		// NC
		PAD_NC(GPP_D1, NONE),
		// NC
		PAD_NC(GPP_D2, NONE),
		// NC
		PAD_NC(GPP_D3, NONE),

	// I2C
		// NC
		PAD_NC(GPP_D4, NONE),

	// CNVI
		// CNVI_RF_RST#
		PAD_CFG_NF(GPP_D5, NONE, DEEP, NF3),
		// XTAL_CLKREQ
		PAD_CFG_NF(GPP_D6, NONE, DEEP, NF3),
		// NC
		PAD_NC(GPP_D7, NONE),
		// NC
		PAD_NC(GPP_D8, NONE),

	// ISH
		// NC
		PAD_NC(GPP_D9, NONE),
		// NC
		PAD_NC(GPP_D10, NONE),
		// NC
		PAD_NC(GPP_D11, NONE),
		// NC
		PAD_NC(GPP_D12, NONE),
		// NC
		PAD_NC(GPP_D13, NONE),
		// NC
		PAD_NC(GPP_D14, NONE),
		// NC
		PAD_NC(GPP_D15, NONE),
		// NC
		PAD_NC(GPP_D16, NONE),

	// DMIC
		// NC
		PAD_NC(GPP_D17, NONE),
		// NC
		PAD_NC(GPP_D18, NONE),
		// NC
		PAD_NC(GPP_D19, NONE),
		// NC
		PAD_NC(GPP_D20, NONE),

	// SPI
		// NC
		PAD_NC(GPP_D21, NONE),
		// NC
		PAD_NC(GPP_D22, NONE),

	// ISH
		// NC
		PAD_NC(GPP_D23, NONE),

// GPP_E
	// SATA
		// NC
		PAD_NC(GPP_E0, NONE),
		// SATAGP1
		PAD_CFG_NF(GPP_E1, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPP_E2, NONE),

	// CPU Misc
		// TODO: EXTTS_SNI_DRV0
		PAD_NC(GPP_E3, NONE),

	// SATA
		// DEVSLP0
		PAD_CFG_NF(GPP_E4, UP_20K, DEEP, NF1),
		// DEVSLP1
		PAD_CFG_NF(GPP_E5, UP_20K, DEEP, NF1),
		// NC
		PAD_NC(GPP_E6, NONE),

	// CPU Misc
		// TODO: TP_ATTN#
		PAD_NC(GPP_E7, NONE),

	// SATA
		// SATA_LED#
		PAD_CFG_NF(GPP_E8, NONE, DEEP, NF1),

	// USB2
		// NC
		PAD_NC(GPP_E9, NONE),
		// NC
		PAD_NC(GPP_E10, NONE),
		// NC
		PAD_NC(GPP_E11, NONE),
		// NC
		PAD_NC(GPP_E12, NONE),

// GPP_F
	// SATA
		// NC
		PAD_NC(GPP_F0, NONE),
		// NC
		PAD_NC(GPP_F1, NONE),
		// NC
		PAD_NC(GPP_F2, NONE),
		// NC
		PAD_NC(GPP_F3, NONE),
		// NC
		PAD_NC(GPP_F4, NONE),
		// KBLED_DET
		PAD_CFG_GPI(GPP_F5, NONE, DEEP),
		// LIGHT_KB_DET#
		PAD_CFG_GPI(GPP_F6, NONE, DEEP),
		// NC
		PAD_NC(GPP_F7, NONE),
		// NC
		PAD_NC(GPP_F8, NONE),
		// NC
		PAD_NC(GPP_F9, NONE),
		// BIOS_REC - strap for bios recovery enable
		PAD_NC(GPP_F10, NONE),
		// PCH_RSVD - unused strap
		PAD_NC(GPP_F11, NONE),
		// MFG_MODE - strap for manufacturing mode
		PAD_NC(GPP_F12, NONE),
		// TODO: GP39_GFX_CRB_DETECT - 0 = normal gfx, 1 = customer gfx
		PAD_NC(GPP_F13, NONE),

	// Power Management
		// H_SKTOCC_N
		PAD_CFG_GPI(GPP_F14, UP_20K, DEEP),

	// USB2
		// NC
		PAD_NC(GPP_F15, NONE),
		// NC
		PAD_NC(GPP_F16, NONE),
		// NC
		PAD_NC(GPP_F17, NONE),
		// NC
		PAD_NC(GPP_F18, NONE),

	// Display Signals
		// NB_ENAVDD
		PAD_CFG_NF(GPP_F19, NONE, DEEP, NF1),
		// BLON
		PAD_CFG_NF(GPP_F20, NONE, DEEP, NF1),
		// EDP_BRIGHTNESS
		PAD_CFG_NF(GPP_F21, NONE, DEEP, NF1),
		// TODO: DGPU_RST#_PCH
		PAD_CFG_TERM_GPO(GPP_F22, 0, NONE, PLTRST),
		// TODO: DGPU_PWR_EN
		PAD_CFG_TERM_GPO(GPP_F23, 0, NONE, DEEP),

// GPP_G
	// SD
		// BOARD_ID1
		PAD_CFG_GPI(GPP_G0, NONE, DEEP),
		// BOARD_ID2
		PAD_CFG_GPI(GPP_G1, NONE, DEEP),
		// TPM_DET
		PAD_CFG_GPI(GPP_G2, NONE, DEEP),
		// TODO: GPIO4_1V8_MAIN_EN_R
		PAD_CFG_GPI(GPP_G3, UP_20K, DEEP),
		// NC
		PAD_NC(GPP_G4, NONE),
		// NC
		PAD_NC(GPP_G5, NONE),
		// NC
		PAD_NC(GPP_G6, NONE),
		// NC
		PAD_NC(GPP_G7, NONE),

// GPP_H
	// Clock Signals
		// WLAN_CLKREQ#
		PAD_CFG_NF(GPP_H0, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPP_H1, NONE),
		// PEG_CLKREQ#
		PAD_CFG_NF(GPP_H2, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPP_H3, NONE),
		// SSD_CLKREQ#
		PAD_CFG_NF(GPP_H4, NONE, DEEP, NF1),
		// SSD2_CLKREQ#
		PAD_CFG_NF(GPP_H5, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPP_H6, NONE),
		// NC
		PAD_NC(GPP_H7, NONE),
		// NC
		PAD_NC(GPP_H8, NONE),
		// NC
		PAD_NC(GPP_H9, NONE),

	// SMBUS
		// NC
		PAD_NC(GPP_H10, NONE),
		// NC
		PAD_NC(GPP_H11, NONE),
		// GPP_H_12 - strap for ESPI flash sharing mode
		PAD_NC(GPP_H12, NONE),
		// NC
		PAD_NC(GPP_H13, NONE),
		// NC
		PAD_NC(GPP_H14, NONE),
		// NC
		PAD_NC(GPP_H15, NONE),
		// NC
		PAD_NC(GPP_H16, NONE),
		// NC
		PAD_NC(GPP_H17, NONE),
		// NC
		PAD_NC(GPP_H18, NONE),

	// ISH
		// NC
		PAD_NC(GPP_H19, NONE),
		// NC
		PAD_NC(GPP_H20, NONE),
		// NC
		PAD_NC(GPP_H21, NONE),
		// NC
		PAD_NC(GPP_H22, NONE),

	// GPIO
		// NC
		PAD_NC(GPP_H23, NONE),

// GPP_I
	// Display Signals
		// NC
		PAD_NC(GPP_I0, NONE),
		// HDMI_HPD
		PAD_CFG_NF(GPP_I1, NATIVE, DEEP, NF1),
		// NC
		PAD_NC(GPP_I2, NONE),
		// MDP_E_HPD
		PAD_CFG_NF(GPP_I3, NONE, DEEP, NF1),
		// EDP_HPD
		PAD_CFG_NF(GPP_I4, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPP_I5, NONE),
		// NC
		PAD_NC(GPP_I6, NONE),
		// HDMI_CTRLCLK
		PAD_CFG_NF(GPP_I7, NONE, DEEP, NF1),
		// HDMI_CTRLDATA
		PAD_CFG_NF(GPP_I8, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPP_I9, NONE),
		// NC
		PAD_NC(GPP_I10, NONE),

	// PCIE
		// TODO: H_SKTOCC_N
		PAD_NC(GPP_I11, NONE),
		// NC
		PAD_NC(GPP_I12, NONE),
		// NC
		PAD_NC(GPP_I13, NONE),
		// NC
		PAD_NC(GPP_I14, NONE),

// GPP_J
	// CNVI
		// CNVI_GNSS_PA_BLANKING
		PAD_CFG_NF(GPP_J0, NATIVE, DEEP, NF1),

	// Power Management
		// NC
		PAD_NC(GPP_J1, NONE),

	// GPIO
		// NC
		PAD_NC(GPP_J2, NONE),
		// NC
		PAD_NC(GPP_J3, NONE),

	// CNVI
		// CNVI_BRI_DT
		PAD_CFG_NF(GPP_J4, NATIVE, DEEP, NF1),
		// CNVI_BRI_RSP
		PAD_CFG_NF(GPP_J5, NONE, DEEP, NF1),
		// CNVI_RGI_DT
		PAD_CFG_NF(GPP_J6, NONE, PLTRST, NF1),
		// CNVI_RGI_RSP
		PAD_CFG_NF(GPP_J7, NONE, DEEP, NF1),
		// CNVI_MFUART2_RXD
		PAD_CFG_NF(GPP_J8, NONE, DEEP, NF1),
		// CNVI_MFUART2_TXD
		PAD_CFG_NF(GPP_J9, NONE, DEEP, NF1),

	// GPIO
		// NC
		PAD_NC(GPP_J10, NONE),

	// A4WP
		// NC
		PAD_NC(GPP_J11, NONE),

// GPP_K
	// GPIO
		// NC
		PAD_NC(GPP_K0, NONE),
		// NC
		PAD_NC(GPP_K1, NONE),
		// NC
		PAD_NC(GPP_K2, NONE),
		// SCI#
		PAD_CFG_GPI_SCI_LOW(GPP_K3, UP_20K, DEEP, LEVEL),
		// NC
		PAD_NC(GPP_K4, NONE),
		// NC
		PAD_NC(GPP_K5, NONE),
		// SWI#
		PAD_CFG_GPI_SCI_LOW(GPP_K6, UP_20K, DEEP, LEVEL),
		// NC
		PAD_NC(GPP_K7, NONE),
		// TODO: SATA_M2_PWR_EN1
		PAD_CFG_GPI(GPP_K8, NONE, DEEP),
		// TODO: SATA_M2_PWR_EN2
		PAD_CFG_GPI(GPP_K9, NONE, DEEP),
		// NC
		PAD_NC(GPP_K10, NONE),
		// NC
		PAD_NC(GPP_K11, NONE),

	// GSX
		// NC
		PAD_NC(GPP_K12, NONE),
		// NC
		PAD_NC(GPP_K13, NONE),
		// NC
		PAD_NC(GPP_K14, NONE),
		// NC
		PAD_NC(GPP_K15, NONE),
		// NC
		PAD_NC(GPP_K16, NONE),

	// GPIO
		// NC
		PAD_NC(GPP_K17, NONE),
		// NC
		PAD_NC(GPP_K18, NONE),
		// SMI#
		_PAD_CFG_STRUCT(GPP_K19, 0x42000100, 0x3000),
		// TODO: GPU_EVENT#
		PAD_CFG_GPI(GPP_K20, NONE, DEEP),
		// TODO: GC6_FB_EN_PCH
		PAD_CFG_GPI(GPP_K21, NONE, DEEP),
		// TODO: DGPU_PWRGD_R
		_PAD_CFG_STRUCT(GPP_K22, 0x80000100, 0x0000),
		// NC
		PAD_NC(GPP_K23, NONE),
};

/* Early pad configuration in romstage. */
static const struct pad_config early_gpio_table[] = {
	// UART2
		// UART2_RXD
		PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
		// UART2_TXD
		PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
};

#endif

#endif
