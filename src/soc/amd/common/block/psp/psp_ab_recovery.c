/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/psp.h>
#include <fmap.h>
#include "psp_def.h"

int psp_ab_recovery_set_bootpartition(const uint32_t partition)
{
	struct mbox_cmd_boot_partition_buffer  boot_partition_cmd = {
		.header = {
			.size = sizeof(boot_partition_cmd)
		},
		.boot_partition = partition,
	};
	int cmd_status;

	cmd_status = send_psp_command(MBOX_BIOS_CMD_SET_BOOTPARTITION, &boot_partition_cmd);

	/* buffer's status shouldn't change but report it if it does */
	psp_print_cmd_status(cmd_status, &boot_partition_cmd.header);

	return cmd_status;
}

int psp_ab_recovery_get_bootpartition(void)
{
	struct mbox_cmd_boot_partition_buffer  boot_partition_cmd = {
		.header = {
			.size = sizeof(boot_partition_cmd)
		},
		.boot_partition = 0xFFFFFFFF,
	};
	int cmd_status;

	cmd_status = send_psp_command(MBOX_BIOS_CMD_GET_BOOTPARTITION, &boot_partition_cmd);

	/* buffer's status shouldn't change but report it if it does */
	psp_print_cmd_status(cmd_status, &boot_partition_cmd.header);

	if (cmd_status < 0)
		return cmd_status;

	return boot_partition_cmd.boot_partition;
}

int psp_ab_recovery_toggle_bootpartition(void)
{
	int cmd_status = psp_ab_recovery_get_bootpartition();
	if (cmd_status < 0)
		return cmd_status;

	return psp_ab_recovery_set_bootpartition(cmd_status ? 0 : 1);
}

/*
 * Select the FMAP region to continue booting from, depending on the state of
 * recovery mode as reported by PSP.
 */
const char *cbfs_fmap_region_hint(void)
{
	if (psp_ab_recovery_get_bootpartition() == 1)
		return "COREBOOT_B";
	else
		return "COREBOOT";
}
