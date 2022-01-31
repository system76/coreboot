/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Check if this is still correct */

#include <acpi/acpi.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/memmap.h>
#include <amdblocks/pmlib.h>
#include <arch/cpu.h>
#include <console/console.h>
#include <fsp/api.h>
#include <program_loading.h>
#include <timestamp.h>

asmlinkage void car_stage_entry(void)
{
	timestamp_add_now(TS_START_ROMSTAGE);

	post_code(0x40);

	console_init();

	post_code(0x41);

	/* Snapshot chipset state prior to any FSP call */
	fill_chipset_state();

	fsp_memory_init(acpi_is_wakeup_s3());

	/* Fixup settings FSP-M should not be changing */
	fch_disable_legacy_dma_io();

	memmap_stash_early_dram_usage();

	run_ramstage();
}
