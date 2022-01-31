/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>

static const struct pad_config gpio_table[] = {
	/* A11 : PCH_SPI_FPMCU_CS_L */
	PAD_CFG_NF(GPP_A11, NONE, DEEP, NF2),
	/* A12 : FPMCU_RST_ODL */
	PAD_CFG_GPO(GPP_A12, 0, DEEP),
	/* A18 : NC */
	PAD_NC(GPP_A18, NONE),
	/* A19 : NC */
	PAD_NC(GPP_A19, NONE),
	/* C12 : FPMCU_PCH_BOOT1 */
	PAD_CFG_GPO(GPP_C12, 0, DEEP),
	/* F1  : NC */
	PAD_NC(GPP_F1, NONE),
	/* F3  : MEM_STRAP_3 */
	PAD_CFG_GPI(GPP_F3, NONE, PLTRST),
	/* F10 : MEM_STRAP_2 */
	PAD_CFG_GPI(GPP_F10, NONE, PLTRST),
	/* F11 : EMMC_CMD ==> EMMC_CMD */
	PAD_CFG_NF(GPP_F11, NONE, DEEP, NF1),
	/* F12 : EMMC_DATA0 ==> EMMC_DAT0 */
	PAD_CFG_NF(GPP_F12, NONE, DEEP, NF1),
	/* F13 : EMMC_DATA1 ==> EMMC_DAT1 */
	PAD_CFG_NF(GPP_F13, NONE, DEEP, NF1),
	/* F14 : EMMC_DATA2 ==> EMMC_DAT2 */
	PAD_CFG_NF(GPP_F14, NONE, DEEP, NF1),
	/* F15 : EMMC_DATA3 ==> EMMC_DAT3 */
	PAD_CFG_NF(GPP_F15, NONE, DEEP, NF1),
	/* F16 : EMMC_DATA4 ==> EMMC_DAT4 */
	PAD_CFG_NF(GPP_F16, NONE, DEEP, NF1),
	/* F17 : EMMC_DATA5 ==> EMMC_DAT5 */
	PAD_CFG_NF(GPP_F17, NONE, DEEP, NF1),
	/* F18 : EMMC_DATA6 ==> EMMC_DAT6 */
	PAD_CFG_NF(GPP_F18, NONE, DEEP, NF1),
	/* F19 : EMMC_DATA7 ==> EMMC_DAT7 */
	PAD_CFG_NF(GPP_F19, NONE, DEEP, NF1),
	/* F20 : EMMC_RCLK ==> EMMC_RCLK */
	PAD_CFG_NF(GPP_F20, NONE, DEEP, NF1),
	/* F21 : EMMC_CLK ==> EMMC_CLK */
	PAD_CFG_NF(GPP_F21, NONE, DEEP, NF1),
	/* F22 : EMMC_RESET# ==> EMMC_RST_L */
	PAD_CFG_NF(GPP_F22, NONE, DEEP, NF1),
	/* H3  : SPKR_PA_EN */
	PAD_CFG_GPO(GPP_H3, 0, DEEP),
	/* H19 : MEM_STRAP_0 */
	PAD_CFG_GPI(GPP_H19, NONE, PLTRST),
	/* H22 : MEM_STRAP_1 */
	PAD_CFG_GPI(GPP_H22, NONE, PLTRST),
};

const struct pad_config *override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

/*
 * GPIOs configured before ramstage
 * Note: the Hatch platform's romstage will configure
 * the MEM_STRAP_* (a.k.a GPIO_MEM_CONFIG_*) pins
 * as inputs before it reads them, so they are not
 * needed in this table.
 */
