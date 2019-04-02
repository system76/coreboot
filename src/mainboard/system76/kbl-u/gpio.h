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
	/* PM_BATLOW# */
	PAD_CFG_NF(GPD0, NONE, PWROK, NF1),
	/* AC_PRESENT */
	PAD_CFG_NF(GPD1, NONE, PWROK, NF1),
	/* LAN_WAKEUP# */
	_PAD_CFG_STRUCT(GPD2, 0x880500, 0x0),
	/* PWR_BTN# */
	PAD_CFG_NF(GPD3, UP_20K, PWROK, NF1),
	/* SUSB#_PCH */
	PAD_CFG_NF(GPD4, NONE, PWROK, NF1),
	/* SUSC#_PCH */
	PAD_CFG_NF(GPD5, NONE, PWROK, NF1),
	/* SLP_A# */
	PAD_CFG_NF(GPD6, NONE, PWROK, NF1),
	/* PCH_GPD7 with pull-up */
	PAD_CFG_NC(GPD7),
	/* SUSCLK */
	PAD_CFG_NF(GPD8, NONE, PWROK, NF1),
	/* PCH_SLP_WLAN#_R */
	PAD_CFG_NF(GPD9, NONE, PWROK, NF1),
	/* NC */
	PAD_CFG_NC(GPD10),
	/* NC */
	PAD_CFG_NC(GPD11),
	/* SB_KBCRST# */
	PAD_CFG_NF(GPP_A0, NONE, DEEP, NF1),
	/* LPC_AD0 */
	PAD_CFG_NF(GPP_A1, NONE, DEEP, NF1),
	/* LPC_AD1 */
	PAD_CFG_NF(GPP_A2, NONE, DEEP, NF1),
	/* LPC_AD2 */
	PAD_CFG_NF(GPP_A3, NONE, DEEP, NF1),
	/* LPC_AD3 */
	PAD_CFG_NF(GPP_A4, NONE, DEEP, NF1),
	/* LPC_FRAME# */
	PAD_CFG_NF(GPP_A5, NONE, DEEP, NF1),
	/* SERIRQ with pull up */
	PAD_CFG_NF(GPP_A6, NONE, DEEP, NF1),
	/* G_INT1 */
	PAD_CFG_GPI(GPP_A7, NONE, DEEP),
	/* PM_CLKRUN# with pull-up */
	PAD_CFG_NF(GPP_A8, NONE, DEEP, NF1),
	/* PCLK_KBC */
	PAD_CFG_NF(GPP_A9, NONE, DEEP, NF1),
	/* PCLK_TPM */
	PAD_CFG_NF(GPP_A10, DN_20K, DEEP, NF1),
	/* LAN_WAKEUP# */
	PAD_CFG_GPO(GPP_A11, 0, DEEP),
	/* PCH_GPP_A12 with pull-up */
	PAD_CFG_GPO(GPP_A12, 0, DEEP),
	/* SUSWARN# with tie to SUS_PWR_ACK */
	PAD_CFG_GPO(GPP_A13, 0, DEEP),
	/* S4_STATE# */
	PAD_CFG_NF(GPP_A14, NONE, DEEP, NF1),
	/* SUS_PWR_ACK with tie to SUSWARN# */
	PAD_CFG_NF(GPP_A15, NONE, DEEP, NF1),
	/* NC */
	PAD_CFG_NC(GPP_A16),
	/* NC */
	PAD_CFG_NC(GPP_A17),
	/* TBTA_ACE_GPIO3 */
	PAD_CFG_GPO(GPP_A18, 1, DEEP),
	/* SATA_PWR_EN */
	PAD_CFG_NF(GPP_A19, NONE, DEEP, NF1),
	/* TBTA_ACE_GPIO0 */
	PAD_CFG_GPO(GPP_A20, 0, DEEP),
	/* TBT_FRC_PW tied to PCH_GPP_E0 */
	_PAD_CFG_STRUCT(GPP_A21, 0x84000200, 0x1000),
	/* PS8338B_SW */
	PAD_CFG_GPO(GPP_A22, 0, PWROK),
	/* PS8338B_PCH */
	PAD_CFG_GPO(GPP_A23, 0, PWROK),
	/* CORE_VID0 with pull-up */
	PAD_CFG_NF(GPP_B0, NONE, DEEP, NF1),
	/* CORE_VID1 with pull-up */
	PAD_CFG_NF(GPP_B1, NONE, DEEP, NF1),
	/* VRALERT# */
	PAD_CFG_GPO(GPP_B2, 0, DEEP),
	/* NC */
	PAD_CFG_NC(GPP_B3),
	/* NC */
	PAD_CFG_NC(GPP_B4),
	/* NC */
	PAD_CFG_NC(GPP_B5),
	/* NC */
	PAD_CFG_NC(GPP_B6),
	/* WLAN_CLKREQ# */
	PAD_CFG_NF(GPP_B7, NONE, DEEP, NF1),
	/* LAN_CLKREQ# */
	PAD_CFG_NF(GPP_B8, NONE, DEEP, NF1),
	/* TBT_CLKREQ# */
	PAD_CFG_NF(GPP_B9, NONE, DEEP, NF1),
	/* SSD_CLKREQ# */
	PAD_CFG_NF(GPP_B10, NONE, DEEP, NF1),
	/* NC */
	PAD_CFG_NC(GPP_B11),
	/* NC */
	PAD_CFG_NC(GPP_B12),
	/* PLT_RST# with pull-down */
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),
	/* PCH_SPKR */
	PAD_CFG_NF(GPP_B14, DN_20K, DEEP, NF1),
	/* NC */
	PAD_CFG_NC(GPP_B15),
	/* T16 */
	PAD_CFG_GPO(GPP_B16, 0, DEEP),
	/* T35 */
	PAD_CFG_GPO(GPP_B17, 0, DEEP),
	/* PCH_GPP_B18 with pull-down - strap for disabling no reboot mode */
	PAD_CFG_NF(GPP_B18, UP_20K, DEEP, NF1),
	/* NC */
	PAD_CFG_NC(GPP_B19),
	/* NC */
	PAD_CFG_NC(GPP_B20),
	/* NC */
	PAD_CFG_NC(GPP_B21),
	/* PCH_GPP_B22 with pull-down - strap for BIOS on SPI on n130wu */
	PAD_CFG_NF(GPP_B22, DN_20K, DEEP, NF1),
	/* NC */
	PAD_CFG_NC(GPP_B23),
	/* SMB_CLK */
	PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),
	/* SMB_DATA */
	PAD_CFG_NF(GPP_C1, DN_20K, DEEP, NF1),
	/* NC */
	PAD_CFG_NC(GPP_C2),
	/* NC */
	PAD_CFG_NC(GPP_C3),
	/* NC */
	PAD_CFG_NC(GPP_C4),
	/* NC */
	PAD_CFG_NC(GPP_C5),
	/* NC */
	PAD_CFG_NC(GPP_C6),
	/* NC */
	PAD_CFG_NC(GPP_C7),
	/* NC */
	PAD_CFG_NC(GPP_C8),
	/* NC */
	PAD_CFG_NC(GPP_C9),
	/* NC */
	PAD_CFG_NC(GPP_C10),
	/* NC */
	PAD_CFG_NC(GPP_C11),
	/* TBTA_ACE_GPIO2 */
	PAD_CFG_NF(GPP_C12, NONE, DEEP, NF1),
	/* TBCIO_PLUG_EVENT tied to PCH_GPP_B17 */
	_PAD_CFG_STRUCT(GPP_C13, 0x82880100, 0x0),
	/* TBTA_MRESET */
	PAD_CFG_NF(GPP_C14, NONE, DEEP, NF1),
	/* TBTA_ACE_GPIO7 */
	PAD_CFG_NF(GPP_C15, NONE, DEEP, NF1),
	/* T_SDA */
	PAD_CFG_GPO(GPP_C16, 0, DEEP),
	/* T_SCL */
	PAD_CFG_GPO(GPP_C17, 0, DEEP),
	/* NC */
	PAD_CFG_NC(GPP_C18),
	/* SWI# */
	_PAD_CFG_STRUCT(GPP_C19, 0x40880100, 0x0),
	/* UART2_RXD */
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
	/* UART2_TXD */
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
	/* UART2_RTS_N */
	PAD_CFG_NF(GPP_C22, NONE, DEEP, NF1),
	/* UART2_CTS_N */
	PAD_CFG_NF(GPP_C23, NONE, DEEP, NF1),
	/* NC */
	PAD_CFG_NC(GPP_D0),
	/* NC */
	PAD_CFG_NC(GPP_D1),
	/* NC */
	PAD_CFG_NC(GPP_D2),
	/* NC */
	PAD_CFG_NC(GPP_D3),
	/* NC */
	PAD_CFG_NC(GPP_D4),
	/* NC */
	PAD_CFG_NC(GPP_D5),
	/* NC */
	PAD_CFG_NC(GPP_D6),
	/* NC */
	PAD_CFG_NC(GPP_D7),
	/* SB_BLON */
	PAD_CFG_GPO(GPP_D8, 1, DEEP),
	/* T_INT */
	PAD_CFG_GPO(GPP_D9, 0, DEEP),
	/* EDP_DET */
	PAD_CFG_GPO(GPP_D10, 0, DEEP),
	/* NC */
	PAD_CFG_NC(GPP_D11),
	/* NC */
	PAD_CFG_NC(GPP_D12),
	/* NC */
	PAD_CFG_NC(GPP_D13),
	/* NC */
	PAD_CFG_NC(GPP_D14),
	/* NC */
	PAD_CFG_NC(GPP_D15),
	/* NC */
	PAD_CFG_NC(GPP_D16),
	/* NC */
	PAD_CFG_NC(GPP_D17),
	/* NC */
	PAD_CFG_NC(GPP_D18),
	/* NC */
	PAD_CFG_NC(GPP_D19),
	/* NC */
	PAD_CFG_NC(GPP_D20),
	/* TPM_DET# */
	PAD_CFG_GPI(GPP_D21, NONE, DEEP),
	/* NC */
	PAD_CFG_NC(GPP_D22),
	/* NC */
	PAD_CFG_NC(GPP_D23),
	/* PCH_GPP_E0 tied to TBT_FRC_PW */
	_PAD_CFG_STRUCT(GPP_E0, 0x42100100, 0x1000),
	/* SATA_ODD_PRSNT# */
	PAD_CFG_NF(GPP_E1, NONE, DEEP, NF1),
	/* SATAGP2 */
	PAD_CFG_NF(GPP_E2, NONE, DEEP, NF1),
	/* NC */
	PAD_CFG_NC(GPP_E3),
	/* NC */
	PAD_CFG_NC(GPP_E4),
	/* NC */
	PAD_CFG_NC(GPP_E5),
	/* DEVSLP2 */
	PAD_CFG_NF(GPP_E6, NONE, DEEP, NF1),
	/* NC */
	PAD_CFG_NC(GPP_E7),
	/* PCH_SATA_LED# */
	PAD_CFG_NF(GPP_E8, NONE, DEEP, NF1),
	/* NC */
	PAD_CFG_NC(GPP_E9),
	/* NC */
	PAD_CFG_NC(GPP_E10),
	/* NC */
	PAD_CFG_NC(GPP_E11),
	/* NC */
	PAD_CFG_NC(GPP_E12),
	/* MUX_HPD */
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF1),
	/* HDMI_HPD */
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),
	/* SMI# */
	_PAD_CFG_STRUCT(GPP_E15, 0x42840100, 0x0),
	/* SCI# */
	PAD_CFG_GPI_SCI_LOW(GPP_E16, NONE, PLTRST, LEVEL),
	/* EDP_HPD */
	PAD_CFG_NF(GPP_E17, NONE, DEEP, NF1),
	/* MDP_CTRLCLK */
	PAD_CFG_NF(GPP_E18, NONE, DEEP, NF1),
	/* MDP_CTRLDATA */
	PAD_CFG_NF(GPP_E19, DN_20K, DEEP, NF1),
	/* HDMI_CTRLCLK */
	PAD_CFG_NF(GPP_E20, NONE, DEEP, NF1),
	/* HDMI_CTRLDATA */
	PAD_CFG_NF(GPP_E21, DN_20K, DEEP, NF1),
	/* NC */
	PAD_CFG_NC(GPP_E22),
	/* NC */
	PAD_CFG_NC(GPP_E23),
	/* NC */
	PAD_CFG_NC(GPP_F0),
	/* NC */
	PAD_CFG_NC(GPP_F1),
	/* NC */
	PAD_CFG_NC(GPP_F2),
	/* NC */
	PAD_CFG_NC(GPP_F3),
	/* Pull-up */
	PAD_CFG_NC(GPP_F4),
	/* Pull-up */
	PAD_CFG_NC(GPP_F5),
	/* Pull-up */
	PAD_CFG_NC(GPP_F6),
	/* Pull-up */
	PAD_CFG_NC(GPP_F7),
	/* Pull-up */
	PAD_CFG_NC(GPP_F8),
	/* Pull-up */
	PAD_CFG_NC(GPP_F9),
	/* NC */
	PAD_CFG_NC(GPP_F10),
	/* NC */
	PAD_CFG_NC(GPP_F11),
	/* NC */
	PAD_CFG_NC(GPP_F12),
	/* NC */
	PAD_CFG_NC(GPP_F13),
	/* NC */
	PAD_CFG_NC(GPP_F14),
	/* NC */
	PAD_CFG_NC(GPP_F15),
	/* NC */
	PAD_CFG_NC(GPP_F16),
	/* NC */
	PAD_CFG_NC(GPP_F17),
	/* NC */
	PAD_CFG_NC(GPP_F18),
	/* NC */
	PAD_CFG_NC(GPP_F19),
	/* NC */
	PAD_CFG_NC(GPP_F20),
	/* NC */
	PAD_CFG_NC(GPP_F21),
	/* NC */
	PAD_CFG_NC(GPP_F22),
	/* LIGHT_KB_DET# */
	_PAD_CFG_STRUCT(GPP_F23, 0x40100100, 0x0),
	/* NC */
	PAD_CFG_NC(GPP_G0),
	/* TBT_Detect */
	PAD_CFG_GPI(GPP_G1, NONE, DEEP),
	/* NC */
	PAD_CFG_NC(GPP_G2),
	/* ASM1543_I_SEL0 */
	PAD_CFG_GPO(GPP_G3, 0, DEEP),
	/* ASM1543_I_SEL1 */
	PAD_CFG_GPO(GPP_G4, 0, DEEP),
	/* NC */
	PAD_CFG_NC(GPP_G5),
	/* NC */
	PAD_CFG_NC(GPP_G6),
	/* NC */
	PAD_CFG_NC(GPP_G7),
};

/* Early pad configuration in romstage. */
static const struct pad_config early_gpio_table[] = {
	/* UART2_RXD */
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
	/* UART2_TXD */
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
};

#endif

#endif
