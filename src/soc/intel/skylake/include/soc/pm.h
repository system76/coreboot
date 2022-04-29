/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_PM_H_
#define _SOC_PM_H_

#include <acpi/acpi.h>
#include <device/mmio.h>
#include <soc/gpe.h>
#include <soc/iomap.h>
#include <soc/pmc.h>
#include <soc/smbus.h>

/* ACPI_BASE_ADDRESS / PMBASE */

#define PM1_STS			0x00
#define  WAK_STS		(1 << 15)
#define  PCIEXPWAK_STS		(1 << 14)
#define  PRBTNOR_STS		(1 << 11)
#define  RTC_STS		(1 << 10)
#define  PWRBTN_STS		(1 << 8)
#define  GBL_STS		(1 << 5)
#define  BM_STS			(1 << 4)
#define  TMROF_STS		(1 << 0)
#define PM1_EN			0x02
#define  PCIEXPWAK_DIS		(1 << 14)
#define  RTC_EN			(1 << 10)
#define  PWRBTN_EN		(1 << 8)
#define  GBL_EN			(1 << 5)
#define  TMROF_EN		(1 << 0)
#define PM1_CNT			0x04
#define  GBL_RLS		(1 << 2)
#define  BM_RLD			(1 << 1)
#define  SCI_EN			(1 << 0)
#define PM1_TMR			0x08
#define SMI_EN			0x30
#define  XHCI_SMI_EN		(1 << 31)
#define  ME_SMI_EN		(1 << 30)
#define  ESPI_SMI_EN		(1 << 28)
#define  GPIO_UNLOCK_SMI_EN	(1 << 27)
#define  INTEL_USB2_EN		(1 << 18)
#define  LEGACY_USB2_EN		(1 << 17)
#define  PERIODIC_EN		(1 << 14)
#define  TCO_SMI_EN		(1 << 13)
#define  MCSMI_EN		(1 << 11)
#define  BIOS_RLS		(1 <<  7)
#define  SWSMI_TMR_EN		(1 <<  6)
#define  APMC_EN		(1 <<  5)
#define  SLP_SMI_EN		(1 <<  4)
#define  LEGACY_USB_EN		(1 <<  3)
#define  BIOS_EN		(1 <<  2)
#define  EOS			(1 <<  1)
#define  GBL_SMI_EN		(1 <<  0)
#define SMI_STS			0x34
#define  SMI_STS_BITS			32
#define  XHCI_SMI_STS_BIT		31
#define  ME_SMI_STS_BIT			30
#define  SERIAL_IO_SMI_STS_BIT		29
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
#define UPWRC			0x3c
#define  UPWRC_WS		(1 << 8)
#define  UPWRC_WE		(1 << 1)
#define  UPWRC_SMI		(1 << 0)
#define GPE_CNTL		0x42
#define  SWGPE_CTRL		(1 << 1)
#define DEVACT_STS		0x44
#define PM2_CNT			0x50

#define GPE0_REG_MAX		4
#define GPE0_REG_SIZE		32
#define GPE0_STS(x)		(0x80 + ((x) * 4))
#define  GPE_31_0		0	/* 0x80/0x90 = GPE[31:0] */
#define  GPE_63_32		1	/* 0x84/0x94 = GPE[63:32] */
#define  GPE_95_64		2	/* 0x88/0x98 = GPE[95:64] */
#define  GPE_STD		3	/* 0x8c/0x9c = Standard GPE */
#define GPE_STS_RSVD            GPE_STD
#define   WADT_STS		(1 << 18)
#define   LAN_WAK_STS		(1 << 16)
#define   GPIO_T2_STS		(1 << 15)
#define   ESPI_STS		(1 << 14)
#define   PME_B0_STS		(1 << 13)
#define   ME_SCI_STS		(1 << 12)
#define   PME_STS		(1 << 11)
#define   BATLOW_STS		(1 << 10)
#define   PCI_EXP_STS		(1 << 9)
#define   SMB_WAK_STS		(1 << 7)
#define   TCOSCI_STS		(1 << 6)
#define   SWGPE_STS		(1 << 2)
#define   HOT_PLUG_STS		(1 << 1)
#define GPE0_EN(x)		(0x90 + ((x) * 4))
#define   WADT_EN		(1 << 18)
#define   LAN_WAK_EN		(1 << 16)
#define   GPIO_T2_EN		(1 << 15)
#define   ESPI_EN		(1 << 14)
#define   PME_B0_EN		(1 << 13)
#define   ME_SCI_EN		(1 << 12)
#define   PME_EN		(1 << 11)
#define   BATLOW_EN		(1 << 10)
#define   PCI_EXP_EN		(1 << 9)
#define   TCOSCI_EN		(1 << 6)
#define   SWGPE_EN		(1 << 2)
#define   HOT_PLUG_EN		(1 << 1)

#define GBLRST_CAUSE0_THERMTRIP	(1 << 5)

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

/* This is defined as ETR3 in EDS. We named it as ETR here for consistency */
#define ETR		0xac
#define  CF9_LOCK	(1 << 31)
#define  CF9_GLB_RST	(1 << 20)

#define PRSTS		0x10

/* P-state configuration */
#define PSS_MAX_ENTRIES		8
#define PSS_RATIO_STEP		2
#define PSS_LATENCY_TRANSITION	10
#define PSS_LATENCY_BUSMASTER	10

struct chipset_power_state {
	uint16_t pm1_sts;
	uint16_t pm1_en;
	uint32_t pm1_cnt;
	uint16_t tco1_sts;
	uint16_t tco2_sts;
	uint32_t gpe0_sts[4];
	uint32_t gpe0_en[4];
	uint32_t gen_pmcon_a;
	uint32_t gen_pmcon_b;
	uint32_t gblrst_cause[2];
	uint32_t prev_sleep_state;
} __packed;

/* Get base address PMC memory mapped registers. */
uint8_t *pmc_mmio_regs(void);

/* Set the DISB after DRAM init */
void pmc_set_disb(void);

/* Return non-zero when RTC failure happened. */
int rtc_failure(void);

static inline int deep_s3_enabled(void)
{
	uint32_t deep_s3_pol;

	deep_s3_pol = read32(pmc_mmio_regs() + S3_PWRGATE_POL);
	return !!(deep_s3_pol & (S3DC_GATE_SUS | S3AC_GATE_SUS));
}

static inline int deep_s5_enabled(void)
{
	uint32_t deep_s5_pol;

	deep_s5_pol = read32(pmc_mmio_regs() + S5_PWRGATE_POL);
	return !!(deep_s5_pol & (S5DC_GATE_SUS | S5AC_GATE_SUS));
}

/* STM Support */
uint16_t get_pmbase(void);

#endif
