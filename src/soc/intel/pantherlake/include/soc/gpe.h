/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_PANTHERLAKE_GPE_H_
#define _SOC_PANTHERLAKE_GPE_H_

#include <intelpch/gpe.h>
#include <soc/pm.h>

/* additional GPE_STD bits in PTL GPE0 blocks */
/* 0x6C GPE0_STS_127_96; 0x7C GPE0_EN_127_96 */
#define GPE0_FIA_DFLEX      96  /* _L60 */

#define GPE0_THERM          100 /* _L64 */
#define GPE0_TC_PME_B0      101 /* _L65 */

#define GPE0_ISH            104 /* _L68 */

#define GPE0_ME_SCI         108 /* _L6C */

#define GPE0_USB_CON_DSX    113 /* _L71 */

#define GPE0_TCSS_SX_WAKE   115 /* _L73 */
#define GPE0_TC_PCI_EXP     118 /* _L76 */
#define GPE0_TC_HOT_PLUG    119 /* _L77 */
#define GPE0_OSSE_SCI       120 /* _L78 */

#undef GPE_MAX
#define GPE_MAX  GPE0_OSSE_SCI


/*
 * NOTE: GPE1 number starts right after GPE0 events and GPE0 number starts from
 * 0. that is:
 * 0 + (number of GPE0 blocks x block size (i.e. 4 bytes) x 8 (byte = 8 bits))
 * In most of the cases, this value is 0x80 (128 dec)
 */
#define GPE1_START_NUM (GPE0_REG_MAX * 4 * 8)
#define GPE1_NUM(blk, bit) (GPE1_START_NUM + (blk) * 32 + (bit))
#define GPE1_0_NUM(b) GPE1_NUM(GPE1_31_0, b)
#define GPE1_1_NUM(b) GPE1_NUM(GPE1_63_32, b)
#define GPE1_2_NUM(b) GPE1_NUM(GPE1_95_64, b)

/* 0x10 GPE1_STS_31_0; 0x1C GPE1_EN_31_0 register bit fields */
#define GPE1_31_0_CNVI_BT_PME_B0       18
#define GPE1_31_0_TC_IOM_PME_B0        17
#define GPE1_31_0_TC_TBT1_PME_B0       16
#define GPE1_31_0_TC_TBT0_PME_B0       15
#define GPE1_31_0_LPSS_PME_B0          14
#define GPE1_31_0_CSE_PME_B0           13
#define GPE1_31_0_XDCI_PME_B0          12
#define GPE1_31_0_ACE_PME_B0           10
#define GPE1_31_0_XHCI_PME_B0          9
#define GPE1_31_0_SATA_PME_B0          8
#define GPE1_31_0_CSME_PME_B0          7
#define GPE1_31_0_GBE_PME_B0           6
#define GPE1_31_0_CNVI_PME_B0          5
#define GPE1_31_0_OSSE_PME_B0          4
#define GPE1_31_0_TBTLSX_PME_B0        1

/* 0x14 GPE1_STS_63_32; 0x20 GPE1_EN_63_32 register bit fields */
#define GPE1_63_32_PG5_PMA0_HOT_PLUG_3 31
#define GPE1_63_32_PG5_PMA0_HOT_PLUG_2 30
#define GPE1_63_32_PG5_PMA0_HOT_PLUG_1 29
#define GPE1_63_32_PG5_PMA0_HOT_PLUG_0 28
#define GPE1_63_32_PG5_PMA1_HOT_PLUG_3 27
#define GPE1_63_32_PG5_PMA1_HOT_PLUG_2 26
#define GPE1_63_32_PG5_PMA1_HOT_PLUG_1 25
#define GPE1_63_32_PG5_PMA1_HOT_PLUG_0 24
#define GPE1_63_32_TC_TBT1_HOT_PLUG    13
#define GPE1_63_32_TC_TBT0_HOT_PLUG    12
#define GPE1_63_32_TC_PCIE3_HOT_PLUG   11
#define GPE1_63_32_TC_PCIE2_HOT_PLUG   10
#define GPE1_63_32_TC_PCIE1_HOT_PLUG   9
#define GPE1_63_32_TC_PCIE0_HOT_PLUG   8
#define GPE1_63_32_IOE_HOT_PLUG        7
#define GPE1_63_32_SPB_HOT_PLUG        1
#define GPE1_63_32_SPA_HOT_PLUG        0

