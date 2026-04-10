/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmio.h>
#include <bootmode.h>
#include <bootsplash.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <edid.h>
#include <elog.h>
#include <gpio.h>
#include <soc/clock.h>
#include <soc/display/edp_ctrl.h>
#include <soc/display/edp_reg.h>
#include <soc/display/mdssreg.h>
#include <soc/pmic_gpio.h>
#include <soc/symbols_common.h>
#include <soc/rpmh_config.h>
#include <timer.h>
#include <timestamp.h>

#include "board.h"
#include "display.h"

#define PMIC_D_GPIO_04 4

#define BATTERY_CHARGING_SPLASH_TIMEOUT_MS 5000
static struct stopwatch splash_sw;

/* Threshold for selecting lower-resolution assets */
#define FHD_WIDTH_THRESHOLD		1920

static struct {
	uint32_t x_res;
} cached_display_params;

/*
 * Mainboard-specific override for logo filenames.
 */
const char *mainboard_bmp_logo_filename(void)
{
	/* For panels at or below Full HD (1920px width), use the
	 * lower-resolution bitmap.
	 */
	if (cached_display_params.x_res <= FHD_WIDTH_THRESHOLD)
		return "cb_plus_logo.bmp";

	return "cb_logo.bmp";
}

static void edp_enable_backlight(void)
{
	/* Enable backlight */
	pmic_gpio_output(PMIC_D_SLAVE_ID, PMIC_D_GPIO_04, true);
}

static void qcom_mdss_edp_init(struct edid *edid, uintptr_t fb_addr)
{
	if (edp_ctrl_init(edid) != CB_SUCCESS)
		return;

	configure_vbif_qos();
	mdss_layer_mixer_setup(edid);
	mdss_source_pipe_config(edid, fb_addr);

	intf_tg_setup(edid);
	intf_fetch_start_config(edid);

	merge_3d_active(edid);
}

static void qcom_mdp_start(uintptr_t fb_addr)
{
	stopwatch_init_msecs_expire(&splash_sw, BATTERY_CHARGING_SPLASH_TIMEOUT_MS);

	write32(&mdp_intf->timing_eng_enable, 1);
}

void display_stop(void)
{
	if (!get_lb_framebuffer())
		return;

	while (!stopwatch_expired(&splash_sw))
		mdelay(100);

	write32(&mdp_intf->timing_eng_enable, 0);
	mdelay(20);
	write32(&edp_ahbclk->sw_reset, 1);
	mdelay(20);
	write32(&edp_ahbclk->sw_reset, 0);

	/* Disable backlight */
	pmic_gpio_output(PMIC_D_SLAVE_ID, PMIC_D_GPIO_04, false);

	/* Panel power off */
	gpio_output(GPIO_PANEL_POWER_ON, 0);
}

static void display_logo(enum lb_fb_orientation orientation, uintptr_t fb_addr,
			 const struct edid *edid)
{
	if (!CONFIG(BMP_LOGO) || !fb_addr)
		return;

	memset((void *)fb_addr, 0, edid->bytes_per_line * edid->y_resolution);

	struct logo_config config = {
		.panel_orientation = orientation,
		.halignment = FW_SPLASH_HALIGNMENT_CENTER,
		.valignment = FW_SPLASH_VALIGNMENT_CENTER,
		.logo_bottom_margin = 200,
	};
	render_logo_to_framebuffer(&config);

	qcom_mdp_start(fb_addr);

	edp_enable_backlight();

	timestamp_add_now(TS_FIRMWARE_SPLASH_RENDERED);

	printk(BIOS_DEBUG, "Firmware Splash Screen : Enabled\n");

	elog_add_event_byte(ELOG_TYPE_FW_SPLASH_SCREEN, 1);
}

void display_startup(void)
{
	if (!display_init_required() || (CONFIG(VBOOT_LID_SWITCH) && !get_lid_switch())) {
		printk(BIOS_INFO, "Skipping display init.\n");
		return;
	}

	struct edid edid = {};
	struct fb_info *fb;
	uintptr_t fb_addr = (REGION_SIZE(framebuffer)) ? (uintptr_t)_framebuffer : 0;
	enum lb_fb_orientation orientation = LB_FB_ORIENTATION_NORMAL;

	/* Initialize RPMh subsystem and display power rails */
	if (display_rpmh_init() != CB_SUCCESS)
		return;

	enable_mdss_clk();
	qcom_mdss_edp_init(&edid, fb_addr);
	if (edid.mode.ha == 0)
		return;

	edid_set_framebuffer_bits_per_pixel(&edid, 32, 0);
	fb = fb_new_framebuffer_info_from_edid(&edid, fb_addr);
	fb_set_orientation(fb, orientation);

	cached_display_params.x_res = edid.x_resolution;
	display_logo(orientation, fb_addr, &edid);
}
