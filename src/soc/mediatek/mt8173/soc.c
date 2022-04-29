/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmem.h>
#include <device/device.h>
#include <symbols.h>
#include <soc/emi.h>

void bootmem_platform_add_ranges(void)
{
	bootmem_add_range(0x101000, 124 * KiB, BM_MEM_BL31);
}

static void soc_read_resources(struct device *dev)
{
	ram_resource(dev, 0, (uintptr_t)_dram / KiB, sdram_size() / KiB);
}

static void soc_init(struct device *dev)
{
}

static struct device_operations soc_ops = {
	.read_resources = soc_read_resources,
	.set_resources = noop_set_resources,
	.init = soc_init,
};

static void enable_soc_dev(struct device *dev)
{
	dev->ops = &soc_ops;
}

struct chip_operations soc_mediatek_mt8173_ops = {
	CHIP_NAME("SOC Mediatek MT8173")
	.enable_dev = enable_soc_dev,
};