/* 0x18 GPE1_STS_95_64; 0x24 GPE1_EN_95_64 register bit fields */
#define GPE1_95_64_PG5_PMA0_PCI_EXP_3  31
#define GPE1_95_64_PG5_PMA0_PCI_EXP_2  30
#define GPE1_95_64_PG5_PMA0_PCI_EXP_1  29
#define GPE1_95_64_PG5_PMA0_PCI_EXP_0  28
#define GPE1_95_64_PG5_PMA1_PCI_EXP_3  27
#define GPE1_95_64_PG5_PMA1_PCI_EXP_2  26
#define GPE1_95_64_PG5_PMA1_PCI_EXP_1  25
#define GPE1_95_64_PG5_PMA1_PCI_EXP_0  24
#define GPE1_95_64_TC_TBT1_PCI_EXP     13
#define GPE1_95_64_TC_TBT0_PCI_EXP     12
#define GPE1_95_64_TC_PCIE3_PCI_EXP    11
#define GPE1_95_64_TC_PCIE2_PCI_EXP    10
#define GPE1_95_64_TC_PCIE1_PCI_EXP    9
#define GPE1_95_64_TC_PCIE0_PCI_EXP    8
#define GPE1_95_64_IOE_PCI_EXP         7
#define GPE1_95_64_SPB_PCI_EXP         1
#define GPE1_95_64_SPA_PCI_EXP         0

/* GPE1 PME_B0 event numbers */
#define GPE1_CNVI_BT_PME_B0      GPE1_0_NUM(GPE1_31_0_CNVI_BT_PME_B0)        /* 146: _L92 */
#define GPE1_TC_IOM_PME_B0       GPE1_0_NUM(GPE1_31_0_TC_IOM_PME_B0)         /* 145: _L91 */
#define GPE1_TC_TBT1_PME_B0      GPE1_0_NUM(GPE1_31_0_TC_TBT1_PME_B0)        /* 144: _L90 */
#define GPE1_TC_TBT0_PME_B0      GPE1_0_NUM(GPE1_31_0_TC_TBT0_PME_B0)        /* 143: _L8F */
#define GPE1_LPSS_PME_B0         GPE1_0_NUM(GPE1_31_0_LPSS_PME_B0)           /* 142: _L8E */
#define GPE1_CSE_PME_B0          GPE1_0_NUM(GPE1_31_0_CSE_PME_B0)            /* 141: _L8D */
#define GPE1_XDCI_PME_B0         GPE1_0_NUM(GPE1_31_0_XDCI_PME_B0)           /* 140: _L8C */
#define GPE1_ACE_PME_B0          GPE1_0_NUM(GPE1_31_0_ACE_PME_B0)            /* 138: _L8A */
#define GPE1_XHCI_PME_B0         GPE1_0_NUM(GPE1_31_0_XHCI_PME_B0)           /* 137: _L89 */
#define GPE1_SATA_PME_B0         GPE1_0_NUM(GPE1_31_0_SATA_PME_B0)           /* 136: _L88 */
#define GPE1_CSME_PME_B0         GPE1_0_NUM(GPE1_31_0_CSME_PME_B0)           /* 135: _L87 */
#define GPE1_GBE_PME_B0          GPE1_0_NUM(GPE1_31_0_GBE_PME_B0)            /* 134: _L86 */
#define GPE1_CNVI_PME_B0         GPE1_0_NUM(GPE1_31_0_CNVI_PME_B0)           /* 133: _L85 */
#define GPE1_OSSE_PME_B0         GPE1_0_NUM(GPE1_31_0_OSSE_PME_B0)           /* 132: _L84 */
#define GPE1_TBTLSX_PME_B0       GPE1_0_NUM(GPE1_31_0_TBTLSX_PME_B0)         /* 129: _L81 */

