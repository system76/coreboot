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

#define PAD_CFG_NC(pad) PAD_NC(pad, NONE)

/* Pad configuration in ramstage. */
static const struct pad_config gpio_table[] = {
// GPD
	// Power Management
		// NC
		PAD_CFG_NC(GPD0),
		// AC_PRESENT
		PAD_CFG_NF(GPD1, NATIVE, DEEP, NF1),
		// NC
		PAD_CFG_NC(GPD2),
		// PWR_BTN#
		PAD_CFG_NF(GPD3, UP_20K, DEEP, NF1),
		// SUSB#_PCH
		PAD_CFG_NF(GPD4, NONE, DEEP, NF1),
		// SUSC#_PCH
		PAD_CFG_NF(GPD5, NONE, DEEP, NF1),
		// NC
		PAD_CFG_NC(GPD6),

	// GPIO
		// NC
		PAD_CFG_NC(GPD7),

	// Power Management
		// SUS_CLK_R
		PAD_CFG_NF(GPD8, NONE, DEEP, NF1),
		// NC
		PAD_CFG_NC(GPD9),
		// NC
		PAD_CFG_NC(GPD10),
		// NC
		PAD_CFG_NC(GPD11),

// GPP_A
	// LPC
		// SB_KBCRST#
		PAD_CFG_NF(GPP_A0, NONE, DEEP, NF1),
		// LPC_AD0
		PAD_CFG_NF(GPP_A1, NATIVE, DEEP, NF1),
		// LPC_AD1
		PAD_CFG_NF(GPP_A2, NATIVE, DEEP, NF1),
		// LPC_AD2
		PAD_CFG_NF(GPP_A3, NATIVE, DEEP, NF1),
		// LPC_AD3
		PAD_CFG_NF(GPP_A4, NATIVE, DEEP, NF1),
		// LPC_FRAME#
		PAD_CFG_NF(GPP_A5, NONE, DEEP, NF1),
		// SERIRQ
		PAD_CFG_NF(GPP_A6, NONE, DEEP, NF1),
		// NC
		PAD_CFG_NC(GPP_A7),
		// PM_CLKRUN#
		PAD_CFG_NF(GPP_A8, NONE, DEEP, NF1),
		// PCLK_KBC
		PAD_CFG_NF(GPP_A9, DN_20K, DEEP, NF1),
		// NC
		PAD_CFG_NC(GPP_A10),

	// Power Management
		// TODO: LAN_WAKEUP#
		PAD_CFG_NC(GPP_A11),
		// NC
		PAD_CFG_NC(GPP_A12),
		// SUSWARN#
		PAD_CFG_NF(GPP_A13, NONE, DEEP, NF1),

	// LPC
		// NC
		PAD_CFG_NC(GPP_A14),

	// Power Management
		// SUS_PWR_ACK
		PAD_CFG_NF(GPP_A15, UP_20K, DEEP, NF1),

	// Clock Signals
		// NC
		PAD_CFG_NC(GPP_A16),

	// ISH
		// NC
		PAD_CFG_NC(GPP_A17),
		// SB_BLON
		PAD_CFG_GPO(GPP_A18, 1, DEEP),
		// NC
		PAD_CFG_NC(GPP_A19),
		// NC
		PAD_CFG_NC(GPP_A20),
		// NC
		PAD_CFG_NC(GPP_A21),
		// SATA_PWR_EN
		PAD_CFG_GPO(GPP_A22, 1, DEEP),
		// NC
		PAD_CFG_NC(GPP_A23),

// GPP_B
	// GSPI
		// TODO: TPM_PIRQ#
		PAD_CFG_NC(GPP_B0),
		// NC
		PAD_CFG_NC(GPP_B1),

	// Power Management
		// NC
		PAD_CFG_NC(GPP_B2),

	// CPU Misc
		// NC
		PAD_CFG_NC(GPP_B3),
		// TODO: EXTTS_SNI_DRV1
		PAD_CFG_NC(GPP_B4),

	// Clock Signals
		// NC
		PAD_CFG_NC(GPP_B5),
		// NC
		PAD_CFG_NC(GPP_B6),
		// NC
		PAD_CFG_NC(GPP_B7),
		// NC
		PAD_CFG_NC(GPP_B8),
		// NC
		PAD_CFG_NC(GPP_B9),
		// LAN_CLKREQ#
		PAD_CFG_NF(GPP_B10, NONE, DEEP, NF1),

	// Audio
		// TODO: GPP_B11: DDR Voltage select - 0 = 1.2V, 1 = 1.35V
		PAD_CFG_GPO(GPP_B11, 0, DEEP),

	// Power Management
		// SLP_S0#
		PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1),
		// PLT_RST#
		PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),

	// Audio
		// PCH_SPKR
		PAD_CFG_NF(GPP_B14, NONE, DEEP, NF1),

	// GSPI
		// NC
		PAD_CFG_NC(GPP_B15),
		// NC
		PAD_CFG_NC(GPP_B16),
		// NC
		PAD_CFG_NC(GPP_B17),
		// LPSS_GSPI0_MOSI - strap for no reboot mode
		PAD_CFG_NC(GPP_B18),
		// NC
		PAD_CFG_NC(GPP_B19),
		// NC
		PAD_CFG_NC(GPP_B20),
		// NC
		PAD_CFG_NC(GPP_B21),
		// LPSS_GSPI1_MOSI - strap for booting from SPI or LPC
		PAD_CFG_NC(GPP_B22),

	// SMBUS
		// PCH_HOT_GNSS_DISABLE - strap for DCI BSSB mode
		PAD_CFG_NC(GPP_B23),

