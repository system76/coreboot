/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/util.h>
#include <soc/meminit.h>
#include <soc/romstage.h>

static const struct mb_cfg board_cfg = {
    .type = MEM_TYPE_DDR4,

	// dq_map unused on DDR4
	// dqs_map unused on DDR4

	//TODO: can we use early command training?
	.ect = false,

	// TGL-U does not support interleaved memory
	.ddr4_config = {
        .dq_pins_interleaved = false,
    }
};

static const struct mem_spd spd = {
	.topo = MEM_TOPO_MIXED,
	.cbfs_index = 0,
	.smbus[1] = {
		.addr_dimm[0] = 0x52,
	},
};

void mainboard_memory_init_params(FSPM_UPD *mupd) {
	// Allow memory clocks higher than 2933 MHz
	mupd->FspmConfig.SaOcSupport = 1;

	const bool half_populated = false;
	memcfg_init(&mupd->FspmConfig, &board_cfg, &spd, half_populated);
}
