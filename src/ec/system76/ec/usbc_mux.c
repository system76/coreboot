/* SPDX-License-Identifier: GPL-2.0-only */

#include "system76_ec.h"
#include <console/console.h>
#include <delay.h>
#include <device/usbc_mux.h>
#include <timer.h>
#include <types.h>

#define CMD_USBC_MUX_INFO 23

enum usbc_mux_flags {
	USBC_MUX_DP = BIT(0),
	USBC_MUX_USB = BIT(1),
	USBC_MUX_CABLE = BIT(2),
	USBC_MUX_POLARITY = BIT(3),
	USBC_MUX_HPD_LVL = BIT(4),
	USBC_MUX_HPD_IRQ = BIT(5),
	USBC_MUX_UFP = BIT(6),
	USBC_MUX_DBG_ACC = BIT(7),
};

static int system76_ec_get_mux_info(int port, struct usbc_mux_info *info)
{
	uint8_t request[1] = { port };
	uint8_t reply[3] = { 0 };
	uint8_t flags;
	uint8_t pin_mode;
	bool res;

	if (!info)
		return -1;

	res = system76_ec_cmd(CMD_USBC_MUX_INFO, request, ARRAY_SIZE(request),
			reply, ARRAY_SIZE(reply));
	if (!res)
		return -1;

	flags = reply[1];
	pin_mode = reply[2];

	info->dp = !!(flags & USBC_MUX_DP);
	info->usb = !!(flags & USBC_MUX_USB);
	info->cable = !!(flags & USBC_MUX_CABLE);
	info->polarity = !!(flags & USBC_MUX_POLARITY);
	info->hpd_lvl = !!(flags & USBC_MUX_HPD_LVL);
	info->hpd_irq = !!(flags & USBC_MUX_HPD_IRQ);
	info->ufp = !!(flags & USBC_MUX_UFP);
	info->dbg_acc = !!(flags & USBC_MUX_DBG_ACC);
	info->dp_pin_mode = pin_mode;

	printk(BIOS_SPEW, "%s: dp=%u, usb=%u\n", __func__, info->dp, info->usb);
	printk(BIOS_SPEW, "%s: cable=%u, polarity=%u\n", __func__, info->cable, info->polarity);
	printk(BIOS_SPEW, "%s: hpd_lvl=%u, hpd_irq=%u\n", __func__, info->hpd_lvl, info->hpd_irq);
	printk(BIOS_SPEW, "%s: ufp=%u, dbg_acc=%u\n", __func__, info->ufp, info->dbg_acc);
	printk(BIOS_SPEW, "%s: pin_mode=0x%x\n", __func__, info->dp_pin_mode);

	return 0;
}

static int system76_ec_wait_for_connection(long timeout_ms)
{
	// TODO
	return 1;
}

static int system76_ec_enter_dp_mode(int port)
{
	// TODO
	return 0;
}

static int system76_ec_wait_for_dp_mode_entry(int port, long timeout_ms)
{
	struct usbc_mux_info info;

	if (system76_ec_get_mux_info(port, &info) < 0) {
		printk(BIOS_WARNING, "%s: could not get usbc mux info\n", __func__);
		return -1;
	}

	if (!info.dp) {
		printk(BIOS_WARNING, "DP mode not ready\n");
		return -1;
	}

	return 0;
}

static int system76_ec_wait_for_hpd(int port, long timeout_ms)
{
	struct usbc_mux_info info;
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, timeout_ms);
	while (1) {
		if (system76_ec_get_mux_info(port, &info) < 0) {
			printk(BIOS_WARNING, "%s: could not get usbc mux info\n", __func__);
			return -1;
		}

		if (info.hpd_lvl)
			break;

		if (stopwatch_expired(&sw)) {
			printk(BIOS_WARNING, "HPD not ready after %ldms\n", timeout_ms);
			return -1;
		}

		mdelay(100);
	}

	printk(BIOS_INFO, "HPD ready after %lldms\n", stopwatch_duration_msecs(&sw));
	return 0;
}

static const struct usbc_ops system76_ec_usbc_ops = {
	.mux_ops = {
		.get_mux_info = system76_ec_get_mux_info,
	},
	.dp_ops = {
		.wait_for_connection = system76_ec_wait_for_connection,
		.enter_dp_mode = system76_ec_enter_dp_mode,
		.wait_for_dp_mode_entry = system76_ec_wait_for_dp_mode_entry,
		.wait_for_hpd = system76_ec_wait_for_hpd,
	},
};

const struct usbc_ops *usbc_get_ops(void)
{
	return &system76_ec_usbc_ops;
}
