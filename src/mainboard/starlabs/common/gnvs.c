/* SPDX-License-Identifier: GPL-2.0-only */

#include <stddef.h>

#include <starlabs/efi_option_smi.h>

size_t size_of_dnvs(void)
{
	return sizeof(struct starlabs_dnvs_efiopt);
}
