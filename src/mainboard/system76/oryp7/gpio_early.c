/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/gpio.h>
#include <soc/gpio.h>

static const struct pad_config early_gpio_table[] = {
	PAD_CFG_TERM_GPO(GPP_C14, 1, NONE, RSMRST), // M.2_PLT_RST_CNTRL1#
	PAD_CFG_TERM_GPO(GPP_C15, 1, NONE, RSMRST), // M.2_PLT_RST_CNTRL2#
	PAD_CFG_TERM_GPO(GPP_F0, 1, NONE, RSMRST), // TBT_PERST_N
	PAD_CFG_TERM_GPO(GPP_F22, 0, NONE, DEEP), // DGPU_RST_N
	PAD_CFG_TERM_GPO(GPP_F23, 0, NONE, DEEP), // DGPU_PWR_EN
	PAD_CFG_TERM_GPO(GPP_H16, 1, NONE, RSMRST), // TBT_RTD3_PWR_EN_R
	PAD_CFG_TERM_GPO(GPP_K8, 1, NONE, RSMRST), // SATA_M2_PWR_EN1
	PAD_CFG_TERM_GPO(GPP_K9, 1, NONE, RSMRST), // SATA_M2_PWR_EN2
	PAD_CFG_TERM_GPO(GPP_K11, 1, NONE, RSMRST), // GPIO_LANRTD3
};

void mainboard_configure_early_gpios(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
