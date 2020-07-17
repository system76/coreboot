/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_GPIO_H
#define MAINBOARD_GPIO_H

#include <soc/gpe.h>
#include <soc/gpio.h>

#define DGPU_RST_N GPP_F22
#define DGPU_PWR_EN GPP_F23

#ifndef __ACPI__

/* Pad configuration in romstage. */
static const struct pad_config early_gpio_table[] = {
		PAD_CFG_TERM_GPO(GPP_F22, 0, NONE, DEEP), // DGPU_RST_N
		PAD_CFG_TERM_GPO(GPP_F23, 0, NONE, DEEP), // DGPU_PWR_EN
};

/* Pad configuration in ramstage. */
static const struct pad_config gpio_table[] = {
		PAD_CFG_NF(GPD0, NONE, DEEP, NF1),
		PAD_CFG_NF(GPD1, NATIVE, DEEP, NF1),
		PAD_CFG_GPI(GPD2, NATIVE, PWROK),
		PAD_CFG_NF(GPD3, UP_20K, DEEP, NF1),
		PAD_CFG_NF(GPD4, NONE, DEEP, NF1),
		PAD_CFG_NF(GPD5, NONE, DEEP, NF1),
		PAD_CFG_NF(GPD6, NONE, DEEP, NF1),
		PAD_CFG_GPI(GPD7, NONE, PWROK),
		PAD_CFG_NF(GPD8, NONE, DEEP, NF1),
		PAD_CFG_NF(GPD9, NONE, PWROK, NF1),
		_PAD_CFG_STRUCT(GPD10, 0x04000601, 0x0000),
		PAD_CFG_NF(GPD11, NONE, DEEP, NF1),
		PAD_CFG_GPI(GPP_A0, NONE, DEEP),
		PAD_CFG_NF(GPP_A1, NATIVE, DEEP, NF1),
		PAD_CFG_NF(GPP_A2, NATIVE, DEEP, NF1),
		PAD_CFG_NF(GPP_A3, NATIVE, DEEP, NF1),
		PAD_CFG_NF(GPP_A4, NATIVE, DEEP, NF1),
		PAD_CFG_NF(GPP_A5, NONE, DEEP, NF1),
		PAD_CFG_NF(GPP_A6, NONE, DEEP, NF1),
		PAD_CFG_GPI(GPP_A7, NONE, DEEP),
		PAD_CFG_NF(GPP_A8, NONE, DEEP, NF1),
		PAD_CFG_NF(GPP_A9, DN_20K, DEEP, NF1),
		PAD_CFG_NF(GPP_A10, DN_20K, DEEP, NF1),
		PAD_CFG_GPI(GPP_A11, UP_20K, DEEP),
		PAD_CFG_GPI(GPP_A12, NONE, DEEP),
		PAD_CFG_NF(GPP_A13, NONE, DEEP, NF1),
		PAD_CFG_GPI_APIC(GPP_A14, NONE, PLTRST, EDGE_SINGLE, INVERT), // TCHPD_INT#
		PAD_CFG_NF(GPP_A15, UP_20K, DEEP, NF1),
		PAD_CFG_GPI(GPP_A16, DN_20K, DEEP),
		PAD_CFG_GPI(GPP_A17, NONE, DEEP),
		PAD_CFG_TERM_GPO(GPP_A18, 1, NONE, DEEP),
		PAD_CFG_GPI(GPP_A19, NONE, DEEP),
		PAD_CFG_GPI(GPP_A20, NONE, DEEP),
		_PAD_CFG_STRUCT(GPP_A21, 0x46080100, 0x0000),
		PAD_CFG_TERM_GPO(GPP_A22, 0, NONE, DEEP),
		PAD_CFG_GPI(GPP_A23, NONE, DEEP),
		_PAD_CFG_STRUCT(GPP_B0, 0x42080100, 0x3000),
		PAD_CFG_GPI(GPP_B1, NONE, DEEP),
		PAD_CFG_GPI(GPP_B2, NONE, DEEP),
		PAD_CFG_TERM_GPO(GPP_B3, 1, NONE, DEEP),
		PAD_CFG_GPI(GPP_B4, NONE, DEEP),
		PAD_CFG_NF(GPP_B5, NONE, DEEP, NF1),
		PAD_CFG_GPI(GPP_B6, NONE, DEEP),
		PAD_CFG_TERM_GPO(GPP_B7, 0, NONE, DEEP),
		PAD_CFG_GPI(GPP_B8, NONE, DEEP),
		PAD_CFG_GPI(GPP_B9, NONE, DEEP),
		PAD_CFG_NF(GPP_B10, NONE, DEEP, NF1),
		PAD_CFG_GPI(GPP_B11, NONE, DEEP),
		PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1),
		_PAD_CFG_STRUCT(GPP_B13, 0x44000601, 0x0000),
		PAD_CFG_NF(GPP_B14, NONE, DEEP, NF1),
		PAD_CFG_GPI(GPP_B15, NONE, DEEP),
		PAD_CFG_GPI(GPP_B16, NONE, DEEP),
		PAD_CFG_GPI(GPP_B17, NONE, DEEP),
		PAD_CFG_GPI(GPP_B18, NONE, DEEP),
		PAD_CFG_GPI(GPP_B19, NONE, DEEP),
		_PAD_CFG_STRUCT(GPP_B20, 0x42840101, 0x0000),
		PAD_CFG_GPI(GPP_B21, NONE, DEEP),
		PAD_CFG_GPI(GPP_B22, NONE, DEEP),
		PAD_CFG_GPI(GPP_B23, NONE, DEEP),
		PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),
		PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1),
		PAD_CFG_GPI(GPP_C2, NONE, DEEP),
		PAD_CFG_GPI(GPP_C3, NONE, DEEP),
		PAD_CFG_GPI(GPP_C4, NONE, DEEP),
		PAD_CFG_GPI(GPP_C5, NONE, DEEP),
		PAD_CFG_GPI(GPP_C6, NONE, DEEP),
		PAD_CFG_GPI(GPP_C7, NONE, DEEP),
		PAD_CFG_GPI(GPP_C8, NONE, DEEP),
		PAD_CFG_GPI(GPP_C9, NONE, DEEP),
		PAD_CFG_GPI(GPP_C10, NONE, DEEP),
		PAD_CFG_GPI(GPP_C11, NONE, DEEP), // TBT_DET#
		PAD_CFG_GPI(GPP_C12, NONE, DEEP), // GC6_FB_EN_PCH
		PAD_CFG_GPI(GPP_C13, NONE, PLTRST),
		PAD_CFG_TERM_GPO(GPP_C14, 1, NONE, DEEP),
		PAD_CFG_GPI(GPP_C15, NONE, DEEP),
		PAD_CFG_NF(GPP_C16, NONE, PLTRST, NF1),
		PAD_CFG_NF(GPP_C17, NONE, PLTRST, NF1),
		PAD_CFG_NF(GPP_C18, NONE, PLTRST, NF1),
		PAD_CFG_NF(GPP_C19, NONE, PLTRST, NF1),
		PAD_CFG_GPI(GPP_C20, NONE, DEEP),
		PAD_CFG_GPI(GPP_C21, NONE, DEEP),
		PAD_CFG_GPI(GPP_C22, NONE, DEEP),
		PAD_CFG_GPI(GPP_C23, NONE, DEEP),
		PAD_CFG_GPI(GPP_D0, NONE, DEEP),
		PAD_CFG_GPI(GPP_D1, NONE, DEEP),
		PAD_CFG_GPI(GPP_D2, NONE, DEEP),
		PAD_CFG_GPI(GPP_D3, NONE, DEEP),
		PAD_CFG_GPI(GPP_D4, NONE, DEEP),
		PAD_CFG_NF(GPP_D5, NONE, DEEP, NF3),
		PAD_CFG_NF(GPP_D6, NONE, DEEP, NF3),
		PAD_CFG_GPI(GPP_D7, NONE, DEEP),
		PAD_CFG_GPI(GPP_D8, NONE, DEEP),
		PAD_CFG_GPI(GPP_D9, NONE, DEEP),
		PAD_CFG_GPI(GPP_D10, NONE, DEEP),
		PAD_CFG_GPI(GPP_D11, NONE, DEEP),
		PAD_CFG_GPI(GPP_D12, NONE, DEEP),
		PAD_CFG_GPI(GPP_D13, NONE, DEEP),
		PAD_CFG_GPI(GPP_D14, NONE, DEEP),
		PAD_CFG_GPI(GPP_D15, NONE, DEEP),
		PAD_CFG_GPI(GPP_D16, NONE, DEEP),
		PAD_CFG_NF(GPP_D17, NONE, DEEP, NF1),
		PAD_CFG_NF(GPP_D18, NONE, DEEP, NF1),
		PAD_CFG_NF(GPP_D19, NONE, DEEP, NF1),
		PAD_CFG_NF(GPP_D20, NONE, DEEP, NF1),
		PAD_CFG_GPI(GPP_D21, NONE, DEEP),
		PAD_CFG_GPI(GPP_D22, NONE, DEEP),
		PAD_CFG_GPI(GPP_D23, NONE, DEEP),
		PAD_CFG_GPI(GPP_E0, NONE, DEEP),
		PAD_CFG_NF(GPP_E1, UP_20K, DEEP, NF1),
		PAD_CFG_GPI(GPP_E2, NONE, DEEP),
		PAD_CFG_GPI(GPP_E3, NONE, DEEP),
		PAD_CFG_GPI(GPP_E4, NONE, DEEP),
		PAD_CFG_NF(GPP_E5, NONE, DEEP, NF1),
		PAD_CFG_TERM_GPO(GPP_E6, 1, NONE, DEEP),
		PAD_CFG_GPI(GPP_E7, NONE, DEEP),
		PAD_CFG_NF(GPP_E8, NONE, DEEP, NF1),
		PAD_CFG_GPI(GPP_E9, NONE, DEEP),
		PAD_CFG_GPI(GPP_E10, NONE, DEEP),
		PAD_CFG_GPI(GPP_E11, NONE, DEEP),
		PAD_CFG_GPI(GPP_E12, NONE, DEEP),
		PAD_CFG_GPI(GPP_F0, NONE, DEEP),
		PAD_CFG_GPI(GPP_F1, NONE, DEEP),
		PAD_CFG_GPI(GPP_F2, NONE, DEEP),
		PAD_CFG_TERM_GPO(GPP_F3, 0, NONE, DEEP),
		PAD_CFG_GPI(GPP_F4, NONE, DEEP),
		PAD_CFG_GPI(GPP_F5, NONE, DEEP),
		PAD_CFG_GPI(GPP_F6, NONE, DEEP),
		PAD_CFG_GPI(GPP_F7, NONE, DEEP),
		PAD_CFG_GPI(GPP_F8, NONE, DEEP),
		PAD_CFG_TERM_GPO(GPP_F9, 0, NONE, DEEP), // PS8331_SW
		PAD_CFG_GPI(GPP_F10, NONE, DEEP),
		PAD_CFG_GPI(GPP_F11, NONE, DEEP),
		PAD_CFG_GPI(GPP_F12, NONE, DEEP),
		PAD_CFG_GPI(GPP_F13, NONE, DEEP),
		PAD_CFG_GPI(GPP_F14, NONE, DEEP),
		PAD_CFG_GPI(GPP_F15, NONE, DEEP),
		PAD_CFG_GPI(GPP_F16, NONE, DEEP),
		PAD_CFG_GPI(GPP_F17, NONE, DEEP),
		PAD_CFG_GPI(GPP_F18, NONE, DEEP),
		PAD_CFG_NF(GPP_F19, NONE, DEEP, NF1),
		PAD_CFG_NF(GPP_F20, NONE, DEEP, NF1), // BLON
		PAD_CFG_NF(GPP_F21, NONE, DEEP, NF1),
		PAD_CFG_TERM_GPO(GPP_F22, 1, NONE, DEEP), // DGPU_RST#_PCH
		PAD_CFG_TERM_GPO(GPP_F23, 1, NONE, DEEP), // DGPU_PWR_EN
		PAD_CFG_GPI(GPP_G0, NONE, DEEP),
		PAD_CFG_GPI(GPP_G1, NONE, DEEP),
		PAD_CFG_GPI(GPP_G2, NONE, DEEP),
		PAD_CFG_GPI(GPP_G3, NONE, DEEP),
		PAD_CFG_GPI(GPP_G4, NONE, DEEP),
		PAD_CFG_GPI(GPP_G5, NONE, DEEP),
		PAD_CFG_GPI(GPP_G6, NONE, DEEP),
		PAD_CFG_GPI(GPP_G7, NONE, DEEP),
		PAD_CFG_NF(GPP_H0, NONE, DEEP, NF1), // WLAN_CLKREQ#
		PAD_CFG_NF(GPP_H1, NONE, DEEP, NF1), // CR_CLKREQ#
		PAD_CFG_NF(GPP_H2, NONE, DEEP, NF1), // PEG_CLKREQ#
		PAD_CFG_NF(GPP_H3, NONE, DEEP, NF1), // SSD1_CLKREQ#
		PAD_CFG_NF(GPP_H4, NONE, DEEP, NF1), // SSD2_CLKREQ#
		PAD_CFG_GPI(GPP_H5, NONE, DEEP),
		PAD_CFG_TERM_GPO(GPP_H6, 1, NONE, DEEP), // PCIE_SSD1_RST#
		PAD_CFG_TERM_GPO(GPP_H7, 1, NONE, DEEP), // PCIE_SSD2_RST#
		PAD_CFG_GPI(GPP_H8, NONE, DEEP),
		_PAD_CFG_STRUCT(GPP_H9, 0x40880100, 0x0000),
		PAD_CFG_GPI(GPP_H10, NONE, DEEP),
		PAD_CFG_GPI(GPP_H11, NONE, DEEP),
		PAD_CFG_GPI(GPP_H12, NONE, DEEP),
		PAD_CFG_GPI(GPP_H13, NONE, DEEP), // TBTA_HRESET
		PAD_CFG_GPI(GPP_H14, NONE, DEEP),
		PAD_CFG_GPI(GPP_H15, NONE, DEEP),
		PAD_CFG_TERM_GPO(GPP_H16, 1, NONE, DEEP), // TBT_RTD3_PWR_EN_R
		PAD_CFG_TERM_GPO(GPP_H17, 1, NONE, PLTRST), // TBT_FORCE_PWR_R
		PAD_CFG_GPI(GPP_H18, NONE, DEEP),
		PAD_CFG_TERM_GPO(GPP_H19, 0, NONE, DEEP),
		PAD_CFG_TERM_GPO(GPP_H20, 0, NONE, DEEP),
		PAD_CFG_GPI(GPP_H21, NONE, DEEP),
		PAD_CFG_GPI(GPP_H22, NONE, DEEP),
		_PAD_CFG_STRUCT(GPP_H23, 0x82880100, 0x0000),
		_PAD_CFG_STRUCT(GPP_I0, 0x46080100, 0x0000),
		_PAD_CFG_STRUCT(GPP_I1, 0x46080100, 0x0000),
		_PAD_CFG_STRUCT(GPP_I2, 0x46080100, 0x0000),
		_PAD_CFG_STRUCT(GPP_I3, 0x46080100, 0x0000),
		PAD_CFG_NF(GPP_I4, NONE, DEEP, NF1),
		PAD_CFG_TERM_GPO(GPP_I5, 1, UP_20K, PLTRST), // TBT_GPIO_RST#
		PAD_CFG_GPI(GPP_I6, NONE, DEEP),
		PAD_CFG_GPI(GPP_I7, NONE, DEEP),
		PAD_CFG_TERM_GPO(GPP_I8, 1, NONE, DEEP), // SSD1_PWR_EN
		PAD_CFG_TERM_GPO(GPP_I9, 1, NONE, DEEP), // SSD2_PWR_EN
		PAD_CFG_GPI(GPP_I10, NONE, DEEP),
		PAD_CFG_GPI(GPP_I11, NONE, DEEP),
		PAD_CFG_TERM_GPO(GPP_I12, 1, NONE, DEEP),
		PAD_CFG_GPI(GPP_I13, NONE, DEEP),
		PAD_CFG_GPI(GPP_I14, NONE, DEEP),
		PAD_CFG_NF(GPP_J0, NONE, DEEP, NF1),
		PAD_CFG_TERM_GPO(GPP_J1, 1, NONE, DEEP),
		PAD_CFG_GPI(GPP_J2, NONE, DEEP),
		PAD_CFG_GPI(GPP_J3, NONE, DEEP),
		PAD_CFG_NF(GPP_J4, NONE, DEEP, NF1),
		PAD_CFG_NF(GPP_J5, UP_20K, DEEP, NF1),
		PAD_CFG_NF(GPP_J6, NONE, DEEP, NF1),
		PAD_CFG_NF(GPP_J7, UP_20K, DEEP, NF1),
		PAD_CFG_NF(GPP_J8, NONE, DEEP, NF1),
		PAD_CFG_NF(GPP_J9, NONE, DEEP, NF1),
		PAD_CFG_GPI(GPP_J10, NONE, DEEP),
		PAD_CFG_GPI(GPP_J11, DN_20K, DEEP),
		PAD_CFG_TERM_GPO(GPP_K0, 0, NONE, DEEP),
		PAD_CFG_TERM_GPO(GPP_K1, 0, NONE, DEEP),
		PAD_CFG_GPI(GPP_K2, NONE, DEEP),
		_PAD_CFG_STRUCT(GPP_K3, 0x40880100, 0x0000),
		_PAD_CFG_STRUCT(GPP_K4, 0x44000101, 0x0000),
		PAD_CFG_GPI(GPP_K5, NONE, DEEP),
		_PAD_CFG_STRUCT(GPP_K6, 0x40880100, 0x0000),
		PAD_CFG_GPI(GPP_K7, NONE, DEEP),
		PAD_CFG_TERM_GPO(GPP_K8, 0, NONE, DEEP),
		PAD_CFG_GPI(GPP_K9, NONE, DEEP),
		PAD_CFG_GPI(GPP_K10, NONE, DEEP),
		PAD_CFG_GPI(GPP_K11, NONE, DEEP),
		PAD_CFG_TERM_GPO(GPP_K12, 0, NONE, DEEP),
		PAD_CFG_GPI(GPP_K13, NONE, DEEP),
		PAD_CFG_TERM_GPO(GPP_K14, 0, NONE, DEEP),
		_PAD_CFG_STRUCT(GPP_K15, 0x80100100, 0x0000),
		PAD_CFG_GPI(GPP_K16, NONE, DEEP),
		PAD_CFG_GPI(GPP_K17, NONE, DEEP),
		PAD_CFG_TERM_GPO(GPP_K18, 1, NONE, DEEP),
		PAD_CFG_GPI(GPP_K19, NONE, DEEP),
		PAD_CFG_GPI(GPP_K20, NONE, DEEP),
		PAD_CFG_GPI(GPP_K21, NONE, DEEP),
		PAD_CFG_TERM_GPO(GPP_K22, 0, NONE, DEEP),
		PAD_CFG_GPI(GPP_K23, NONE, DEEP),
};

#endif

#endif