static const struct pad_config early_gpio_table[] = {
	/* B15 : H1_SLAVE_SPI_CS_L */
	PAD_CFG_NF(GPP_B15, NONE, DEEP, NF1),
	/* B16 : H1_SLAVE_SPI_CLK */
	PAD_CFG_NF(GPP_B16, NONE, DEEP, NF1),
	/* B17 : H1_SLAVE_SPI_MISO_R */
	PAD_CFG_NF(GPP_B17, NONE, DEEP, NF1),
	/* B18 : H1_SLAVE_SPI_MOSI_R */
	PAD_CFG_NF(GPP_B18, NONE, DEEP, NF1),
	/* C8  : UART_PCH_RX_DEBUG_TX */
	PAD_CFG_NF(GPP_C8, NONE, DEEP, NF1),
	/* C9  : UART_PCH_TX_DEBUG_RX */
	PAD_CFG_NF(GPP_C9, NONE, DEEP, NF1),
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
	/* F2  : MEM_CH_SEL */
	PAD_CFG_GPI(GPP_F2, NONE, PLTRST),
	/* F11 : EMMC_CMD ==> EMMC_CMD */
	PAD_CFG_NF(GPP_F11, NONE, DEEP, NF1),
	/* F12 : EMMC_DATA0 ==> EMMC_DAT0 */
	PAD_CFG_NF(GPP_F12, NONE, DEEP, NF1),
	/* F13 : EMMC_DATA1 ==> EMMC_DAT1 */
	PAD_CFG_NF(GPP_F13, NONE, DEEP, NF1),
	/* F14 : EMMC_DATA2 ==> EMMC_DAT2 */
	PAD_CFG_NF(GPP_F14, NONE, DEEP, NF1),
	/* F15 : EMMC_DATA3 ==> EMMC_DAT3 */
	PAD_CFG_NF(GPP_F15, NONE, DEEP, NF1),
	/* F16 : EMMC_DATA4 ==> EMMC_DAT4 */
	PAD_CFG_NF(GPP_F16, NONE, DEEP, NF1),
	/* F17 : EMMC_DATA5 ==> EMMC_DAT5 */
	PAD_CFG_NF(GPP_F17, NONE, DEEP, NF1),
	/* F18 : EMMC_DATA6 ==> EMMC_DAT6 */
	PAD_CFG_NF(GPP_F18, NONE, DEEP, NF1),
	/* F19 : EMMC_DATA7 ==> EMMC_DAT7 */
	PAD_CFG_NF(GPP_F19, NONE, DEEP, NF1),
	/* F20 : EMMC_RCLK ==> EMMC_RCLK */
	PAD_CFG_NF(GPP_F20, NONE, DEEP, NF1),
	/* F21 : EMMC_CLK ==> EMMC_CLK */
	PAD_CFG_NF(GPP_F21, NONE, DEEP, NF1),
	/* F22 : EMMC_RESET# ==> EMMC_RST_L */
	PAD_CFG_NF(GPP_F22, NONE, DEEP, NF1),
};

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

/*
 * Default GPIO settings before entering non-S5 sleep states.
 * Configure A12: FPMCU_RST_ODL as GPO before entering sleep.
 * This guarantees that A12's native3 function is disabled.
 * See commit c41b0e8ea3b2714bf6d6d88a6b66c333c6919f07.
 */
static const struct pad_config default_sleep_gpio_table[] = {
	PAD_CFG_GPO(GPP_A12, 1, DEEP), /* FPMCU_RST_ODL */
};

/*
 * GPIO settings before entering S5, which are same as default_sleep_gpio_table
 * but also, turn off FPMCU.
 */
static const struct pad_config s5_sleep_gpio_table[] = {
	PAD_CFG_GPO(GPP_A12, 0, DEEP), /* FPMCU_RST_ODL */
	PAD_CFG_GPO(GPP_C11, 0, DEEP), /* PCH_FP_PWR_EN */
};

const struct pad_config *variant_sleep_gpio_table(u8 slp_typ, size_t *num)
{
	if (slp_typ == ACPI_S5) {
		*num = ARRAY_SIZE(s5_sleep_gpio_table);
		return s5_sleep_gpio_table;
	}
	*num = ARRAY_SIZE(default_sleep_gpio_table);
	return default_sleep_gpio_table;
}
