/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_def.h>
#include <intelblocks/cse.h>
#include <intelblocks/smihandler.h>
#include <soc/soc_chip.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>

/*
 * Specific SOC SMI handler during ramstage finalize phase
 *
 * BIOS can't make CSME function disable as is due to POSTBOOT_SAI
 * restriction in place from JSP chipset. Hence create SMI Handler to
 * perform CSME function disabling logic during SMM mode.
 */
void smihandler_soc_at_finalize(void)
{
	if (CONFIG(DISABLE_HECI1_AT_PRE_BOOT) && CONFIG(HECI_DISABLE_USING_SMM))
		heci_disable();
}

int smihandler_soc_disable_busmaster(pci_devfn_t dev)
{
	/* Skip disabling PMC bus master to keep IO decode enabled */
	if (dev == PCH_DEV_PMC)
		return 0;
	return 1;
}

const smi_handler_t southbridge_smi[SMI_STS_BITS] = {
	[SMI_ON_SLP_EN_STS_BIT] = smihandler_southbridge_sleep,
	[APM_STS_BIT] = smihandler_southbridge_apmc,
	[PM1_STS_BIT] = smihandler_southbridge_pm1,
	[GPE0_STS_BIT] = smihandler_southbridge_gpe0,
	[GPIO_STS_BIT] = smihandler_southbridge_gpi,
	[ESPI_SMI_STS_BIT] = smihandler_southbridge_espi,
	[MCSMI_STS_BIT] = smihandler_southbridge_mc,
#if CONFIG(SOC_INTEL_COMMON_BLOCK_SMM_TCO_ENABLE)
	[TCO_STS_BIT] = smihandler_southbridge_tco,
#endif
	[PERIODIC_STS_BIT] = smihandler_southbridge_periodic,
	[MONITOR_STS_BIT] = smihandler_southbridge_monitor,
};