// GPP_C
	// SMBUS
		// SMB_CLK
		PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),
		// SMB_DATA
		PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1),
		// NC
		PAD_CFG_NC(GPP_C2),
		// NC
		PAD_CFG_NC(GPP_C3),
		// NC
		PAD_CFG_NC(GPP_C4),
		// NC
		PAD_CFG_NC(GPP_C5),
		// NC
		PAD_CFG_NC(GPP_C6),
		// NC
		PAD_CFG_NC(GPP_C7),

	// UART
		// NC
		PAD_CFG_NC(GPP_C8),
		// TODO: CNVI_DET#
		PAD_CFG_NC(GPP_C9),
		// NC
		PAD_CFG_NC(GPP_C10),
		// NC
		PAD_CFG_NC(GPP_C11),
		// NC
		PAD_CFG_NC(GPP_C12),
		// NC
		PAD_CFG_NC(GPP_C13),
		// NC
		PAD_CFG_NC(GPP_C14),
		// NC
		PAD_CFG_NC(GPP_C15),

	// I2C
		// I2C_SCL_TP
		PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1),
		// I2C_SDA_TP
		PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1),
		// NC
		PAD_CFG_NC(GPP_C18),
		// NC
		PAD_CFG_NC(GPP_C19),

	// UART
		// UART2_RXD
		PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
		// UART2_TXD
		PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
		// NC
		PAD_CFG_NC(GPP_C22),
		// NC
		PAD_CFG_NC(GPP_C23),

// GPP_D
	// SPI
		// NC
		PAD_CFG_NC(GPP_D0),
		// NC
		PAD_CFG_NC(GPP_D1),
		// NC
		PAD_CFG_NC(GPP_D2),
		// NC
		PAD_CFG_NC(GPP_D3),

	// I2C
		// NC
		PAD_CFG_NC(GPP_D4),

	// CNVI
		// CNVI_RF_RST#
		PAD_CFG_NF(GPP_D5, NONE, DEEP, NF3),
		// XTAL_CLKREQ
		PAD_CFG_NF(GPP_D6, NONE, DEEP, NF3),
		// NC
		PAD_CFG_NC(GPP_D7),
		// NC
		PAD_CFG_NC(GPP_D8),

	// ISH
		// NC
		PAD_CFG_NC(GPP_D9),
		// NC
		PAD_CFG_NC(GPP_D10),
		// NC
		PAD_CFG_NC(GPP_D11),
		// NC
		PAD_CFG_NC(GPP_D12),
		// NC
		PAD_CFG_NC(GPP_D13),
		// NC
		PAD_CFG_NC(GPP_D14),
		// NC
		PAD_CFG_NC(GPP_D15),
		// NC
		PAD_CFG_NC(GPP_D16),

	// DMIC
		// NC
		PAD_CFG_NC(GPP_D17),
		// NC
		PAD_CFG_NC(GPP_D18),
		// NC
		PAD_CFG_NC(GPP_D19),
		// NC
		PAD_CFG_NC(GPP_D20),

	// SPI
		// NC
		PAD_CFG_NC(GPP_D21),
		// NC
		PAD_CFG_NC(GPP_D22),

	// ISH
		// NC
		PAD_CFG_NC(GPP_D23),

// GPP_E
	// SATA
		// NC
		PAD_CFG_NC(GPP_E0),
		// SATAGP1
		PAD_CFG_NF(GPP_E1, UP_20K, DEEP, NF1),
		// NC
		PAD_CFG_NC(GPP_E2),

	// CPU Misc
		// TODO: EXTTS_SNI_DRV0
		PAD_CFG_NC(GPP_E3),

	// SATA
		// DEVSLP0
		PAD_CFG_NF(GPP_E4, NONE, DEEP, NF1),
		// DEVSLP1
		PAD_CFG_NF(GPP_E5, NONE, DEEP, NF1),
		// NC
		PAD_CFG_NC(GPP_E6),

	// CPU Misc
		// TODO: TP_ATTN#
		PAD_CFG_NC(GPP_E7),

	// SATA
		// SATA_LED#
		PAD_CFG_NF(GPP_E8, NONE, DEEP, NF1),

	// USB2
		// NC
		PAD_CFG_NC(GPP_E9),
		// NC
		PAD_CFG_NC(GPP_E10),
		// NC
		PAD_CFG_NC(GPP_E11),
		// NC
		PAD_CFG_NC(GPP_E12),

