/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "nct5525d.h"
#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>

static void nct5525d_init(struct device *dev)
{
	if (!dev->enabled)
		return;
}

static struct device_operations ops = {
	.read_resources = pnp_read_resources,
	.set_resources = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable = pnp_alt_enable,
	.init = nct5525d_init,
	.ops_pnp_mode = &pnp_conf_mode_8787_aa,
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, NCT5525D_UARTA, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_nuvoton_nct5525d_ops = {
	.name = "NUVOTON NCT5525D Super I/O",
	.enable_dev = enable_dev,
};
