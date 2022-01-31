/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <commonlib/console/post_codes.h>
#include <console/console.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/smm.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/p2sb.h>
#include <intelblocks/pcr.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/tco.h>
#include <intelblocks/thermal.h>
#include <soc/me.h>
#include <soc/p2sb.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/pm.h>
#include <soc/smbus.h>
#include <soc/systemagent.h>
#include <spi-generic.h>

#include "chip.h"

#define PSF_BASE_ADDRESS	0xA00
#define PCR_PSFX_T0_SHDW_PCIEN	0x1C
#define PCR_PSFX_T0_SHDW_PCIEN_FUNDIS	(1 << 8)

static void pch_disable_heci(void)
{
	/* unhide p2sb device */
	p2sb_unhide();

	/* disable heci */
	pcr_or32(PID_PSF1, PSF_BASE_ADDRESS + PCR_PSFX_T0_SHDW_PCIEN,
		PCR_PSFX_T0_SHDW_PCIEN_FUNDIS);

	p2sb_disable_sideband_access();
}

static void pch_finalize_script(struct device *dev)
{
	tco_lockdown();

	/* Display me status before we hide it */
	intel_me_status();

	/*
	 * Set low maximum temp value used for dynamic thermal sensor
	 * shutdown consideration.
	 *
	 * If Dynamic Thermal Shutdown is enabled then PMC logic shuts down the
	 * thermal sensor when CPU is in a C-state and DTS Temp <= LTT.
	 */
	pch_thermal_configuration();

	/* we should disable Heci1 based on the config */
	if (CONFIG(DISABLE_HECI1_AT_PRE_BOOT))
		pch_disable_heci();

	/* Hide p2sb device as the OS must not change BAR0. */
	p2sb_hide();
}

static void soc_lockdown(struct device *dev)
{
	struct soc_intel_skylake_config *config;
	u8 reg8;

	config = config_of(dev);

	/* Global SMI Lock */
	if (config->LockDownConfigGlobalSmi == 0) {
		reg8 = pci_read_config8(dev, GEN_PMCON_A);
		reg8 |= SMI_LOCK;
		pci_write_config8(dev, GEN_PMCON_A, reg8);
	}

	/*
	 * Lock chipset memory registers to protect SMM.
	 * When SkipMpInit=0, this is done by FSP.
	 */
	if (!CONFIG(USE_INTEL_FSP_MP_INIT))
		cpu_lt_lock_memory();
}

static void soc_finalize(void *unused)
{
	struct device *dev;

	dev = PCH_DEV_PMC;

	/* Check if PMC is enabled, else return */
	if (dev == NULL)
		return;

	printk(BIOS_DEBUG, "Finalizing chipset.\n");

	pch_finalize_script(dev);

	soc_lockdown(dev);
	apm_control(APM_CNT_FINALIZE);

	/* Indicate finalize step with post code */
	post_code(POST_OS_BOOT);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, soc_finalize, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, soc_finalize, NULL);