/* GPE1 HOT_PUG event numbers */
#define GPE1_PG5_PMA0_HOT_PLUG_3 GPE1_1_NUM(GPE1_63_32_PG5_PMA0_HOT_PLUG_3) /* 191: _LBF */
#define GPE1_PG5_PMA0_HOT_PLUG_2 GPE1_1_NUM(GPE1_63_32_PG5_PMA0_HOT_PLUG_2) /* 190: _LBE */
#define GPE1_PG5_PMA0_HOT_PLUG_1 GPE1_1_NUM(GPE1_63_32_PG5_PMA0_HOT_PLUG_1) /* 189: _LBD */
#define GPE1_PG5_PMA0_HOT_PLUG_0 GPE1_1_NUM(GPE1_63_32_PG5_PMA0_HOT_PLUG_0) /* 188: _LBC */
#define GPE1_PG5_PMA1_HOT_PLUG_3 GPE1_1_NUM(GPE1_63_32_PG5_PMA1_HOT_PLUG_3) /* 187: _LBB */
#define GPE1_PG5_PMA1_HOT_PLUG_2 GPE1_1_NUM(GPE1_63_32_PG5_PMA1_HOT_PLUG_2) /* 186: _LBA */
#define GPE1_PG5_PMA1_HOT_PLUG_1 GPE1_1_NUM(GPE1_63_32_PG5_PMA1_HOT_PLUG_1) /* 185: _LB9 */
#define GPE1_PG5_PMA1_HOT_PLUG_0 GPE1_1_NUM(GPE1_63_32_PG5_PMA1_HOT_PLUG_0) /* 184: _LB8 */
#define GPE1_TC_TBT1_HOT_PLUG    GPE1_1_NUM(GPE1_63_32_TC_TBT1_HOT_PLUG)    /* 173: _LAD */
#define GPE1_TC_TBT0_HOT_PLUG    GPE1_1_NUM(GPE1_63_32_TC_TBT0_HOT_PLUG)    /* 172: _LAC */
#define GPE1_TC_PCIE3_HOT_PLUG   GPE1_1_NUM(GPE1_63_32_TC_PCIE3_HOT_PLUG)   /* 171: _LAB */
#define GPE1_TC_PCIE2_HOT_PLUG   GPE1_1_NUM(GPE1_63_32_TC_PCIE2_HOT_PLUG)   /* 170: _LAA */
#define GPE1_TC_PCIE1_HOT_PLUG   GPE1_1_NUM(GPE1_63_32_TC_PCIE1_HOT_PLUG)   /* 169: _LA9 */
#define GPE1_TC_PCIE0_HOT_PLUG   GPE1_1_NUM(GPE1_63_32_TC_PCIE0_HOT_PLUG)   /* 168: _LA8 */
#define GPE1_IOE_HOT_PLUG        GPE1_1_NUM(GPE1_63_32_IOE_HOT_PLUG)        /* 167: _LA7 */
#define GPE1_SPB_HOT_PLUG        GPE1_1_NUM(GPE1_63_32_SPB_HOT_PLUG)        /* 161: _LA1 */
#define GPE1_SPA_HOT_PLUG        GPE1_1_NUM(GPE1_63_32_SPA_HOT_PLUG)        /* 160: _LA0 */

/* GPE1 PCI_EXP event numbers */
#define GPE1_PG5_PMA0_PCI_EXP_3  GPE1_2_NUM(GPE1_95_64_PG5_PMA0_PCI_EXP_3)  /* 223: _LDF */
#define GPE1_PG5_PMA0_PCI_EXP_2  GPE1_2_NUM(GPE1_95_64_PG5_PMA0_PCI_EXP_2)  /* 222: _LDE */
#define GPE1_PG5_PMA0_PCI_EXP_1  GPE1_2_NUM(GPE1_95_64_PG5_PMA0_PCI_EXP_1)  /* 221: _LDD */
#define GPE1_PG5_PMA0_PCI_EXP_0  GPE1_2_NUM(GPE1_95_64_PG5_PMA0_PCI_EXP_0)  /* 220: _LDC */
#define GPE1_PG5_PMA1_PCI_EXP_3  GPE1_2_NUM(GPE1_95_64_PG5_PMA1_PCI_EXP_3)  /* 219: _LDB */
#define GPE1_PG5_PMA1_PCI_EXP_2  GPE1_2_NUM(GPE1_95_64_PG5_PMA1_PCI_EXP_2)  /* 218: _LDA */
#define GPE1_PG5_PMA1_PCI_EXP_1  GPE1_2_NUM(GPE1_95_64_PG5_PMA1_PCI_EXP_1)  /* 217: _LD9 */
#define GPE1_PG5_PMA1_PCI_EXP_0  GPE1_2_NUM(GPE1_95_64_PG5_PMA1_PCI_EXP_0)  /* 216: _LD8 */
#define GPE1_TC_TBT1_PCI_EXP     GPE1_2_NUM(GPE1_95_64_TC_TBT1_PCI_EXP)     /* 205: _LCD */
#define GPE1_TC_TBT0_PCI_EXP     GPE1_2_NUM(GPE1_95_64_TC_TBT0_PCI_EXP)     /* 204: _LCC */
#define GPE1_TC_PCIE3_PCI_EXP    GPE1_2_NUM(GPE1_95_64_TC_PCIE3_PCI_EXP)    /* 203: _LCB */
#define GPE1_TC_PCIE2_PCI_EXP    GPE1_2_NUM(GPE1_95_64_TC_PCIE2_PCI_EXP)    /* 202: _LCA */
#define GPE1_TC_PCIE1_PCI_EXP    GPE1_2_NUM(GPE1_95_64_TC_PCIE1_PCI_EXP)    /* 201: _LC9 */
#define GPE1_TC_PCIE0_PCI_EXP    GPE1_2_NUM(GPE1_95_64_TC_PCIE0_PCI_EXP)    /* 200: _LC8 */
#define GPE1_IOE_PCI_EXP         GPE1_2_NUM(GPE1_95_64_IOE_PCI_EXP)         /* 199: _LC7 */
#define GPE1_SPB_PCI_EXP         GPE1_2_NUM(GPE1_95_64_SPB_PCI_EXP)         /* 193: _LC1 */
#define GPE1_SPA_PCI_EXP         GPE1_2_NUM(GPE1_95_64_SPA_PCI_EXP)         /* 192: _LC0 */

