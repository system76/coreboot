/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <device/device.h>
#include <soc/mmu.h>
#include <soc/mmu_common.h>
#include <soc/symbols_common.h>
#include <soc/pcie.h>
#include <soc/clock.h>
#include <soc/cpucp.h>
#include <soc/variant.h>
#include <program_loading.h>

/*
 * Weak implementation of mainboard-specific display initialization.
 * This can be overridden by mainboard-specific code.
 */
__weak void mainboard_soc_init(void)
{
	/* Default implementation: do nothing */
}

static struct device_operations pci_domain_ops = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources
};

static void soc_read_resources(struct device *dev)
{
	/* placeholder */
}

static void qtee_fw_config_load(void)
{
	if (!CONFIG(ARM64_USE_SECURE_OS))
		return;

	struct prog devcfg_tz = PROG_INIT(PROG_PAYLOAD,
					CONFIG_CBFS_PREFIX"/tzoem_cfg");
	if (!selfload(&devcfg_tz))
		die("devcfg_tz load failed");

	struct prog tzqti_cfg = PROG_INIT(PROG_PAYLOAD,
					CONFIG_CBFS_PREFIX"/tzqti_cfg");
	if (!selfload(&tzqti_cfg))
		die("tzqti_cfg load failed");

	struct prog tzac_cfg = PROG_INIT(PROG_PAYLOAD,
					CONFIG_CBFS_PREFIX"/tzac_cfg");
	if (!selfload(&tzac_cfg))
		die("tzac_cfg load failed");

	struct prog hypac_cfg = PROG_INIT(PROG_PAYLOAD,
					CONFIG_CBFS_PREFIX"/hypac_cfg");
	if (!selfload(&hypac_cfg))
		die("hypac_cfg load failed");
}

static void soc_init(struct device *dev)
{
	cpucp_fw_load_reset();
	qtee_fw_config_load();
}

static struct device_operations soc_ops = {
	.read_resources = soc_read_resources,
	.set_resources = noop_set_resources,
	.init = soc_init,
};

static void enable_soc_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		if (mainboard_needs_pcie_init())
			dev->ops = &pci_domain_ops;
		else
			printk(BIOS_DEBUG, "Skip setting PCIe ops\n");
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
		dev->ops = &soc_ops;
}

struct chip_operations soc_qualcomm_calypso_ops = {
	.name = "Calypso",
	.enable_dev = enable_soc_dev,
};

static void soc_late_init(void *unused)
{
	/* placeholder code in sync w/ x1p42100 SoC */
	mainboard_soc_init();
}

BOOT_STATE_INIT_ENTRY(BS_WRITE_TABLES, BS_ON_ENTRY, soc_late_init, NULL);
