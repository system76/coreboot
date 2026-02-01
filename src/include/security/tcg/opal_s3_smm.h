/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SECURITY_TCG_OPAL_S3_SMM_H
#define SECURITY_TCG_OPAL_S3_SMM_H

#include <types.h>

/*
 * OPAL S3 SMM helpers.
 *
 * These helpers are called from SMI handlers. When the feature is disabled,
 * these APIs compile to no-ops so callers do not need preprocessor guards.
 */
#if CONFIG(TCG_OPAL_S3_UNLOCK)
int opal_s3_smi_apmc(u8 apmc);
void opal_s3_smi_sleep(u8 slp_typ);
void opal_s3_smi_sleep_finalize(u8 slp_typ);
#else
static inline int opal_s3_smi_apmc(u8 apmc)
{
	(void)apmc;
	return 0;
}

static inline void opal_s3_smi_sleep(u8 slp_typ)
{
	(void)slp_typ;
}

static inline void opal_s3_smi_sleep_finalize(u8 slp_typ)
{
	(void)slp_typ;
}
#endif

#endif
