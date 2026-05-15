/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Common helper functions for dealing with power management registers
 * for modern Intel platforms.
 *
 * This driver consolidates PM utility functions that were duplicated
 * across multiple Intel SoC platforms.
 */

#define __SIMPLE_DEVICE__

#include <acpi/acpi_pm.h>
#include <console/console.h>
#include <device/mmio.h>
#include <device/device.h>
#include <device/pci.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/rtc.h>
#include <intelblocks/tco.h>
#include <intelpch/espi.h>
#include <security/vboot/vbnv.h>
#include <soc/gpe.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/smbus.h>
#include <soc/soc_chip.h>
#include <static.h>
#include <types.h>

/*
 * SMI
 */
const char *const *soc_smi_sts_array(size_t *a)
{
	static const char *const smi_sts_bits[] = {
		[BIOS_STS_BIT] = "BIOS",
		[LEGACY_USB_STS_BIT] = "LEGACY_USB",
		[SMI_ON_SLP_EN_STS_BIT] = "SLP_SMI",
		[APM_STS_BIT] = "APM",
		[SWSMI_TMR_STS_BIT] = "SWSMI_TMR",
		[PM1_STS_BIT] = "PM1",
		[GPE0_STS_BIT] = "GPE0",
		[GPIO_STS_BIT] = "GPI",
		[MCSMI_STS_BIT] = "MCSMI",
		[DEVMON_STS_BIT] = "DEVMON",
		[TCO_STS_BIT] = "TCO",
		[PERIODIC_STS_BIT] = "PERIODIC",
		[SERIRQ_SMI_STS_BIT] = "SERIRQ_SMI",
		[SMBUS_SMI_STS_BIT] = "SMBUS_SMI",
		[PCI_EXP_SMI_STS_BIT] = "PCI_EXP_SMI",
		[MONITOR_STS_BIT] = "MONITOR",
		[SPI_SMI_STS_BIT] = "SPI",
		[GPIO_UNLOCK_SMI_STS_BIT] = "GPIO_UNLOCK",
		[ESPI_SMI_STS_BIT] = "ESPI_SMI",
	};

	*a = ARRAY_SIZE(smi_sts_bits);
	return smi_sts_bits;
}

/*
 * TCO
 */
const char *const *soc_tco_sts_array(size_t *a)
{
	static const char *const tco_sts_bits[] = {
		[0] = "NMI2SMI",
		[1] = "SW_TCO",
		[2] = "TCO_INT",
		[3] = "TIMEOUT",
		[7] = "NEWCENTURY",
		[8] = "BIOSWR",
		[9] = "DMISCI",
		[10] = "DMISMI",
		[12] = "DMISERR",
		[13] = "SLVSEL",
		[16] = "INTRD_DET",
		[17] = "SECOND_TO",
		[18] = "BOOT",
		[20] = "SMLINK_SLV"
	};

	*a = ARRAY_SIZE(tco_sts_bits);
	return tco_sts_bits;
}

/*
 * GPE0
 */
const char *const *soc_std_gpe_sts_array(size_t *a)
{
	static const char *const gpe_sts_bits[] = {
		[1] = "HOTPLUG",
		[2] = "SWGPE",
		[6] = "TCO_SCI",
		[7] = "SMB_WAK",
		[9] = "PCI_EXP",
		[10] = "BATLOW",
		[11] = "PME",
		[12] = "ME",
		[13] = "PME_B0",
		[14] = "eSPI",
		[15] = "GPIO Tier-2",
		[16] = "LAN_WAKE",
		[18] = "WADT"
	};

	*a = ARRAY_SIZE(gpe_sts_bits);
	return gpe_sts_bits;
}

