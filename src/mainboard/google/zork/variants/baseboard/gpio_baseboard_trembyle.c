/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <baseboard/variants.h>
#include <delay.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <soc/smi.h>
#include <variant/gpio.h>

static const struct soc_amd_gpio gpio_set_stage_ram[] = {
	/* PWR_BTN_L */
	PAD_NF(GPIO_0, PWR_BTN_L, PULL_NONE),
	/* SYS_RESET_L */
	PAD_NF(GPIO_1, SYS_RESET_L, PULL_NONE),
	/* WIFI_PCIE_WAKE_ODL */
	PAD_NF_SCI(GPIO_2, WAKE_L, PULL_NONE, EDGE_LOW),
	/* H1_FCH_INT_ODL */
	PAD_INT(GPIO_3, PULL_NONE, EDGE_LOW, STATUS_DELIVERY),
	/* PEN_DETECT_ODL */
	PAD_WAKE(GPIO_4, PULL_NONE, EDGE_HIGH, S3),
	/* PEN_POWER_EN - Enabled*/
	PAD_GPO(GPIO_5, HIGH),
	/* FPMCU_INT_L */
	PAD_SCI(GPIO_6, PULL_NONE, LEVEL_LOW),
	/* I2S_SDIN */
	PAD_NF(GPIO_7, ACP_I2S_SDIN, PULL_NONE),
	/* I2S_LRCLK - Bit banged in depthcharge */
	PAD_NF(GPIO_8, ACP_I2S_LRCLK, PULL_NONE),
	/* TOUCHPAD_INT_ODL */
	PAD_SCI(GPIO_9, PULL_NONE, LEVEL_LOW),
	/* S0iX SLP - goes to EC & FPMCU */
	PAD_GPO(GPIO_10, HIGH),
	/* USI_INT_ODL */
	PAD_GPI(GPIO_12, PULL_NONE),
	/* EN_PWR_TOUCHPAD_PS2 */
	PAD_GPO(GPIO_13, HIGH),
	/* BT_DISABLE */
	PAD_GPO(GPIO_14, LOW),
	/* GPIO_15: Not available */
	/* USB_OC0_L - USB C0 + USB A0 */
	PAD_NF(GPIO_16, USB_OC0_L, PULL_NONE),
	/* USB_OC1_L - USB C1 + USB A1 */
	PAD_NF(GPIO_17, USB_OC1_L, PULL_NONE),
	/* WIFI_DISABLE */
	PAD_GPO(GPIO_18, LOW),
	/* I2C3_SCL - H1 */
	PAD_NF(GPIO_19, I2C3_SCL, PULL_NONE),
	/* I2C3_SDA - H1 */
	PAD_NF(GPIO_20, I2C3_SDA, PULL_NONE),
	/* EMMC_CMD */
	PAD_NF(GPIO_21, EMMC_CMD, PULL_NONE),
	/* EC_FCH_SCI_ODL */
	PAD_SCI(GPIO_22, PULL_NONE, EDGE_LOW),
	/* AC_PRES */
	PAD_NF(GPIO_23, AC_PRES, PULL_UP),
	/* EC_FCH_WAKE_L */
	PAD_SCI(GPIO_24, PULL_NONE, EDGE_LOW),
	/* GPIO_25: Not available */
	/* PCIE_RST0_L - Fixed timings */
	PAD_NF(GPIO_26, PCIE_RST_L, PULL_NONE),
	/* PCIE_RST1_L (unused) */
	PAD_NC(GPIO_27),
	/* GPIO_28: Not available */
	/* GPIO_29: HP_INT_ODL */
	PAD_GPI(GPIO_29, PULL_NONE),
	/* FCH_ESPI_EC_CS_L */
	PAD_NF(GPIO_30, ESPI_CS_L, PULL_NONE),
	/*  EC_AP_INT_ODL (Sensor Framesync) */
	PAD_GPI(GPIO_31, PULL_NONE),
	/* GPIO_33 - GPIO_39: Not available */
	/* NVME_AUX_RESET_L */
	PAD_GPO(GPIO_40, HIGH),
	/* GPIO_41: Not available */
	/* GPIO_42: Handled in bootblock for wifi power/reset control. */
	/* GPIO_43 - GPIO_66: Not available */
	/* DMIC SEL */
	/*
	 * Make sure Ext ROM Sharing is disabled before using this GPIO.  Otherwise SPI flash
	 * access will be very slow.
	 */
	PAD_GPO(GPIO_67, LOW), // Select Camera 1 Dmic
	/* EMMC_RESET_L */
	PAD_GPO(GPIO_68, HIGH),
	/* FPMCU_BOOT0 */
	PAD_GPO(GPIO_69, LOW),
	/* EMMC_CLK */
	PAD_NF(GPIO_70, EMMC_CLK, PULL_NONE),
	/* GPIO_71 - GPIO_73: Not available */
	/* EMMC_DATA4 */
	PAD_NF(GPIO_74, EMMC_DATA4, PULL_NONE),
	/* EMMC_DATA6 */
	PAD_NF(GPIO_75, EMMC_DATA6, PULL_NONE),
	/* EN_PWR_CAMERA */
	PAD_GPO(GPIO_76, HIGH),
	/* GPIO_77 - GPIO_83: Not available */
	/* RAM_ID_4 */
	PAD_GPI(GPIO_84, PULL_NONE),
	/* APU_EDP_BL_DISABLE */
	PAD_GPO(GPIO_85, LOW),
	/* WIFI_AUX_RESET_L */
	PAD_GPO(GPIO_86, HIGH),
	/* EMMC_DATA7 */
	PAD_NF(GPIO_87, EMMC_DATA7, PULL_NONE),
	/* EMMC_DATA5 */
	PAD_NF(GPIO_88, EMMC_DATA5, PULL_NONE),
	/* GPIO_89 - unused */
	PAD_NC(GPIO_89),
	/* EN_PWR_TOUCHSCREEN */
	PAD_GPO(GPIO_90, LOW),
	/* EN_SPKR */
	PAD_GPO(GPIO_91, LOW),
	/* CLK_REQ0_L - WIFI */
	PAD_NF(GPIO_92, CLK_REQ0_L, PULL_NONE),
	/* GPIO_93 - GPIO_103: Not available */
	/* EMMC_DATA0 */
	PAD_NF(GPIO_104, EMMC_DATA0, PULL_NONE),
	/* EMMC_DATA1 */
	PAD_NF(GPIO_105, EMMC_DATA1, PULL_NONE),
	/* EMMC_DATA2 */
	PAD_NF(GPIO_106, EMMC_DATA2, PULL_NONE),
	/* EMMC_DATA3 */
	PAD_NF(GPIO_107, EMMC_DATA3, PULL_NONE),
	/* ESPI_ALERT_L */
	PAD_NF(GPIO_108, ESPI_ALERT_L, PULL_UP),
	/* EMMC_DS */
	PAD_NF(GPIO_109, EMMC_DS, PULL_NONE),
	/* GPIO_110 - GPIO112: Not available */
	/* I2C2_SCL - USI/Touchpad */
	PAD_NF(GPIO_113, I2C2_SCL, PULL_NONE),
	/* I2C2_SDA - USI/Touchpad */
	PAD_NF(GPIO_114, I2C2_SDA, PULL_NONE),
	/* CLK_REQ1_L - SD Card */
	PAD_NF(GPIO_115, CLK_REQ1_L, PULL_NONE),
	/* RAM_ID_3 */
	PAD_GPI(GPIO_116, PULL_NONE),
	/* GPIO_117 - GPIO_119: Not available */
	/* RAM_ID_1 */
	PAD_GPI(GPIO_120, PULL_NONE),
	/* RAM_ID_0 */
	PAD_GPI(GPIO_121, PULL_NONE),
	/* GPIO_122 - GPIO_128: Not available */
	/* KBRST_L */
	PAD_NF(GPIO_129, KBRST_L, PULL_NONE),
	/* EC_IN_RW_OD */
	PAD_GPI(GPIO_130, PULL_NONE),
	/* RAM_ID_2 */
	PAD_GPI(GPIO_131, PULL_NONE),
	/* CLK_REQ4_L - SSD */
	PAD_NF(GPIO_132, CLK_REQ4_L, PULL_NONE),
	/* GPIO_133 - GPIO_134: Not available */
	/* DEV_BEEP_CODEC_IN (Dev beep Data out) */
	PAD_GPI(GPIO_135, PULL_NONE),
	/* UART0_RXD - DEBUG */
	PAD_NF(GPIO_136, UART0_RXD, PULL_NONE),
	/* BIOS_FLASH_WP_ODL */
	PAD_GPI(GPIO_137, PULL_NONE),
	/* UART0_TXD - DEBUG */
	PAD_NF(GPIO_138, UART0_TXD, PULL_NONE),
	/* DEV_BEEP_BCLK */
	PAD_GPI(GPIO_139, PULL_NONE),
	/* USI_RESET_L */
	PAD_GPO(GPIO_140, LOW),
	/* UART1_RXD - FPMCU */
	PAD_NF(GPIO_141, UART1_RXD, PULL_NONE),
	/* SD_AUX_RESET_L */
	PAD_GPO(GPIO_142, HIGH),
	/* UART1_TXD - FPMCU */
	PAD_NF(GPIO_143, UART1_TXD, PULL_NONE),
	/* USI_REPORT_EN */
	PAD_GPO(GPIO_144, LOW),
};

