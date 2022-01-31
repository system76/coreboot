/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Check if this is still correct */

#include <amdblocks/mca.h>
#include <cpu/x86/msr.h>
#include <types.h>

static const char *const mca_bank_name[] = {
	[0]  = "Load-store unit",
	[1]  = "Instruction fetch unit",
	[2]  = "L2 cache unit",
	[3]  = "Decode unit",
	[4]  = "",
	[5]  = "Execution unit",
	[6]  = "Floating point unit",
	[7]  = "L3 cache unit",
	[8]  = "L3 cache unit",
	[9]  = "L3 cache unit",
	[10] = "L3 cache unit",
	[11] = "L3 cache unit",
	[12] = "L3 cache unit",
	[13] = "L3 cache unit",
	[14] = "L3 cache unit",
	[15] = "",
	[16] = "",
	[17] = "UMC",
	[18] = "UMC",
	[19] = "CS",
	[20] = "CS",
	[21] = "",
	[22] = "",
	[23] = "",
	[24] = "",
	[25] = "",
	[26] = "",
	[27] = "PIE",
};

bool mca_has_expected_bank_count(void)
{
	return ARRAY_SIZE(mca_bank_name) == mca_get_bank_count();
}

bool mca_is_valid_bank(unsigned int bank)
{
	return (bank < ARRAY_SIZE(mca_bank_name) && mca_bank_name[bank] != NULL);
}

const char *mca_get_bank_name(unsigned int bank)
{
	if (mca_is_valid_bank(bank))
		return mca_bank_name[bank];
	else
		return "";
}