// GPP_F
	// SATA
		// NC
		PAD_CFG_NC(GPP_F0),
		// NC
		PAD_CFG_NC(GPP_F1),
		// NC
		PAD_CFG_NC(GPP_F2),
		// NC
		PAD_CFG_NC(GPP_F3),
		// NC
		PAD_CFG_NC(GPP_F4),
		// TODO: KBLED_DET
		PAD_CFG_NC(GPP_F5),
		// TODO: LIGHT_KB_DET#
		PAD_CFG_NC(GPP_F6),
		// NC
		PAD_CFG_NC(GPP_F7),
		// NC
		PAD_CFG_NC(GPP_F8),
		// NC
		PAD_CFG_NC(GPP_F9),
		// BIOS_REC - strap for bios recovery enable
		PAD_CFG_NC(GPP_F10),
		// PCH_RSVD - unused strap
		PAD_CFG_NC(GPP_F11),
		// MFG_MODE - strap for manufacturing mode
		PAD_CFG_NC(GPP_F12),
		// TODO: GP39_GFX_CRB_DETECT - 0 = normal gfx, 1 = customer gfx
		PAD_CFG_NC(GPP_F13),

	// Power Management
		// TODO: H_SKTOCC_N
		PAD_CFG_NC(GPP_F14),

	// USB2
		// NC
		PAD_CFG_NC(GPP_F15),
		// NC
		PAD_CFG_NC(GPP_F16),
		// NC
		PAD_CFG_NC(GPP_F17),
		// NC
		PAD_CFG_NC(GPP_F18),

	// Display Signals
		// NB_ENAVDD
		PAD_CFG_NF(GPP_F19, NONE, DEEP, NF1),
		// BLON
		PAD_CFG_NF(GPP_F20, NONE, DEEP, NF1),
		// EDP_BRIGHTNESS
		PAD_CFG_NF(GPP_F21, NONE, DEEP, NF1),
		// TODO: DGPU_RST#_PCH
		PAD_CFG_GPO(GPP_F22, 1, DEEP),
		// TODO: DGPU_PWR_EN
		PAD_CFG_GPO(GPP_F23, 1, DEEP),

// GPP_G
	// SD
		// TODO: BOARD_ID1
		PAD_CFG_NC(GPP_G0),
		// TODO: BOARD_ID2
		PAD_CFG_NC(GPP_G1),
		// TODO: TPM_DET
		PAD_CFG_NC(GPP_G2),
		// //TODO: GPIO4_1V8_MAIN_EN_R
		PAD_CFG_NC(GPP_G3),
		// NC
		PAD_CFG_NC(GPP_G4),
		// NC
		PAD_CFG_NC(GPP_G5),
		// NC
		PAD_CFG_NC(GPP_G6),
		// NC
		PAD_CFG_NC(GPP_G7),

// GPP_H
	// Clock Signals
		// WLAN_CLKREQ#
		PAD_CFG_NF(GPP_H0, NONE, DEEP, NF1),
		// NC
		PAD_CFG_NC(GPP_H1),
		// PEG_CLKREQ#
		PAD_CFG_NF(GPP_H2, NONE, DEEP, NF1),
		// NC
		PAD_CFG_NC(GPP_H3),
		// SSD_CLKREQ#
		PAD_CFG_NF(GPP_H4, NONE, DEEP, NF1),
		// SSD2_CLKREQ#
		PAD_CFG_NF(GPP_H5, NONE, DEEP, NF1),
		// NC
		PAD_CFG_NC(GPP_H6),
		// NC
		PAD_CFG_NC(GPP_H7),
		// NC
		PAD_CFG_NC(GPP_H8),
		// NC
		PAD_CFG_NC(GPP_H9),

	// SMBUS
		// NC
		PAD_CFG_NC(GPP_H10),
		// NC
		PAD_CFG_NC(GPP_H11),
		// GPP_H_12 - strap for ESPI flash sharing mode
		PAD_CFG_NC(GPP_H12),
		// NC
		PAD_CFG_NC(GPP_H13),
		// NC
		PAD_CFG_NC(GPP_H14),
		// NC
		PAD_CFG_NC(GPP_H15),
		// NC
		PAD_CFG_NC(GPP_H16),
		// NC
		PAD_CFG_NC(GPP_H17),
		// NC
		PAD_CFG_NC(GPP_H18),

	// ISH
		// NC
		PAD_CFG_NC(GPP_H19),
		// NC
		PAD_CFG_NC(GPP_H20),
		// NC
		PAD_CFG_NC(GPP_H21),
		// NC
		PAD_CFG_NC(GPP_H22),

	// GPIO
		// NC
		PAD_CFG_NC(GPP_H23),

