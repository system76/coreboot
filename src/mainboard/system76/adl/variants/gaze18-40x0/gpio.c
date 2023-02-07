/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/gpio.h>
#include <soc/gpio.h>

static const struct pad_config gpio_table[] = {
	/* ------- GPIO Group GPD ------- */
	PAD_CFG_NF(GPD0, UP_20K, PWROK, NF1),
	PAD_CFG_NF(GPD1, NATIVE, PWROK, NF1),
	PAD_CFG_NF(GPD2, NONE, PWROK, NF1),
	PAD_CFG_NF(GPD3, UP_20K, PWROK, NF1),
	PAD_CFG_NF(GPD4, NONE, PWROK, NF1),
	PAD_CFG_NF(GPD5, NONE, PWROK, NF1),
	PAD_CFG_NF(GPD6, NONE, PWROK, NF1),
	PAD_CFG_GPO(GPD7, 0, PWROK),
	PAD_CFG_NF(GPD8, NONE, PWROK, NF1),
	PAD_CFG_NF(GPD9, NONE, PWROK, NF1),
	PAD_CFG_NF(GPD10, NONE, PWROK, NF1),
	PAD_CFG_NF(GPD11, NONE, PWROK, NF1),
	_PAD_CFG_STRUCT(GPD12, 0x04000300, 0x0000),

	/* ------- GPIO Group GPP_A ------- */
	PAD_CFG_NF(GPP_A0, UP_20K, DEEP, NF1),
	PAD_CFG_NF(GPP_A1, UP_20K, DEEP, NF1),
	PAD_CFG_NF(GPP_A2, UP_20K, DEEP, NF1),
	PAD_CFG_NF(GPP_A3, UP_20K, DEEP, NF1),
	PAD_CFG_NF(GPP_A4, UP_20K, DEEP, NF1),
	PAD_CFG_NF(GPP_A5, DN_20K, DEEP, NF1),
	PAD_CFG_NF(GPP_A6, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_A7, UP_20K, DEEP, NF1),
	PAD_CFG_NF(GPP_A8, UP_20K, DEEP, NF1),
	PAD_CFG_NF(GPP_A9, UP_20K, DEEP, NF1),
	PAD_CFG_NF(GPP_A10, UP_20K, DEEP, NF1),
	PAD_CFG_NF(GPP_A11, UP_20K, DEEP, NF1),
	PAD_CFG_NF(GPP_A12, UP_20K, DEEP, NF1),
	PAD_CFG_NF(GPP_A13, UP_20K, DEEP, NF1),
	PAD_NC(GPP_A14, NONE),

	/* ------- GPIO Group GPP_B ------- */
	_PAD_CFG_STRUCT(GPP_B0, 0x40100100, 0x0000),
	PAD_NC(GPP_B1, NONE),
	PAD_CFG_GPI(GPP_B2, NONE, DEEP),
	PAD_CFG_GPO(GPP_B3, 1, DEEP),
	PAD_NC(GPP_B4, NONE),
	PAD_NC(GPP_B5, NONE),
	PAD_NC(GPP_B6, NONE),
	PAD_NC(GPP_B7, NONE),
	PAD_NC(GPP_B8, NONE),
	PAD_NC(GPP_B9, NONE),
	PAD_NC(GPP_B10, NONE),
	PAD_NC(GPP_B11, NONE),
	PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),
	PAD_CFG_GPO(GPP_B14, 0, DEEP),
	PAD_CFG_GPI(GPP_B15, NONE, DEEP),
	PAD_NC(GPP_B16, NONE),
	PAD_NC(GPP_B17, NONE),
	PAD_CFG_NF(GPP_B18, NONE, PWROK, NF1),
	PAD_CFG_GPO(GPP_B19, 1, DEEP),
	PAD_CFG_NF(GPP_B20, DN_20K, DEEP, NF1),
	_PAD_CFG_STRUCT(GPP_B21, 0x42880100, 0x0000),
	PAD_CFG_GPO(GPP_B22, 1, DEEP),
	PAD_CFG_GPO(GPP_B23, 0, DEEP),

	/* ------- GPIO Group GPP_C ------- */
	PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1),
	PAD_CFG_GPO(GPP_C2, 0, DEEP),
	PAD_CFG_NF(GPP_C3, NONE, DEEP, NF3),
	PAD_CFG_NF(GPP_C4, NONE, DEEP, NF3),
	PAD_CFG_GPO(GPP_C5, 0, DEEP),
	PAD_CFG_NF(GPP_C6, NONE, DEEP, NF2),
	PAD_CFG_NF(GPP_C7, NONE, DEEP, NF2),
	PAD_CFG_GPI(GPP_C8, NONE, DEEP),
	PAD_NC(GPP_C9, NONE),
	PAD_CFG_GPO(GPP_C10, 1, DEEP),
	PAD_CFG_GPO(GPP_C11, 1, DEEP),
	PAD_NC(GPP_C12, NONE),
	PAD_NC(GPP_C13, NONE),
	PAD_NC(GPP_C14, NONE),
	PAD_NC(GPP_C15, NONE),
	PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_C18, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_C19, NONE, DEEP, NF1),
	PAD_NC(GPP_C20, NONE),
	PAD_NC(GPP_C21, NONE),
	PAD_NC(GPP_C22, NONE),
	PAD_NC(GPP_C23, NONE),

	/* ------- GPIO Group GPP_D ------- */
	PAD_NC(GPP_D0, NONE),
	PAD_NC(GPP_D1, NONE),
	PAD_NC(GPP_D2, NONE),
	PAD_NC(GPP_D3, NONE),
	PAD_NC(GPP_D4, NONE),
	PAD_NC(GPP_D5, NONE),
	PAD_NC(GPP_D6, NONE),
	PAD_NC(GPP_D7, NONE),
	PAD_NC(GPP_D8, NONE),
	PAD_CFG_NF(GPP_D9, NATIVE, DEEP, NF1),
	PAD_CFG_NF(GPP_D10, NATIVE, DEEP, NF1),
	PAD_CFG_NF(GPP_D11, NATIVE, DEEP, NF1),
	PAD_CFG_NF(GPP_D12, NATIVE, DEEP, NF1),
	PAD_CFG_NF(GPP_D13, NATIVE, DEEP, NF1),
	PAD_CFG_NF(GPP_D14, NATIVE, DEEP, NF1),
	PAD_CFG_NF(GPP_D15, NATIVE, DEEP, NF1),
	PAD_CFG_NF(GPP_D16, NATIVE, DEEP, NF1),
	PAD_CFG_NF(GPP_D17, NATIVE, DEEP, NF1),
	PAD_CFG_NF(GPP_D18, NATIVE, DEEP, NF1),
	PAD_CFG_NF(GPP_D19, NATIVE, DEEP, NF1),
	PAD_CFG_NF(GPP_D20, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_D21, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_D22, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_D23, NONE, DEEP, NF1),

	/* ------- GPIO Group GPP_E ------- */
	PAD_NC(GPP_E0, NONE),
	PAD_NC(GPP_E1, NONE),
	PAD_NC(GPP_E2, NONE),
	_PAD_CFG_STRUCT(GPP_E3, 0x42840101, 0x0000),
	PAD_NC(GPP_E4, NONE),
	PAD_NC(GPP_E5, NONE),
	PAD_NC(GPP_E6, NONE),
	_PAD_CFG_STRUCT(GPP_E7, 0x80100100, 0x0000),
	PAD_CFG_NF(GPP_E8, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_E9, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_E10, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_E11, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_E12, NONE, DEEP, NF1),
	PAD_NC(GPP_E13, NONE),
	PAD_NC(GPP_E14, NONE),
	PAD_CFG_GPO(GPP_E15, 0, DEEP),
	PAD_NC(GPP_E16, NONE),
	PAD_CFG_GPI(GPP_E17, DN_20K, DEEP),
	PAD_CFG_GPO(GPP_E18, 1, DEEP),
	PAD_NC(GPP_E19, NONE),
	PAD_NC(GPP_E20, NONE),
	PAD_NC(GPP_E21, NONE),

	/* ------- GPIO Group GPP_F ------- */
	PAD_CFG_NF(GPP_F0, NONE, DEEP, NF2),
	PAD_NC(GPP_F1, NONE),
	PAD_CFG_GPO(GPP_F2, 1, PLTRST),
	PAD_CFG_GPO(GPP_F3, 1, PLTRST),
	PAD_CFG_GPO(GPP_F4, 1, PLTRST),
	PAD_CFG_NF(GPP_F5, NONE, DEEP, NF1),
	PAD_NC(GPP_F6, NONE),
	PAD_CFG_GPI(GPP_F7, NONE, PLTRST),
	PAD_CFG_GPI(GPP_F8, NONE, DEEP),
	PAD_CFG_GPO(GPP_F9, 1, DEEP),
	PAD_NC(GPP_F10, NONE),
	PAD_NC(GPP_F11, NONE),
	PAD_NC(GPP_F12, NONE),
	PAD_NC(GPP_F13, NONE),
	PAD_CFG_NF(GPP_F14, NONE, DEEP, NF1),
	PAD_CFG_GPI(GPP_F15, NONE, DEEP),
	PAD_NC(GPP_F16, NONE),
	PAD_CFG_GPI(GPP_F17, NONE, DEEP),
	PAD_CFG_GPO(GPP_F18, 0, PLTRST),
	PAD_CFG_NF(GPP_F19, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_F20, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_F21, NONE, DEEP, NF1),
	PAD_NC(GPP_F22, NONE),
	PAD_NC(GPP_F23, NONE),

	/* ------- GPIO Group GPP_G ------- */
	PAD_CFG_GPI(GPP_G0, NONE, DEEP),
	PAD_CFG_GPI(GPP_G1, NONE, DEEP),
	PAD_CFG_NF(GPP_G2, DN_20K, DEEP, NF1),
	PAD_CFG_GPI(GPP_G3, NONE, DEEP),
	PAD_CFG_GPI(GPP_G4, NONE, DEEP),
	PAD_CFG_NF(GPP_G5, NONE, DEEP, NF1),
	PAD_CFG_GPI(GPP_G6, NONE, DEEP),
	PAD_CFG_GPI(GPP_G7, NONE, DEEP),

	/* ------- GPIO Group GPP_H ------- */
	PAD_NC(GPP_H0, NONE),
	PAD_CFG_GPI(GPP_H1, NONE, DEEP),
	PAD_CFG_NF(GPP_H2, NONE, DEEP, NF1),
	PAD_NC(GPP_H3, NONE),
	PAD_CFG_NF(GPP_H4, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_H5, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_H7, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_H8, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_H9, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_H10, NONE, PLTRST, NF1),
	PAD_CFG_NF(GPP_H11, NONE, PLTRST, NF1),
	PAD_CFG_GPO(GPP_H12, 0, DEEP),
	PAD_CFG_NF(GPP_H13, NONE, PLTRST, NF1),
	PAD_CFG_NF(GPP_H14, NONE, PLTRST, NF1),
	PAD_CFG_NF(GPP_H15, NONE, PLTRST, NF1),
	PAD_CFG_NF(GPP_H16, NONE, PLTRST, NF1),
	PAD_CFG_GPO(GPP_H17, 1, PLTRST),
	PAD_CFG_GPO(GPP_H18, 0, DEEP),
	PAD_NC(GPP_H19, NONE),
	PAD_NC(GPP_H20, NONE),
	PAD_CFG_GPO(GPP_H21, 0, DEEP),
	PAD_CFG_GPO(GPP_H22, 0, DEEP),
	PAD_NC(GPP_H23, NONE),

	/* ------- GPIO Group GPP_I ------- */
	PAD_CFG_GPI(GPP_I0, NONE, DEEP),
	PAD_CFG_NF(GPP_I1, NONE, DEEP, NF1),
	_PAD_CFG_STRUCT(GPP_I2, 0x86800100, 0x0000),
	PAD_CFG_NF(GPP_I3, NONE, DEEP, NF1),
	_PAD_CFG_STRUCT(GPP_I4, 0x86800100, 0x0000),
	PAD_CFG_GPO(GPP_I5, 1, PLTRST),
	PAD_CFG_GPO(GPP_I6, 0, DEEP),
	PAD_NC(GPP_I7, NONE),
	PAD_CFG_GPO(GPP_I8, 0, DEEP),
	PAD_NC(GPP_I9, NONE),
	PAD_NC(GPP_I10, NONE),
	PAD_CFG_NF(GPP_I11, NONE, PLTRST, NF1),
	PAD_CFG_NF(GPP_I12, NONE, PLTRST, NF1),
	PAD_CFG_NF(GPP_I13, NONE, PLTRST, NF1),
	PAD_CFG_NF(GPP_I14, NONE, PLTRST, NF1),
	PAD_NC(GPP_I15, NONE),
	PAD_NC(GPP_I16, NONE),
	PAD_NC(GPP_I17, NONE),
	PAD_CFG_GPO(GPP_I18, 0, DEEP),
	PAD_NC(GPP_I19, NONE),
	PAD_NC(GPP_I20, NONE),
	PAD_NC(GPP_I21, NONE),
	PAD_CFG_GPO(GPP_I22, 0, DEEP),

	/* ------- GPIO Group GPP_J ------- */
	PAD_CFG_NF(GPP_J0, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_J1, NONE, PLTRST, NF1),
	PAD_CFG_NF(GPP_J2, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_J3, UP_20K, DEEP, NF1),
	PAD_CFG_NF(GPP_J4, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_J5, UP_20K, DEEP, NF1),
	PAD_CFG_NF(GPP_J6, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_J7, NONE, DEEP, NF1),
	PAD_NC(GPP_J8, NONE),
	PAD_NC(GPP_J9, NONE),
	PAD_CFG_NF(GPP_J10, DN_20K, DEEP, NF1),
	PAD_CFG_NF(GPP_J11, DN_20K, DEEP, NF1),

	/* ------- GPIO Group GPP_K ------- */
	_PAD_CFG_STRUCT(GPP_K0, 0x42800100, 0x0000),
	PAD_NC(GPP_K1, NONE),
	PAD_NC(GPP_K2, NONE),
	PAD_CFG_GPO(GPP_K3, 1, PLTRST),
	PAD_CFG_GPO(GPP_K4, 0, PWROK),
	PAD_NC(GPP_K5, NONE),
	PAD_CFG_NF(GPP_K6, UP_20K, DEEP, NF2),
	PAD_CFG_NF(GPP_K7, DN_20K, DEEP, NF2),
	PAD_CFG_NF(GPP_K8, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_K9, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_K10, UP_20K, DEEP, NF2),
	PAD_NC(GPP_K11, NONE),

	/* ------- GPIO Group GPP_R ------- */
	PAD_CFG_NF(GPP_R0, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_R1, NATIVE, DEEP, NF1),
	PAD_CFG_NF(GPP_R2, NATIVE, DEEP, NF1),
	PAD_CFG_NF(GPP_R3, NATIVE, DEEP, NF1),
	PAD_CFG_NF(GPP_R4, NONE, DEEP, NF1),
	PAD_NC(GPP_R5, NONE),
	PAD_NC(GPP_R6, NONE),
	PAD_NC(GPP_R7, NONE),
	PAD_CFG_GPI(GPP_R8, NONE, PLTRST),
	PAD_CFG_NF(GPP_R9, NONE, DEEP, NF1),
	PAD_NC(GPP_R10, NONE),
	PAD_NC(GPP_R11, NONE),
	PAD_NC(GPP_R12, NONE),
	PAD_NC(GPP_R13, NONE),
	PAD_NC(GPP_R14, NONE),
	PAD_NC(GPP_R15, NONE),
	PAD_CFG_GPO(GPP_R16, 1, DEEP),
	PAD_NC(GPP_R17, NONE),
	PAD_NC(GPP_R18, NONE),
	PAD_NC(GPP_R19, NONE),
	PAD_NC(GPP_R20, NONE),
	PAD_CFG_GPO(GPP_R21, 0, DEEP),

	/* ------- GPIO Group GPP_S ------- */
	PAD_NC(GPP_S0, NONE),
	PAD_NC(GPP_S1, NONE),
	PAD_NC(GPP_S2, NONE),
	PAD_NC(GPP_S3, NONE),
	PAD_NC(GPP_S4, NONE),
	PAD_NC(GPP_S5, NONE),
	PAD_CFG_NF(GPP_S6, NONE, DEEP, NF2),
	PAD_CFG_NF(GPP_S7, NONE, DEEP, NF2),
};

void mainboard_configure_gpios(void)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