#if CONFIG(SOC_INTEL_COMMON_BLOCK_ACPI_USE_GPE1)
const char *const *soc_std_gpe1_sts_array(int idx, size_t *a)
{
	static const char *const gpe1_sts_bits[GPE1_REG_MAX][32] = {
		/* 0x10  GPE1_31_0   General Purpose Event 1 Status [31:0] */
		[0] = {                           /* GPE# dec   hex   GPE1 bit position */
			[18] = "CNVI_BT_PME_B0",      /*   146      0x92       [18]         */
			[17] = "TC_IOM_PME_B0",       /*   145      0x91       [17]         */
			[16] = "TC_TBT1_PME_B0",      /*   144      0x90       [16]         */
			[15] = "TC_TBT0_PME_B0",      /*   143      0x8F       [15]         */
			[14] = "LPSS_PME_B0",         /*   142      0x8E       [14]         */
			[13] = "CSE_PME_B0",          /*   141      0x8D       [13]         */
			[12] = "XDCI_PME_B0",         /*   140      0x8C       [12]         */
			[10] = "ACE_PME_B0",          /*   138      0x8A       [10]         */
			[9] = "XHCI_PME_B0",          /*   137      0x89       [9]          */
			[8] = "SATA_PME_B0",          /*   136      0x88       [8]          */
			[7] = "CSME_PME_B0",          /*   135      0x87       [7]          */
			[6] = "GBE_PME_B0",           /*   134      0x86       [6]          */
			[5] = "CNVI_PME_B0",          /*   133      0x85       [5]          */
			[4] = "OSSE_PME_B0",          /*   132      0x84       [4]          */
			[1] = "TBTLSX_PME_B0",        /*   129      0x81       [1]          */
		},
		/* 0x14  GPE1_63_32 General Purpose Event 1 Status [63:32] */
		[1] = {                            /* GPE# dec  hex   GPE1 bit position */
			[31] = "PG5_PMA0_HOT_PLUG_3",  /*  191      0xBF       [63]         */
			[30] = "PG5_PMA0_HOT_PLUG_2",  /*  190      0xBE       [62]         */
			[29] = "PG5_PMA0_HOT_PLUG_1",  /*  189      0xBD       [61]         */
			[28] = "PG5_PMA0_HOT_PLUG_0",  /*  188      0xBC       [60]         */
			[27] = "PG5_PMA1_HOT_PLUG_3",  /*  187      0xBB       [59]         */
			[26] = "PG5_PMA1_HOT_PLUG_2",  /*  186      0xBA       [58]         */
			[25] = "PG5_PMA1_HOT_PLUG_1",  /*  185      0xB9       [57]         */
			[24] = "PG5_PMA1_HOT_PLUG_0",  /*  184      0xB8       [56]         */
			[13] = "TC_TBT1_HOT_PLUG",     /*  173      0xAD       [45]         */
			[12] = "TC_TBT0_HOT_PLUG",     /*  172      0xAC       [44]         */
			[11] = "TC_PCIE3_HOT_PLUG",    /*  171      0xAB       [43]         */
			[10] = "TC_PCIE2_HOT_PLUG",    /*  170      0xAA       [42]         */
			[9]  = "TC_PCIE1_HOT_PLUG",    /*  169      0xA9       [41]         */
			[8]  = "TC_PCIE0_HOT_PLUG",    /*  168      0xA8       [40]         */
			[7]  = "IOE_HOT_PLUG",         /*  167      0xA7       [39]         */
			[1]  = "SPB_HOT_PLUG",         /*  161      0xA1       [33]         */
			[0]  = "SPA_HOT_PLUG",         /*  160      0xA0       [32]         */
		},
		/* 0x18  GPE1_95_64 General Purpose Event 1 Status [63:32] */
		[2] = {                            /* GPE# dec  hex   GPE1 bit position */
			[31] = "PG5_PMA0_PCI_EXP_3",   /*  223      0xDF       [95]         */
			[30] = "PG5_PMA0_PCI_EXP_2",   /*  222      0xDE       [94]         */
			[29] = "PG5_PMA0_PCI_EXP_1",   /*  221      0xDD       [93]         */
			[28] = "PG5_PMA0_PCI_EXP_0",   /*  220      0xDC       [92]         */
			[27] = "PG5_PMA1_PCI_EXP_3",   /*  219      0xDB       [91]         */
			[26] = "PG5_PMA1_PCI_EXP_2",   /*  218      0xDA       [90]         */
			[25] = "PG5_PMA1_PCI_EXP_1",   /*  217      0xD9       [89]         */
			[24] = "PG5_PMA1_PCI_EXP_0",   /*  216      0xD8       [88]         */
			[13] = "TC_TBT1_PCI_EXP",      /*  205      0xCD       [77]         */
			[12] = "TC_TBT0_PCI_EXP",      /*  204      0xCC       [76]         */
			[11] = "TC_PCIE3_PCI_EXP",     /*  203      0xCb       [75]         */
			[10] = "TC_PCIE2_PCI_EXP",     /*  202      0xCA       [74]         */
			[9]  = "TC_PCIE1_PCI_EXP",     /*  201      0xC9       [73]         */
			[8]  = "TC_PCIE0_PCI_EXP",     /*  200      0xC8       [72]         */
			[7]  = "IOE_PCI_EXP",          /*  199      0xC7       [71]         */
			[1]  = "SPB_PCI_EXP",          /*  193      0xC1       [65]         */
			[0]  = "SPA_PCI_EXP",          /*  192      0xC0       [64]         */
		}
	};
	*a = ARRAY_SIZE(gpe1_sts_bits[idx]);
	return gpe1_sts_bits[idx];
}

