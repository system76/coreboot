/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DRIVERS_CK505_CHIP_H
#define DRIVERS_CK505_CHIP_H

void mb_pre_ck505_init(void);
void mb_post_ck505_init(void);

struct drivers_i2c_ck505_config {
	const int nregs;
	const u8 regs[32];
	const u8 mask[32];
};

#endif
