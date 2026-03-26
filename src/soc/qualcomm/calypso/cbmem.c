/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <console/console.h>

uintptr_t cbmem_top_chipset(void)
{
	printk(BIOS_ERR, "%s: Update CBMEM TOP address.\n", __func__);
	return (uintptr_t)NULL;
}
