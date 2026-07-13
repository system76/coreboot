/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/gpio.h>
#include <soc/gpio.h>

static const struct pad_config early_gpio_table[] = {
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1), // UART2_RX
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1), // UART2_TX

	PAD_CFG_GPO(GPP_F9, 0, DEEP), // DGPU_PWR_EN
	PAD_CFG_GPI(GPP_R8, NONE, DEEP), // DGPU_PWRGD_R
	PAD_CFG_GPO(GPP_R16, 0, DEEP), // DGPU_RST#_PCH
};

void mainboard_configure_early_gpios(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
