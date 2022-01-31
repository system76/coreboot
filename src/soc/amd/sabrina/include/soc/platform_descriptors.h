/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Check if this is still correct */

#ifndef AMD_SABRINA_PLATFORM_DESCRIPTORS_H
#define AMD_SABRINA_PLATFORM_DESCRIPTORS_H

#include <types.h>
#include <platform_descriptors.h>
#include <FspmUpd.h>

/* Mainboard callback to obtain DXI/PCIe and DDI descriptors. */
void mainboard_get_dxio_ddi_descriptors(
		const fsp_dxio_descriptor **dxio_descs, size_t *dxio_num,
		const fsp_ddi_descriptor **ddi_descs, size_t *ddi_num);

void mb_pre_fspm(void);

#endif /* AMD_SABRINA_PLATFORM_DESCRIPTORS_H */
