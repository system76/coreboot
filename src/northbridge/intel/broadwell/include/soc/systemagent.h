/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BROADWELL_SYSTEMAGENT_H_
#define _BROADWELL_SYSTEMAGENT_H_

#define SA_IGD_OPROM_VENDEV	0x80860406

#define IGD_HASWELL_ULT_GT1	0x0a06
#define IGD_HASWELL_ULT_GT2	0x0a16
#define IGD_HASWELL_ULT_GT3	0x0a26
#define IGD_HASWELL_ULX_GT1	0x0a0e
#define IGD_HASWELL_ULX_GT2	0x0a1e
#define IGD_BROADWELL_U_GT1	0x1606
#define IGD_BROADWELL_U_GT2	0x1616
#define IGD_BROADWELL_U_GT3_15W	0x1626
#define IGD_BROADWELL_U_GT3_28W	0x162b
#define IGD_BROADWELL_Y_GT2	0x161e
#define IGD_BROADWELL_H_GT2	0x1612
#define IGD_BROADWELL_H_GT3	0x1622

#define MCH_BROADWELL_ID_U_Y	0x1604
#define MCH_BROADWELL_REV_D0	0x06
#define MCH_BROADWELL_REV_E0	0x08
#define MCH_BROADWELL_REV_F0	0x09

#include <northbridge/intel/haswell/registers/host_bridge.h>

#define ARCHDIS		0xff0	/* DMA Remap Engine Policy Control */
#define  DMAR_LCKDN	(1 << 31)
#define  PRSCAPDIS	(1 << 2)

#include <northbridge/intel/common/fixed_bars.h>
#include <northbridge/intel/haswell/registers/mchbar.h>

/* System Agent identification */
u8 systemagent_revision(void);

#endif
