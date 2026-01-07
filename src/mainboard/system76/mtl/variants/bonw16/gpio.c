/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/gpio.h>
#include <soc/gpio.h>

static const struct pad_config gpio_table[] = {
//TODO
};

void mainboard_configure_gpios(void)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
