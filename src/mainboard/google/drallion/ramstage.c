/* SPDX-License-Identifier: GPL-2.0-only */

#include <smbios.h>
#include <soc/ramstage.h>
#include <variant/gpio.h>

/* mainboard silk screen shows DIMM-A and DIMM-B */
void smbios_fill_dimm_locator(const struct dimm_info *dimm,
	struct smbios_type17 *t)
{
	switch (dimm->channel_num) {
	case 0:
		t->device_locator = smbios_add_string(t->eos, "DIMM-A");
		break;
	case 1:
		t->device_locator = smbios_add_string(t->eos, "DIMM-B");
		break;
	default:
		t->device_locator = smbios_add_string(t->eos, "UNKNOWN");
		break;
	}
}

static void mainboard_init(void *chip_info)
{
	const struct pad_config *gpio_table;
	size_t num_gpios;

	gpio_table = variant_gpio_table(&num_gpios);
	gpio_configure_pads(gpio_table, num_gpios);
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};
