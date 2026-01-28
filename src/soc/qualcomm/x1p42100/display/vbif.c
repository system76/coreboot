/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/clock.h>
#include <soc/display/mdssreg.h>
#include <console/console.h>
#include <soc/qcom_spmi.h>
#include <gpio.h>
#include <soc/display/edp_reg.h>

void configure_vbif_qos(void)
{
	// Global Init
	write32(&vbif_rt->vbif_clkon, 0x00000000); // dynamic clock gating enabled
	write32(&vbif_rt->vbif_ddr_out_max_burst,
		0x00000707); // DDR AXI max bursts: RD=7+1, WR=7+1
	write32(&vbif_rt->vbif_out_axi_aooo_en,
		0x0000FFFF); // enable SW override of AOOO per client
	write32(&vbif_rt->vbif_out_axi_aooo, 0xFFFFFFFF); // force AOOORD/AOOOWR for all clients

	write32(&vbif_rt->vbif_out_axi_amemtype_conf0, 0x33333333);
	write32(&vbif_rt->vbif_out_axi_amemtype_conf1, 0x00333333);

	write32(&vbif_rt->qos_rp_remap[0].vbif_xinl_qos_rp_remap, 0x00000003);
	write32(&vbif_rt->qos_rp_remap[1].vbif_xinl_qos_rp_remap, 0x11111113);
	write32(&vbif_rt->qos_rp_remap[2].vbif_xinl_qos_rp_remap, 0x22222224);
	write32(&vbif_rt->qos_rp_remap[3].vbif_xinl_qos_rp_remap, 0x33333334);
	write32(&vbif_rt->qos_rp_remap[4].vbif_xinl_qos_rp_remap, 0x44444445);
	write32(&vbif_rt->qos_rp_remap[7].vbif_xinl_qos_rp_remap, 0x77777776);
	write32(&vbif_rt->qos_lvl_remap[0].vbif_xinl_qos_lvl_remap, 0x00000003);
	write32(&vbif_rt->qos_lvl_remap[1].vbif_xinl_qos_lvl_remap, 0x11111113);
	write32(&vbif_rt->qos_lvl_remap[2].vbif_xinl_qos_lvl_remap, 0x22222224);
	write32(&vbif_rt->qos_lvl_remap[3].vbif_xinl_qos_lvl_remap, 0x33333334);
	write32(&vbif_rt->qos_lvl_remap[4].vbif_xinl_qos_lvl_remap, 0x44444445);
	write32(&vbif_rt->qos_lvl_remap[5].vbif_xinl_qos_lvl_remap, 0x77777776);
}

void merge_3d_active(void)
{
	write32(&mdp_ctl_0->merge_3d_flush, 0x1);
	write32(&mdp_ctl_0->ctl_intf_flush, 0x20);
	write32(&mdp_ctl_0->periph_flush, 0x20);
	write32(&mdp_ctl_0->ctl_flush, 0xC08200C3);
	write32(&edp_lclk->vsc_db16_db17_db18_pb8, 0x10100);
	write32(&edp_lclk->compression_mode_ctrl, 0x2800);
	write32(&mdp_intf->intf_config2, 0x111);
	write32(&edp_lclk->db_ctrl, 0x01);
	write32(&mdp_intf->intf_config, 0x800000);
}
