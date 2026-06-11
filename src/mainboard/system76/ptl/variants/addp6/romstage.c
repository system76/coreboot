/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/gpio.h>
#include <soc/meminit.h>
#include <soc/romstage.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	const struct mb_cfg board_cfg = {
		.type = MEM_TYPE_DDR5,
		.rcomp = { .resistor = 100, },
		.ect = true,
		.user_bd = BOARD_TYPE_ULT_ULX,
		.lp_ddr_dq_dqs_re_training = 1,
		.ddr_config = { .dq_pins_interleaved = false, },
	};
	const struct mem_spd spd_info = {
		.topo = MEM_TOPO_DIMM_MODULE,
		.smbus = {
			[0] = { .addr_dimm[0] = 0x50 },
			[1] = { .addr_dimm[0] = 0x52 },
		},
	};
	const bool half_populated = false;

	memcfg_init(mupd, &board_cfg, &spd_info, half_populated);
}
