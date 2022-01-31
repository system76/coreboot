/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage */
static const struct pad_config override_gpio_table[] = {
	/* A7  : SRCCLK_OE7# ==> LAN_WAKE_ODL */
	PAD_CFG_GPI_SCI_LOW(GPP_A7, NONE, DEEP, EDGE_SINGLE),
	/* A8  : SRCCLKREQ7# ==> USB2_A2_RT_RST_ODL */
	PAD_CFG_GPO(GPP_A8, 1, DEEP),
	/* A12 : SATAXPCIE1 ==> EN_PP3300_LAN_X */
	PAD_CFG_GPO(GPP_A12, 1, DEEP),
	/* A14 : USB_OC1# ==> USB_C0_OC_ODL */
	PAD_CFG_NF(GPP_A14, NONE, DEEP, NF1),
	/* A15 : USB_OC2# ==> USB_C2_OC_ODL */
	PAD_CFG_NF(GPP_A15, NONE, DEEP, NF1),
	/* A17 : DISP_MISCC ==> EN_GPU_PPVAR_GPU_NVVDD_X_PCH */
	PAD_CFG_GPO(GPP_A17, 1, DEEP),
	/* A19 : DDSP_HPD1 ==> EN_PCH_PPVAR_GPU_FBVDDQ_X_L */
	PAD_CFG_GPO(GPP_A19, 0, DEEP),
	/* A20 : DDSP_HPD2 ==> NC */
	PAD_NC(GPP_A20, NONE),
	/* A21 : DDPC_CTRCLK ==> EN_PP3300_GPU_X */
	PAD_CFG_GPO(GPP_A21, 0, DEEP),
	/* A22 : DDPC_CTRCLK ==> PG_PP3300_GPU_X_OD */
	PAD_CFG_GPI(GPP_A22, NONE, DEEP),

	/* B3  : PROC_GP2 ==> GPU_PERST_L */
	PAD_CFG_GPO(GPP_B3, 1, DEEP),
	/* B5  : ISH_I2C0_SDA ==> PCH_I2C_NVDD_GPU_SDA */
	PAD_CFG_NF(GPP_B5, NONE, DEEP, NF2),
	/* B6  : ISH_I2C0_SDA ==> PCH_I2C_NVDD_GPU_SCL */
	PAD_CFG_NF(GPP_B6, NONE, DEEP, NF2),
	/* B15  : TIME_SYNC0 ==> NC */
	PAD_NC(GPP_B15, NONE),

	/* C0  : SMBCLK ==> NC */
	PAD_NC(GPP_C0, NONE),
	/* C1  : SMBDATA ==> NC */
	PAD_NC(GPP_C1, NONE),
	/* C3 : SML0CLK ==> NC */
	PAD_NC(GPP_C3, NONE),
	/* C4 : SML0DATA ==> NC */
	PAD_NC(GPP_C4, NONE),
	/* C6  : SML1CLK ==> NC */
	PAD_NC(GPP_C6, NONE),
	/* C7  : SML1DATA ==> NC */
	PAD_NC(GPP_C7, NONE),

	/* D0  : ISH_GP0 ==> NC */
	PAD_NC(GPP_D0, NONE),
	/* D1  : ISH_GP1 ==> NC */
	PAD_NC(GPP_D1, NONE),
	/* D2  : ISH_GP2 ==> LAN_PR_ISOLATE_ODL */
	PAD_CFG_GPO(GPP_D2, 1, DEEP),
	/* D3 : ISH_GP3 ==> NC */
	PAD_NC(GPP_D3, NONE),
	/* D5  : SRCCLKREQ0# ==> GPU_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D5, NONE, DEEP, NF1),
	/* D9  : ISH_SPI_CS# ==> GPU_THERM_INT_ODL */
	PAD_CFG_GPI(GPP_D9, NONE, DEEP),
	/* D10 : ISH_SPI_CLK ==> GPP_D10_STRAP */
	PAD_NC(GPP_D10, NONE),
	/* D13 : ISH_UART0_RXD ==> NC */
	PAD_NC(GPP_D13, NONE),
	/* D15 : ISH_UART0_RTS# ==> NC */
	PAD_NC(GPP_D15, NONE),
	/* D16 : ISH_UART0_CTS# ==> NC */
	PAD_NC(GPP_D16, NONE),

	/* E0 : SATAXPCIE0 ==> EN_PPVAR_GPU_NVVDD_X_OD */
	PAD_CFG_GPO(GPP_E0, 0, DEEP),
	/* E3  : PROC_GP0 ==> NC */
	PAD_NC(GPP_E3, NONE),
	/* E4  : SATA_DEVSLP0 ==> PG_PPVAR_GPU_FBVDDQ_X_OD */
	PAD_CFG_GPO(GPP_E4, 0, DEEP),
	/* E5  : SATA_DEVSLP1 ==> PG_GPU_ALLRAILS */
	PAD_CFG_GPI(GPP_E5, NONE, DEEP),
	/* E7  : PROC_GP1 ==> NC */
	PAD_NC(GPP_E7, NONE),
	/* E9  : USB_OC0# ==> USB_A2_OC_ODL */
	PAD_CFG_NF(GPP_E9, NONE, DEEP, NF1),
	/* E10 : THC0_SPI1_CS# ==> EN_PP0950_GPU_X */
	PAD_CFG_GPO(GPP_E10, 0, DEEP),
	/* E16 : RSVD_TP ==> PG_PPVAR_GPU_NVVDD_X_OD */
	PAD_CFG_GPO(GPP_E16, 0, DEEP),
	/* E17 : RSVD_TP ==> PG_PP0950_GPU_X_OD */
	PAD_CFG_GPI(GPP_E17, NONE, DEEP),
	/* E18 : DDP1_CTRLCLK ==> EN_PP1800_GPU_X */
	PAD_CFG_GPO(GPP_E18, 0, DEEP),
	/* E19 : DDP1_CTRLDATA ==> GPP_E19_STRAP */
	PAD_NC(GPP_E19, NONE),
	/* E20 : DDP2_CTRLCLK ==> PG_PP1800_GPU_X_OD */
	PAD_CFG_GPI(GPP_E20, NONE, DEEP),
	/* E21 : DDP2_CTRLDATA ==> GPP_E21_STRAP */
	PAD_NC(GPP_E21, NONE),

	/* F6  : CNV_PA_BLANKING ==> NC */
	PAD_NC(GPP_F6, NONE),
	/* F11 : THC1_SPI2_CLK ==> NC */
	PAD_NC(GPP_F11, NONE),
	/* F12 : GSXDOUT ==> NC */
	PAD_NC(GPP_F12, NONE),
	/* F13 : GSXDOUT ==> NC */
	PAD_NC(GPP_F13, NONE),
	/* F15 : GSXSRESET# ==> NC */
	PAD_NC(GPP_F15, NONE),
	/* F16 : GSXCLK ==> NC */
	PAD_NC(GPP_F16, NONE),
	/* F19 : SRCCLKREQ6# ==> NC */
	PAD_NC(GPP_F19, NONE),
	/* F20 : EXT_PWR_GATE# ==> NC */
	PAD_NC(GPP_F20, NONE),
	/* F21 : EXT_PWR_GATE2# ==> NC */
	PAD_NC(GPP_F21, NONE),

	/* H6  : I2C1_SDA ==> PCH_I2C_GPU_SDA */
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1),
	/* H7  : I2C1_SCL ==> PCH_I2C_GPU_SCL */
	PAD_CFG_NF(GPP_H7, NONE, DEEP, NF1),
	/* H8  : I2C4_SDA ==> NC */
	PAD_NC(GPP_H8, NONE),
	/* H9  : I2C4_SCL ==> NC */
	PAD_NC(GPP_H9, NONE),
	/* H19 : SRCCLKREQ4# ==> LAN_CLKREQ_ODL */
	PAD_CFG_NF(GPP_H19, NONE, DEEP, NF1),
	/* H21 : IMGCLKOUT2 ==> NC */
	PAD_NC(GPP_H21, NONE),
	/* H22 : IMGCLKOUT3 ==> NC */
	PAD_NC(GPP_H22, NONE),

	/* R4 : HDA_RST# ==> DMIC_CLK0 */
	PAD_CFG_NF(GPP_R4, NONE, DEEP, NF3),
	/* R5 : HDA_SDI1 ==> DMIC_DATA0 */
	PAD_CFG_NF(GPP_R5, NONE, DEEP, NF3),
	/* R6 : I2S_PCH_TX_SPKR_RX ==> NC */
	PAD_NC(GPP_R6, NONE),
	/* R7 : I2S_PCH_RX_SPKR_TX ==> NC */
	PAD_NC(GPP_R7, NONE),

	/* S0 : SNDW0_CLK ==> I2S_SPKR_SCLK */
	PAD_CFG_NF(GPP_S0, NONE, DEEP, NF4),
	/* S1 : SNDW0_DATA ==> I2S_SPKR_SFRM */
	PAD_CFG_NF(GPP_S1, NONE, DEEP, NF4),
	/* S2 : SNDW1_CLK ==> I2S_PCH_SPKR_RX */
	PAD_CFG_NF(GPP_S2, NONE, DEEP, NF4),
	/* S3 : SNDW1_DATA ==> I2S_PCH_SPKR_TX */
	PAD_CFG_NF(GPP_S3, NONE, DEEP, NF4),
	/* S6 : SNDW3_CLK ==> SDW_HP_CLK */
	PAD_CFG_NF(GPP_S6, NONE, DEEP, NF1),
	/* S7 : SNDW3_DATA ==> SDW_HP_DATA */
	PAD_CFG_NF(GPP_S7, NONE, DEEP, NF1),

};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* A13 : PMC_I2C_SCL ==> GSC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* B7  : ISH_12C1_SDA ==> PCH_I2C_TPM_SDA */
	PAD_CFG_NF(GPP_B7, NONE, DEEP, NF2),
	/* B8  : ISH_I2C1_SCL ==> PCH_I2C_TPM_SCL */
	PAD_CFG_NF(GPP_B8, NONE, DEEP, NF2),
	/* D11 : ISH_SPI_MISO ==> EN_PP3300_SSD */
	PAD_CFG_GPO(GPP_D11, 1, DEEP),
	/* E13 : THC0_SPI1_IO2 ==> MEM_CH_SEL */
	PAD_CFG_GPI(GPP_E13, NONE, DEEP),
	/* E15 : RSVD_TP ==> PCH_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E15, NONE, DEEP),
	/* F18 : THC1_SPI2_INT# ==> EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_F18, NONE, DEEP),
	/* H10 : UART0_RXD ==> UART_PCH_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_PCH_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/*
	 * enable EN_PP3300_SSD in bootblock, then PERST# is asserted, and
	 * then deassert PERST# in romstage
	 */
	/* H13 : I2C7_SCL ==> EN_PP3300_SD */
	PAD_CFG_GPO(GPP_H13, 1, DEEP),
	/* B4  : PROC_GP3 ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 0, DEEP),
};

static const struct pad_config romstage_gpio_table[] = {
	/* B4  : PROC_GP3 ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 1, DEEP),
};

const struct pad_config *variant_gpio_override_table(size_t *num)
{
	*num = ARRAY_SIZE(override_gpio_table);
	return override_gpio_table;
}

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

const struct pad_config *variant_romstage_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(romstage_gpio_table);
	return romstage_gpio_table;
}
