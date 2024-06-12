/* SPDX-License-Identifier: GPL-2.0-only */

#include <drivers/gfx/nvidia/gpu.h>
#include <soc/meminit.h>
#include <soc/romstage.h>
#include <variant/gpio.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	const struct mb_cfg board_cfg = {
		.type = MEM_TYPE_DDR4,
	};
	const struct mem_spd spd_info = {
		.topo = MEM_TOPO_DIMM_MODULE,
		.smbus = {
			[0] = { .addr_dimm[0] = 0x50, },
			[1] = { .addr_dimm[0] = 0x52, },
		},
	};
	const bool half_populated = false;

	const struct nvidia_gpu_config config = {
		.power_gpio = DGPU_PWR_EN,
		.reset_gpio = DGPU_RST_N,
		.enable = true,
	};

	// Enable dGPU power
	nvidia_set_power(&config);

	// Set primary display to internal graphics
	// NOTE: Use iGFX as some units don't have a dGPU for hybrid graphics
	mupd->FspmConfig.PrimaryDisplay = 0;

	memcfg_init(mupd, &board_cfg, &spd_info, half_populated);
}
