/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/gpio.h>
#include <soc/platform_descriptors.h>
#include <types.h>

static const fsp_dxio_descriptor kudu6_dxio_descriptors[] = {
	{ /* DGPU */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 16,
		.end_logical_lane = 23,
		.device_number = 1,
		.function_number = 1,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ0,
		.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122}
	},
	{ /* LAN */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 0,
		.end_logical_lane = 0,
		.device_number = 2,
		.function_number = 1,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ1,
		.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122}
	},
	{ /* WLAN */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 1,
		.end_logical_lane = 1,
		.device_number = 2,
		.function_number = 2,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ6,
		.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122}
	},
	{ /* SSD2 */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 4,
		.end_logical_lane = 7,
		.device_number = 2,
		.function_number = 3,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ4_GFX,
		.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122}
	},
	{ /* SSD1 */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 8,
		.end_logical_lane = 11,
		.device_number = 2,
		.function_number = 4,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ5,
		.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122}
	},
	{ /* HDD */
		.engine_type = SATA_ENGINE,
		.port_present = true,
		.start_logical_lane = 0,
		.end_logical_lane = 0,
		.channel_type = SATA_CHANNEL_LONG,
	}
};

static const fsp_ddi_descriptor kudu6_ddi_descriptors[] = {
	{ /* DDI0 - eDP */
		.connector_type = DDI_EDP,
		.aux_index = DDI_AUX1,
		.hdp_index = DDI_HDP1
	},
	{ /* DDI1 */
		.connector_type = DDI_UNUSED_TYPE,
		.aux_index = DDI_AUX2,
		.hdp_index = DDI_HDP2
	},
	{ /* DDI2 */
		.connector_type = DDI_UNUSED_TYPE,
		.aux_index = DDI_AUX3,
		.hdp_index = DDI_HDP3,
	},
	{ /* DDI3 */
		.connector_type = DDI_UNUSED_TYPE,
		.aux_index = DDI_AUX3,
		.hdp_index = DDI_HDP3,
	},
	{ /* DDI4 */
		.connector_type = DDI_UNUSED_TYPE,
		.aux_index = DDI_AUX4,
		.hdp_index = DDI_HDP4,
	}
};

void mainboard_get_dxio_ddi_descriptors(
		const fsp_dxio_descriptor **dxio_descs, size_t *dxio_num,
		const fsp_ddi_descriptor **ddi_descs, size_t *ddi_num)
{
	*dxio_descs = kudu6_dxio_descriptors;
	*dxio_num = ARRAY_SIZE(kudu6_dxio_descriptors);
	*ddi_descs = kudu6_ddi_descriptors;
	*ddi_num = ARRAY_SIZE(kudu6_ddi_descriptors);
}
