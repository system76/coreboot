/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>

static const struct pad_config gpio_table[] = {
	/* A16 : SD_OC_ODL */
	PAD_CFG_GPI(GPP_A16, NONE, DEEP),
	/* A18 : LAN_PE_ISOLATE_ODL */
	PAD_CFG_GPO(GPP_A18, 1, DEEP),
	/* A23 : M2_WLAN_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A23, NONE, PLTRST, LEVEL, INVERT),

	/* B5 : LAN_CLKREQ_ODL */
	PAD_CFG_NF(GPP_B5, NONE, DEEP, NF1),

	/* C0 : SMBCLK */
	PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),
	/* C1 : SMBDATA */
	PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1),
	/* C6: M2_WLAN_WAKE_ODL */
	PAD_CFG_GPI_SCI_LOW(GPP_C6, NONE, DEEP, EDGE_SINGLE),
	/* C7 : LAN_WAKE_ODL */
	PAD_CFG_GPI_SCI_LOW(GPP_C7, NONE, DEEP, EDGE_SINGLE),
	/* C10 : PCH_PCON_RST_ODL */
	PAD_CFG_GPO(GPP_C10, 1, DEEP),
	/* C11 : PCH_PCON_PDB_ODL */
	PAD_CFG_GPO(GPP_C11, 1, DEEP),
	/* C15  : WLAN_OFF_L */
	PAD_CFG_GPO(GPP_C15, 1, DEEP),

	/* E2  : EN_PP_MST_OD */
	PAD_CFG_GPO(GPP_E2, 1, DEEP),
	/* E9 : USB_A0_OC_ODL */
	PAD_CFG_NF(GPP_E9, NONE, DEEP, NF1),
	/* E10 : USB_A1_OC_ODL */
	PAD_CFG_NF(GPP_E10, NONE, DEEP, NF1),

	/* F11 : EMMC_CMD */
	PAD_CFG_NF(GPP_F11, NONE, DEEP, NF1),
	/* F12 : EMMC_DATA0 */
	PAD_CFG_NF(GPP_F12, NONE, DEEP, NF1),
	/* F13 : EMMC_DATA1 */
	PAD_CFG_NF(GPP_F13, NONE, DEEP, NF1),
	/* F14 : EMMC_DATA2 */
	PAD_CFG_NF(GPP_F14, NONE, DEEP, NF1),
	/* F15 : EMMC_DATA3 */
	PAD_CFG_NF(GPP_F15, NONE, DEEP, NF1),
	/* F16 : EMMC_DATA4 */
	PAD_CFG_NF(GPP_F16, NONE, DEEP, NF1),
	/* F17 : EMMC_DATA5 */
	PAD_CFG_NF(GPP_F17, NONE, DEEP, NF1),
	/* F18 : EMMC_DATA6 */
	PAD_CFG_NF(GPP_F18, NONE, DEEP, NF1),
	/* F19 : EMMC_DATA7 */
	PAD_CFG_NF(GPP_F19, NONE, DEEP, NF1),
	/* F20 : EMMC_RCLK */
	PAD_CFG_NF(GPP_F20, NONE, DEEP, NF1),
	/* F21 : EMMC_CLK */
	PAD_CFG_NF(GPP_F21, NONE, DEEP, NF1),
	/* F22 : EMMC_RST_L */
	PAD_CFG_NF(GPP_F22, NONE, DEEP, NF1),

	/* H4: PCH_I2C_PCON_SDA */
	PAD_CFG_NF(GPP_H4, NONE, DEEP, NF1),
	/* H5: PCH_I2C_PCON_SCL */
	PAD_CFG_NF(GPP_H5, NONE, DEEP, NF1),
	/* H22 : PWM_PP3300_BIOZZER */
	PAD_CFG_GPO(GPP_H22, 0, DEEP),
};

const struct pad_config *override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* B14 : GPP_B14_STRAP */
	PAD_NC(GPP_B14, NONE),
	/* B22 : GPP_B22_STRAP */
	PAD_NC(GPP_B22, NONE),
	/* E19 : GPP_E19_STRAP */
	PAD_NC(GPP_E19, NONE),
	/* E21 : GPP_E21_STRAP */
	PAD_NC(GPP_E21, NONE),
	/* B15 : H1_SLAVE_SPI_CS_L */
	PAD_CFG_NF(GPP_B15, NONE, DEEP, NF1),
	/* B16 : H1_SLAVE_SPI_CLK */
	PAD_CFG_NF(GPP_B16, NONE, DEEP, NF1),
	/* B17 : H1_SLAVE_SPI_MISO_R */
	PAD_CFG_NF(GPP_B17, NONE, DEEP, NF1),
	/* B18 : H1_SLAVE_SPI_MOSI_R */
	PAD_CFG_NF(GPP_B18, NONE, DEEP, NF1),
	/* C14 : BT_DISABLE_L */
	PAD_CFG_GPO(GPP_C14, 0, DEEP),
	/* PCH_WP_OD */
	PAD_CFG_GPI(GPP_C20, NONE, DEEP),
	/* C21 : H1_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_C21, NONE, PLTRST, LEVEL, INVERT),
	/* C22 : EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_C22, NONE, DEEP),
	/* C23 : WLAN_PE_RST# */
	PAD_CFG_GPO(GPP_C23, 1, DEEP),
	/* E1  : M2_SSD_PEDET */
	PAD_CFG_NF(GPP_E1, NONE, DEEP, NF1),
	/* E5  : SATA_DEVSLP1 */
	PAD_CFG_NF(GPP_E5, NONE, PLTRST, NF1),
};

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}
