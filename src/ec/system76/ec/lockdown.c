/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <commonlib/region.h>
#include <fmap.h>
#include <spi_flash.h>

#include "system76_ec.h"

static int protect_region_by_name(const char *name)
{
	int res;
	struct region region;

	res = fmap_locate_area(name, &region);
	if (res < 0) {
		printk(BIOS_ERR, "fmap_locate_area '%s' failed: %d\n", name, res);
		return res;
	}

	res = spi_flash_ctrlr_protect_region(
		boot_device_spi_flash(),
		&region,
		WRITE_PROTECT
	);
	if (res < 0) {
		printk(BIOS_ERR, "spi_flash_ctrlr_protect_region '%s' failed: %d\n", name, res);
		return res;
	}

	printk(BIOS_INFO, "protected '%s'\n", name);
	return 0;
}

static void lock(void *unused)
{
	uint8_t state = SYSTEM76_EC_SECURITY_STATE_UNLOCK;
	if (system76_ec_security_get(&state) < 0) {
		printk(BIOS_INFO, "failed to get security state, assuming unlocked\n");
		state = SYSTEM76_EC_SECURITY_STATE_UNLOCK;
	}

	printk(BIOS_INFO, "security state: %d\n", state);
	if (state != SYSTEM76_EC_SECURITY_STATE_UNLOCK) {
		// Protect WP_RO region, which should contain FMAP and COREBOOT
		protect_region_by_name("WP_RO");
		// Protect RW_MRC_CACHE region, this must be done after it is written
		protect_region_by_name("RW_MRC_CACHE");
		//TODO: protect entire flash except when in SMM?
	}
}

/*
 * Keep in sync with mrc_cache.c
 */

#if CONFIG(MRC_WRITE_NV_LATE)
BOOT_STATE_INIT_ENTRY(BS_OS_RESUME_CHECK, BS_ON_EXIT, lock, NULL);
#else
BOOT_STATE_INIT_ENTRY(BS_DEV_RESOURCES, BS_ON_ENTRY, lock, NULL);
#endif
