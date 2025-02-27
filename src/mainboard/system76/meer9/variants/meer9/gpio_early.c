/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/gpio.h>
#include <soc/gpio.h>

static const struct pad_config early_gpio_table[] = {
	PAD_CFG_NF(GPP_C00, NONE, DEEP, NF1), // SMB_CLK
	PAD_CFG_NF(GPP_C01, NONE, DEEP, NF1), // SMB_DATA
};

void mainboard_configure_early_gpios(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
