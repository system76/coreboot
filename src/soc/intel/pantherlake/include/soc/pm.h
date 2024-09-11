/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_PANTHERLAKE_PM_H_
#define _SOC_PANTHERLAKE_PM_H_

#define PM1_STS			0x00
#define  WAK_STS		BIT(15)
#define  PCIEXPWAK_STS		BIT(14)
#define  PRBTNOR_STS		BIT(11)
#define  RTC_STS		BIT(10)
#define  PWRBTN_STS		BIT(8)
#define  GBL_STS		BIT(5)
#define  BM_STS			BIT(4)
#define  TMROF_STS		BIT(0)
#define PM1_EN			0x02
#define  PCIEXPWAK_DIS		BIT(14)
#define  RTC_EN			BIT(10)
#define  PWRBTN_EN		BIT(8)
#define  GBL_EN			BIT(5)
#define  TMROF_EN		BIT(0)
#define PM1_CNT			0x04
#define  GBL_RLS		BIT(2)
#define  BM_RLD			BIT(1)
#define  SCI_EN			BIT(0)
#define PM1_TMR			0x08
#define SMI_EN			0x30
#define  XHCI_SMI_EN		BIT(31)
#define  ME_SMI_EN		BIT(30)
#define  ESPI_SMI_EN		BIT(28)
#define  GPIO_UNLOCK_SMI_EN	BIT(27)
#define  INTEL_USB2_EN		BIT(18)
#define  LEGACY_USB2_EN		BIT(17)
#define  PERIODIC_EN		BIT(14)
#define  TCO_SMI_EN		BIT(13)
#define  MCSMI_EN		BIT(11)
#define  BIOS_RLS		BIT(7)
#define  SWSMI_TMR_EN		BIT(6)
#define  APMC_EN		BIT(5)
#define  SLP_SMI_EN		BIT(4)
#define  LEGACY_USB_EN		BIT(3)
#define  BIOS_EN		BIT(2)
#define  EOS			BIT(1)
#define  GBL_SMI_EN		BIT(0)
#define SMI_STS			0x34
#define  SMI_STS_BITS			32
#define  XHCI_SMI_STS_BIT		31
#define  ME_SMI_STS_BIT			30
#define  ESPI_SMI_STS_BIT		28
#define  GPIO_UNLOCK_SMI_STS_BIT	27
#define  SPI_SMI_STS_BIT		26
#define  SCC_SMI_STS_BIT		25
#define  MONITOR_STS_BIT		21
#define  PCI_EXP_SMI_STS_BIT		20
#define  SMBUS_SMI_STS_BIT		16
#define  SERIRQ_SMI_STS_BIT		15
#define  PERIODIC_STS_BIT		14
#define  TCO_STS_BIT			13
#define  DEVMON_STS_BIT			12
#define  MCSMI_STS_BIT			11
#define  GPIO_STS_BIT			10
#define  GPE0_STS_BIT			9
#define  PM1_STS_BIT			8
#define  SWSMI_TMR_STS_BIT		6
#define  APM_STS_BIT			5
#define  SMI_ON_SLP_EN_STS_BIT		4
#define  LEGACY_USB_STS_BIT		3
#define  BIOS_STS_BIT			2
#define GPE_CNTL		0x42
#define  SWGPE_CTRL		BIT(1)
#define DEVACT_STS		0x44
#define PM2_CNT			0x50

