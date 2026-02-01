/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SECURITY_TCG_OPAL_S3_H
#define SECURITY_TCG_OPAL_S3_H

#include <stddef.h>
#include <types.h>

/*
 * ABI between payload software and the SMM OPAL S3 unlock handler.
 *
 * The payload is expected to place this structure in normal RAM and pass its
 * address in RBX when calling call_smm(APM_CNT_OPAL_SVC, OPAL_SMM_SUBCMD_*, ctx).
 */

#define OPAL_SMM_SUBCMD_SET_SECRET   0x01
#define OPAL_SMM_SUBCMD_CLEAR_SECRET 0x02

#define OPAL_S3_SMM_CTX_SIGNATURE 0x3353504f /* "OPS3" */
#define OPAL_S3_SMM_CTX_VERSION   0x0001
/*
 * Fixed size for a stable payload<->SMM ABI.
 *
 * Keep this as a constant (not a Kconfig option): changing it requires a
 * coordinated payload update.
 */
#define OPAL_S3_MAX_PASSWORD_LEN 32

struct opal_s3_smm_ctx {
	u32 signature;
	u16 version;
	u16 size;

	u8 bus;
	u8 dev;
	u8 func;
	u8 reserved0;

	u16 base_comid;
	u16 reserved1;

	u8 password_len;
	u8 reserved2[3];
	u8 password[OPAL_S3_MAX_PASSWORD_LEN];
} __packed;

#endif
