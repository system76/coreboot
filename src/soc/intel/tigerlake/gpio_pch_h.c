/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <intelblocks/gpio.h>
#include <intelblocks/pcr.h>
#include <soc/pcr_ids.h>
#include <soc/pmc.h>

/*
 * This file is created based on Intel Tiger Lake Processor PCH Datasheet
 * Document number: 575857
 * Chapter number: 27
 */

static const struct reset_mapping rst_map[] = {
	{ .logical = PAD_CFG0_LOGICAL_RESET_RSMRST, .chipset = 0U << 30 },
	{ .logical = PAD_CFG0_LOGICAL_RESET_DEEP, .chipset = 1U << 30 },
	{ .logical = PAD_CFG0_LOGICAL_RESET_PLTRST, .chipset = 2U << 30 },
};
static const struct reset_mapping rst_map_com2[] = {
	{ .logical = PAD_CFG0_LOGICAL_RESET_PWROK, .chipset = 0U << 30 },
	{ .logical = PAD_CFG0_LOGICAL_RESET_DEEP, .chipset = 1U << 30 },
	{ .logical = PAD_CFG0_LOGICAL_RESET_PLTRST, .chipset = 2U << 30 },
	{ .logical = PAD_CFG0_LOGICAL_RESET_RSMRST, .chipset = 3U << 30 },
};

/*
 * The GPIO pinctrl driver for Tiger Lake on Linux expects 32 GPIOs per pad
 * group, regardless of whether or not there is a physical pad for each
 * exposed GPIO number.
 *
 * This results in the OS having a sparse GPIO map, and devices that need
 * to export an ACPI GPIO must use the OS expected number.
 *
 * Not all pins are usable as GPIO and those groups do not have a pad base.
 *
 * This layout matches the Linux kernel pinctrl map for TGL at:
 * linux/drivers/pinctrl/intel/pinctrl-tigerlake.c
 */
static const struct pad_group tgl_community0_groups[] = {
	INTEL_GPP_BASE(GPP_A0, GPP_A0, GPP_A14, 0),			/* GPP_A */
	INTEL_GPP_BASE(GPP_A0, GPP_R0, GPP_R19, 32),			/* GPP_R */
	INTEL_GPP_BASE(GPP_A0, GPP_B0, GPP_B23, 64),			/* GPP_B */
};

static const struct pad_group tgl_community1_groups[] = {
	INTEL_GPP_BASE(GPP_D0, GPP_D0, GPP_D23, 96),			/* GPP_D */
	INTEL_GPP_BASE(GPP_D0, GPP_C0, GPP_C23, 128),			/* GPP_C */
	INTEL_GPP_BASE(GPP_D0, GPP_S0, GPP_S7, 160),			/* GPP_S */
	INTEL_GPP_BASE(GPP_D0, GPP_G0, GPP_G15, 192),			/* GPP_G */
};

static const struct pad_group tgl_community2_groups[] = {
	INTEL_GPP(GPD0, GPD0, GPD12),			/* GPD */
};

static const struct pad_group tgl_community3_groups[] = {
	INTEL_GPP_BASE(GPP_E0, GPP_E0, GPP_E12, 224),			/* GPP_E */
	INTEL_GPP_BASE(GPP_E0, GPP_F0, GPP_F23, 256),			/* GPP_F */
};

static const struct pad_group tgl_community4_groups[] = {
	INTEL_GPP_BASE(GPP_H0, GPP_H0, GPP_H23, 288),			/* GPP_H */
	INTEL_GPP_BASE(GPP_H0, GPP_J0, GPP_J9, 320),			/* GPP_J */
	INTEL_GPP_BASE(GPP_H0, GPP_K0, GPP_K11, 352),			/* GPP_K */
};

static const struct pad_group tgl_community5_groups[] = {
	INTEL_GPP_BASE(GPP_I0, GPP_I0, GPP_I14, 384),			/* GPP_I */
};

