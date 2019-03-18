/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 System76
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <soc/cnl_memcfg_init.h>
#include <soc/romstage.h>

//TODO: find correct values
static const struct cnl_mb_cfg memcfg = {
	/*
	 * For each channel, there are 3 sets of DQ byte mappings,
	 * where each set has a package 0 and a package 1 value (package 0
	 * represents the first 64-bit lpddr4 chip combination, and package 1
	 * represents the second 64-bit lpddr4 chip combination).
	 * The first three sets are for CLK, CMD, and CTL.
	 * The fsp package actually expects 6 sets, but the last 3 sets are
	 * not used in CNL, so we only define the three sets that are used
	 * and let the meminit_lpddr4() routine take care of clearing the
	 * unused fields for the caller.
	 */
	.dq_map[DDR_CH0] = {
		{0x0F, 0xF0}, {0x00, 0xF0}, {0x0F, 0xF0},
	 	//{0x0F, 0x00}, {0xFF, 0x00}, {0xFF, 0x00}
	},
	.dq_map[DDR_CH1] = {
		{0x33, 0xCC}, {0x00, 0xCC}, {0x33, 0xCC},
	 	//{0x33, 0x00}, {0xFF, 0x00}, {0xFF, 0x00}
	},

	/*
	 * DQS CPU<>DRAM map Ch0 and Ch1.  Each array entry represents a
	 * mapping of a dq bit on the CPU to the bit it's connected to on
	 * the memory part.  The array index represents the dqs bit number
	 * on the memory part, and the values in the array represent which
	 * pin on the CPU that DRAM pin connects to.
	 */
	.dqs_map[DDR_CH0] = {0, 1, 3, 2, 4, 5, 6, 7},
	.dqs_map[DDR_CH1] = {1, 0, 4, 5, 2, 3, 6, 7},

	/*
	 * Rcomp resistor values.  These values represent the resistance in
	 * ohms of the three rcomp resistors attached to the DDR_COMP_0,
	 * DDR_COMP_1, and DDR_COMP_2 pins on the DRAM.
	 */
	.rcomp_resistor = { 121, 81, 100 },

	/*
	 * Rcomp target values.  These will typically be the following
	 * values for Cannon Lake : { 80, 40, 40, 40, 30 }
	 */
	.rcomp_targets = { 100, 40, 20, 20, 26 },

	/*
	 * Indicates whether memory is interleaved.
	 * Set to 1 for an interleaved design,
	 * set to 0 for non-interleaved design.
	 */
	.dq_pins_interleaved = 1,

	/*
	 * VREF_CA configuraation.
	 * Set to 0 VREF_CA goes to both CH_A and CH_B,
	 * set to 1 VREF_CA goes to CH_A and VREF_DQ_A goes to CH_B,
	 * set to 2 VREF_CA goes to CH_A and VREF_DQ_B goes to CH_B.
	 */
	.vref_ca_config = 2,

	/* Early Command Training Enabled */
	.ect = 1,
};

void mainboard_memory_init_params(FSPM_UPD *memupd) {
	const struct spd_info spd = {
		.spd_smbus_address[0] = 0xA0,
		.spd_smbus_address[2] = 0xA4,
	};

	cannonlake_memcfg_init(&memupd->FspmConfig, &memcfg, &spd);
}
