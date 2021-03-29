/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_GPIO_H
#define MAINBOARD_GPIO_H

#include <soc/gpe.h>
#include <soc/gpio.h>

#ifndef __ACPI__

/* Early pad configuration in romstage. */
static const struct pad_config early_gpio_table[] = {
	// UART2
		// UART2_RXD
		PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
		// UART2_TXD
		PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPP_C22, NONE),
		// NC
		PAD_NC(GPP_C23, NONE),
};

/* Pad configuration in ramstage. */
static const struct pad_config gpio_table[] = {
// GPD
	// Power Management
		// PM_BATLOW#
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
		// SLP_A#
		PAD_CFG_NF(GPD6, NONE, DEEP, NF1),

	// GPIO
		// NC
		PAD_NC(GPD7, NONE),

	// Clock Signals
		// SUS_CLK
		PAD_CFG_NF(GPD8, NONE, DEEP, NF1),

	// Power Management
		// GPD9_RTD3
		PAD_NC(GPD9, NONE),
		// NC
		PAD_CFG_NF(GPD10, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPD11, NONE),

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
		// SERIRQ with pull up
		PAD_CFG_NF(GPP_A6, NONE, DEEP, NF1),

	// GSPI0
		// TPM_PIRQ#
		PAD_NC(GPP_A7, NONE),

	// LPC
		// PM_CLKRUN# with pull-up
		PAD_CFG_NF(GPP_A8, NONE, DEEP, NF1),
		// PCLK_KBC
		PAD_CFG_NF(GPP_A9, DN_20K, DEEP, NF1),
		// NC
		PAD_CFG_NF(GPP_A10, DN_20K, DEEP, NF1),

	// GSPI1
		// NC
		PAD_NC(GPP_A11, NONE),

	// ISH_GP
		// PCH_GPP_A12
		PAD_NC(GPP_A12, NONE),

	// Power Management
		// SUSWARN#
		PAD_CFG_NF(GPP_A13, NONE, DEEP, NF1),

	// LPC
		// NC
		PAD_CFG_NF(GPP_A14, NONE, DEEP, NF1),

	// Power Management
		// SUS_PWR_ACK
		PAD_CFG_NF(GPP_A15, UP_20K, DEEP, NF1),

	// SD
		// NC
		PAD_NC(GPP_A16, NONE),
		// LIGHT_KB_DET#
		PAD_NC(GPP_A17, NONE),

	// ISH_GP
		// NC
		PAD_NC(GPP_A18, NONE),
		// SATA_PWR_EN
		PAD_CFG_GPO(GPP_A19, 1, DEEP),
		// NC
		PAD_NC(GPP_A20, NONE),
		// NC
		PAD_NC(GPP_A21, NONE),
		// PS8338B_SW
		PAD_CFG_TERM_GPO(GPP_A22, 0, NONE, DEEP),
		// PS8338B_PCH
		PAD_NC(GPP_A23, NONE),

// GPP_B
	// Power
		// CORE_VID0
		PAD_NC(GPP_B0, NONE),
		// CORE_VID1
		PAD_NC(GPP_B1, NONE),

	// Power Management
		// CNVI_WAKE#
		PAD_NC(GPP_B2, NONE),

	// CPU Misc
		// NC
		PAD_NC(GPP_B3, NONE),
		// NC
		PAD_NC(GPP_B4, NONE),

	// Clock Signals
		// NC
		PAD_NC(GPP_B5, NONE),
		// NC
		PAD_NC(GPP_B6, NONE),
		// WLAN_CLKREQ#
		PAD_CFG_NF(GPP_B7, NONE, DEEP, NF1),
		// LAN_CLKREQ#
		PAD_CFG_NF(GPP_B8, NONE, DEEP, NF1),
		// TBT_CLKREQ#
		PAD_CFG_NF(GPP_B9, NONE, DEEP, NF1),
		// SSD_CLKREQ#
		PAD_CFG_NF(GPP_B10, NONE, DEEP, NF1),

	// Power Management
		// EXT_PWR_GATE#
		PAD_NC(GPP_B11, NONE),
		// SLP_S0#
		PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1),
		// PLT_RST#
		PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),

	// SPKR
		// PCH_SPKR
		PAD_CFG_NF(GPP_B14, NONE, DEEP, NF1),

	// GSPI0
		// NC
		PAD_NC(GPP_B15, NONE),
		// PCH_GPP_B16
		PAD_NC(GPP_B16, NONE),
		// PCH_GPP_B17
		PAD_NC(GPP_B17, NONE),
		// PCH_GPP_B18 - strap for disabling no reboot mode
		PAD_NC(GPP_B18, NONE),

	// GSPI1
		// NC
		PAD_NC(GPP_B19, NONE),
		// NC
		PAD_NC(GPP_B20, NONE),
		// NC
		PAD_NC(GPP_B21, NONE),
		// PCH_GPP_B22
		PAD_NC(GPP_B22, NONE),

	// SMBUS
		// NC
		PAD_NC(GPP_B23, NONE),

