/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/util.h>
#include <soc/meminit.h>
#include <soc/romstage.h>

static const struct mb_ddr4_cfg board_cfg = {
	// dq_map unused on DDR4
	// dqs_map unused on DDR4

	.dq_pins_interleaved = 1,
	.ect = 0,
};

static const struct spd_info spd = {
	.topology = SODIMM,
	.smbus_info[0] = {
		.addr_dimm0 = 0x50,
	},
	.smbus_info[1] = {
		.addr_dimm0 = 0x52,
	},
};

void mainboard_memory_init_params(FSPM_UPD *mupd) {
	// Enable M.2 PCIE 4.0 and PEG1
	mupd->FspmConfig.CpuPcieRpEnableMask = 0b11;

	// Set primary display to internal graphics
	mupd->FspmConfig.PrimaryDisplay = 0;

	const bool half_populated = false;
	meminit_ddr4(&mupd->FspmConfig, &board_cfg, &spd, half_populated);
}
