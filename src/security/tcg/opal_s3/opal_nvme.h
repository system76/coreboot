/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SECURITY_TCG_OPAL_S3_OPAL_NVME_H
#define SECURITY_TCG_OPAL_S3_OPAL_NVME_H

#include <device/pci_type.h>
#include <types.h>

struct nvme_sq_entry;
struct nvme_cq_entry;

struct opal_nvme {
	pci_devfn_t dev;
	u8 *regs;
	u64 cap;
	u32 dstrd_bytes;

	struct nvme_sq_entry *sq;
	struct nvme_cq_entry *cq;
	u32 *sq_db;
	u32 *cq_db;

	u16 sq_tail;
	u16 cq_head;
	u8 cq_phase;
};

/*
 * Initialize a minimal NVMe admin queue in the provided scratch buffer.
 *
 * The scratch memory must not be in SMRAM. It must be preserved across S3.
 */
int opal_nvme_init(struct opal_nvme *nvme, pci_devfn_t dev, void *scratch, size_t scratch_size);
void opal_nvme_deinit(struct opal_nvme *nvme);

int opal_nvme_security_send(struct opal_nvme *nvme, u8 protocol, u16 sp_specific,
			    const void *buf, size_t buf_size);
int opal_nvme_security_recv(struct opal_nvme *nvme, u8 protocol, u16 sp_specific,
			    void *buf, size_t buf_size, size_t *xfer_out);

#endif
