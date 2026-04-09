/* SPDX-License-Identifier: GPL-2.0-only */

#include <variants.h>

/* Early pad configuration in bootblock */
/* clang-format off */
const struct soc_amd_gpio early_gpio_table[] = {
	/* PCIe */
	PAD_NFO(GPIO_26,	PCIE_RST_L,	LOW),		/* Reset asserted */
	PAD_GPO(GPIO_40,			LOW),		/* AUX reset asserted */

	/* Debug Connector */
	PAD_NF(GPIO_141,	UART0_RXD,	PULL_NONE),	/* RXD */
	PAD_NF(GPIO_143,	UART0_TXD,	PULL_NONE),	/* TXD */
};
/* clang-format on */

const struct soc_amd_gpio *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

/* clang-format off */
const struct soc_amd_gpio gpio_table[] = {
	/* Power */
	PAD_NF(GPIO_0,		PWR_BTN_L,	PULL_UP),	/* Power Button */
	PAD_NF(GPIO_1,		SYS_RESET_L,	PULL_UP),	/* Platform Reset */
	PAD_NFO(GPIO_10,	S0A3,		HIGH),		/* S0i3 Support */
	PAD_NF(GPIO_12,		LLB_L,		PULL_NONE),	/* Battery Low */
	PAD_NF(GPIO_23,		AC_PRES,	PULL_UP),	/* Charger Connected */
	PAD_NFO(GPIO_26,	PCIE_RST_L,	HIGH),		/* Reset deasserted */
	PAD_NF(GPIO_32,		LPC_RST_L,	PULL_UP),	/* LPC Reset */
	PAD_NF(GPIO_129,	KBRST_L,	PULL_UP),	/* EC Reset */
	PAD_NF(GPIO_130,	SATA_ACT_L,	PULL_UP),	/* SATA Activity */
	PAD_GPI(GPIO_144,			PULL_NONE),	/* Shutdown */

	/* EC and LPC */
	PAD_SCI(GPIO_8,		PULL_UP,	EDGE_HIGH),	/* EC SCI */
	PAD_NF_SCI(GPIO_22,	LPC_PME_L,	PULL_UP,	EDGE_HIGH),	/* LPC PME */
	PAD_NF(GPIO_74,		LPCCLK0,	PULL_NONE),	/* LPC Clock EC */
	PAD_NF(GPIO_75,		LPCCLK1,	PULL_NONE),	/* LPC Clock 1 */
	PAD_NF(GPIO_76,		SPI_ROM_GNT,	PULL_NONE),	/* SPI ROM Grant */
	PAD_NF(GPIO_86,		LPC_SMI_L,	PULL_UP),	/* EC SMI */
	PAD_NF(GPIO_87,		SERIRQ,		PULL_NONE),	/* LPC SERIRQ */
	PAD_NF(GPIO_88,		LPC_CLKRUN_L,	PULL_NONE),	/* Clock Run */
	PAD_NF(GPIO_104,	LAD0,		PULL_NONE),	/* LPC LAD0 */
	PAD_NF(GPIO_105,	LAD1,		PULL_NONE),	/* LPC LAD1 */
	PAD_NF(GPIO_106,	LAD2,		PULL_NONE),	/* LPC LAD2 */
	PAD_NF(GPIO_107,	LAD3,		PULL_NONE),	/* LPC LAD3 */
	PAD_NF(GPIO_109,	LFRAME_L,	PULL_NONE),	/* LPC Frame */

	/* SSD */
	PAD_NF_SCI(GPIO_2,	WAKE_L,		PULL_NONE,	EDGE_HIGH),	/* Wake */
	PAD_GPI(GPIO_4,				PULL_NONE),	/* Detect */
	PAD_NF(GPIO_5,		DEVSLP0,	PULL_DOWN),	/* Device Sleep */
	PAD_GPO(GPIO_40,			HIGH),		/* AUX reset deasserted */
	PAD_NF(GPIO_115,	CLK_REQ1_L,	PULL_UP),	/* Clock Request */

	/* Wireless */
	PAD_SCI(GPIO_18,	PULL_UP,	EDGE_HIGH),	/* WLAN Wake */
	PAD_GPO(GPIO_69,			HIGH),		/* Bluetooth RF Kill */
	PAD_GPO(GPIO_91,			HIGH),		/* WiFi Disable */
	PAD_NF(GPIO_121,	CLK_REQ6_L,	PULL_UP),	/* Clock Request */

	/* Touchpad */
	PAD_SCI(GPIO_9,		PULL_NONE,	LEVEL_LOW),	/* Interrupt */
	PAD_NF(GPIO_19,		I2C3_SCL,	PULL_NONE),	/* Clock */
	PAD_NF(GPIO_20,		I2C3_SDA,	PULL_NONE),	/* Data */

	/* High-Definition Audio */
	PAD_NF(GPIO_108,	GPIOxx,		PULL_UP),	/* Interrupt */

	/* SMBus */
	PAD_NF(GPIO_113,	SCL0,		PULL_NONE),	/* Clock */
	PAD_NF(GPIO_114,	SDA0,		PULL_NONE),	/* Data */

	/* USB */
	PAD_NF(GPIO_16,		USB_OC0_L,	PULL_UP),	/* Type-C VBUS OC */
	PAD_NF(GPIO_17,		USB_OC1_L,	PULL_NONE),	/* USB OC1 */

	/* Not connected */
	PAD_NC(GPIO_3),
	PAD_NC(GPIO_6),
	PAD_NC(GPIO_7),
	PAD_NC(GPIO_11),
	PAD_NC(GPIO_21),
	PAD_NC(GPIO_24),
	PAD_NC(GPIO_27),
	PAD_NC(GPIO_29),
	PAD_NC(GPIO_30),
	PAD_NC(GPIO_31),
	PAD_NC(GPIO_42),
	PAD_NC(GPIO_67),
	PAD_NC(GPIO_68),
	PAD_NC(GPIO_70),
	PAD_NC(GPIO_84),
	PAD_NC(GPIO_85),
	PAD_NC(GPIO_89),
	PAD_NC(GPIO_90),
	PAD_NC(GPIO_92),
	PAD_NC(GPIO_116),
	PAD_NC(GPIO_120),
	PAD_NC(GPIO_131),
	PAD_NC(GPIO_132),
	PAD_NC(GPIO_140),
	PAD_NC(GPIO_142),
	PAD_NC(GPIO_145),
	PAD_NC(GPIO_146),
	PAD_NC(GPIO_147),
	PAD_NC(GPIO_148),
	PAD_NC(GPIO_256),
	PAD_NC(GPIO_257),
	PAD_NC(GPIO_258),
	PAD_NC(GPIO_259),
	PAD_NC(GPIO_260),
	PAD_NC(GPIO_261),
	PAD_NC(GPIO_262),
	PAD_NC(GPIO_263),
	PAD_NC(GPIO_264),
	PAD_NC(GPIO_265),
	PAD_NC(GPIO_266),
	PAD_NC(GPIO_267),
	PAD_NC(GPIO_268),
	PAD_NC(GPIO_269),
	PAD_NC(GPIO_270),
	PAD_NC(GPIO_271),
};
/* clang-format on */


/* Pad configuration in romstage. */
const struct soc_amd_gpio *variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}
