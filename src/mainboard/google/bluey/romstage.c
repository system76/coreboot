/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include "board.h"
#include <bootmode.h>
#include <cbmem.h>
#include <commonlib/bsd/cbmem_id.h>
#include <commonlib/coreboot_tables.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>
#include <security/vboot/vboot_common.h>
#include <soc/aop_common.h>
#include <soc/pcie.h>
#include <soc/pmic.h>
#include <soc/qcom_spmi.h>
#include <soc/qclib_common.h>
#include <soc/shrm.h>
#include <soc/watchdog.h>

static enum boot_mode_t boot_mode = LB_BOOT_MODE_NORMAL;
static bool battery_present = true;
static bool battery_below_threshold = false;

/*
 * is_off_mode - Check if the system is booting due to an off-mode power event.
 *
 * This function provides the board-level policy wrapper for detecting if the
 * system power-on was triggered by an external charging event (e.g., cable
 * insertion). This is typically used to enter LB_BOOT_MODE_OFFMODE_CHARGING.
 *
 * @return true if the system was triggered by a specific off-mode reason
 * (e.g., charging cable insertion).
 * @return false otherwise.
 */
bool is_off_mode(void)
{
	return is_pon_on_ac();
}

static enum boot_mode_t set_boot_mode(void)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return boot_mode;

	enum boot_mode_t boot_mode_new;

	if (google_chromeec_is_rtc_event()) {
		boot_mode_new = LB_BOOT_MODE_RTC_WAKE;
	} else if (is_off_mode() && battery_present) {
		boot_mode_new = LB_BOOT_MODE_OFFMODE_CHARGING;
	} else if (battery_below_threshold) {
		if (google_chromeec_is_charger_present())
			boot_mode_new = LB_BOOT_MODE_LOW_BATTERY_CHARGING;
		else
			boot_mode_new = LB_BOOT_MODE_LOW_BATTERY;
	} else {
		boot_mode_new = LB_BOOT_MODE_NORMAL;
	}

	boot_mode = boot_mode_new;
	return boot_mode_new;
}

static bool is_pd_sync_required(void)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return false;

	const uint64_t manual_pwron_event_mask =
		(EC_HOST_EVENT_MASK(EC_HOST_EVENT_POWER_BUTTON) |
		EC_HOST_EVENT_MASK(EC_HOST_EVENT_LID_OPEN));
	uint64_t ec_events = google_chromeec_get_events_b();

	if (!(ec_events & EC_HOST_EVENT_MASK(EC_HOST_EVENT_AC_CONNECTED)))
		return false;

	if (!(ec_events & manual_pwron_event_mask) || battery_below_threshold || !battery_present)
		return true;

	return false;
}

/* Check if it is okay to enable PD sync. */
static bool vboot_can_enable_pd_sync(void)
{
	if (!CONFIG(VBOOT))
		return false;

	/* Always enable if in developer or recovery mode */
	if (vboot_developer_mode_enabled() || vboot_recovery_mode_enabled() ||
			 vboot_check_recovery_request())
		return true;

	/* Otherwise disable */
	return false;
}

int qclib_mainboard_override(struct qclib_cb_if_table *table)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return 0;

	if (is_pd_sync_required() || vboot_can_enable_pd_sync())
		table->global_attributes |= QCLIB_GA_ENABLE_PD_NEGOTIATION;
	else
		table->global_attributes &= ~QCLIB_GA_ENABLE_PD_NEGOTIATION;
	return 0;
}

static void early_setup_usb_typec(void)
{
	gpio_output(GPIO_USB_C1_RETIMER_RESET_L, 0);
	gpio_output(GPIO_USB_C1_EN_PP3300, 0);
	gpio_output(GPIO_USB_C1_EN_PP1800, 0);
	gpio_output(GPIO_USB_C1_EN_PP0900, 0);
}

static void early_setup_usb(void)
{
	early_setup_usb_typec();
}

static void platform_init_lightbar(void)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC_LED_CONTROL))
		return;

	/*
	 * Early initialization of the Chrome EC lightbar.
	 * Ensures visual continuity if the AP firmware disabled the lightbar
	 * in a previous boot without a subsequent EC reset.
	 */
	google_chromeec_lightbar_on();
}

static void edp_configure_gpios(void)
{
	/* Ensure enabling power for Touchscreen if available */
	if (CONFIG_MAINBOARD_GPIO_PIN_FOR_TOUCHSCREEN_POWER)
		gpio_output(GPIO_TS_POWER_EN, 1);

	/* Panel power on GPIO enable */
	gpio_output(GPIO_PANEL_POWER_ON, 1);

	/* Panel HPD GPIO enable */
	gpio_input(GPIO_PANEL_HPD);
}

/**
 * Update and cache battery status from the EC.
 * This should be called once, early in the boot process,
 * after the EC is reachable.
 */
static void update_battery_status(void)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return;

	battery_present = google_chromeec_is_battery_present();
	battery_below_threshold = google_chromeec_is_below_critical_threshold();
}

/* Perform romstage early hardware initialization */
static void mainboard_setup_peripherals_early(void)
{
	platform_init_lightbar();

	update_battery_status();

	/*
	 * Power on NVMe early so that the DDR init and other operations
	 * that follow provide an organic >50ms delay before PCIe PERST
	 * de-assertion in platform_romstage_postram(), satisfying the
	 * NVMe spec requirement without a static mdelay().
	 */
	gcom_pcie_power_on_ep();

	edp_configure_gpios();

	/* This GPIO has external pullup hence disable default PD */
	gpio_input(GPIO_LID_OPEN_S3);

	/* Setup early USB related config */
	early_setup_usb();

	/* Watchdog must be checked first to avoid erasing watchdog info later. */
	check_wdog();
}

/*
 * Perform romstage late hardware initialization based on boot mode.
 * Handles PCIe host setup and fingerprint sensor power rails.
 */
static void mainboard_setup_peripherals_late(int mode)
{
	/* Perform PCIe setup early in async mode if supported to save 100ms */
	if (mode == LB_BOOT_MODE_NORMAL)
		qcom_setup_pcie_host(NULL);
	else
		gcom_pcie_power_off_ep();

	/*
	 * Enable fingerprint power rail early for stability prior to
	 * its reset being deasserted in ramstage.
	 * Requires >=200ms delay after its pin was driven low in bootblock.
	 */
	if (CONFIG(MAINBOARD_HAS_FINGERPRINT_VIA_SPI)) {
		if (mode == LB_BOOT_MODE_NORMAL)
			gpio_output(GPIO_EN_FP_RAILS, 1);
	}
}

void platform_romstage_main(void)
{
	mainboard_setup_peripherals_early();

	if (!qclib_check_dload_mode())
		shrm_fw_load_reset();

	/* QCLib: DDR init & train */
	qclib_load_and_run();

	init_sdam_config();

	/* Underlying PMIC registers are accessible only at this point */
	set_boot_mode();

	if (!qclib_check_dload_mode())
		aop_fw_load_reset();

	mainboard_setup_peripherals_late(boot_mode);

	qclib_rerun();
}

void platform_romstage_postram(void)
{
	enum boot_mode_t *boot_mode_ptr = cbmem_add(CBMEM_ID_BOOT_MODE, sizeof(*boot_mode_ptr));
	if (boot_mode_ptr) {
		*boot_mode_ptr = boot_mode;
		printk(BIOS_INFO, "Boot mode is %d\n", *boot_mode_ptr);
	}
}
