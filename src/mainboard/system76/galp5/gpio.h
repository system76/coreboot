/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_GPIO_H
#define MAINBOARD_GPIO_H

#include <soc/gpio.h>

#define DGPU_RST_N	GPP_U4
#define DGPU_PWR_EN	GPP_U5
#define DGPU_GC6	GPP_D2

#ifndef __ACPI__

static const struct pad_config early_gpio_table[] = {
	PAD_CFG_NF(GPP_C20, UP_20K, DEEP, NF1), // UART2_RXD
	PAD_CFG_NF(GPP_C21, UP_20K, DEEP, NF1), // UART2_TXD
	PAD_CFG_GPO(GPP_U4, 0, DEEP), // DGPU_RST#_PCH
	PAD_CFG_GPO(GPP_U5, 0, DEEP), // DGPU_PWR_EN
};

static const struct pad_config gpio_table[] = {
	PAD_CFG_NF(GPD0, UP_20K, PWROK, NF1), // PM_BATLOW#
	PAD_CFG_NF(GPD1, NATIVE, PWROK, NF1), // AC_PRESENT
	PAD_CFG_GPI(GPD2, NONE, PWROK), // LAN_WAKEUP#
	PAD_CFG_NF(GPD3, UP_20K, PWROK, NF1), // PWR_BTN#
	PAD_CFG_NF(GPD4, NONE, PWROK, NF1), // SUSB#_PCH
	PAD_CFG_NF(GPD5, NONE, PWROK, NF1), // SUSC#_PCH
	PAD_CFG_NF(GPD6, NONE, DEEP, NF1), // SLP_A# - test point
	PAD_CFG_GPO(GPD7, 1, PWROK), // GPD7_REST
	PAD_CFG_NF(GPD8, NONE, PWROK, NF1), // SUS_CLK
	PAD_CFG_GPO(GPD9, 0, PWROK), // GPD9_RTD3
	PAD_CFG_NF(GPD10, UP_20K, DEEP, NF1), // SLP_S5# - test point
	PAD_CFG_GPI(GPD11, UP_20K, DEEP), // LAN_DISABLE#

	PAD_CFG_NF(GPP_A0, UP_20K, DEEP, NF1), // ESPI_IO0_EC
	PAD_CFG_NF(GPP_A1, UP_20K, DEEP, NF1), // ESPI_IO1_EC
	PAD_CFG_NF(GPP_A2, UP_20K, DEEP, NF1), // ESPI_IO2_EC
	PAD_CFG_NF(GPP_A3, UP_20K, DEEP, NF1), // ESPI_IO3_EC
	PAD_CFG_NF(GPP_A4, UP_20K, DEEP, NF1), // ESPI_CS_EC#
	PAD_CFG_NF(GPP_A5, DN_20K, DEEP, NF1), // ESPI_CLK_EC
	PAD_CFG_NF(GPP_A6, NONE, DEEP, NF1), // ESPI_RESET_N
	PAD_NC(GPP_A7, NONE),
	PAD_CFG_NF(GPP_A8, NONE, DEEP, NF2), // CNVI_RST#
	PAD_CFG_NF(GPP_A9, NONE, DEEP, NF3), // CNVI_CLKREQ
	PAD_NC(GPP_A10, NONE),
	PAD_NC(GPP_A11, NONE),
	PAD_NC(GPP_A12, NONE), // SATAGP1
	PAD_CFG_GPO(GPP_A13, 1, PLTRST), // PCH_BT_EN
	PAD_NC(GPP_A14, NONE),
	PAD_NC(GPP_A15, NONE),
	PAD_NC(GPP_A16, NONE), // 10K pull-up
	PAD_NC(GPP_A17, NONE),
	PAD_CFG_NF(GPP_A18, NONE, DEEP, NF1), // HDMI_HPD
	PAD_NC(GPP_A19, NONE),
	PAD_NC(GPP_A20, NONE),
	PAD_NC(GPP_A21, NONE),
	PAD_NC(GPP_A22, NONE),
	PAD_CFG_GPO(GPP_A23, 0, PLTRST), // GPPC_A23_TBT_FORCE_PWR

	PAD_CFG_NF(GPP_B0, NONE, DEEP, NF1), // VCCIN_AUX_VID0
	PAD_CFG_NF(GPP_B1, NONE, DEEP, NF1), // VCCIN_AUX_VID1
	PAD_CFG_GPI(GPP_B2, UP_20K, DEEP), // VRALERT#_PD
	PAD_CFG_GPI_INT(GPP_B3, NONE, PLTRST, LEVEL), // GPP_B3 - touchpad interrupt
	PAD_NC(GPP_B4, NONE),
	PAD_NC(GPP_B5, NONE), // test point
	PAD_NC(GPP_B6, NONE), // test point
	PAD_NC(GPP_B7, NONE),
	PAD_CFG_GPO(GPP_B8, 1, DEEP), // SB_BLON
	PAD_NC(GPP_B9, NONE),
	PAD_NC(GPP_B10, NONE),
	PAD_CFG_NF(GPP_B11, NONE, PWROK, NF1), // TBTA-PCH_I2C_INT
	PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1), // SLP_S0#
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1), // PLT_RST#
	PAD_CFG_GPO(GPP_B14, 0, DEEP), // PCH_SPKR
	PAD_CFG_GPO(GPP_B15, 1, DEEP), // PCH_GPP_B15 - TODO
	PAD_NC(GPP_B16, NONE),
	PAD_NC(GPP_B17, NONE),
	PAD_NC(GPP_B18, NONE), // No reboot strap
	PAD_NC(GPP_B19, NONE),
	PAD_NC(GPP_B20, NONE),
	PAD_NC(GPP_B21, NONE),
	PAD_NC(GPP_B22, NONE), // PCH_GPP_B22 - 20k pull-down
	PAD_CFG_GPO(GPP_B23, 0, DEEP), // Clock frequency strap

	PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1), // SMB_CLK and SMB_CLK_DDR
	PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1), // SMB_DATA and SMB_DAT_DDR
	PAD_NC(GPP_C2, NONE), // Intel AMT TLS strap
	PAD_CFG_NF(GPP_C3, NONE, DEEP, NF1), // SML0_CLK
	PAD_CFG_NF(GPP_C4, NONE, DEEP, NF1), // SML0_DATA
	PAD_NC(GPP_C5, NONE), // Boot strap bit 0
	PAD_CFG_NF(GPP_C6, NONE, PWROK, NF1), // TBT-PCH_I2C_SCL
	PAD_CFG_NF(GPP_C7, NONE, PWROK, NF1), // TBT-PCH_I2C_SDA
	PAD_NC(GPP_C8, NONE),
	PAD_NC(GPP_C9, NONE),
	PAD_NC(GPP_C10, NONE),
	PAD_NC(GPP_C11, NONE),
	PAD_NC(GPP_C12, NONE),
	PAD_NC(GPP_C13, NONE),
	_PAD_CFG_STRUCT(GPP_C14, 0x40100100, 0x3000), // TPM_PIRQ#
	PAD_NC(GPP_C15, NONE),
	PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1), // T_SDA
	PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1), // T_SCL
	PAD_CFG_NF(GPP_C18, NONE, DEEP, NF1), // PCH_I2C_SDA
	PAD_CFG_NF(GPP_C19, NONE, DEEP, NF1), // PCH_I2C_SCL
	//PAD_CFG_NF(GPP_C20, UP_20K, DEEP, NF1), // UART2_RXD
	//PAD_CFG_NF(GPP_C21, UP_20K, DEEP, NF1), // UART2_TXD
	PAD_CFG_GPO(GPP_C22, 1, PLTRST), // LAN_PLT_RST#
	_PAD_CFG_STRUCT(GPP_C23, 0x40880100, 0x0000), // PCH_GPP_C23 - 4.7k pull-down

	PAD_CFG_GPI(GPP_D0, NONE, DEEP), // DGPU_SELECT#
	PAD_CFG_GPO(GPP_D1, 1, PLTRST), // GPU_EVENT#
	PAD_CFG_GPI(GPP_D2, NONE, PLTRST), // GC6_FB_EN_PCH
	PAD_CFG_GPI(GPP_D3, NONE, PLTRST), // DGPU_PWRGD_R
	PAD_NC(GPP_D4, NONE),
	PAD_CFG_NF(GPP_D5, NONE, DEEP, NF1), // SSD1_CLKREQ#
	PAD_CFG_NF(GPP_D6, NONE, DEEP, NF1), // WLAN_CLKREQ#
	PAD_CFG_NF(GPP_D7, NONE, DEEP, NF1), // PEG_CLKREQ#
	PAD_CFG_NF(GPP_D8, NONE, DEEP, NF1), // CARD_CLKREQ#
	PAD_NC(GPP_D9, NONE),
	PAD_NC(GPP_D10, NONE), // 4.7k pull-up
	PAD_CFG_GPI(GPP_D11, DN_20K, DEEP), // BOARD_ID - low = GTX 1650Ti, high = GTX 1650
	PAD_CFG_GPI(GPP_D12, DN_20K, DEEP), // GPP_D12 - low = NVIDIA GPU, high = Intel GPU
	PAD_CFG_GPO(GPP_D13, 0, DEEP), // dGPU_OVRM
	PAD_CFG_GPO(GPP_D14, 1, PLTRST), // SSD1_PWR_DN#
	PAD_NC(GPP_D15, NONE),
	PAD_NC(GPP_D16, NONE),
	PAD_CFG_GPI(GPP_D17, DN_20K, DEEP), // DGPU_PRSNT#
	PAD_CFG_GPI(GPP_D18, DN_20K, DEEP), // 1V8_MAIN_EN_R
	PAD_NC(GPP_D19, NONE),

	PAD_NC(GPP_E0, NONE), // test point
	PAD_CFG_GPO(GPP_E1, 0, PLTRST), // ROM_I2C_EN
	_PAD_CFG_STRUCT(GPP_E2, 0x40880100, 0x0000), // SWI#
	PAD_CFG_GPI(GPP_E3, DN_20K, DEEP), // SCI# - unused, tunneled over eSPI
	PAD_NC(GPP_E4, NONE), // test point
	PAD_NC(GPP_E5, NONE), // DEVSLP1
	PAD_NC(GPP_E6, NONE), // PCH_GPP_E6
	_PAD_CFG_STRUCT(GPP_E7, 0x82840100, 0x0000), // SMI#
	PAD_NC(GPP_E8, NONE), // PCH_SATAHDD_LED#
	PAD_NC(GPP_E9, NONE), // 10k pull-up
	PAD_NC(GPP_E10, NONE), // PCH_GPP_E10
	PAD_NC(GPP_E11, NONE), // PCH_GPP_E11
	PAD_NC(GPP_E12, NONE),
	PAD_NC(GPP_E13, NONE),
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1), // EDP_HPD
	PAD_NC(GPP_E15, NONE), // ALERT#_R
	PAD_CFG_GPI(GPP_E16, DN_20K, DEEP), // SB_KBCRST#
	PAD_NC(GPP_E17, NONE),
	PAD_NC(GPP_E18, NONE), // TBT_LSX0_TXD - programmed by FSP, see Intel document 617016
	PAD_NC(GPP_E19, NONE), // TBT_LSX0_RXD - programmed by FSP, see Intel document 617016
	PAD_NC(GPP_E20, NONE),
	PAD_NC(GPP_E21, NONE),
	PAD_NC(GPP_E22, NONE),
	PAD_NC(GPP_E23, NONE),

	PAD_CFG_NF(GPP_F0, NONE, DEEP, NF1), // CNVI_BRI_DT
	PAD_CFG_NF(GPP_F1, UP_20K, DEEP, NF1), // CNVI_BRI_RSP
	PAD_CFG_NF(GPP_F2, NONE, DEEP, NF1), // CNVI_RGI_DT
	PAD_CFG_NF(GPP_F3, UP_20K, DEEP, NF1), // CNVI_RGI_RSP
	PAD_NC(GPP_F4, NONE),
	PAD_NC(GPP_F5, NONE),
	PAD_NC(GPP_F6, NONE), // CNVI_GNSS_PA_PLANKING - TODO
	PAD_CFG_GPO(GPP_F7, 1, DEEP), // GPIO_LANRTD3
	PAD_NC(GPP_F8, NONE),
	PAD_CFG_GPO(GPP_F9, 1, DEEP), // GPIO_LAN_EN
	PAD_NC(GPP_F10, NONE), // 4.7k pull-up
	PAD_NC(GPP_F11, NONE),
	PAD_NC(GPP_F12, NONE),
	PAD_NC(GPP_F13, NONE),
	PAD_NC(GPP_F14, NONE),
	PAD_NC(GPP_F15, NONE),
	PAD_NC(GPP_F16, NONE),
	PAD_CFG_GPI(GPP_F17, NONE, PLTRST), // TPM_DET#
	PAD_NC(GPP_F18, NONE),
	PAD_NC(GPP_F19, NONE),
	PAD_NC(GPP_F20, NONE),
	PAD_CFG_GPI(GPP_F21, DN_20K, DEEP), // EXT_PWR_GATE# - TODO
	PAD_NC(GPP_F22, NONE), // VNN_CTRL - TODO
	PAD_NC(GPP_F23, NONE), // 1P05_CTRL - TODO

	PAD_CFG_GPO(GPP_H0, 1, PLTRST), // GPP_H0_RTD3
	PAD_NC(GPP_H1, NONE), // 4.7k pull-up
	PAD_NC(GPP_H2, NONE), // 4.7k pull-up
	PAD_CFG_GPI(GPP_H3, DN_20K, DEEP),
	PAD_CFG_NF(GPP_H4, NONE, DEEP, NF1), // GPPH_I2C2_SDA
	PAD_CFG_NF(GPP_H5, NONE, DEEP, NF1), // GPPH_I2C2_SCL
	PAD_NC(GPP_H6, NONE),
	PAD_NC(GPP_H7, NONE), // SWI# - TODO
	PAD_CFG_GPI(GPP_H8, DN_20K, DEEP), // CNVI_MFUART2_RXD - TODO
	PAD_CFG_GPI(GPP_H9, DN_20K, DEEP), // CNVI_MFUART2_TXD - TODO
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF1), // LAN_CLKREQ#
	PAD_NC(GPP_H11, NONE),
	PAD_NC(GPP_H12, NONE),
	PAD_NC(GPP_H13, NONE),
	PAD_NC(GPP_H14, NONE),
	PAD_NC(GPP_H15, NONE),
	PAD_CFG_NF(GPP_H16, NONE, DEEP, NF1), // HDMI_CTRLCLK
	PAD_CFG_NF(GPP_H17, NONE, DEEP, NF1), // HDMI_CTRLDATA
	PAD_CFG_NF(GPP_H18, NONE, DEEP, NF1), // CPU_C10_GATE#
	PAD_NC(GPP_H19, NONE), // CNVI_WAKE#
	PAD_NC(GPP_H20, NONE), // PM_CLKRUN#
	PAD_NC(GPP_H21, NONE),
	PAD_NC(GPP_H22, NONE),
	PAD_NC(GPP_H23, NONE),

	PAD_CFG_NF(GPP_R0, NONE, DEEP, NF1), // HDA_BITCLK
	PAD_CFG_NF(GPP_R1, NATIVE, DEEP, NF1), // HDA_SYNC
	PAD_CFG_NF(GPP_R2, NATIVE, DEEP, NF1), // HDA_SDOUT
	PAD_CFG_NF(GPP_R3, NATIVE, DEEP, NF1), // HDA_SDIN0
	PAD_CFG_NF(GPP_R4, NONE, DEEP, NF1), // AZ_RST#_R
	PAD_NC(GPP_R5, NONE),
	PAD_NC(GPP_R6, NONE),
	PAD_NC(GPP_R7, NONE),

	PAD_NC(GPP_S0, NONE),
	PAD_NC(GPP_S1, NONE),
	PAD_NC(GPP_S2, NONE),
	PAD_NC(GPP_S3, NONE),
	PAD_NC(GPP_S4, NONE),
	PAD_NC(GPP_S5, NONE),
	PAD_NC(GPP_S6, NONE), // GPPC_DMIC_CLK - TODO
	PAD_NC(GPP_S7, NONE), // GPPC_DMIC_DATA - TODO

	PAD_NC(GPP_T2, NONE),
	PAD_NC(GPP_T3, NONE),

	//PAD_CFG_GPO(GPP_U4, 0, DEEP), // DGPU_RST#_PCH
	//PAD_CFG_GPO(GPP_U5, 0, DEEP), // DGPU_PWR_EN
};

#endif /* __ACPI__ */

#endif /* MAINBOARD_GPIO_H */
