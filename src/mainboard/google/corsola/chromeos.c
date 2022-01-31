/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <gpio.h>
#include <security/tpm/tis.h>

#include "gpio.h"

void setup_chromeos_gpios(void)
{
	/* Set up open-drain pins */
	gpio_input(GPIO_SAR_INT_ODL);
	gpio_input(GPIO_BT_WAKE_AP_ODL);
	gpio_input(GPIO_WIFI_INT_ODL);
	gpio_input(GPIO_DPBRDG_INT_ODL);
	gpio_input(GPIO_EDPBRDG_INT_ODL);
	gpio_input(GPIO_EC_AP_HPD_OD);
	gpio_input(GPIO_TCHPAD_INT_ODL);
	gpio_input(GPIO_TCHSCR_INT_1V8_ODL);
	gpio_input(GPIO_EC_AP_INT_ODL);
	gpio_input(GPIO_EC_IN_RW_ODL);
	gpio_input(GPIO_GSC_AP_INT_ODL);
	gpio_input(GPIO_AP_WP_ODL);
	gpio_input(GPIO_HP_INT_ODL);
	gpio_input(GPIO_PEN_EJECT_OD);
	gpio_input(GPIO_UCAM_DET_ODL);

	/* Set up GPIOs */
	gpio_output(GPIO_RESET, 0);
	gpio_output(GPIO_XHCI_DONE, 0);
	gpio_output(GPIO_EN_SPK, 0);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{GPIO_EC_AP_INT_ODL.id, ACTIVE_LOW, -1, "EC interrupt"},
		{GPIO_EC_IN_RW_ODL.id, ACTIVE_LOW, -1, "EC in RW"},
		{GPIO_GSC_AP_INT_ODL.id, ACTIVE_HIGH, -1, "TPM interrupt"},
		{GPIO_EN_SPK.id, ACTIVE_HIGH, -1, "speaker enable"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_ec_is_trusted(void)
{
	/* EC is trusted if not in RW. This is active low. */
	return !!gpio_get(GPIO_EC_IN_RW_ODL);
}

int tis_plat_irq_status(void)
{
	return gpio_eint_poll(GPIO_GSC_AP_INT_ODL);
}
