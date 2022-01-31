/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_MSR_H_
#define _SOC_MSR_H_

#include <intelblocks/msr.h>

#define MSR_BIOS_DONE			0x151
#define  ENABLE_IA_UNTRUSTED		(1 << 0)
#define MSR_VR_CURRENT_CONFIG		0x601
#define MSR_PL3_CONTROL			0x615
#define MSR_VR_MISC_CONFIG2		0x636
#define MSR_PLATFORM_POWER_LIMIT	0x65c

#endif