#define GPE0_REG_MAX		4
#define GPE0_REG_SIZE		32
#define GPE0_STS(x)		(0x60 + ((x) * 4))
#define  GPE_31_0		0	/* 0x60/0x70 = GPE0[31:0] */
#define  GPE_63_32		1	/* 0x64/0x74 = GPE0[63:32] */
#define  GPE_95_64		2	/* 0x68/0x78 = GPE0[95:64] */
#define  GPE_STD		3	/* 0x6c/0x7c = Standard GPE */
#define GPE_STS_RSVD		GPE_STD
#define   OSSE_SCI_STS		BIT(24)
#define   TC_HOT_PLUG_STS	BIT(23)
#define   TC_PCI_EXP_STS	BIT(22)
#define   TCSS_SX_WAKE_STS	BIT(19)
#define   WADT_STS		BIT(18)
#define   USB_CON_DSX_STS	BIT(17)
#define   LANWAKE_STS		BIT(16)
#define   GPIO_T2_STS		BIT(15)
#define   ESPI_STS		BIT(14)
#define   PME_B0_STS		BIT(13)
#define   ME_SCI_STS		BIT(12)
#define   PME_STS		BIT(11)
#define   BATLOW_STS		BIT(10)
#define   PCI_EXP_STS		BIT(9)
#define   ISH_STS		BIT(8)
#define   SMB_WAK_STS		BIT(7)
#define   TCOSCI_STS		BIT(6)
#define   TC_PME_B0_STS		BIT(5)
#define   THERM_STS		BIT(4)
#define   SWGPE_STS		BIT(2)
#define   HOT_PLUG_STS		BIT(1)
#define   FIA_DFLEX_STS		BIT(0)

#define GPE0_EN(x)		(0x70 + ((x) * 4))
#define   OSSE_SCI_EN		BIT(24)
#define   TC_HOT_PLUG_EN	BIT(23)
#define   TC_PCI_EXP_EN		BIT(22)
#define   TCSS_SX_WAKE_EN	BIT(19)
#define   WADT_EN		BIT(18)
#define   USB_CON_DSX_EN	BIT(17)
#define   LANWAKE_EN		BIT(16)
#define   GPIO_T2_EN		BIT(15)
#define   ESPI_EN		BIT(14)
#define   PME_B0_EN		BIT(13)
#define   ME_SCI_EN		BIT(12)
#define   PME_EN		BIT(11)
#define   BATLOW_EN		BIT(10)
#define   PCI_EXP_EN		BIT(9)
#define   ISH_EN		BIT(8)
/*
  SMB_WAK_ENi:  bit 7 is reserved
  NOTE: For Intel PCH, the SMBus slave will always be enabled as a wake event.
*/
#define   TCOSCI_EN		BIT(6)
#define   TC_PME_B0_EN		BIT(5)
#define   THERM_EN		BIT(4)
#define   SWGPE_EN		BIT(2)
#define   HOT_PLUG_EN		BIT(1)
#define   FIA_DFLEX_EN		BIT(0)

#define EN_BLOCK		3

/*
 * SOC_INTEL_COMMON_BLOCK_ACPI_HAVE_GPE1 is set for PTL SoC, hence the
 * following three GPE1_* macros are required.
 */
#define GPE1_REG_MAX	3
#define GPE1_EN(x)	(0x1c + ((x) * 4))
#define GPE1_STS(x)	(0x10 + ((x) * 4))

#define GPE1_REG_SIZE	32
#define  GPE1_31_0		0	/* 0x10/0x1c = GPE1[31:0]  */
#define  GPE1_63_32		1	/* 0x14/0x20 = GPE1[63:32] */
#define  GPE1_95_64		2	/* 0x18/0x2c = GPE1[95:64] */

