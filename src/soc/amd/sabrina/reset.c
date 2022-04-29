/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Check if this is still correct */

#include <arch/io.h>
#include <cf9_reset.h>
#include <reset.h>
#include <soc/southbridge.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/reset.h>

void do_cold_reset(void)
{
	/* De-assert and then assert all PwrGood signals on CF9 reset. */
	pm_write16(PWR_RESET_CFG, pm_read16(PWR_RESET_CFG) |
		TOGGLE_ALL_PWR_GOOD);
	outb(RST_CPU | SYS_RST, RST_CNT);
}

void do_warm_reset(void)
{
	/* Assert reset signals only. */
	outb(RST_CPU | SYS_RST, RST_CNT);
}

void do_board_reset(void)
{
	do_cold_reset();
}
