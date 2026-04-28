/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/gpio.h>
#include <soc/gpio.h>

static const struct pad_config early_gpio_table[] = {
	// Model detection
	PAD_CFG_GPI(GPP_E11, NONE, PLTRST), // BOARD_ID1
	PAD_CFG_GPI(GPP_E14, NONE, PLTRST), // BOARD_ID2
	PAD_CFG_GPI(GPP_E15, NONE, PLTRST), // BOARD_ID3
	PAD_CFG_GPI(GPP_E17, NONE, PLTRST), // BOARD_ID4
	PAD_CFG_GPI(GPP_C00, UP_20K, PLTRST), // BOARD_ID5
	PAD_CFG_GPI(GPP_C01, UP_20K, PLTRST), // BOARD_ID6

	// Debug
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1), // UART0_RX
	PAD_CFG_NF(GPP_H09, NONE, DEEP, NF1), // UART0_TX

	// SSD
	PAD_CFG_GPO(GPP_B09, 0, PLTRST), // M2_SSD1_RST#
	PAD_CFG_GPO(GPP_B10, 0, PLTRST), // SSD1_PWR_EN
	PAD_CFG_GPI(GPP_D18, NONE, DEEP), // SSD1_CLKREQ#_N
};

void mainboard_configure_early_gpios(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
