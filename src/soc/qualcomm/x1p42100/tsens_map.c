/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * SoC Specific TSENS Map
 */

#include <soc/qcom_tsens.h>

/* Controller Configuration for SoC TSENS */
static const struct tsens_controller tsens_blocks[] = {
	/* Name,   TM base,    SROT base,  Sensor count */
	{"tsens0", 0x0c271000, 0x0c222000, 13},
	{"tsens1", 0x0c272000, 0x0c223000, 11},
	{"tsens2", 0x0c273000, 0x0c224000, 11},
	{"tsens3", 0x0c274000, 0x0c225000, 15}
};

/* Sensor Definition Mapping */
const struct thermal_zone_map qcom_thermal_zones[] = {
	/* Label,     Type,       Controller,    HW ID, Threshold (in milli degree C) */
	/* SoC TSENS Controller 0 */
	{"aoss-0",    TYPE_TSENS, &tsens_blocks[0], 0,  105000},
	{"cpu-0-0-0", TYPE_TSENS, &tsens_blocks[0], 1,  108000},
	{"cpu-0-0-1", TYPE_TSENS, &tsens_blocks[0], 2,  108000},
	{"cpu-0-1-0", TYPE_TSENS, &tsens_blocks[0], 3,  108000},
	{"cpu-0-1-1", TYPE_TSENS, &tsens_blocks[0], 4,  108000},
	{"cpu-0-2-0", TYPE_TSENS, &tsens_blocks[0], 5,  108000},
	{"cpu-0-2-1", TYPE_TSENS, &tsens_blocks[0], 6,  108000},
	{"cpu-0-3-0", TYPE_TSENS, &tsens_blocks[0], 7,  108000},
	{"cpu-0-3-1", TYPE_TSENS, &tsens_blocks[0], 8,  108000},
	{"cpuss-0-0", TYPE_TSENS, &tsens_blocks[0], 9,  105000},
	{"cpuss-0-1", TYPE_TSENS, &tsens_blocks[0], 10, 105000},
	{"ddr-0",     TYPE_TSENS, &tsens_blocks[0], 11, 105000},
	{"video",     TYPE_TSENS, &tsens_blocks[0], 12, 105000},

	/* SoC TSENS Controller 1 */
	{"aoss-1",    TYPE_TSENS, &tsens_blocks[1], 0,  105000},
	{"cpu-1-0-0", TYPE_TSENS, &tsens_blocks[1], 1,  108000},
	{"cpu-1-0-1", TYPE_TSENS, &tsens_blocks[1], 2,  108000},
	{"cpu-1-1-0", TYPE_TSENS, &tsens_blocks[1], 3,  108000},
	{"cpu-1-1-1", TYPE_TSENS, &tsens_blocks[1], 4,  108000},
	{"cpu-1-2-0", TYPE_TSENS, &tsens_blocks[1], 5,  108000},
	{"cpu-1-2-1", TYPE_TSENS, &tsens_blocks[1], 6,  108000},
	{"cpu-1-3-0", TYPE_TSENS, &tsens_blocks[1], 7,  108000},
	{"cpu-1-3-1", TYPE_TSENS, &tsens_blocks[1], 8,  108000},
	{"cpuss-1-0", TYPE_TSENS, &tsens_blocks[1], 9,  105000},
	{"cpuss-1-1", TYPE_TSENS, &tsens_blocks[1], 10, 105000},

	/* SoC TSENS Controller 2 */
	{"aoss-2",    TYPE_TSENS, &tsens_blocks[2], 0,  105000},
	{"cpu-2-0-0", TYPE_TSENS, &tsens_blocks[2], 1,  108000},
	{"cpu-2-0-1", TYPE_TSENS, &tsens_blocks[2], 2,  108000},
	{"cpu-2-1-0", TYPE_TSENS, &tsens_blocks[2], 3,  108000},
	{"cpu-2-1-1", TYPE_TSENS, &tsens_blocks[2], 4,  108000},
	{"cpu-2-2-0", TYPE_TSENS, &tsens_blocks[2], 5,  108000},
	{"cpu-2-2-1", TYPE_TSENS, &tsens_blocks[2], 6,  108000},
	{"cpu-2-3-0", TYPE_TSENS, &tsens_blocks[2], 7,  108000},
	{"cpu-2-3-1", TYPE_TSENS, &tsens_blocks[2], 8,  108000},
	{"cpuss-2-0", TYPE_TSENS, &tsens_blocks[2], 9,  105000},
	{"cpuss-2-1", TYPE_TSENS, &tsens_blocks[2], 10, 105000},

	/* SoC TSENS Controller 3 */
	{"aoss-3",    TYPE_TSENS, &tsens_blocks[3], 0,  105000},
	{"nsp-0",     TYPE_TSENS, &tsens_blocks[3], 1,  105000},
	{"nsp-1",     TYPE_TSENS, &tsens_blocks[3], 2,  105000},
	{"nsp-2",     TYPE_TSENS, &tsens_blocks[3], 3,  105000},
	{"nsp-3",     TYPE_TSENS, &tsens_blocks[3], 4,  105000},
	{"gpuss-0",   TYPE_TSENS, &tsens_blocks[3], 5,  95000},
	{"gpuss-1",   TYPE_TSENS, &tsens_blocks[3], 6,  95000},
	{"gpuss-2",   TYPE_TSENS, &tsens_blocks[3], 7,  95000},
	{"gpuss-3",   TYPE_TSENS, &tsens_blocks[3], 8,  95000},
	{"gpuss-4",   TYPE_TSENS, &tsens_blocks[3], 9,  95000},
	{"gpuss-5",   TYPE_TSENS, &tsens_blocks[3], 10, 95000},
	{"gpuss-6",   TYPE_TSENS, &tsens_blocks[3], 11, 95000},
	{"gpuss-7",   TYPE_TSENS, &tsens_blocks[3], 12, 95000},
	{"camera-0",  TYPE_TSENS, &tsens_blocks[3], 13, 105000},
	{"camera-1",  TYPE_TSENS, &tsens_blocks[3], 14, 105000},

	/* Sentinel */
	{NULL, 0, NULL, 0, 0}
};
