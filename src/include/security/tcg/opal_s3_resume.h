/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SECURITY_TCG_OPAL_S3_RESUME_H
#define SECURITY_TCG_OPAL_S3_RESUME_H

/*
 * Trigger an SMM-assisted OPAL unlock during S3 resume.
 *
 * The unlock implementation lives in SMM; this is a small ramstage helper
 * that issues the APMC trigger. It is called automatically from the x86
 * resume path when enabled.
 */
void opal_s3_resume_unlock(void);

#endif