const __weak
struct soc_amd_gpio *variant_base_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(gpio_set_stage_ram);
	return gpio_set_stage_ram;
}

static void wifi_power_reset_configure_active_low_power(void)
{
	/*
	 * Configure WiFi GPIOs such that:
	 * - WIFI_AUX_RESET_L is configured first to assert PERST# to WiFi device.
	 * - Enable power to WiFi using EN_PWR_WIFI_L.
	 * - Wait for >50ms after power to WiFi is enabled. (Time between bootblock & ramstage)
	 * - WIFI_AUX_RESET_L gets deasserted later in mainboard_configure_gpios in ramstage
	 */
	static const struct soc_amd_gpio v3_wifi_table[] = {
		/* WIFI_AUX_RESET_L */
		PAD_GPO(GPIO_86, LOW),
		/* EN_PWR_WIFI_L */
		PAD_GPO(GPIO_42, LOW),
	};
	gpio_configure_pads(v3_wifi_table, ARRAY_SIZE(v3_wifi_table));

}

static void wifi_power_reset_configure_active_high_power(void)
{
	/*
	 * When GPIO_42 is configured as active high for enabling WiFi power, WIFI_AUX_RESET_L
	 * gets pulled high because of external PU to PP3300_WIFI. Thus, EN_PWR_WIFI needs to be
	 * set low before driving it high to trigger a WiFi power cycle to meet PCIe
	 * requirements. Thus, configura GPIOs such that:
	 * - WIFI_AUX_RESET_L is configured first to assert PERST# to WiFi device
	 * - Disable power to WiFi.
	 * - Wait 10ms for WiFi power to go low.
	 * - Enable power to WiFi using EN_PWR_WIFI.
	 * - Deassert WIFI_AUX_RESET_L.
	 */
	static const struct soc_amd_gpio v3_wifi_table[] = {
		/* WIFI_AUX_RESET_L */
		PAD_GPO(GPIO_86, LOW),
		/* EN_PWR_WIFI */
		PAD_GPO(GPIO_42, LOW),
	};
	gpio_configure_pads(v3_wifi_table, ARRAY_SIZE(v3_wifi_table));

	mdelay(10);
	gpio_set(GPIO_42, 1);
	mdelay(50);
	gpio_set(GPIO_86, 1);
}

