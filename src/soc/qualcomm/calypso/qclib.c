/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <soc/qclib_common.h>

__weak int qclib_mainboard_override(struct qclib_cb_if_table *table) { return 0; }

int qclib_soc_override(struct qclib_cb_if_table *table)
{
	/* hook for platform specific policy configuration */
	if (qclib_mainboard_override(table)) {
		printk(BIOS_ERR, "qclib_mainboard_override failed\n");
		return -1;
	}

	return 0;
}
