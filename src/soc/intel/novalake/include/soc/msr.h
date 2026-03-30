/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_NOVALAKE_MSR_H_
#define _SOC_NOVALAKE_MSR_H_

#include <intelblocks/msr.h>

#define MSR_BIOS_DONE		        0x151
#define   ENABLE_IA_UNTRUSTED	        BIT(0)
#define MSR_VR_MISC_CONFIG2		0x636

#endif /* _SOC_NOVALAKE_MSR_H_ */
