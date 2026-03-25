/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cache.h>
#include <arch/mmu.h>
#include <console/console.h>
#include <program_loading.h>
#include <soc/cpucp.h>
#include <device/mmio.h>
#include <security/vboot/vboot_common.h>
#include <soc/addressmap.h>
#include <soc/mmu_common.h>
#include <soc/symbols_common.h>
#include <symbols.h>

void cpucp_fw_load_reset(void)
{
	/* map to cached region to force address to be 4 byte aligned */
	mmu_config_range((void *)_cpucp, REGION_SIZE(cpucp), CACHED_RAM);

	struct prog cpucp_dtbs_prog =
		PROG_INIT(PROG_PAYLOAD, CONFIG_CBFS_PREFIX "/cpucp_dtbs");

	if (!selfload(&cpucp_dtbs_prog))
		die("SOC image: CPUCP DTBS load failed");

	printk(BIOS_DEBUG, "SOC image: CPUCP DTBS image loaded successfully.\n");

	const char *cpucp_name = (CONFIG(VBOOT) && !vboot_recovery_mode_enabled())
			 ? CONFIG_CBFS_PREFIX "/cpucp_rw"
			 : CONFIG_CBFS_PREFIX "/cpucp_ro";

	struct prog cpucp_fw_prog = PROG_INIT(PROG_PAYLOAD, cpucp_name);

	if (!selfload(&cpucp_fw_prog))
		die("SOC image: CPUCP load failed");

	/* flush cached region */
	dcache_clean_by_mva(_cpucp, REGION_SIZE(cpucp));
	/* remap back to device memory */
	mmu_config_range((void *)_cpucp, REGION_SIZE(cpucp), DEV_MEM);

	printk(BIOS_DEBUG, "SOC image: CPUCP image loaded successfully.\n");

	write32((void *) HWIO_APSS_CPUCP_CPUCP_LPM_SEQ_WAIT_EVT_CTRL_MASK_ADDR, 0x0);
	setbits32((void *) HWIO_APSS_CPUCP_CPUCP_SW_WAKEUP_REQ_ADDR, 0x1);
}
