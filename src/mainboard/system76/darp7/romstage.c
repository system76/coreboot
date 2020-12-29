/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/util.h>
#include <soc/meminit.h>
#include <soc/romstage.h>

static const struct mb_ddr4_cfg board_cfg = {
	// dq_map unused on DDR4
	// dqs_map unused on DDR4

	// TGL-U does not support interleaved memory
	.dq_pins_interleaved = 0,

	//TODO: can we use early command training?
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
	//TODO: Allow memory clocks higher than 2933 MHz
	mupd->FspmConfig.SaOcSupport = 1;

	//TODO: what is this for?
	const bool half_populated = false;
	meminit_ddr4(&mupd->FspmConfig, &board_cfg, &spd, half_populated);
}
