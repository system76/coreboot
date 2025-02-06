/* SPDX-License-Identifier: GPL-2.0-only */

#include <option.h>
#include <soc/meminit.h>
#include <soc/romstage.h>
#include <types.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	const struct mb_cfg mem_config = {
		.type = MEM_TYPE_DDR5,
		.ect = false,
		.UserBd = BOARD_TYPE_MOBILE,
		.ddr_config = {
			.dq_pins_interleaved = false,
		},
		.rcomp = {
			.resistor = 100,
			.targets = {70, 30, 25, 25, 25},
		}
	};

	const bool half_populated = false;

	const struct mem_spd ddr4_spd_info = {
		.topo = MEM_TOPO_DIMM_MODULE,
		.smbus = {
			[0] = {
				.addr_dimm[0] = 0x50,
			},
			[1] = {
				.addr_dimm[0] = 0x52,
			},
		},
	};

	memcfg_init(mupd, &mem_config, &ddr4_spd_info, half_populated);

	const uint8_t vtd = get_uint_option("vtd", 1);
	mupd->FspmConfig.VtdDisable = !vtd;

	/* Enable/Disable Wireless (RP09) based on CMOS settings */
	if (get_uint_option("wireless", 1) == 0)
		mupd->FspmConfig.PcieRpEnableMask &= ~(1 << 8);

	mupd->FspmConfig.PchHdaSubSystemIds = 0x70381e50;
};
