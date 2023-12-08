/* SPDX-License-Identifier: GPL-2.0-only */

#include <delay.h>
#include <device/usbc_mux.h>
#include <intelblocks/tcss.h>

static int system76_ec_get_mux_info(int port, struct usbc_mux_info *info)
{
	if (!info)
		return -1;

	// TODO: Get actual TBT port info
	info->dp = true;
	info->usb = true;
	info->cable = false;
	info->polarity = false;
	info->hpd_lvl = false;
	info->hpd_irq = true;
	info->ufp = false;
	info->dbg_acc = false;
	info->dp_pin_mode = MODE_DP_PIN_E;

	return 0;
}

static int system76_ec_wait_for_connection(long timeout_ms)
{
	mdelay(50);
	return 1;
}

static int system76_ec_enter_dp_mode(int port)
{
	return 0;
}

static int system76_ec_wait_for_dp_mode_entry(int port, long timeout_ms)
{
	return 0;
}

static int system76_ec_wait_for_hpd(int port, long timeout_ms)
{
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
