/* SPDX-License-Identifier: GPL-2.0-only */

#include <bl31.h>
#include <bootmode.h>
#include <console/console.h>
#include <device/device.h>
#include <gpio.h>
#include <soc/msdc.h>
#include <soc/spm.h>
#include <soc/usb.h>

#include "display.h"
#include "gpio.h"

#include <arm-trusted-firmware/include/export/plat/mediatek/common/plat_params_exp.h>

static void register_reset_to_bl31(void)
{
	static struct bl_aux_param_gpio param_reset = {
		.h = { .type = BL_AUX_PARAM_MTK_RESET_GPIO },
		.gpio = { .polarity = ARM_TF_GPIO_LEVEL_HIGH },
	};

	param_reset.gpio.index = GPIO_RESET.id;
	register_bl31_aux_param(&param_reset.h);
}

static void configure_audio(void)
{
	mtcmos_audio_power_on();

	/* Set up I2S */
	gpio_set_mode(GPIO(I2S2_MCK), PAD_I2S2_MCK_FUNC_I2S2_MCK);
	gpio_set_mode(GPIO(I2S2_BCK), PAD_I2S2_BCK_FUNC_I2S2_BCK);
	gpio_set_mode(GPIO(I2S2_LRCK), PAD_I2S2_LRCK_FUNC_I2S2_LRCK);
	gpio_set_mode(GPIO(EINT4), PAD_EINT4_FUNC_I2S3_DO);
}

static void mainboard_init(struct device *dev)
{
	mtk_msdc_configure_emmc(true);

	if (CONFIG(SDCARD_INIT)) {
		printk(BIOS_INFO, "SD card init\n");

		/* External SD Card connected via USB */
		gpio_output(GPIO_EN_PP3300_SDBRDG_X, 1);
	}

	setup_usb_host();

	configure_audio();

	if (spm_init())
		printk(BIOS_ERR, "spm init failed, system suspend may not work\n");

	register_reset_to_bl31();

	if (display_init_required()) {
		if (configure_display() < 0)
			printk(BIOS_ERR, "%s: Failed to init display\n", __func__);
	} else {
		printk(BIOS_INFO, "%s: Skipped display init\n", __func__);
	}
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name = CONFIG_MAINBOARD_PART_NUMBER,
	.enable_dev = mainboard_enable,
};