/* All PME B0 events except TCSS in GPE1 block 0 */
#define GPE1_PME_B0_EVENT_EN_BITS ( \
	CNVI_BT_PME_B0_EN | \
	LPSS_PME_B0_EN    | \
	CSE_PME_B0_EN     | \
	XDCI_PME_B0_EN    | \
	ACE_PME_B0_EN     | \
	XHCI_PME_B0_EN    | \
	SATA_PME_B0_EN    | \
	CSME_PME_B0_EN    | \
	GBE_PME_B0_EN     | \
	CNVI_PME_B0_EN    | \
	OSSE_PME_B0_EN)

/* All TCSS PME B0 events in GPE1 block 0 */
#define GPE1_TC_PME_B0_EVENT_EN_BITS ( \
	TC_IOM_PME_B0_EN | \
	TC_TBT1_PME_B0_EN| \
	TC_TBT0_PME_B0_EN| \
	TBTLSX_PME_B0_EN)

/* All hot plug events except TCSS in GPE1 block 1 */
#define GPE1_HOT_PLUG_EVENT_EN_BITS ( \
	PG5_PMA0_HOT_PLUG_EN_3 | \
	PG5_PMA0_HOT_PLUG_EN_2 | \
	PG5_PMA0_HOT_PLUG_EN_1 | \
	PG5_PMA0_HOT_PLUG_EN_0 | \
	PG5_PMA1_HOT_PLUG_EN_3 | \
	PG5_PMA1_HOT_PLUG_EN_2 | \
	PG5_PMA1_HOT_PLUG_EN_1 | \
	PG5_PMA1_HOT_PLUG_EN_0 | \
	IOE_HOT_PLUG_EN |        \
	SPB_HOT_PLUG_EN |        \
	SPA_HOT_PLUG_EN)

/* All GPE1 TCSS hot plug events in GPE1 block 1 */
#define GPE1_TC_HOT_PLUG_EVENT_EN_BITS ( \
	TC_TBT1_HOT_PLUG_EN  | \
	TC_TBT0_HOT_PLUG_EN  | \
	TC_PCIE3_HOT_PLUG_EN | \
	TC_PCIE2_HOT_PLUG_EN | \
	TC_PCIE1_HOT_PLUG_EN | \
	TC_PCIE0_HOT_PLUG_EN)

/* All PCIe events except TCSS in GPE1 block 2 */
#define GPE1_PCI_EXP_EVENT_EN_BITS ( \
	PG5_PMA0_PCI_EXP_EN_3 | \
	PG5_PMA0_PCI_EXP_EN_2 | \
	PG5_PMA0_PCI_EXP_EN_1 | \
	PG5_PMA0_PCI_EXP_EN_0 | \
	PG5_PMA1_PCI_EXP_EN_3 | \
	PG5_PMA1_PCI_EXP_EN_2 | \
	PG5_PMA1_PCI_EXP_EN_1 | \
	PG5_PMA1_PCI_EXP_EN_0 | \
	IOE_PCI_EXP_EN |        \
	SPB_PCI_EXP_EN |        \
	SPA_PCI_EXP_EN)

/* All GPE1 TCSS PCIe events in GPE1 block 2 */
#define GPE1_TC_PCI_EXP_EVENT_EN_BITS ( \
	TC_TBT1_PCI_EXP_EN  | \
	TC_TBT0_PCI_EXP_EN  | \
	TC_PCIE3_PCI_EXP_EN | \
	TC_PCIE2_PCI_EXP_EN | \
	TC_PCIE1_PCI_EXP_EN | \
	TC_PCIE0_PCI_EXP_EN)

#endif /* _SOC_PANTHERLAKE_GPE_H_ */
