/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <ec/intel/board_id.h>
#include <soc/romstage.h>
#include <soc/meminit.h>

static const struct mb_cfg gcs_mem_config = {
	.type = MEM_TYPE_LP5X,

	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = {  14,  8,  15,  9,  10,  12,  11,  13, },
			.dq1 = {  6,  7,  5,  4,  1,  3,  0,  2, },
		},
		.ddr1 = {
			.dq0 = {  0,  1,  3,  4,  2,  5,  6,  7, },
			.dq1 = {  12,  13,  14,  15,  10,  11,  8,  9, },
		},
		.ddr2 = {
			.dq0 = {  10,  9,  8,  11,  12,  15,  14,  13, },
			.dq1 = {  1,  3,  0,  2,  6,  7,  5,  4, },
		},
		.ddr3 = {
			.dq0 = {  5,  6,  7,  4,  2,  3,  1,  0,},
			.dq1 = {  15,  14,  10,  11,  12,  13,  9,  8, },
		},
		.ddr4 = {
			.dq0 = {  8,  10,  9,  11,  13,  12,  14,  15, },
			.dq1 = {  4,  5,  1,  3,  7,  0,  2,  6, },
		},
		.ddr5 = {
			.dq0 = {  1,  0,  3,  2,  6,  7,  4,  5, },
			.dq1 = {  11,  14,  10,  13,  9,  12,  8,  15, },
		},
		.ddr6 = {
			.dq0 = {  10,  8,  15,  9,  13,  12,  14,  11, },
			.dq1 = {  3,  4,  7,  0,  2,  5,  6,  1, },
		},
		.ddr7 = {
			.dq0 = {  0,  1,  2,  7,  3,  6,  5,  4,  },
			.dq1 = {  11,  15,  10,  8,  13,  9,  12,  14, },
		},
	},

	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr1 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr2 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr3 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr4 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr5 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr6 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr7 = { .dqs0 = 0, .dqs1 = 1 }
	},

	.ect = true, /* Early Command Training */

	.lp_ddr_dq_dqs_re_training = 1,

	.user_bd = BOARD_TYPE_ULT_ULX,

	.lp5x_config = {
		.ccc_config = 0xFF,
	},
};

static const struct mb_cfg lp5_mem_config = {
	.type = MEM_TYPE_LP5X,

	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = {  13,  14,  12,  15, 11,  10,  8,  9, },
			.dq1 = {  7,  5,  4,  6,  0,  3,  1,  2 },
		},
		.ddr1 = {
			.dq0 = {  1,  3,  0,  2,  7,  4,  6,  5, },
			.dq1 = {  12,  13,  14,  15,  11,  10,  9,  8 },
		},
		.ddr2 = {
			.dq0 = {  0,  2,  1,  3,  6,  4,  7,  5 },
			.dq1 = {  14,  13,  15,  12,  8,  11,  10,  9, },
		},
		.ddr3 = {
			.dq0 = {  6,  5,  7,  4,  2,  3,  1,  0, },
			.dq1 = {  10,  8,  11,  9,  12,  15,  13,  14 },
		},
		.ddr4 = {
			.dq0 = {  2,  1,  3,  0,  4,  7,  5,  6 },
			.dq1 = {  15,  14,  12,  13,  9,  11,  10,  8, },
		},
		.ddr5 = {
			.dq0 = {  6,  5,  7,  4,  3,  1,  0,  2, },
			.dq1 = {  10,  9,  11,  8,  13,  14,  12,  15 },
		},
		.ddr6 = {
			.dq0 = {  9,  10,  11,  8,  14,  12,  13, 15, },
			.dq1 = {  0,  1,  2,  3,  5,  7,  4,  6 },
		},
		.ddr7 = {
			.dq0 = {  0,  1,  2,  3,  7,  5,  6,  4, },
			.dq1 = {  14,  13,  15,  12,  10,  8,  11,  9 },
		},
	},

	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr1 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr2 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr3 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr4 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr5 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr6 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr7 = { .dqs0 = 0, .dqs1 = 1 }
	},

	.ect = true, /* Early Command Training */

	.lp_ddr_dq_dqs_re_training = 1,

	.user_bd = BOARD_TYPE_ULT_ULX,

	.lp5x_config = {
		.ccc_config = 0xFF,
	},
};

const struct mb_cfg *variant_memory_params(void)
{
	int board_id = get_rvp_board_id();

	switch (board_id) {
	case PTLP_LP5_T3_RVP:
	case PTLP_LP5_T4_RVP:
		return &lp5_mem_config;
	case GCS_32GB:
	case GCS_64GB:
		return &gcs_mem_config;
	default:
		die("Unknown board id = 0x%x\n", board_id);
		break;
	}
}

void variant_get_spd_info(struct mem_spd *spd_info)
{
	spd_info->topo = MEM_TOPO_MEMORY_DOWN;
	spd_info->cbfs_index = variant_memory_sku();
}
