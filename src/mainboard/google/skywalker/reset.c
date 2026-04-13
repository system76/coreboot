/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <baseboard/gpio.h>
#include <gpio.h>
#include <reset.h>

void do_board_reset(void)
{
	gpio_output(GPIO_AP_EC_WARM_RST_REQ, 1);
}
