/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/psp.h>
#include <amdblocks/smn.h>
#include <types.h>
#include "psp_def.h"

uint64_t psp_get_base(void)
{
	return SMN_PSP_PUBLIC_BASE;
}

uint32_t psp_read32(uint64_t base, uint32_t offset)
{
	return smn_read32(base | offset);
}

void psp_write32(uint64_t base, uint32_t offset, uint32_t data)
{
	smn_write32(base | offset, data);
}

void psp_write64(uint64_t base, uint32_t offset, uint64_t data)
{
	smn_write32(base | (offset + sizeof(uint32_t)), data >> 32);
	smn_write32(base | offset, data & 0xffffffff);
}