// GPP_C
	// SMBUS
		// SMB_CLK_DDR
		PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),
		// SMB_DAT_DDR
		PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1),
		// PCH_GPP_C2 with pull-up
		PAD_NC(GPP_C2, NONE),
		// NC
		PAD_NC(GPP_C3, NONE),
		// NC
		PAD_NC(GPP_C4, NONE),
		// NC
		PAD_NC(GPP_C5, NONE),
		// LAN_WAKEUP#
		PAD_NC(GPP_C6, NONE),
		// NC
		PAD_NC(GPP_C7, NONE),

	// UART0
		// NC
		PAD_NC(GPP_C8, NONE),
		// TBCIO_PLUG_EVENT
		_PAD_CFG_STRUCT(GPP_C9, 0x82880100, 0x3000),
		// TBT_FRC_PWR
		PAD_CFG_TERM_GPO(GPP_C10, 1, NONE, PLTRST),
		// NC
		PAD_NC(GPP_C11, NONE),

	// UART1
		// GPP_C12_RTD3
		PAD_CFG_TERM_GPO(GPP_C12, 1, NONE, PLTRST),
		// SSD_PWR_DN#
		PAD_CFG_TERM_GPO(GPP_C13, 1, NONE, PLTRST),
		// TBTA_HRESET
		PAD_CFG_TERM_GPO(GPP_C14, 0, NONE, PLTRST),
		// TBT_PERST_N
		PAD_CFG_TERM_GPO(GPP_C15, 1, UP_20K, PLTRST),

	// I2C
		// T_SDA
		PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1),
		// T_SCL
		PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPP_C18, NONE),
		// SWI
		PAD_NC(GPP_C19, NONE),

	// UART2
		// UART2_RXD
		PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
		// UART2_TXD
		PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPP_C22, NONE),
		// TP_ATTN#
		PAD_CFG_GPI_APIC(GPP_C23, NONE, PLTRST, EDGE_SINGLE, INVERT),

// GPP_D
	// SPI1
		// NC
		PAD_NC(GPP_D0, NONE),
		// NC
		PAD_NC(GPP_D1, NONE),
		// NC
		PAD_NC(GPP_D2, NONE),
		// NC
		PAD_NC(GPP_D3, NONE),

	// IMGCLKOUT
		// NC
		PAD_NC(GPP_D4, NONE),

	// I2C
		// NC
		PAD_NC(GPP_D5, NONE),
		// NC
		PAD_NC(GPP_D6, NONE),
		// NC
		PAD_NC(GPP_D7, NONE),
		// SB_BLON
		PAD_CFG_TERM_GPO(GPP_D8, 1, NONE, DEEP),

	// GSPI2
		// SWI#
		_PAD_CFG_STRUCT(GPP_D9, 0x40880100, 0x0000),
		// NC
		PAD_NC(GPP_D10, NONE),
		// RTD3_PCIE_WAKE#
		_PAD_CFG_STRUCT(GPP_D11, 0x40880100, 0x3000),
		// PCH_GPP_D12
		PAD_NC(GPP_D12, NONE),

	// UART0
		// NC
		PAD_NC(GPP_D13, NONE),
		// NC
		PAD_NC(GPP_D14, NONE),
		// NC
		PAD_NC(GPP_D15, NONE),
		// RTD3_3G_PW R_EN
		PAD_CFG_TERM_GPO(GPP_D16, 1, NONE, PWROK),

	// DMIC
		// NC
		PAD_NC(GPP_D17, NONE),
		// NC
		PAD_NC(GPP_D18, NONE),
		// GPPC_DMIC_CLK
		PAD_CFG_NF(GPP_D19, NONE, DEEP, NF1),
		// GPPC_DMIC_DATA
		PAD_CFG_NF(GPP_D20, NONE, DEEP, NF1),

	// SPI1
		// TPM_DET#
		PAD_NC(GPP_D21, NONE),
		// TPM_TCM_Detect
		PAD_NC(GPP_D22, NONE),

	// I2S
		// NC
		PAD_NC(GPP_D23, NONE),