/* disable the corresponding GPE1 bits based on STD GPE0 bits */
void soc_pmc_disable_std_gpe1(uint32_t std_gpe0_mask)
{
	if (std_gpe0_mask & PME_B0_EN)
		pmc_disable_gpe1(GPE1_EN(0), GPE1_PME_B0_EVENT_EN_BITS);

	if (std_gpe0_mask & TC_PME_B0_EN)
		pmc_disable_gpe1(GPE1_EN(0), GPE1_TC_PME_B0_EVENT_EN_BITS);

	if (std_gpe0_mask & HOT_PLUG_STS)
		pmc_disable_gpe1(GPE1_EN(1), GPE1_HOT_PLUG_EVENT_EN_BITS);

	if (std_gpe0_mask & TC_HOT_PLUG_EN)
		pmc_disable_gpe1(GPE1_EN(1), GPE1_TC_HOT_PLUG_EVENT_EN_BITS);

	if (std_gpe0_mask & PCI_EXP_EN)
		pmc_disable_gpe1(GPE1_EN(2), GPE1_PCI_EXP_EVENT_EN_BITS);

	if (std_gpe0_mask & TC_PCI_EXP_EN)
		pmc_disable_gpe1(GPE1_EN(2), GPE1_TC_PCI_EXP_EVENT_EN_BITS);
}

/* enable the corresponding GPE1 bits based on STD GPE0 bits */
void soc_pmc_enable_std_gpe1(uint32_t std_gpe0_mask)
{
	if (std_gpe0_mask & PME_B0_EN)
		pmc_enable_gpe1(GPE1_EN(0), GPE1_PME_B0_EVENT_EN_BITS);

	if (std_gpe0_mask & TC_PME_B0_EN)
		pmc_enable_gpe1(GPE1_EN(0), GPE1_TC_PME_B0_EVENT_EN_BITS);

	if (std_gpe0_mask & HOT_PLUG_STS)
		pmc_enable_gpe1(GPE1_EN(1), GPE1_HOT_PLUG_EVENT_EN_BITS);

	if (std_gpe0_mask & TC_HOT_PLUG_EN)
		pmc_enable_gpe1(GPE1_EN(1), GPE1_TC_HOT_PLUG_EVENT_EN_BITS);

	if (std_gpe0_mask & PCI_EXP_EN)
		pmc_enable_gpe1(GPE1_EN(2), GPE1_PCI_EXP_EVENT_EN_BITS);

	if (std_gpe0_mask & TC_PCI_EXP_EN)
		pmc_enable_gpe1(GPE1_EN(2), GPE1_TC_PCI_EXP_EVENT_EN_BITS);
}
#endif

void pmc_set_disb(void)
{
	/* Set the DISB after DRAM init */
	uint8_t disb_val;
	/* Only care about bits [23:16] of register GEN_PMCON_A */
	uint8_t *addr = (uint8_t *)(pmc_mmio_regs() + GEN_PMCON_A + 2);

	disb_val = read8(addr);
	disb_val |= (DISB >> 16);

	/* Don't clear bits that are write-1-to-clear */
	disb_val &= ~((MS4V | SUS_PWR_FLR) >> 16);
	write8(addr, disb_val);
}

/*
 * PMC controller gets hidden from PCI bus
 * during FSP-Silicon init call. Hence PWRMBASE
 * can't be accessible using PCI configuration space
 * read/write.
 */
uint8_t *pmc_mmio_regs(void)
{
	return (void *)(uintptr_t)PCH_PWRM_BASE_ADDRESS;
}

uintptr_t soc_read_pmc_base(void)
{
	return (uintptr_t)pmc_mmio_regs();
}

uint32_t *soc_pmc_etr_addr(void)
{
	return (uint32_t *)(soc_read_pmc_base() + ETR);
}

static void pmc_gpe0_different_values(const config_t *config)
{
	bool all_zero = (config->pmc_gpe0_dw0 == 0) &&
			 (config->pmc_gpe0_dw1 == 0) &&
			 (config->pmc_gpe0_dw2 == 0);

	/* Check if all values are different AND not all zero */
	bool all_different = (config->pmc_gpe0_dw0 != config->pmc_gpe0_dw1) &&
			 (config->pmc_gpe0_dw0 != config->pmc_gpe0_dw2) &&
			 (config->pmc_gpe0_dw1 != config->pmc_gpe0_dw2);

	assert(all_different || all_zero);
}

