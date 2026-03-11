/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VENDORCODE_TCG_OPAL_UNLOCK_H
#define VENDORCODE_TCG_OPAL_UNLOCK_H

#include <device/pci_type.h>
#include <stddef.h>
#include <types.h>

u32 opal_nvme_opal_unlock(pci_devfn_t dev, u16 base_comid, const u8 *password,
			  u8 password_len, void *scratch, size_t scratch_size);

#endif