// GPP_E
	// SATA
		// PCH_GPP_E0 with pull-up
		PAD_NC(GPP_E0, NONE),
		// SATA_ODD_PRSNT#
		PAD_NC(GPP_E1, NONE),
		// SATAGP2
		PAD_CFG_NF(GPP_E2, UP_20K, DEEP, NF1),

	// CPU Misc
		// NC
		PAD_NC(GPP_E3, NONE),

	// DEVSLP
		// NC
		PAD_NC(GPP_E4, NONE),
		// NC
		PAD_NC(GPP_E5, NONE),
		// DEVSLP2
		PAD_CFG_NF(GPP_E6, NONE, DEEP, NF1),

	// CPU Misc
		// NC
		PAD_NC(GPP_E7, NONE),

	// SATA
		// PCH_SATAHDD_LED#
		PAD_CFG_NF(GPP_E8, NONE, DEEP, NF1),

	// USB2
		// GP_BSSB_CLK
		PAD_NC(GPP_E9, NONE),
		// GPP_E10
		PAD_NC(GPP_E10, NONE),
		// GPP_E11
		PAD_NC(GPP_E11, NONE),
		// USB_OC#78
		PAD_NC(GPP_E12, NONE),

	// Display Signals
		// MUX_HPD
		PAD_CFG_NF(GPP_E13, NONE, DEEP, NF1),
		// HDMI_HPD
		PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),
		// SMI#
		_PAD_CFG_STRUCT(GPP_E15, 0x42840100, 0x0),
		// SCI#
		_PAD_CFG_STRUCT(GPP_E16, 0x80880100, 0x0000),
		// EDP_HPD
		PAD_CFG_NF(GPP_E17, NONE, DEEP, NF1),
		// MDP_CTRLCLK
		PAD_CFG_NF(GPP_E18, NONE, DEEP, NF1),
		// MDP_CTRLDATA
		PAD_CFG_NF(GPP_E19, NONE, DEEP, NF1),
		// HDMI_CTRLCLK
		PAD_CFG_NF(GPP_E20, NONE, DEEP, NF1),
		// HDMI_CTRLDATA
		PAD_CFG_NF(GPP_E21, NONE, DEEP, NF1),
		// NC
		PAD_NC(GPP_E22, NONE),
		// NC
		PAD_NC(GPP_E23, NONE),