static const struct pad_community tgl_communities[] = {
	[COMM_0] = {
		.port = PID_GPIOCOM0,
		.first_pad = GPP_A0,
		.last_pad = GPP_B23,
		.num_gpi_regs = NUM_GPIO_COM0_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_ARB",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = tgl_community0_groups,
		.num_groups = ARRAY_SIZE(tgl_community0_groups),
	},
	[COMM_1] = {
		.port = PID_GPIOCOM1,
		.first_pad = GPP_D0,
		.last_pad = GPP_G15,
		.num_gpi_regs = NUM_GPIO_COM1_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_DCSG",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = tgl_community1_groups,
		.num_groups = ARRAY_SIZE(tgl_community1_groups),
	},
	[COMM_2] = {
		.port = PID_GPIOCOM2,
		.first_pad = GPD0,
		.last_pad = GPD12,
		.num_gpi_regs = NUM_GPIO_COM2_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPD",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map_com2,
		.num_reset_vals = ARRAY_SIZE(rst_map_com2),
		.groups = tgl_community2_groups,
		.num_groups = ARRAY_SIZE(tgl_community2_groups),
	},
	[COMM_3] = {
		.port = PID_GPIOCOM3,
		.first_pad = GPP_E0,
		.last_pad = GPP_F23,
		.num_gpi_regs = NUM_GPIO_COM3_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_EF",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = tgl_community3_groups,
		.num_groups = ARRAY_SIZE(tgl_community3_groups),
	},
	[COMM_4] = {
		.port = PID_GPIOCOM4,
		.first_pad = GPP_H0,
		.last_pad = GPP_K11,
		.num_gpi_regs = NUM_GPIO_COM4_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_HJK",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = tgl_community4_groups,
		.num_groups = ARRAY_SIZE(tgl_community4_groups),
	},
	[COMM_5] = {
		.port = PID_GPIOCOM5,
		.first_pad = GPP_I0,
		.last_pad = GPP_I14,
		.num_gpi_regs = NUM_GPIO_COM5_GPI_REGS,
		.pad_cfg_base = PAD_CFG_BASE,
		.host_own_reg_0 = HOSTSW_OWN_REG_0,
		.gpi_int_sts_reg_0 = GPI_INT_STS_0,
		.gpi_int_en_reg_0 = GPI_INT_EN_0,
		.gpi_smi_sts_reg_0 = GPI_SMI_STS_0,
		.gpi_smi_en_reg_0 = GPI_SMI_EN_0,
		.max_pads_per_group = GPIO_MAX_NUM_PER_GROUP,
		.name = "GPP_I",
		.acpi_path = "\\_SB.PCI0.GPIO",
		.reset_map = rst_map,
		.num_reset_vals = ARRAY_SIZE(rst_map),
		.groups = tgl_community5_groups,
		.num_groups = ARRAY_SIZE(tgl_community5_groups),
	},
};

const struct pad_community *soc_gpio_get_community(size_t *num_communities)
{
	*num_communities = ARRAY_SIZE(tgl_communities);
	return tgl_communities;
}

const struct pmc_to_gpio_route *soc_pmc_gpio_routes(size_t *num)
{
	static const struct pmc_to_gpio_route routes[] = {
		{ PMC_GPD, GPD },
		{ PMC_GPP_A, GPP_A },
		{ PMC_GPP_B, GPP_B },
		{ PMC_GPP_C, GPP_C },
		{ PMC_GPP_D, GPP_D },
		{ PMC_GPP_E, GPP_E },
		{ PMC_GPP_F, GPP_F },
		{ PMC_GPP_G, GPP_G },
		{ PMC_GPP_H, GPP_H },
		{ PMC_GPP_I, GPP_I },
		{ PMC_GPP_J, GPP_J },
		{ PMC_GPP_K, GPP_K },
		{ PMC_GPP_R, GPP_R },
		{ PMC_GPP_S, GPP_S },
	};
	*num = ARRAY_SIZE(routes);
	return routes;
}
