/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <chip.h>
#include <ec/google/chromeec/ec.h>
#include <ec/google/chromeec/ec_commands.h>
#include <intelblocks/power_limit.h>

const struct cpu_power_limits limits[] = {
	/* SKU_ID, TDP (Watts), pl1_min, pl1_max, pl2_min, pl2_max, pl4 */
	{ PCI_DID_INTEL_ADL_P_ID_10, 15, 15000, 15000,  55000,  55000, 123000 },
	{ PCI_DID_INTEL_ADL_P_ID_7, 15, 15000, 15000,  55000,  55000, 123000 },
	{ PCI_DID_INTEL_ADL_P_ID_6, 15, 15000, 15000,  55000,  55000, 123000 },
	{ PCI_DID_INTEL_ADL_P_ID_5, 28, 28000, 28000,  64000,  64000, 140000 },
	{ PCI_DID_INTEL_ADL_P_ID_3, 28, 28000, 28000,  64000,  64000, 140000 },
	{ PCI_DID_INTEL_RPL_P_ID_5, 15, 15000, 15000,  55000,  55000, 100000 },
	{ PCI_DID_INTEL_RPL_P_ID_4, 15, 15000, 15000,  55000,  55000, 100000 },
	{ PCI_DID_INTEL_RPL_P_ID_3, 15, 15000, 15000,  55000,  55000, 100000 },
};

const struct system_power_limits sys_limits[] = {
	/* SKU_ID, TDP (Watts), psys_pl2 (Watts) */
	{ PCI_DID_INTEL_ADL_P_ID_10, 15, 90 },
	{ PCI_DID_INTEL_ADL_P_ID_7, 15, 90 },
	{ PCI_DID_INTEL_ADL_P_ID_6, 15, 90 },
	{ PCI_DID_INTEL_ADL_P_ID_5, 28, 90 },
	{ PCI_DID_INTEL_ADL_P_ID_3, 28, 90 },
	{ PCI_DID_INTEL_RPL_P_ID_5, 15, 90 },
	{ PCI_DID_INTEL_RPL_P_ID_4, 15, 90 },
	{ PCI_DID_INTEL_RPL_P_ID_3, 15, 90 },
};

/*
 * Psys_pmax considerations.
 *
 * Given the hardware design in bujia, the serial shunt resistor is 0.005ohm.
 * The full scale of hardware PSYS signal 1.6v maps to system current 8A
 * instead of real system power. The equation is shown below:
 * PSYS = 1.6v = (0.005ohm x 8A) x 50 (INA213, gain 50V/V) x R501/(R501 + R510)
 * R501/(R501 + R510) = 0.8 = 45.3K / (45.3K + 11.5K)
 *
 * The Psys_pmax is a SW setting which tells IMVP9.1 the mapping b/w system input
 * current and the actual system power. Since there is no voltage information
 * from PSYS, different voltage input would map to different Psys_pmax settings:
 * For Type-C 15V, the Psys_pmax should be 15v x 8A = 120W
 * For Type-C 20V, the Psys_pmax should be 20v x 8A = 160W
 * For a barrel jack, the Psys_pmax should be 20v x 8A = 160W
 *
 * Imagine that there is a Type-c 100W (20V/5A) connected to DUT w/ full loading,
 * and the Psys_pmax setting is 160W. Then IMVP9.1 can calculate the current system
 * power = 160W * 5A / 8A = 100W, which is the actual system power.
 */
struct psys_config psys_config = {
	.efficiency = 97,
	.psys_imax_ma = 8000,
	.bj_volts_mv = 12000
};

static int32_t get_input_power_voltage(void)
{
	struct ec_params_adc_read req = {
		.adc_channel = 4,
	};
	struct ec_response_adc_read info;
	struct chromeec_command cmd = {
		.cmd_code     = EC_CMD_ADC_READ,
		.cmd_size_in  = sizeof(req),
		.cmd_data_in  = &req,
		.cmd_size_out = sizeof(info),
		.cmd_data_out = &info,
	};
	if (!google_chromeec_command(&cmd))
		return psys_config.bj_volts_mv;

	return info.adc_value;
}

void variant_devtree_update(void)
{
	psys_config.bj_volts_mv = get_input_power_voltage();
	size_t total_entries = ARRAY_SIZE(limits);
	variant_update_psys_power_limits(limits, sys_limits, total_entries, &psys_config);
	variant_update_power_limits(limits, total_entries);
}
