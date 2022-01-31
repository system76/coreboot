/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <bootmode.h>
#include "board.h"
#include <security/tpm/tis.h>

void setup_chromeos_gpios(void)
{
	gpio_input_pullup(GPIO_EC_IN_RW);
	gpio_input_pullup(GPIO_AP_EC_INT);
	gpio_input_pullup(GPIO_SD_CD_L);
	gpio_input_irq(GPIO_H1_AP_INT, IRQ_TYPE_RISING_EDGE, GPIO_PULL_UP);
	gpio_output(GPIO_AMP_ENABLE, 0);
	gpio_output(GPIO_BACKLIGHT_ENABLE, 0);

	if (CONFIG(TROGDOR_HAS_MIPI_PANEL)) {
		gpio_output(GPIO_MIPI_1V8_ENABLE, 0);
		gpio_output(GPIO_AVDD_LCD_ENABLE, 0);
		gpio_output(GPIO_VDD_RESET_1V8, 0);
		gpio_output(GPIO_AVEE_LCD_ENABLE, 0);
	} else {
		gpio_output(GPIO_EN_PP3300_DX_EDP, 0);
		gpio_output(GPIO_EDP_BRIDGE_ENABLE, 0);
		gpio_output(GPIO_PS8640_EDP_BRIDGE_3V3_ENABLE, 0);
		gpio_output(GPIO_PS8640_EDP_BRIDGE_RST_L, 0);
	}

	if (CONFIG(TROGDOR_HAS_FINGERPRINT)) {
		gpio_output(GPIO_FPMCU_BOOT0, 0);
		gpio_output(GPIO_FP_RST_L, 0);
		gpio_output(GPIO_EN_FP_RAILS, 0);
	}
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{GPIO_EC_IN_RW.addr, ACTIVE_LOW, gpio_get(GPIO_EC_IN_RW),
			"EC in RW"},
		{GPIO_AP_EC_INT.addr, ACTIVE_LOW, gpio_get(GPIO_AP_EC_INT),
			"EC interrupt"},
		{GPIO_H1_AP_INT.addr, ACTIVE_HIGH, gpio_get(GPIO_H1_AP_INT),
			"TPM interrupt"},
		{GPIO_SD_CD_L.addr, ACTIVE_LOW, gpio_get(GPIO_SD_CD_L),
			"SD card detect"},
		{GPIO_AMP_ENABLE.addr, ACTIVE_HIGH, gpio_get(GPIO_AMP_ENABLE),
			"speaker enable"},
		{GPIO_BACKLIGHT_ENABLE.addr, ACTIVE_HIGH,
			gpio_get(GPIO_BACKLIGHT_ENABLE), "backlight"},
	};

	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int tis_plat_irq_status(void)
{
	return gpio_irq_status(GPIO_H1_AP_INT);
}

int get_ec_is_trusted(void)
{
	/* EC is trusted if not in RW. This is active low. */
	return !!gpio_get(GPIO_EC_IN_RW);
}