static void wifi_power_reset_configure_v3(void)
{
	if (variant_has_active_low_wifi_power())
		wifi_power_reset_configure_active_low_power();
	else
		wifi_power_reset_configure_active_high_power();
}

static void wifi_power_reset_configure_pre_v3(void)
{
	/*
	 * Configure WiFi GPIOs such that:
	 * - WIFI_AUX_RESET_L is configured first to assert PERST# to WiFi device.
	 * - Disable power to WiFi since GPIO_29 goes high on PWRGOOD but has a glitch on RESET#
	 *   deassertion causing WiFi to enter a bad state.
	 * - Wait 10ms for WiFi power to go low.
	 * - Enable power to WiFi using EN_PWR_WIFI.
	 * - Wait for 50ms after power to WiFi is enabled.
	 * - Deassert WIFI_AUX_RESET_L.
	 */
	static const struct soc_amd_gpio pre_v3_wifi_table[] = {
		/* WIFI_AUX_RESET_L */
		PAD_GPO(GPIO_42, LOW),
		/* EN_PWR_WIFI */
		PAD_GPO(GPIO_29, LOW),
	};
	gpio_configure_pads(pre_v3_wifi_table, ARRAY_SIZE(pre_v3_wifi_table));

	mdelay(10);
	gpio_set(GPIO_29, 1);
	mdelay(50);
	gpio_set(GPIO_42, 1);
}

__weak void variant_pcie_gpio_configure(void)
{
	static const struct soc_amd_gpio pcie_gpio_table[] = {
		/* NVME_AUX_RESET_L */
		PAD_GPO(GPIO_40, HIGH),
		/* CLK_REQ0_L - WIFI */
		PAD_NF(GPIO_92, CLK_REQ0_L, PULL_NONE),
		/* CLK_REQ1_L - SD Card */
		PAD_NF(GPIO_115, CLK_REQ1_L, PULL_NONE),
		/* CLK_REQ4_L - SSD */
		PAD_NF(GPIO_132, CLK_REQ4_L, PULL_NONE),
		/* SD_AUX_RESET_L */
		PAD_GPO(GPIO_142, HIGH),
	};

	gpio_configure_pads(pcie_gpio_table, ARRAY_SIZE(pcie_gpio_table));

	if (variant_uses_v3_schematics())
		wifi_power_reset_configure_v3();
	else
		wifi_power_reset_configure_pre_v3();
}

