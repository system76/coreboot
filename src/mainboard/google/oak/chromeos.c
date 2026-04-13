/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <boardid.h>
#include <boot/coreboot_tables.h>
#include <bootmode.h>
#include <gpio.h>

void setup_chromeos_gpios(void)
{
	gpio_input(GPIO_WP);
	gpio_input_pullup(EC_IN_RW);
	gpio_input_pullup(EC_IRQ);
	gpio_input(LID);
	gpio_input_pullup(POWER_BUTTON);
	if (board_id() + CONFIG_BOARD_ID_ADJUSTMENT < 5)
		gpio_output(EC_SUSPEND_L, 1);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{LID.id, ACTIVE_HIGH, -1, "lid"},
		{POWER_BUTTON.id, ACTIVE_HIGH, -1, "power"},
		{EC_IN_RW.id, ACTIVE_HIGH, -1, "EC in RW"},
		{EC_IRQ.id, ACTIVE_LOW, -1, "EC interrupt"},
		{CR50_IRQ.id, ACTIVE_HIGH, -1, "TPM interrupt"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_write_protect_state(void)
{
	return !gpio_get(GPIO_WP);
}

int get_ec_is_trusted(void)
{
	/* EC is trusted if not in RW. */
	return !gpio_get(EC_IN_RW);
}
