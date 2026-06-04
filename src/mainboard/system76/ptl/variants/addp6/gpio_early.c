/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/gpio.h>
#include <soc/gpio.h>

static const struct pad_config early_gpio_table[] = {
	// Model detection
	PAD_CFG_GPI(GPP_E11, NONE, PLTRST), // BOARD_ID1
	PAD_CFG_GPI(GPP_E14, NONE, PLTRST), // BOARD_ID2
	PAD_CFG_GPI(GPP_E15, NONE, PLTRST), // BOARD_ID3
	PAD_CFG_GPI(GPP_E17, NONE, PLTRST), // BOARD_ID4

	// Debug
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1), // UART0_RX
	PAD_CFG_NF(GPP_H09, NONE, DEEP, NF1), // UART0_TX

	// SSD1
	PAD_CFG_GPO(GPP_B16, 0, PLTRST), // SSD1_PWR_EN
	PAD_CFG_GPO(GPP_E03, 0, PLTRST), // M2_SSD1_RST#
	PAD_CFG_GPI(GPP_C14, NONE, DEEP), // SSD1_CLK5REQ#_N

	// SSD2
	PAD_CFG_GPO(GPP_E08, 0, PLTRST), // SSD2_PWR_EN
	PAD_CFG_GPO(GPP_B09, 0, PLTRST), // M2_SSD2_RST#
	PAD_CFG_GPI(GPP_D18, NONE, DEEP), // SSD2_CLK6REQ#_N

	// dGPU
	PAD_CFG_GPO(GPP_F14, 0, DEEP), // DGPU_PWR_EN
	PAD_CFG_GPI(GPP_F20, NONE, DEEP), // DGPU_PWRGD
	PAD_CFG_GPO(GPP_D09, 0, DEEP), // DGPU_RST#

};

void mainboard_configure_early_gpios(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
