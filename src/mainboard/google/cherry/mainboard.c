/* SPDX-License-Identifier: GPL-2.0-only */

#include <bl31.h>
#include <bootmode.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/mmio.h>
#include <edid.h>
#include <framebuffer_info.h>
#include <gpio.h>
#include <soc/ddp.h>
#include <soc/dpm.h>
#include <soc/dptx.h>
#include <soc/gpio.h>
#include <soc/i2c.h>
#include <soc/msdc.h>
#include <soc/mtcmos.h>
#include <soc/spm.h>
#include <soc/usb.h>

#include "gpio.h"

#include <arm-trusted-firmware/include/export/plat/mediatek/common/plat_params_exp.h>

/* GPIO to schematics names */
#define GPIO_AP_EDP_BKLTEN GPIO(DGI_D5)
#define GPIO_BL_PWM_1V8 GPIO(DISP_PWM0)
#define GPIO_EDP_HPD_1V8 GPIO(GPIO_07)
#define GPIO_EN_PP3300_DISP_X GPIO(I2SO1_D2)

static void register_reset_to_bl31(void)
{
	static struct bl_aux_param_gpio param_reset = {
		.h = { .type = BL_AUX_PARAM_MTK_RESET_GPIO },
		.gpio = { .polarity = ARM_TF_GPIO_LEVEL_HIGH },
	};

	param_reset.gpio.index = GPIO_RESET.id;
	register_bl31_aux_param(&param_reset.h);
}

/* Set up backlight control pins as output pin and power-off by default */
static void configure_panel_backlight(void)
{
	gpio_output(GPIO_AP_EDP_BKLTEN, 0);
	gpio_output(GPIO_BL_PWM_1V8, 0);
}

static void power_on_panel(void)
{
	/* Default power sequence for most panels. */
	gpio_set_pull(GPIO_EDP_HPD_1V8, GPIO_PULL_ENABLE, GPIO_PULL_UP);
	gpio_set_mode(GPIO_EDP_HPD_1V8, 2);
	gpio_output(GPIO_EN_PP3300_DISP_X, 1);
}

static bool configure_display(void)
{
	struct edid edid;
	struct fb_info *info;
	const char *name;

	printk(BIOS_INFO, "%s: Starting display initialization\n", __func__);

	mtcmos_display_power_on();
	mtcmos_protect_display_bus();
	configure_panel_backlight();
	power_on_panel();

	mtk_ddp_init();
	mdelay(200);

	if (mtk_edp_init(&edid) < 0) {
		printk(BIOS_ERR, "%s: Failed to initialize eDP\n", __func__);
		return false;
	}
	name = edid.ascii_string;
	if (name[0] == '\0')
		name = "unknown name";
	printk(BIOS_INFO, "%s: '%s %s' %dx%d@%dHz\n", __func__,
	       edid.manufacturer_name, name, edid.mode.ha, edid.mode.va,
	       edid.mode.refresh);

	edid_set_framebuffer_bits_per_pixel(&edid, 32, 0);

	mtk_ddp_mode_set(&edid);
	info = fb_new_framebuffer_info_from_edid(&edid, (uintptr_t)0);
	if (info)
		fb_set_orientation(info, LB_FB_ORIENTATION_NORMAL);

	return true;
}

static void mainboard_init(struct device *dev)
{
	if (display_init_required())
		configure_display();
	else
		printk(BIOS_INFO, "%s: Skipped display initialization\n", __func__);

	mtk_msdc_configure_emmc(true);
	mtk_msdc_configure_sdcard();
	setup_usb_host();

	/* for audio usage */
	if (CONFIG(CHERRY_USE_RT1011))
		mtk_i2c_bus_init(I2C2, I2C_SPEED_FAST);

	if (dpm_init())
		printk(BIOS_ERR, "dpm init failed, DVFS may not work\n");

	if (spm_init())
		printk(BIOS_ERR, "spm init failed, system suspend may not work\n");

	register_reset_to_bl31();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name = CONFIG_MAINBOARD_PART_NUMBER,
	.enable_dev = mainboard_enable,
};