// GPP_F
	// CNVI
		// CNVI_GNSS_PA_BLANKING
		PAD_CFG_NF(GPP_F0, NONE, DEEP, NF1),

	// GPIO
		// NC
		PAD_NC(GPP_F1, NONE),
		// NC
		PAD_NC(GPP_F2, NONE),
		// NC
		PAD_NC(GPP_F3, NONE),

	// CNVI
		// CNVI_BRI_DT
		PAD_CFG_NF(GPP_F4, NONE, DEEP, NF1),
		// CNVI_BRI_RSP
		PAD_CFG_NF(GPP_F5, UP_20K, DEEP, NF1),
		// CNVI_RGI_DT
		PAD_CFG_NF(GPP_F6, NONE, DEEP, NF1),
		// CNVI_RGI_RSP
		PAD_CFG_NF(GPP_F7, UP_20K, DEEP, NF1),
		// CNVI_MFUART2_RXD
		PAD_CFG_NF(GPP_F8, NONE, DEEP, NF1),
		// CNVI_MFUART2_TXD
		PAD_CFG_NF(GPP_F9, NONE, DEEP, NF1),

	// GPIO
		// NC
		PAD_NC(GPP_F10, NONE),

	// EMMC
		// NC
		PAD_NC(GPP_F11, NONE),
		// NC
		PAD_NC(GPP_F12, NONE),
		// NC
		PAD_NC(GPP_F13, NONE),
		// NC
		PAD_NC(GPP_F14, NONE),
		// NC
		PAD_NC(GPP_F15, NONE),
		// NC
		PAD_NC(GPP_F16, NONE),
		// NC
		PAD_NC(GPP_F17, NONE),
		// NC
		PAD_NC(GPP_F18, NONE),
		// NC
		PAD_NC(GPP_F19, NONE),
		// NC
		PAD_NC(GPP_F20, NONE),
		// NC
		PAD_NC(GPP_F21, NONE),
		// NC
		PAD_NC(GPP_F22, NONE),

	// A4WP
		// A4WP_PRESENT
		PAD_CFG_GPI(GPP_F23, DN_20K, DEEP),

// GPP_G
	// SD
		// EDP_DET
		PAD_NC(GPP_G0, NONE),
		// NC
		PAD_NC(GPP_G1, NONE),
		// NC
		PAD_NC(GPP_G2, NONE),
		// ASM1543_I_SEL0
		PAD_NC(GPP_G3, NONE),
		// ASM1543_I_SEL1
		PAD_NC(GPP_G4, NONE),
		// BOARD_ID
		PAD_NC(GPP_G5, NONE),
		// NC
		PAD_NC(GPP_G6, NONE),
		// TBT_Detect
		PAD_NC(GPP_G7, NONE),

// GPP_H
	// CNVI
		// NC
		PAD_NC(GPP_H0, NONE),
		// CNVI_RST#
		PAD_CFG_NF(GPP_H1, NONE, DEEP, NF3),
		// CNVI_CLKREQ
		PAD_CFG_NF(GPP_H2, NONE, DEEP, NF3),
		// NC
		PAD_NC(GPP_H3, NONE),

	// I2C
		// T23
		PAD_NC(GPP_H4, NONE),
		// T22
		PAD_NC(GPP_H5, NONE),
		// NC
		PAD_NC(GPP_H6, NONE),
		// NC
		PAD_NC(GPP_H7, NONE),
		// NC
		PAD_NC(GPP_H8, NONE),
		// NC
		PAD_NC(GPP_H9, NONE),

	// I2C
		// NC
		PAD_NC(GPP_H10, NONE),
		// NC
		PAD_NC(GPP_H11, NONE),

	// PCIE
		// NC
		PAD_NC(GPP_H12, NONE),
		// NC
		PAD_NC(GPP_H13, NONE),
		// G_INT1
		PAD_NC(GPP_H14, NONE),
		// NC
		PAD_NC(GPP_H15, NONE),

	// Display Signals
		// NC
		PAD_NC(GPP_H16, NONE),
		// NC
		PAD_NC(GPP_H17, NONE),

	// CPU Power
		// CPU_C10_GATE#
		PAD_CFG_NF(GPP_H18, NONE, DEEP, NF1),

	// TIMESYNC
		// NC
		PAD_NC(GPP_H19, NONE),

	// IMGCLKOUT
		// NC
		PAD_NC(GPP_H20, NONE),

	// GPIO
		// GPPC_H21
		PAD_NC(GPP_H21, NONE),
		// TBT_RTD3_PWR_EN_R
		PAD_CFG_TERM_GPO(GPP_H22, 1, NONE, PLTRST),
		// NC, WIGIG_PEWAKE
		PAD_NC(GPP_H23, NONE),
};

#endif

#endif
