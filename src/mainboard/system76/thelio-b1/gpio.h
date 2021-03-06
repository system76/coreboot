/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_GPIO_H
#define MAINBOARD_GPIO_H

#include <soc/gpe.h>
#include <soc/gpio.h>

#ifndef __ACPI__

/* Pad configuration in ramstage. */
static const struct pad_config gpio_table[] = {
	/* ------- GPIO Group GPD ------- */
	_PAD_CFG_STRUCT(GPD0, 0x4000600, 0x0), // N_-BATLOW
	_PAD_CFG_STRUCT(GPD1, 0x4000700, 0x3c00), // N_GP_D1
	_PAD_CFG_STRUCT(GPD2, 0x80500, 0x3c00), // N_-LAN_WAKE
	_PAD_CFG_STRUCT(GPD3, 0x4000500, 0x3000), // O_PWRBTSW
	_PAD_CFG_STRUCT(GPD4, 0x4000600, 0x0), // N_-SLP_S3
	_PAD_CFG_STRUCT(GPD5, 0x4000600, 0x0), // N_-S4_S5
	_PAD_CFG_STRUCT(GPD6, 0x4000600, 0x0), // N_-SLP_A
	_PAD_CFG_STRUCT(GPD7, 0x4000200, 0x0), // N_GPD_7
	_PAD_CFG_STRUCT(GPD8, 0x4000600, 0x0), // N_SUSCLK
	_PAD_CFG_STRUCT(GPD9, 0x4000600, 0x0), // NC
	_PAD_CFG_STRUCT(GPD10, 0x4000600, 0x0), // N_-SLP_S5
	_PAD_CFG_STRUCT(GPD11, 0x4000600, 0x0), // N_-LAN_DIS

	/* ------- GPIO Group A ------- */
	_PAD_CFG_STRUCT(GPP_A0, 0x44000700, 0x0), // N_-KBRST
	_PAD_CFG_STRUCT(GPP_A1, 0x44000700, 0x3c00), // N_LAD0
	_PAD_CFG_STRUCT(GPP_A2, 0x44000700, 0x3c00), // N_LAD1
	_PAD_CFG_STRUCT(GPP_A3, 0x44000700, 0x3c00), // N_LAD2
	_PAD_CFG_STRUCT(GPP_A4, 0x44000700, 0x3c00), // N_LAD3
	_PAD_CFG_STRUCT(GPP_A5, 0x44000700, 0x0), // N_-LFRAME
	_PAD_CFG_STRUCT(GPP_A6, 0x44000700, 0x0), // N_SERIRQ
	_PAD_CFG_STRUCT(GPP_A7, 0x44000700, 0x0), // N_-LDRQ0
	_PAD_CFG_STRUCT(GPP_A8, 0x44000700, 0x0), // N_GPP_A8
	_PAD_CFG_STRUCT(GPP_A9, 0x44000700, 0x1000), // T_TPMCLK
	_PAD_CFG_STRUCT(GPP_A10, 0x44000700, 0x1000), // N_LPC24MA
	_PAD_CFG_STRUCT(GPP_A11, 0x44000700, 0x3000), // NC
	_PAD_CFG_STRUCT(GPP_A12, 0x44000300, 0x0), // N_GPP_A12
	_PAD_CFG_STRUCT(GPP_A13, 0x44000700, 0x0), // N_-S_WARN
	_PAD_CFG_STRUCT(GPP_A14, 0x44000700, 0x0), // N_GPP_A14
	_PAD_CFG_STRUCT(GPP_A15, 0x44000700, 0x3000), // N_-S_ACK
	_PAD_CFG_STRUCT(GPP_A16, 0x44000300, 0x0), // N_GPP_A16
	_PAD_CFG_STRUCT(GPP_A17, 0x84000201, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_A18, 0x84000201, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_A19, 0x84000201, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_A20, 0x84000201, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_A21, 0x84000201, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_A22, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_A23, 0x44000300, 0x0), // NC

	/* ------- GPIO Group B ------- */
	_PAD_CFG_STRUCT(GPP_B0, 0x40000700, 0x0), // N_-DDR_V_SEL
	_PAD_CFG_STRUCT(GPP_B1, 0x44000300, 0x0), // NTP104
	_PAD_CFG_STRUCT(GPP_B2, 0x44000300, 0x0), // N_-VRALERT
	_PAD_CFG_STRUCT(GPP_B3, 0x44000300, 0x0), // N_GPP_B3
	_PAD_CFG_STRUCT(GPP_B4, 0x84000101, 0x0), // N_GPP_B4
	_PAD_CFG_STRUCT(GPP_B5, 0x84000101, 0x0), // -PCIEX16_PR
	_PAD_CFG_STRUCT(GPP_B6, 0x44000300, 0x0), // N_GPP_B6
	_PAD_CFG_STRUCT(GPP_B7, 0x44000300, 0x0), // N_GPP_B7
	_PAD_CFG_STRUCT(GPP_B8, 0x44000300, 0x0), // M2Q_-CLKREQ
	_PAD_CFG_STRUCT(GPP_B9, 0x44000700, 0x0), // N_GPP_B9
	_PAD_CFG_STRUCT(GPP_B10, 0x44000300, 0x0), // M2P_-CLKREQ
	_PAD_CFG_STRUCT(GPP_B11, 0x44000700, 0x0), // NTP106
	_PAD_CFG_STRUCT(GPP_B12, 0x4000100, 0x0), // N_-SLP_S0
	_PAD_CFG_STRUCT(GPP_B13, 0x44000700, 0x0), // N_-PFMRST
	_PAD_CFG_STRUCT(GPP_B14, 0x44000700, 0x0), // N_SPKR
	_PAD_CFG_STRUCT(GPP_B15, 0x44000600, 0x0), // N_GPP_B15
	_PAD_CFG_STRUCT(GPP_B16, 0x84000101, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_B17, 0x84000101, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_B18, 0x84000101, 0x0), // N_GPP_B18
	_PAD_CFG_STRUCT(GPP_B19, 0x44000200, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_B20, 0x4000100, 0x0), // N_GPP_B20
	_PAD_CFG_STRUCT(GPP_B21, 0x82000301, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_B22, 0x84000101, 0x0), // N_GPP_B22
	_PAD_CFG_STRUCT(GPP_B23, 0x44000200, 0x0), // N_-PCH_HOT

	/* ------- GPIO Group C ------- */
	_PAD_CFG_STRUCT(GPP_C0, 0x44000700, 0x0), // N_SMBCLK
	_PAD_CFG_STRUCT(GPP_C1, 0x44000700, 0x0), // N_SMBDATA
	_PAD_CFG_STRUCT(GPP_C2, 0x44000200, 0x0), // N_-LPCPME
	_PAD_CFG_STRUCT(GPP_C3, 0x44000700, 0x0), // N_SML0CLK
	_PAD_CFG_STRUCT(GPP_C4, 0x44000700, 0x0), // N_SML0DAT
	_PAD_CFG_STRUCT(GPP_C5, 0x44000200, 0x0), // N_GPP_C5
	_PAD_CFG_STRUCT(GPP_C6, 0x44000300, 0x0), // N_SML1CLK
	_PAD_CFG_STRUCT(GPP_C7, 0x44000300, 0x0), // N_SML1DAT
	_PAD_CFG_STRUCT(GPP_C8, 0x44000300, 0x0), // N_GPP_C8
	_PAD_CFG_STRUCT(GPP_C9, 0x44000300, 0x0), // N_GPP_C9
	_PAD_CFG_STRUCT(GPP_C10, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_C11, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_C12, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_C13, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_C14, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_C15, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_C16, 0x44000300, 0x0), // NTP127
	_PAD_CFG_STRUCT(GPP_C17, 0x44000300, 0x0), // NTP126
	_PAD_CFG_STRUCT(GPP_C18, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_C19, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_C20, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_C21, 0x44000300, 0x0), // N_GPP_C21
	_PAD_CFG_STRUCT(GPP_C22, 0x44000300, 0x0), // N_GPP_C22
	_PAD_CFG_STRUCT(GPP_C23, 0x44000300, 0x0), // N_GPP_C23

	/* ------- GPIO Group D ------- */
	_PAD_CFG_STRUCT(GPP_D0, 0x84000200, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_D1, 0x84000200, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_D2, 0x84000200, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_D3, 0x44000100, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_D4, 0x44000300, 0x0), // N_GPP_D_4
	_PAD_CFG_STRUCT(GPP_D5, 0x44000f00, 0x0), // -CNVI_RF_RESET
	_PAD_CFG_STRUCT(GPP_D6, 0x44000f00, 0x0), // CNVI_CLKREQ
	_PAD_CFG_STRUCT(GPP_D7, 0x44000f00, 0x0), // CNVI_PCM_IN
	_PAD_CFG_STRUCT(GPP_D8, 0x44000f00, 0x0), // -CNVI_PCM_CLK
	_PAD_CFG_STRUCT(GPP_D9, 0x84000200, 0x0), // N_GPP_D9
	_PAD_CFG_STRUCT(GPP_D10, 0x44000300, 0x0), // N_GPP_D10
	_PAD_CFG_STRUCT(GPP_D11, 0x44001700, 0x0), // N_GPP_D11
	_PAD_CFG_STRUCT(GPP_D12, 0x44001700, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_D13, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_D14, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_D15, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_D16, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_D17, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_D18, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_D19, 0x4000200, 0x0), // N_GPP_D19
	_PAD_CFG_STRUCT(GPP_D20, 0x44000300, 0x0), // N_GPP_D20
	_PAD_CFG_STRUCT(GPP_D21, 0x44000300, 0x0), // N_GPP_D21
	_PAD_CFG_STRUCT(GPP_D22, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_D23, 0x44000300, 0x0), // N_GPP_D23

	/* ------- GPIO Group E ------- */
	_PAD_CFG_STRUCT(GPP_E0, 0x84000500, 0x3000), // N_GPP_E0
	_PAD_CFG_STRUCT(GPP_E1, 0x84000500, 0x3000), // N_GPP_E1
	_PAD_CFG_STRUCT(GPP_E2, 0x84000500, 0x3000), // N_GPP_E2
	_PAD_CFG_STRUCT(GPP_E3, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_E4, 0x44000500, 0x0), // N_DEVSLP0
	_PAD_CFG_STRUCT(GPP_E5, 0x44000500, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_E6, 0x44000500, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_E7, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_E8, 0x44000700, 0x0), // N_-SATALED
	_PAD_CFG_STRUCT(GPP_E9, 0x44000500, 0x0), // N_-USBOC_R
	_PAD_CFG_STRUCT(GPP_E10, 0x44000500, 0x0), // N_-USBOC_F
	_PAD_CFG_STRUCT(GPP_E11, 0x44000500, 0x0), // N_-USBOC_F
	_PAD_CFG_STRUCT(GPP_E12, 0x44000500, 0x0), // N_-USBOC_R

	/* ------- GPIO Group F ------- */
	_PAD_CFG_STRUCT(GPP_F0, 0x84000500, 0x3000), // N_GPP_F0
	_PAD_CFG_STRUCT(GPP_F1, 0x84000500, 0x3000), // N_GPP_F1
	_PAD_CFG_STRUCT(GPP_F2, 0x84000500, 0x3000), // N_GPP_F2
	_PAD_CFG_STRUCT(GPP_F3, 0x44000700, 0x0), // N_GPP_F3
	_PAD_CFG_STRUCT(GPP_F4, 0x44000700, 0x0), // N_GPP_F4
	_PAD_CFG_STRUCT(GPP_F5, 0x44000500, 0x0), // N_GPP_F5
	_PAD_CFG_STRUCT(GPP_F6, 0x44000500, 0x0), // N_DEVSLP4
	_PAD_CFG_STRUCT(GPP_F7, 0x44000500, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_F8, 0x44000500, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_F9, 0x44000500, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_F10, 0x84000100, 0x0), // N_GPP_F10
	_PAD_CFG_STRUCT(GPP_F11, 0x44000300, 0x0), // N_GPP_F11
	_PAD_CFG_STRUCT(GPP_F12, 0x44000300, 0x0), // N_GPP_F12
	_PAD_CFG_STRUCT(GPP_F13, 0x44000300, 0x0), // N_GPP_F13
	_PAD_CFG_STRUCT(GPP_F14, 0x44000b00, 0x0), // NTP124
	_PAD_CFG_STRUCT(GPP_F15, 0x44000500, 0x0), // N_-USBOC_F
	_PAD_CFG_STRUCT(GPP_F16, 0x44000500, 0x0), // N_-USBOC_7
	_PAD_CFG_STRUCT(GPP_F17, 0x44000500, 0x0), // N_-USBOC_7
	_PAD_CFG_STRUCT(GPP_F18, 0x44000500, 0x0), // N_-USBOC_7
	_PAD_CFG_STRUCT(GPP_F19, 0x44000700, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_F20, 0x44000700, 0x0), // NTP88
	_PAD_CFG_STRUCT(GPP_F21, 0x44000700, 0x0), // NTP87
	_PAD_CFG_STRUCT(GPP_F22, 0x44000300, 0x0), // N_GPP_F22
	_PAD_CFG_STRUCT(GPP_F23, 0x44000200, 0x0), // N_GPP_F23

	/* ------- GPIO Group G ------- */
	_PAD_CFG_STRUCT(GPP_G0, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_G1, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_G2, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_G3, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_G4, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_G5, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_G6, 0x44000300, 0x0), // N_GPPC_G6
	_PAD_CFG_STRUCT(GPP_G7, 0x44000300, 0x0), // NC

	/* ------- GPIO Group H ------- */
	_PAD_CFG_STRUCT(GPP_H0, 0x44000700, 0x0), // LB_-CLKREQ
	_PAD_CFG_STRUCT(GPP_H1, 0x44000100, 0x0), // N_GPP_H1
	_PAD_CFG_STRUCT(GPP_H2, 0x44000300, 0x0), // N_GPP_H2
	_PAD_CFG_STRUCT(GPP_H3, 0x44000300, 0x0), // N_GPP_H3
	_PAD_CFG_STRUCT(GPP_H4, 0x44000300, 0x0), // N_GPP_H4
	_PAD_CFG_STRUCT(GPP_H5, 0x44000300, 0x0), // N_GPP_H5
	_PAD_CFG_STRUCT(GPP_H6, 0x44000300, 0x0), // N_GPP_H6
	_PAD_CFG_STRUCT(GPP_H7, 0x44000300, 0x0), // N_GPP_H7
	_PAD_CFG_STRUCT(GPP_H8, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_H9, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_H10, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_H11, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_H12, 0x44000200, 0x0), // N_GPP_H12
	_PAD_CFG_STRUCT(GPP_H13, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_H14, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_H15, 0x44000300, 0x0), // N_GPP_H15
	_PAD_CFG_STRUCT(GPP_H16, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_H17, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_H18, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_H19, 0x44000300, 0x0), // N_GPP_H19
	_PAD_CFG_STRUCT(GPP_H20, 0x44000300, 0x0), // N_GPP_H20
	_PAD_CFG_STRUCT(GPP_H21, 0x44000300, 0x0), // N_GPP_H21
	_PAD_CFG_STRUCT(GPP_H22, 0x44000300, 0x0), // N_GPP_H22
	_PAD_CFG_STRUCT(GPP_H23, 0x44000300, 0x0), // NC

	/* ------- GPIO Group I ------- */
	_PAD_CFG_STRUCT(GPP_I0, 0x40000700, 0x3c00), // N_HDMI_HDP_F
	_PAD_CFG_STRUCT(GPP_I1, 0x40000700, 0x3c00), // N_HDMI20_HDP_F
	_PAD_CFG_STRUCT(GPP_I2, 0x40000700, 0x3c00), // DP_HDP
	_PAD_CFG_STRUCT(GPP_I3, 0x40000700, 0x0), // N_GPP_I3
	_PAD_CFG_STRUCT(GPP_I4, 0x40000700, 0x0), // N_GPP_I4
	_PAD_CFG_STRUCT(GPP_I5, 0x40000700, 0x0), // N_DDPB_CTRLCLK
	_PAD_CFG_STRUCT(GPP_I6, 0x40000700, 0x0), // N_DDPB_CTRLDATA
	_PAD_CFG_STRUCT(GPP_I7, 0x40000700, 0x0), // N_DDPC_CTRLCLK
	_PAD_CFG_STRUCT(GPP_I8, 0x40000700, 0x0), // N_DDPC_CTRLDATA
	_PAD_CFG_STRUCT(GPP_I9, 0x40000700, 0x1000), // N_DDPD_CTRLCLK
	_PAD_CFG_STRUCT(GPP_I10, 0x40000700, 0x1000), // N_DDPD_CTRLDATA
	_PAD_CFG_STRUCT(GPP_I11, 0x40000700, 0x3c00), // A_-SKTOCC
	_PAD_CFG_STRUCT(GPP_I12, 0x40000700, 0x3c00), // NC
	_PAD_CFG_STRUCT(GPP_I13, 0x40000700, 0x3c00), // NC
	_PAD_CFG_STRUCT(GPP_I14, 0x40000700, 0x3c00), // NC

	/* ------- GPIO Group J ------- */
	_PAD_CFG_STRUCT(GPP_J0, 0x40000700, 0x3c00), // CNVI_PA_BLANKING
	_PAD_CFG_STRUCT(GPP_J1, 0x40000700, 0x3c00), // CPU_VCCIO_PW R_GATEB
	_PAD_CFG_STRUCT(GPP_J2, 0x40000700, 0x3c00), // NC
	_PAD_CFG_STRUCT(GPP_J3, 0x40000700, 0x3c00), // NC
	_PAD_CFG_STRUCT(GPP_J4, 0x40000700, 0x3c00), // CNVI_BRI_DT
	_PAD_CFG_STRUCT(GPP_J5, 0x44000500, 0x0), // CNVI_BRI_RSP
	_PAD_CFG_STRUCT(GPP_J6, 0x44000500, 0x0), // CNVI_RGI_DT
	_PAD_CFG_STRUCT(GPP_J7, 0x44000500, 0x0), // CNVI_RGI_RSP
	_PAD_CFG_STRUCT(GPP_J8, 0x44000500, 0x0), // CNVI_MFUART2_RXD
	_PAD_CFG_STRUCT(GPP_J9, 0x44000500, 0x0), // CNVI_MFUART2_TXD
	_PAD_CFG_STRUCT(GPP_J10, 0x44000500, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_J11, 0x44000600, 0x0), // NTP122

	/* ------- GPIO Group K ------- */
	_PAD_CFG_STRUCT(GPP_K0, 0x4000100, 0x0), // N_GPP_K0
	_PAD_CFG_STRUCT(GPP_K1, 0x4000100, 0x0), // N_GPP_K1
	_PAD_CFG_STRUCT(GPP_K2, 0x44000300, 0x0), // N_GPP_K2
	_PAD_CFG_STRUCT(GPP_K3, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_K4, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_K5, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_K6, 0x44000300, 0x0), // N_GPP_K6
	_PAD_CFG_STRUCT(GPP_K7, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_K8, 0x44000700, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_K9, 0x44000700, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_K10, 0x44000700, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_K11, 0x44000700, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_K12, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_K13, 0x44000300, 0x0), // NC
	_PAD_CFG_STRUCT(GPP_K14, 0x44000300, 0x0), // N_GT_S
	_PAD_CFG_STRUCT(GPP_K15, 0x44000300, 0x0), // N_CPU_S
	_PAD_CFG_STRUCT(GPP_K16, 0x44000300, 0x0), // M2_BT_DISABLE
	_PAD_CFG_STRUCT(GPP_K17, 0x44000300, 0x0), // M2_WIFI_DISABLE
	_PAD_CFG_STRUCT(GPP_K18, 0x44000300, 0x0), // N_GPP_K18
	_PAD_CFG_STRUCT(GPP_K19, 0x44000300, 0x0), // N_GPP_K19
	_PAD_CFG_STRUCT(GPP_K20, 0x44000700, 0x0), // N_GPP_K20
	_PAD_CFG_STRUCT(GPP_K21, 0x44000700, 0x0), // N_GPP_K21
	_PAD_CFG_STRUCT(GPP_K22, 0x44000300, 0x0), // N_GPP_K22
	_PAD_CFG_STRUCT(GPP_K23, 0x44000300, 0x0), // NC
};

#endif

#endif
