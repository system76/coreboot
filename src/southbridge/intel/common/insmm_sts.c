/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/intel/msr.h>
#include <cpu/x86/msr.h>
#include <device/mmio.h>
#include "insmm_sts.h"

void set_insmm_sts(const bool enable_writes)
{
	msr_t msr = {
		.lo = read32p(0xfed30880),
		.hi = 0,
	};
	if (enable_writes)
		msr.lo |= 1;
	else
		msr.lo &= ~1;

	wrmsr(MSR_SPCL_CHIPSET_USAGE, msr);
}