// GPP_I
	// Display Signals
		// NC
		PAD_CFG_NC(GPP_I0),
		// HDMI_HPD
		PAD_CFG_NF(GPP_I1, NONE, DEEP, NF1),
		// NC
		PAD_CFG_NC(GPP_I2),
		// MDP_E_HPD
		PAD_CFG_NF(GPP_I3, NONE, DEEP, NF1),
		// EDP_HPD
		PAD_CFG_NF(GPP_I4, NONE, DEEP, NF1),
		// NC
		PAD_CFG_NC(GPP_I5),
		// NC
		PAD_CFG_NC(GPP_I6),
		// HDMI_CTRLCLK
		PAD_CFG_NF(GPP_I7, NONE, DEEP, NF1),
		// HDMI_CTRLDATA
		PAD_CFG_NC(GPP_I8, NONE, DEEP, NF1),
		// NC
		PAD_CFG_NC(GPP_I9),
		// NC
		PAD_CFG_NC(GPP_I10),

	// PCIE
		// TODO: H_SKTOCC_N
		PAD_CFG_NC(GPP_I11),
		// NC
		PAD_CFG_NC(GPP_I12),
		// NC
		PAD_CFG_NC(GPP_I13),
		// NC
		PAD_CFG_NC(GPP_I14),

// GPP_J
	// CNVI
		// CNVI_GNSS_PA_BLANKING
		PAD_CFG_NF(GPP_J0, NONE, DEEP, NF1),

	// Power Management
		// NC
		PAD_CFG_NC(GPP_J1),

	// GPIO
		// NC
		PAD_CFG_NC(GPP_J2),
		// NC
		PAD_CFG_NC(GPP_J3),

	// CNVI
		// CNVI_BRI_DT
		PAD_CFG_NF(GPP_J4), NONE, DEEP, NF1,
		// CNVI_BRI_RSP
		PAD_CFG_NF(GPP_J5, NONE, DEEP, NF1),
		// CNVI_RGI_DT
		PAD_CFG_NF(GPP_J6, NONE, DEEP, NF1),
		// CNVI_RGI_RSP
		PAD_CFG_NF(GPP_J7, NONE, DEEP, NF1),
		// CNVI_MFUART2_RXD
		PAD_CFG_NF(GPP_J8, NONE, DEEP, NF1),
		// CNVI_MFUART2_TXD
		PAD_CFG_NF(GPP_J9, NONE, DEEP, NF1),

	// GPIO
		// NC
		PAD_CFG_NC(GPP_J10),

	// A4WP
		// NC
		PAD_CFG_NC(GPP_J11),

// GPP_K
	// GPIO
		// NC
		PAD_CFG_NC(GPP_K0),
		// NC
		PAD_CFG_NC(GPP_K1),
		// NC
		PAD_CFG_NC(GPP_K2),
		// SCI#
		PAD_CFG_GPI_SCI_LOW(GPP_K3, NONE, DEEP, LEVEL),
		// NC
		PAD_CFG_NC(GPP_K4),
		// NC
		PAD_CFG_NC(GPP_K5),
		// SWI#
		PAD_CFG_GPI_SCI_LOW(GPP_K6, NONE, DEEP, LEVEL),
		// NC
		PAD_CFG_NC(GPP_K7),
		// SATA_M2_PWR_EN1
		PAD_CFG_GPO(GPP_K8, 1, DEEP),
		// SATA_M2_PWR_EN2
		PAD_CFG_GPO(GPP_K9, 1, DEEP),
		// NC
		PAD_CFG_NC(GPP_K10),
		// NC
		PAD_CFG_NC(GPP_K11),

	// GSX
		// NC
		PAD_CFG_NC(GPP_K12),
		// NC
		PAD_CFG_NC(GPP_K13),
		// NC
		PAD_CFG_NC(GPP_K14),
		// NC
		PAD_CFG_NC(GPP_K15),
		// NC
		PAD_CFG_NC(GPP_K16),

	// GPIO
		// NC
		PAD_CFG_NC(GPP_K17),
		// NC
		PAD_CFG_NC(GPP_K18),
		// SMI#
		_PAD_CFG_STRUCT(GPP_K19, 0x42840100, 0x0),
		// TODO: GPU_EVENT#
		PAD_CFG_NC(GPP_K20),
		// TODO: GC6_FB_EN_PCH
		PAD_CFG_NC(GPP_K21),
		// TODO: DGPU_PWRGD_R
		PAD_CFG_NC(GPP_K22),
		// NC
		PAD_CFG_NC(GPP_K23),
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
