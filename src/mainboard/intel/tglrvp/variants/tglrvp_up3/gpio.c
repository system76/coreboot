/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* Pad configuration in ramstage */
static const struct pad_config gpio_table[] = {
	/* PCH M.2 SSD */
	PAD_CFG_GPO(GPP_B16, 1, DEEP),
	PAD_CFG_GPO(GPP_H0, 1, DEEP),

	/* Camera */
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1), /* I2C3_SDA */
	PAD_CFG_NF(GPP_H7, NONE, DEEP, NF1), /* I2C3_SCL */
	PAD_CFG_NF(GPP_B9, NONE, DEEP, NF1), /* I2C5_SDA */
	PAD_CFG_NF(GPP_B10, NONE, DEEP, NF1), /* I2C5_SCL */
	PAD_CFG_GPO(GPP_B23, 0, DEEP),
	PAD_CFG_GPO(GPP_C15, 0, DEEP),
	PAD_CFG_GPO(GPP_R6, 0, DEEP),
	PAD_CFG_GPO(GPP_H12, 0, DEEP),

	/* Image clock: IMGCLKOUT_0, IMGCLKOUT_1 */
	PAD_CFG_NF(GPP_D4, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_H20, NONE, DEEP, NF1),

	/* ISH UART0 RX/TX */
	PAD_CFG_NF(GPP_D13, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_D14, NONE, DEEP, NF1),

	/* ISH I2C0 */
	PAD_CFG_NF(GPP_B5, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_B6, NONE, DEEP, NF1),

	/* ISH GPI 0-6 */
	PAD_CFG_NF(GPP_D0, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_D1, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_D2, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_D3, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_D17, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_D18, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_E15, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_E16, NONE, DEEP, NF1),

	/* Audio */
	PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1), /* I2C0_SDA */
	PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1), /* I2C0_SCL */
	PAD_CFG_GPO(GPP_C5, 1, DEEP),
	PAD_CFG_GPI_APIC(GPP_C12, NONE, DEEP, EDGE_BOTH, INVERT), /* AUDIO JACK IRQ */

	PAD_CFG_NF(GPP_D19, NONE, DEEP, NF1), /* I2S_MCLK1 */
	PAD_CFG_NF(GPP_F8, NONE, DEEP, NF1),  /* I2S_MCLK2 */

	/* CNVi */
	PAD_CFG_NF(GPP_F4, NONE, DEEP, NF1),  /* CNV_RF_RST_L */
	PAD_CFG_NF(GPP_F5, NONE, DEEP, NF3),  /* CNV_CLKREQ0 */

	/* EC_SYNC_IRQ */
	PAD_CFG_GPI_APIC(GPP_A15, NONE, PLTRST, LEVEL, INVERT), /* MECC_HPD2 */

	PAD_CFG_GPO(GPP_H1, 1, DEEP), /* AUDIO_PWREN */
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* UART */
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1), /* UART2 RX */
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1), /* UART2 TX */

	/* Audio */
	PAD_CFG_NF(GPP_R0, NONE, DEEP, NF2),   /* I2S0_HP_SCLK */
	PAD_CFG_NF(GPP_R1, NONE, DEEP, NF2),   /* I2S0_HP_SFRM */
	PAD_CFG_NF(GPP_R2, NONE, DEEP, NF2),   /* I2S0_HP_TX */
	PAD_CFG_NF(GPP_R3, NONE, DEEP, NF2),   /* I2S0_HP_RX */
	PAD_CFG_NF(GPP_R4, DN_20K, DEEP, NF1), /* HDA_RST_L */

	PAD_CFG_NF(GPP_A7, NONE, DEEP, NF1),   /* I2S2_SPKR_SCLK */
	PAD_CFG_NF(GPP_A8, NONE, DEEP, NF1),   /* I2S2_SPKR_SFRM */
	PAD_CFG_NF(GPP_A9, NONE, DEEP, NF1),   /* I2S2_SPKR_TX */
	PAD_CFG_NF(GPP_A10, NONE, DEEP, NF1),  /* I2S2_SPKR_RX */

	PAD_CFG_NF(GPP_S0, NONE, DEEP, NF1),   /* SNDW0_HP_CLK */
	PAD_CFG_NF(GPP_S1, NONE, DEEP, NF1),   /* SNDW0_HP_DATA */

	PAD_CFG_NF(GPP_S2, NONE, DEEP, NF2),   /* DMIC0_CLK_B */
	PAD_CFG_NF(GPP_S6, NONE, DEEP, NF2),   /* DMIC0_CLK_A */
	PAD_CFG_NF(GPP_S7, NONE, DEEP, NF2),   /* DMIC0_DATA */
	PAD_CFG_NF(GPP_S3, NONE, DEEP, NF2),   /* DMIC1_CLK_B */
	PAD_CFG_NF(GPP_S4, NONE, DEEP, NF2),   /* DMIC1_CLK_A */
	PAD_CFG_NF(GPP_S5, NONE, DEEP, NF2),   /* DMIC1_DATA */

	/* DP */
	PAD_CFG_NF(GPP_L_BKLTEN, NONE, DEEP, NF1), /* L_BKLTEN */
	PAD_CFG_NF(GPP_L_BKLTCTL, NONE, DEEP, NF1), /* L_BKLTCTL */
	PAD_CFG_NF(GPP_L_VDDEN, NONE, DEEP, NF1), /* L_VDDEN */
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1), /* HPD_A */
	PAD_CFG_NF(GPP_A18, NONE, DEEP, NF1), /* HPD_B */
	PAD_CFG_NF(GPP_A19, NONE, DEEP, NF1), /* HPD_1 */
	PAD_CFG_NF(GPP_E18, NONE, DEEP, NF1), /* DDP_1_CTRCLK */
	PAD_CFG_NF(GPP_E19, NONE, DEEP, NF1), /* DDP_1_CTRDATA */

	/* TPM */
	/* B19 : GSPI1_CS0B */
	PAD_CFG_NF(GPP_B19, NONE, DEEP, NF1),
	/* B20 : GSPI1_CLK */
	PAD_CFG_NF(GPP_B20, NONE, DEEP, NF1),
	/* B21 : GSPI1_MISO */
	PAD_CFG_NF(GPP_B21, NONE, DEEP, NF1),
	/* B22 : GSPI1_MOSI */
	PAD_CFG_NF(GPP_B22, NONE, DEEP, NF1),

	/* WWAN */
	PAD_CFG_GPO(GPP_H23, 1, DEEP), /* WWAN_PWREN */
	PAD_CFG_NF(GPP_D7, NONE, DEEP, NF1), /* CLK SRC 2 */
	PAD_CFG_GPI_SCI(GPP_C9, NONE, DEEP, LEVEL, INVERT), /* WWAN_WAKE_N */
	PAD_CFG_GPO(GPP_C11, 1, DEEP), /* FULL_CARD_POWER_OFF_N */
	PAD_CFG_GPO(GPP_C10, 1, DEEP), /* WWAN_RST_N */
	PAD_CFG_GPO(GPP_B17, 1, DEEP), /* WWAN_PERST_N */
	PAD_CFG_GPO(GPP_D15, 1, DEEP), /* WWAN_DISABLE_N */
};

const struct pad_config *variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
};

const struct cros_gpio *variant_cros_gpios(size_t *num)
{
	*num = ARRAY_SIZE(cros_gpios);
	return cros_gpios;
}