__weak void finalize_gpios(int slp_typ)
{
	if (variant_has_fingerprint() && slp_typ != ACPI_S3) {

		if (fpmcu_needs_delay())
			mdelay(550);

		/*
		 * Enable the FPMCU by enabling EN_PWR_FP, then bringing it out
		 * of reset by setting FPMCU_RST_L high 3ms later.
		 */
		gpio_set(GPIO_32, 1);
		mdelay(3);
		gpio_set(GPIO_11, 1);
	}
}

static const struct soc_amd_gpio gpio_fingerprint_bootblock_table[] = {
	/* FPMCU_RST_L */
	PAD_GPO(GPIO_11, LOW),
	/* EN_PWR_FP */
	PAD_GPO(GPIO_32, LOW),
};

static const struct soc_amd_gpio gpio_no_fingerprint_bootblock_table[] = {
	/* FPMCU_RST_L */
	PAD_NC(GPIO_11),
	/* EN_PWR_FP */
	PAD_NC(GPIO_32),
};

const __weak struct soc_amd_gpio *variant_bootblock_gpio_table(size_t *size, int slp_typ)
{
	if (variant_has_fingerprint()) {
		if (slp_typ == ACPI_S3)
			return NULL;

		*size = ARRAY_SIZE(gpio_fingerprint_bootblock_table);
		return gpio_fingerprint_bootblock_table;
	}

	*size = ARRAY_SIZE(gpio_no_fingerprint_bootblock_table);
	return gpio_no_fingerprint_bootblock_table;
}

static const struct soc_amd_gpio gpio_sleep_table[] = {
	/* S0iX SLP */
	PAD_GPO(GPIO_10, LOW),
	/* NVME_AUX_RESET_L */
	PAD_GPO(GPIO_40, LOW),
	/* EN_PWR_CAMERA */
	PAD_GPO(GPIO_76, LOW),
};

static const struct soc_amd_gpio gpio_fp_shutdown_table[] = {
	/* NVME_AUX_RESET_L */
	PAD_GPO(GPIO_40, LOW),
	/* EN_PWR_CAMERA */
	PAD_GPO(GPIO_76, LOW),

	/* FPMCU_RST_L */
	PAD_GPO(GPIO_11, LOW),
	/* EN_PWR_FP */
	PAD_GPO(GPIO_32, LOW),
};

const __weak struct soc_amd_gpio *variant_sleep_gpio_table(size_t *size, int slp_typ)
{
	if (slp_typ == SLP_TYP_S5) {
		*size = ARRAY_SIZE(gpio_fp_shutdown_table);
		return gpio_fp_shutdown_table;
	}

	*size = ARRAY_SIZE(gpio_sleep_table);
	return gpio_sleep_table;
}

static const struct soc_amd_gpio espi_gpio_table[] = {
	/* PCIE_RST0_L - Fixed timings */
	PAD_NF(GPIO_26, PCIE_RST_L, PULL_NONE),
	/* FCH_ESPI_EC_CS_L */
	PAD_NF(GPIO_30, ESPI_CS_L, PULL_NONE),
	/* ESPI_ALERT_L */
	PAD_NF(GPIO_108, ESPI_ALERT_L, PULL_NONE),
};

const struct soc_amd_gpio *variant_espi_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(espi_gpio_table);
	return espi_gpio_table;
}

static const struct soc_amd_gpio tpm_gpio_table[] = {
	/* H1_FCH_INT_ODL */
	PAD_INT(GPIO_3, PULL_NONE, EDGE_LOW, STATUS),
	/* I2C3_SCL - H1 */
	PAD_NF(GPIO_19, I2C3_SCL, PULL_NONE),
	/* I2C3_SDA - H1 */
	PAD_NF(GPIO_20, I2C3_SDA, PULL_NONE),
	/* EC_IN_RW_OD */
	PAD_GPI(GPIO_130, PULL_NONE),
};

const struct soc_amd_gpio *variant_tpm_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(tpm_gpio_table);
	return tpm_gpio_table;
}

static const struct soc_amd_gpio early_gpio_table[] = {
	/* UART0_RXD - DEBUG */
	PAD_NF(GPIO_136, UART0_RXD, PULL_NONE),
	/* UART0_TXD - DEBUG */
	PAD_NF(GPIO_138, UART0_TXD, PULL_NONE),
};

const struct soc_amd_gpio *variant_early_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}