/* 0x10		GPE1_STS_31_0	General Purpose Event 1 Status [31:0] */
#define CNVI_BT_PME_B0_STS	BIT(18)
#define TC_IOM_PME_B0_STS	BIT(17)
#define TC_TBT1_PME_B0_STS	BIT(16)
#define TC_TBT0_PME_B0_STS	BIT(15)
#define LPSS_PME_B0_STS		BIT(14)
#define CSE_PME_B0_STS		BIT(13)
#define XDCI_PME_B0_STS		BIT(12)
#define ACE_PME_B0_STS		BIT(10)
#define XHCI_PME_B0_STS		BIT(9)
#define SATA_PME_B0_STS		BIT(8)
#define CSME_PME_B0_STS		BIT(7)
#define GBE_PME_B0_STS		BIT(6)
#define CNVI_PME_B0_STS		BIT(5)
#define OSSE_PME_B0_STS		BIT(4)
#define TBTLSX_PME_B0_STS	BIT(1)
/* 0x14		GPE1_STS_63_32	General Purpose Event 1 Status [63:32] */
#define PG5_PMA0_HOT_PLUG_STS_3	BIT(31)
#define PG5_PMA0_HOT_PLUG_STS_2	BIT(30)
#define PG5_PMA0_HOT_PLUG_STS_1	BIT(29)
#define PG5_PMA0_HOT_PLUG_STS_0	BIT(28)
#define PG5_PMA1_HOT_PLUG_STS_3	BIT(27)
#define PG5_PMA1_HOT_PLUG_STS_2	BIT(26)
#define PG5_PMA1_HOT_PLUG_STS_1	BIT(25)
#define PG5_PMA1_HOT_PLUG_STS_0	BIT(24)
#define TC_TBT1_HOT_PLUG_STS	BIT(13)
#define TC_TBT0_HOT_PLUG_STS	BIT(12)
#define TC_PCIE3_HOT_PLUG_STS	BIT(11)
#define TC_PCIE2_HOT_PLUG_STS	BIT(10)
#define TC_PCIE1_HOT_PLUG_STS	BIT(9)
#define TC_PCIE0_HOT_PLUG_STS	BIT(8)
#define IOE_HOT_PLUG_STS	BIT(7)
#define SPB_HOT_PLUG_STS	BIT(1)
#define SPA_HOT_PLUG_STS	BIT(0)
/* 0x18		GPE1_STS_95_64	General Purpose Event 1 Status [63:32] */
#define PG5_PMA0_PCI_EXP_STS_3	BIT(31)
#define PG5_PMA0_PCI_EXP_STS_2	BIT(30)
#define PG5_PMA0_PCI_EXP_STS_1	BIT(29)
#define PG5_PMA0_PCI_EXP_STS_0	BIT(28)
#define PG5_PMA1_PCI_EXP_STS_3	BIT(27)
#define PG5_PMA1_PCI_EXP_STS_2	BIT(26)
#define PG5_PMA1_PCI_EXP_STS_1	BIT(25)
#define PG5_PMA1_PCI_EXP_STS_0	BIT(24)
#define TC_TBT1_PCI_EXP_STS	BIT(13)
#define TC_TBT0_PCI_EXP_STS	BIT(12)
#define TC_PCIE3_PCI_EXP_STS	BIT(11)
#define TC_PCIE2_PCI_EXP_STS	BIT(10)
#define TC_PCIE1_PCI_EXP_STS	BIT(9)
#define TC_PCIE0_PCI_EXP_STS	BIT(8)
#define IOE_PCI_EXP_STS		BIT(7)
#define SPB_PCI_EXP_STS		BIT(1)
#define SPA_PCI_EXP_STS		BIT(0)
/* 0x1C		GPE1_EN_31_0	General Purpose Event 1 Enable [31:0] */
#define CNVI_BT_PME_B0_EN	BIT(18)
#define TC_IOM_PME_B0_EN	BIT(17)
#define TC_TBT1_PME_B0_EN	BIT(16)
#define TC_TBT0_PME_B0_EN	BIT(15)
#define LPSS_PME_B0_EN		BIT(14)
#define CSE_PME_B0_EN		BIT(13)
#define XDCI_PME_B0_EN		BIT(12)
#define ACE_PME_B0_EN		BIT(10)
#define XHCI_PME_B0_EN		BIT(9)
#define SATA_PME_B0_EN		BIT(8)
#define CSME_PME_B0_EN		BIT(7)
#define GBE_PME_B0_EN		BIT(6)
#define CNVI_PME_B0_EN		BIT(5)
#define OSSE_PME_B0_EN		BIT(4)
#define TBTLSX_PME_B0_EN	BIT(1)
/* 0x20		GPE1_EN_63_32	General Purpose Event 1 Enable [63:32] */
#define PG5_PMA0_HOT_PLUG_EN_3	BIT(31)
#define PG5_PMA0_HOT_PLUG_EN_2	BIT(30)
#define PG5_PMA0_HOT_PLUG_EN_1	BIT(29)
#define PG5_PMA0_HOT_PLUG_EN_0	BIT(28)
#define PG5_PMA1_HOT_PLUG_EN_3	BIT(27)
#define PG5_PMA1_HOT_PLUG_EN_2	BIT(26)
#define PG5_PMA1_HOT_PLUG_EN_1	BIT(25)
#define PG5_PMA1_HOT_PLUG_EN_0	BIT(24)
#define TC_TBT1_HOT_PLUG_EN	BIT(13)
#define TC_TBT0_HOT_PLUG_EN	BIT(12)
#define TC_PCIE3_HOT_PLUG_EN	BIT(11)
#define TC_PCIE2_HOT_PLUG_EN	BIT(10)
#define TC_PCIE1_HOT_PLUG_EN	BIT(9)
#define TC_PCIE0_HOT_PLUG_EN	BIT(8)
#define IOE_HOT_PLUG_EN		BIT(7)
#define SPB_HOT_PLUG_EN		BIT(1)
#define SPA_HOT_PLUG_EN		BIT(0)
/* 0x24		GPE1_EN_95_64	General Purpose Event 1 Enable [63:32] */
#define PG5_PMA0_PCI_EXP_EN_3	BIT(31)
#define PG5_PMA0_PCI_EXP_EN_2	BIT(30)
#define PG5_PMA0_PCI_EXP_EN_1	BIT(29)
#define PG5_PMA0_PCI_EXP_EN_0	BIT(28)
#define PG5_PMA1_PCI_EXP_EN_3	BIT(27)
#define PG5_PMA1_PCI_EXP_EN_2	BIT(26)
#define PG5_PMA1_PCI_EXP_EN_1	BIT(25)
#define PG5_PMA1_PCI_EXP_EN_0	BIT(24)
#define TC_TBT1_PCI_EXP_EN	BIT(13)
#define TC_TBT0_PCI_EXP_EN	BIT(12)
#define TC_PCIE3_PCI_EXP_EN	BIT(11)
#define TC_PCIE2_PCI_EXP_EN	BIT(10)
#define TC_PCIE1_PCI_EXP_EN	BIT(9)
#define TC_PCIE0_PCI_EXP_EN	BIT(8)
#define IOE_PCI_EXP_EN		BIT(7)
#define SPB_PCI_EXP_EN		BIT(1)
#define SPA_PCI_EXP_EN		BIT(0)

