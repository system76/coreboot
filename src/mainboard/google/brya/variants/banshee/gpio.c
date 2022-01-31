/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage */
static const struct pad_config override_gpio_table[] = {
	/* A0 thru A5, A9 and A10 come configured out of reset, do not touch */
	/* A0  : ESPI_IO0 ==> ESPI_IO_0 */
	/* A1  : ESPI_IO1 ==> ESPI_IO_1 */
	/* A2  : ESPI_IO2 ==> ESPI_IO_2 */
	/* A3  : ESPI_IO3 ==> ESPI_IO_3 */
	/* A4  : ESPI_CS# ==> ESPI_CS_L */
	/* A5  : ESPI_ALERT0# ==> NC */
	/* A6  : ESPI_ALERT1# ==> NC */
	PAD_NC(GPP_A6, NONE),
	/* A7  : SRCCLK_OE7# ==> NC */
	PAD_NC(GPP_A7, NONE),
	/* A8  : SRCCLKREQ7# ==> NC */
	PAD_NC(GPP_A8, NONE),
	/* A9  : ESPI_CLK ==> ESPI_CLK */
	/* A10 : ESPI_RESET# ==> ESPI_PCH_RST_EC_L */
	/* A11 : PMC_I2C_SDA ==> EN_SPKR_PA */
	/* A12 : SATAXPCIE1 ==> EN_PP3300_SSD */
	/* A13 : PMC_I2C_SCL ==> GSC_PCH_INT_ODL */
	/* A14 : USB_OC1# ==> USB_C1_OC_ODL */
	/* A15 : USB_OC2# ==> USB_C2_OC_ODL */
	/* A16 : USB_OC3# ==> USB_C3_OC_ODL */
	/* A17 : DISP_MISCC ==> EN_FCAM_PWR */
	/* A18 : DDSP_HPDB ==> NC */
	PAD_NC(GPP_A18, NONE),
	/* A19 : DDSP_HPD1 ==> NC */
	PAD_NC(GPP_A19, NONE),
	/* A20 : DDSP_HPD2 ==> NC */
	PAD_NC(GPP_A20, NONE),
	/* A21 : DDPC_CTRCLK ==> NC */
	PAD_NC(GPP_A21, NONE),
	/* A22 : DDPC_CTRLDATA ==> NC */
	PAD_NC(GPP_A22, NONE),
	/* A23 : ESPI_CS1# ==> AUD_HP_INT_L */

	/* B0  : SOC_VID0 */
	/* B1  : SOC_VID1 */
	/* B2  : VRALERT# ==> M2_SSD_PLA_L */
	/* B3  : PROC_GP2 ==> NC */
	PAD_NC(GPP_B3, NONE),
	/* B4  : PROC_GP3 ==> SSD_PERST_L */
	/* B5  : ISH_I2C0_SDA ==> NC */
	PAD_NC(GPP_B5, NONE),
	/* B6  : ISH_I2C0_SCL ==> NC */
	PAD_NC(GPP_B6, NONE),
	/* B7  : ISH_12C1_SDA ==> PCH_I2C_TPM_SDA */
	/* B8  : ISH_I2C1_SCL ==> PCH_I2C_TPM_SCL */
	/* B9  : NC */
	/* B10 : NC */
	/* B11 : PMCALERT# ==> EN_PP3300_WLAN */
	/* B12 : SLP_S0# ==> SLP_S0_L */
	/* B13 : PLTRST# ==> PLT_RST_L */
	/* B14 : SPKR ==> GPP_B14_STRAP */
	/* B15 : TIME_SYNC0 ==> NC */
	PAD_NC(GPP_B15, NONE),
	/* B16 : I2C5_SDA ==> PCH_I2C_TCHPAD_SDA */
	/* B17 : I2C5_SCL ==> PCH_I2C_TCHPAD_SCL */
	/* B18 : ADR_COMPLETE ==> GPP_B18_STRAP */
	/* B19 : NC */
	/* B20 : NC */
	/* B21 : NC */
	/* B22 : NC */
	/* B23 : SML1ALERT# ==> PCHHOT_ODL_STRAP */

	/* C0  : SMBCLK ==> DDR_SMB_CLK */
	PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),
	/* C1  : SMBDATA ==> DDR_SMB_DATA */
	PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1),
	/* C2  : SMBALERT# ==> GPP_C2_STRAP */
	/* C3 : SML0CLK ==> NC */
	PAD_NC(GPP_C3, NONE),
	/* C4 : SML0DATA ==> NC */
	PAD_NC(GPP_C4, NONE),
	/* C5  : SML0ALERT# ==> GPP_C5_BOOT_STRAP0 */
	/* C6  : SML1CLK ==> USI_REPORT_EN */
	/* C7  : SML1DATA ==> USI_INT */

	/* D0  : ISH_GP0 ==> NC */
	PAD_NC(GPP_D0, NONE),
	/* D1  : ISH_GP1 ==> NC */
	PAD_NC(GPP_D1, NONE),
	/* D2  : ISH_GP2 ==> NC */
	PAD_NC(GPP_D2, NONE),
	/* D3  : ISH_GP3 ==> NC */
	PAD_NC(GPP_D3, NONE),
	/* D4  : IMGCLKOUT0 ==> BT_DISABLE_L */
	/* D5  : SRCCLKREQ0# ==> SSD_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D5, NONE, DEEP, NF1),
	/* D6  : SRCCLKREQ1# ==> NC */
	PAD_NC(GPP_D6, NONE),
	/* D7  : SRCCLKREQ2# ==> WLAN_CLKREQ_ODL */
	/* D8  : SRCCLKREQ3# ==> NC */
	PAD_NC(GPP_D8, NONE),
	/* D9  : ISH_SPI_CS# ==> USB_C2_LSX_TX */
	/* D10 : ISH_SPI_CLK ==> USB_C2_LSX_RX_STRAP */
	/* D11 : ISH_SPI_MISO ==> USB_C3_LSX_TX */
	PAD_CFG_NF(GPP_D11, NONE, DEEP, NF4),
	/* D12 : ISH_SPI_MOSI ==> GPP_D12_STRAP */
	/* D13 : ISH_UART0_RXD ==> NC */
	PAD_NC(GPP_D13, NONE),
	/* D14 : ISH_UART0_TXD ==> NC */
	PAD_NC(GPP_D14, NONE),
	/* D15 : ISH_UART0_RTS# ==> NC */
	PAD_NC(GPP_D15, NONE),
	/* D16 : ISH_UART0_CTS# ==> NC */
	PAD_NC(GPP_D16, NONE),
	/* D17 : UART1_RXD ==> NC */
	PAD_NC(GPP_D17, NONE),
	/* D18 : UART1_TXD ==> USI_RST_L */
	PAD_CFG_GPO(GPP_D18, 0, DEEP),
	/* D19 : I2S_MCLK1_OUT ==> I2S_MCLK_R */

	/* E0  : SATAXPCIE0 ==> NC */
	PAD_NC(GPP_E0, NONE),
	/* E1  : THC0_SPI1_IO2 ==> NC */
	PAD_NC(GPP_E1, NONE),
	/* E2  : THC0_SPI1_IO3 ==> NC */
	PAD_NC(GPP_E2, NONE),
	/* E3  : PROC_GP0 ==> NC */
	PAD_NC(GPP_E3, NONE),
	/* E4  : SATA_DEVSLP0 ==> USB4_BB_RT_FORCE_PWR */
	/* E5  : SATA_DEVSLP1 ==> NC */
	PAD_NC(GPP_E5, NONE),
	/* E6  : THC0_SPI1_RST# ==> GPPE6_STRAP */
	/* E7  : PROC_GP1 ==> NC */
	PAD_NC(GPP_E7, NONE),
	/* E8  : SLP_DRAM# ==> WIFI_DISABLE_L */
	/* E9  : USB_OC0# ==> USB_C0_OC_ODL */
	/* E10 : THC0_SPI1_CS# ==> NC */
	PAD_NC(GPP_E10, NONE),
	/* E11 : THC0_SPI1_CLK ==> NC */
	PAD_NC(GPP_E11, NONE),
	/* E12 : THC0_SPI1_IO1 ==> NC */
	PAD_NC(GPP_E12, NONE),
	/* E13 : THC0_SPI1_IO2 ==> NC */
	PAD_NC(GPP_E13, NONE),
	/* E14 : DDSP_HPDA ==> SOC_EDP_HPD */
	/* E15 : RSVD_TP ==> PCH_WP_OD */
	/* E16 : RSVD_TP ==> NC */
	PAD_NC(GPP_E16, NONE),
	/* E17 : THC0_SPI1_INT# ==> NC */
	PAD_NC(GPP_E17, NONE),
	/* E18 : DDP1_CTRLCLK ==> USB_C0_LSX_SOC_TX */
	/* E19 : DDP1_CTRLDATA ==> USB0_C0_LSX_SOC_RX_STRAP */
	/* E20 : DDP2_CTRLCLK ==> USB_C1_LSX_SOC_TX */
	/* E21 : DDP2_CTRLDATA ==> USB_C1_LSX_SOC_RX_STRAP */
	/* E22 : DDPA_CTRLCLK ==> NC */
	PAD_NC(GPP_E22, NONE),
	/* E23 : DDPA_CTRLDATA ==> NC */
	PAD_NC(GPP_E23, NONE),

	/* F0  : CNV_BRI_DT ==> CNV_BRI_DT_STRAP */
	/* F1  : CNV_BRI_RSP ==> CNV_BRI_RSP */
	/* F2  : CNV_RGI_DT ==> CNV_RGI_DT_STRAP */
	/* F3  : CNV_RGI_RSP ==> CNV_RGI_RSP */
	/* F4  : CNV_RF_RESET# ==> CNV_RF_RST_L */
	/* F5  : MODEM_CLKREQ ==> CNV_CLKREQ0 */
	/* F6  : CNV_PA_BLANKING ==> NC */
	PAD_NC(GPP_F6, NONE),
	/* F7  : GPPF7_STRAP */
	/* F8  : NC */
	/* F9  : BOOTMPC ==> SLP_S0_GATE_R */
	/* F10 : GPPF10_STRAP */
	/* F11 : THC1_SPI2_CLK ==> NC */
	PAD_NC(GPP_F11, NONE),
	/* F12 : GSXDOUT ==> NC */
	PAD_NC(GPP_F12, NONE),
	/* F13 : GSXDOUT ==> NC */
	PAD_NC(GPP_F13, NONE),
	/* F14 : GSXDIN ==> TCHPAD_INT_ODL */
	/* F15 : GSXSRESET# ==> NC */
	PAD_NC(GPP_F15, NONE),
	/* F16 : GSXCLK ==> NC */
	PAD_NC(GPP_F16, NONE),
	/* F17 : THC1_SPI2_RST# ==> EC_PCH_INT_ODL */
	/* F18 : THC1_SPI2_INT# ==> EC_IN_RW_OD */
	/* F19 : SRCCLKREQ6# ==> NC */
	PAD_NC(GPP_F19, NONE),
	/* F20 : EXT_PWR_GATE# ==> NC */
	PAD_NC(GPP_F20, NONE),
	/* F21 : EXT_PWR_GATE2# ==> NC */
	PAD_NC(GPP_F21, NONE),
	/* F22 : NC */
	/* F23 : NC */

	/* H0  : GPPH0_BOOT_STRAP1 */
	/* H1  : GPPH1_BOOT_STRAP2 */
	/* H2  : GPPH2_BOOT_STRAP3 */
	/* H3  : SX_EXIT_HOLDOFF# ==> WLAN_PCIE_WAKE_ODL */
	/* H4  : I2C0_SDA ==> PCH_I2C_AUD_SDA */
	/* H5  : I2C0_SCL ==> PCH_I2C_AUD_SCL */
	/* H6  : I2C1_SDA ==> PCH_I2C_TCHSCR_SDA */
	/* H7  : I2C1_SCL ==> PCH_I2C_TCHSCR_SCL */
	/* H8  : I2C4_SDA ==> NC */
	PAD_NC(GPP_H8, NONE),
	/* H9  : I2C4_SCL ==> NC */
	PAD_NC(GPP_H9, NONE),
	/* H10 : UART0_RXD ==> UART_PCH_RX_DBG_TX */
	/* H12 : I2C7_SDA ==> NC */
	PAD_NC(GPP_H12, NONE),
	/* H13 : I2C7_SCL ==> EN_PP3300_TCHSCR */
	PAD_CFG_GPO(GPP_H13, 1, DEEP),
	/* H14 : NC */
	/* H15 : DDPB_CTRLCLK ==> NC */
	PAD_NC(GPP_H15, NONE),
	/* H16 : NC */
	/* H17 : DDPB_CTRLDATA ==> NC */
	PAD_NC(GPP_H17, NONE),
	/* H18 : PROC_C10_GATE# ==> CPU_C10_GATE_L */
	/* H19 : SRCCLKREQ4# ==> NC */
	PAD_NC(GPP_H19, NONE),
	/* H20 : IMGCLKOUT1 ==> WLAN_PERST_L */
	/* H21 : IMGCLKOUT2 ==>  NC */
	PAD_NC(GPP_H21, NONE),
	/* H22 : IMGCLKOUT3 ==> NC */
	PAD_NC(GPP_H22, NONE),
	/* H23 : SRCCLKREQ5# ==> NC */
	PAD_NC(GPP_H23, NONE),

	/* R0 : HDA_BCLK ==> I2S_HP_SCLK_R */
	/* R1 : HDA_SYNC ==> I2S_HP_SFRM_R */
	/* R2 : HDA_SDO ==> I2S_PCH_TX_HP_RX_STRAP */
	/* R3 : HDA_SDIO ==> I2S_PCH_RX_HP_TX */
	/* R4 : HDA_RST# ==> I2S_SPKR_SCLK_R */
	/* R5 : HDA_SDI1 ==> I2S_SPKR_SFRM_R */
	/* R6 : I2S2_TXD ==> I2S_PCH_TX_SPKR_RX_R */
	/* R7 : I2S2_RXD ==> NC */
	PAD_NC(GPP_R7, NONE),

	/* S0 : SNDW0_CLK ==> NC */
	PAD_NC(GPP_S0, NONE),
	/* S1 : SNDW0_DATA ==> NC */
	PAD_NC(GPP_S1, NONE),
	/* S2 : SNDW1_CLK ==> DMIC_CLK0_R */
	/* S3 : SNDW1_DATA ==> DMIC_DATA0_R */
	/* S4 : SNDW2_CLK ==> NC */
	PAD_NC(GPP_S4, NONE),
	/* S5 : SNDW2_DATA ==> NC */
	PAD_NC(GPP_S5, NONE),
	/* S6 : SNDW3_CLK ==> NC */
	PAD_NC(GPP_S6, NONE),
	/* S7 : SNDW3_DATA ==> NC */
	PAD_NC(GPP_S7, NONE),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* A13 : PMC_I2C_SCL ==> GSC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* B4  : PROC_GP3 ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 0, DEEP),
	/* A12 : SATAXPCIE1 ==> EN_PP3300_SSD */
	PAD_CFG_GPO(GPP_A12, 1, DEEP),
	/* E15 : RSVD_TP ==> PCH_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E15, NONE, DEEP),
	/* F18 : THC1_SPI2_INT# ==> EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_F18, NONE, DEEP),
	/* H6  : I2C1_SDA ==> PCH_I2C_TPM_SDA */
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1),
	/* H7  : I2C1_SCL ==> PCH_I2C_TPM_SCL */
	PAD_CFG_NF(GPP_H7, NONE, DEEP, NF1),
	/* H10 : UART0_RXD ==> UART_PCH_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_PCH_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),

	/* CPU PCIe VGPIO for PEG60 */
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_48, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_49, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_50, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_51, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_52, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_53, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_54, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_55, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_56, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_57, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_58, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_59, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_60, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_61, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_62, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_63, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_76, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_77, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_78, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_79, NONE, PLTRST, NF1),
};

static const struct pad_config romstage_gpio_table[] = {
	/*
	 * B4  : PROC_GP3 ==> SSD_PERST_L
	 * B4 is programmed here so that it is sequenced after EN_PP3300_SSD.
	 */
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
