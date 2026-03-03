/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_QUALCOMM_X1P42100_ADSP_H__
#define __SOC_QUALCOMM_X1P42100_ADSP_H__

#include <commonlib/helpers.h>
#include <soc/addressmap.h>
#include <soc/platform_info.h>
#include <types.h>

/* ADSP CBFS firmware paths */
#define ADSP_CBFS_DTBS				CONFIG_CBFS_PREFIX "/adsp_dtbs"
#define ADSP_CBFS_FIRMWARE			CONFIG_CBFS_PREFIX "/adsp_lite"

/* LPASS QDSP6SS Register Structure */
struct x1p42100_lpass_qdsp6ss {
	u8 _res0[0x10];
	u32 rst_evb;
	u8 _res1[0x60 - 0x14];
	u32 boot_params[6];
};

check_member(x1p42100_lpass_qdsp6ss, rst_evb, 0x10);
check_member(x1p42100_lpass_qdsp6ss, boot_params, 0x60);

/* LPASS EFUSE Q6SS Register Structure */
struct x1p42100_lpass_efuse_q6ss {
	u32 evb_sel;
	u32 evb_addr;
};

check_member(x1p42100_lpass_efuse_q6ss, evb_sel, 0x0);
check_member(x1p42100_lpass_efuse_q6ss, evb_addr, 0x4);

static struct x1p42100_lpass_qdsp6ss *const lpass_qdsp6ss = (void *)LPASS_QDSP6SS_PUB_BASE;
static struct x1p42100_lpass_efuse_q6ss *const lpass_efuse_q6ss = (void *)LPASS_EFUSE_Q6SS_BASE;

void adsp_fw_load(void);

#endif /* __SOC_QUALCOMM_X1P42100_ADSP_H__ */