/*
 * Enable SMI generation:
 *  - on APMC writes (io 0xb2)
 *  - on writes to SLP_EN (sleep states)
 *  - on writes to GBL_RLS (bios commands)
 *  - on eSPI events (does nothing on LPC systems)
 * No SMIs:
 *  - on TCO events, unless enabled in common code
 *  - on microcontroller writes (io 0x62/0x66)
 */
#define ENABLE_SMI_PARAMS \
	(APMC_EN | SLP_SMI_EN | GBL_SMI_EN | ESPI_SMI_EN | EOS)

#define	PSS_RATIO_STEP		2
#define	PSS_MAX_ENTRIES		8
#define	PSS_LATENCY_TRANSITION	10
#define	PSS_LATENCY_BUSMASTER	10

#if !defined(__ACPI__)

#include <acpi/acpi.h>
#include <soc/gpe.h>
#include <soc/iomap.h>
#include <soc/smbus.h>
#include <soc/pmc.h>

struct chipset_power_state {
	uint16_t pm1_sts;
	uint16_t pm1_en;
	uint32_t pm1_cnt;
#if CONFIG(SOC_INTEL_COMMON_BLOCK_ACPI_HAVE_GPE1)
	uint32_t gpe1_sts[3];
	uint32_t gpe1_en[3];
#endif
	uint16_t tco1_sts;
	uint16_t tco2_sts;
	uint32_t gpe0_sts[4];
	uint32_t gpe0_en[4];
	uint32_t gen_pmcon_a;
	uint32_t gen_pmcon_b;
	uint32_t gblrst_cause[2];
	uint32_t hpr_cause0;
	uint32_t prev_sleep_state;
} __packed;

/* Get base address PMC memory mapped registers. */
uint8_t *pmc_mmio_regs(void);

/* Get base address of TCO I/O registers. */
uint16_t smbus_tco_regs(void);

/* Set the DISB after DRAM init */
void pmc_set_disb(void);

/* STM Support */
uint16_t get_pmbase(void);
#endif /* !defined(__ACPI__) */

#endif /* _SOC_PANTHERLAKE_PM_H_ */