void soc_get_gpi_gpe_configs(uint8_t *dw0, uint8_t *dw1, uint8_t *dw2)
{
	DEVTREE_CONST config_t *config;

	config = config_of_soc();
	if (config == NULL) {
		printk(BIOS_ERR, "Configuration could not be retrieved.\n");
		return;
	}

	pmc_gpe0_different_values(config);

	/* Assign to out variable */
	*dw0 = config->pmc_gpe0_dw0;
	*dw1 = config->pmc_gpe0_dw1;
	*dw2 = config->pmc_gpe0_dw2;
}

static int rtc_failed(uint32_t gen_pmcon_b)
{
	return !!(gen_pmcon_b & RTC_BATTERY_DEAD);
}

int soc_get_rtc_failed(void)
{
	const struct chipset_power_state *ps;

	if (acpi_fetch_pm_state(&ps, PS_CLAIMER_RTC) < 0)
		return 1;

	return rtc_failed(ps->gen_pmcon_b);
}

int vbnv_cmos_failed(void)
{
	return rtc_failed(read32(pmc_mmio_regs() + GEN_PMCON_B));
}

static inline int deep_s3_enabled(void)
{
	uint32_t deep_s3_pol;

	deep_s3_pol = read32(pmc_mmio_regs() + S3_PWRGATE_POL);
	return !!(deep_s3_pol & (S3DC_GATE_SUS | S3AC_GATE_SUS));
}

/* Return 0, 3, or 5 to indicate the previous sleep state. */
int soc_prev_sleep_state(const struct chipset_power_state *ps, int prev_sleep_state)
{
	/*
	 * Check for any power failure to determine if this a wake from
	 * S5 because the PCH does not set the WAK_STS bit when waking
	 * from a true G3 state.
	 */
	if (!(ps->pm1_sts & WAK_STS) && (ps->gen_pmcon_a & (PWR_FLR | SUS_PWR_FLR)))
		prev_sleep_state = ACPI_S5;

	/*
	 * If waking from S3 determine if deep S3 is enabled. If not,
	 * need to check both deep sleep well and normal suspend well.
	 * Otherwise just check deep sleep well.
	 */
	if (prev_sleep_state == ACPI_S3) {
		/* PWR_FLR represents deep sleep power well loss. */
		uint32_t mask = PWR_FLR;

		/* If deep s3 isn't enabled check the suspend well too. */
		if (!deep_s3_enabled())
			mask |= SUS_PWR_FLR;

		if (ps->gen_pmcon_a & mask)
			prev_sleep_state = ACPI_S5;
	}

	return prev_sleep_state;
}

void soc_fill_power_state(struct chipset_power_state *ps)
{
	uint8_t *pmc;

	ps->tco1_sts = tco_read_reg(TCO1_STS);
	ps->tco2_sts = tco_read_reg(TCO2_STS);

	printk(BIOS_DEBUG, "TCO_STS:   %04x %04x\n", ps->tco1_sts, ps->tco2_sts);

	pmc = pmc_mmio_regs();
	ps->gen_pmcon_a = read32(pmc + GEN_PMCON_A);
	ps->gen_pmcon_b = read32(pmc + GEN_PMCON_B);
	ps->gblrst_cause[0] = read32(pmc + GBLRST_CAUSE0);
	ps->gblrst_cause[1] = read32(pmc + GBLRST_CAUSE1);
	ps->hpr_cause0 = read32(pmc + HPR_CAUSE0);

	printk(BIOS_DEBUG, "GEN_PMCON: %08x %08x\n",
		ps->gen_pmcon_a, ps->gen_pmcon_b);

	printk(BIOS_DEBUG, "GBLRST_CAUSE: %08x %08x\n",
		ps->gblrst_cause[0], ps->gblrst_cause[1]);

	printk(BIOS_DEBUG, "HPR_CAUSE0: %08x\n", ps->hpr_cause0);
}

/* STM Support */
uint16_t get_pmbase(void)
{
	return (uint16_t)ACPI_BASE_ADDRESS;
}

/*
 * Set which power state system will be after reapplying
 * the power (from G3 State)
 */
void pmc_soc_set_afterg3_en(const bool on)
{
	uint8_t reg8;
	uint8_t *const pmcbase = pmc_mmio_regs();

	reg8 = read8(pmcbase + GEN_PMCON_A);
	if (on)
		reg8 &= ~SLEEP_AFTER_POWER_FAIL;
	else
		reg8 |= SLEEP_AFTER_POWER_FAIL;
	write8(pmcbase + GEN_PMCON_A, reg8);
}
