/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/clock.h>
#include <soc/display/edp_reg.h>
#include <soc/display/edp_phy.h>
#include <timer.h>
#include <soc/display/mdssreg.h>

static void edp_phy_ssc_en(bool en)
{
	if (en) {
		write32(&edp_phy_pll->qserdes_com_ssc_en_center, 0x01);
		write32(&edp_phy_pll->qserdes_com_ssc_adj_per1, 0x00);
		write32(&edp_phy_pll->qserdes_com_ssc_per1, 0x6B);
		write32(&edp_phy_pll->qserdes_com_ssc_per2, 0x02);
		write32(&edp_phy_pll->qserdes_com_ssc_step_size1_mode0, 0x92);
		write32(&edp_phy_pll->qserdes_com_ssc_step_size2_mode0, 0x01);
	} else {
		write32(&edp_phy_pll->qserdes_com_ssc_en_center, 0x00);
		write32(&edp_phy_pll->qserdes_com_ssc_adj_per1, 0x00);
		write32(&edp_phy_pll->qserdes_com_ssc_per1, 0x00);
		write32(&edp_phy_pll->qserdes_com_ssc_per2, 0x00);
		write32(&edp_phy_pll->qserdes_com_ssc_step_size1_mode0, 0x00);
		write32(&edp_phy_pll->qserdes_com_ssc_step_size2_mode0, 0x00);
	}
}

void early_phy_enable(void)
{
	write32(&edp_phy->pd_ctl, 0x2);
	write32(&edp_phy->pd_ctl, 0x7D);

	write32(&edp_phy->aux_interrupt_mask, 0x1F);

	write32(&edp_phy_pll->qserdes_com_bias_en_clkbuflr_en, 0x1F);

	write32(&edp_phy->aux_cfg[0], 0x00);
	write32(&edp_phy->aux_cfg[1], 0x00);
	write32(&edp_phy->aux_cfg[2], 0xA4);
	write32(&edp_phy->aux_cfg[3], 0x00);
	write32(&edp_phy->aux_cfg[4], 0x0A);
	write32(&edp_phy->aux_cfg[5], 0x26);
	write32(&edp_phy->aux_cfg[6], 0x0A);
	write32(&edp_phy->aux_cfg[7], 0x03);
	write32(&edp_phy->aux_cfg[8], 0x37);
	write32(&edp_phy->aux_cfg[9], 0x03);

	write32(&edp_phy->aux_interrupt_mask, 0x1f);
	write32(&edp_phy->mode, 0xFC);

	if (!wait_us(10000, read32(&edp_phy_pll->qserdes_com_cmn_status) & BIT(7)))
		printk(BIOS_ERR, "%s: refgen not ready : 0x%x , Add= %p\n", __func__,
		       read32(&edp_phy_pll->qserdes_com_cmn_status),
		       (void *)(&edp_phy_pll->qserdes_com_cmn_status));

	printk(BIOS_DEBUG, "qserdes_com_cmn_status = 0x%x\n",
	       read32(&edp_phy_pll->qserdes_com_cmn_status));

	write32(&edp_phy_lane_tx0->tx_lane_mode1, 0x00);
	write32(&edp_phy_lane_tx1->tx_lane_mode1, 0x00);
}

int edp_phy_enable(uint32_t link_rate)
{
	write32(&edp_phy->pd_ctl, 0x2);
	write32(&edp_phy->pd_ctl, 0x7D);

	write32(&edp_phy->aux_interrupt_mask, 0x1F);

	write32(&edp_phy_pll->qserdes_com_bias_en_clkbuflr_en, 0x1F);

	write32(&edp_phy->aux_cfg[0], 0x00);
	write32(&edp_phy->aux_cfg[1], 0x00);
	write32(&edp_phy->aux_cfg[2], 0xA4);
	write32(&edp_phy->aux_cfg[3], 0x00);
	write32(&edp_phy->aux_cfg[4], 0x0A);
	write32(&edp_phy->aux_cfg[5], 0x26);
	write32(&edp_phy->aux_cfg[6], 0x0A);
	write32(&edp_phy->aux_cfg[7], 0x03);
	write32(&edp_phy->aux_cfg[8], 0x37);
	write32(&edp_phy->aux_cfg[9], 0x03);

	write32(&edp_phy->aux_interrupt_mask, 0x1f);
	write32(&edp_phy->mode, 0xFC);

	if (!wait_us(10000, read32(&edp_phy_pll->qserdes_com_cmn_status) & BIT(7)))
		printk(BIOS_ERR, "%s: refgen not ready : 0x%x , Add= %p\n", __func__,
		       read32(&edp_phy_pll->qserdes_com_cmn_status),
		       (void *)(&edp_phy_pll->qserdes_com_cmn_status));

	printk(BIOS_DEBUG, "qserdes_com_cmn_status = 0x%x\n",
	       read32(&edp_phy_pll->qserdes_com_cmn_status));

	write32(&edp_phy_lane_tx0->tx_lane_mode1, 0x00);
	write32(&edp_phy_lane_tx1->tx_lane_mode1, 0x00);

	if (link_rate >= 324000) {
		printk(BIOS_DEBUG, "%s: link rate %u\n", __func__, link_rate);
		write32(&edp_phy_lane_tx0->tx_ldo_config, 0x91);
		write32(&edp_phy_lane_tx1->tx_ldo_config, 0x91);
	} else {
		printk(BIOS_DEBUG, "%s: link rate %u\n", __func__, link_rate);
		write32(&edp_phy_lane_tx0->tx_ldo_config, 0x51);
		write32(&edp_phy_lane_tx1->tx_ldo_config, 0x51);
	}

	write32(&edp_phy_lane_tx0->tx_lane_mode1, 0x00);
	write32(&edp_phy_lane_tx1->tx_lane_mode1, 0x00);

	return 0;
}


static const u8 edp_rbr_voltage_swing[4][4] = {
	{0x07, 0x0f, 0x16, 0x1f},
	{0x0d, 0x16, 0x1e, 0xff},
	{0x11, 0x1b, 0xff, 0xff},
	{0x16, 0xff, 0xff, 0xff}
};
static const u8 edp_rbr_pre_emphasis[4][4] = {
	{0x05, 0x11, 0x17, 0x1d},
	{0x05, 0x11, 0x18, 0xff},
	{0x06, 0x11, 0xff, 0xff},
	{0x00, 0xff, 0xff, 0xff}
};

static const u8 edp_hbr2_voltage_swing[4][4] = {
	{0x0b, 0x11, 0x17, 0x1c}, /* sw0, 0.4v  */
	{0x10, 0x19, 0x1f, 0xFF}, /* sw1, 0.6 v */
	{0x19, 0x1F, 0xFF, 0xFF}, /* sw1, 0.8 v */
	{0x1f, 0xFF, 0xFF, 0xFF}  /* sw1, 1.2 v, optional */
};

static const u8 edp_hbr2_pre_emphasis[4][4] = {
	{0x0c, 0x15, 0x19, 0x1e}, /* pe0, 0 db */
	{0x0B, 0x15, 0x19, 0xFF}, /* pe1, 3.5 db */
	{0x0e, 0x14, 0xFF, 0xFF}, /* pe2, 6.0 db */
	{0x0d, 0xFF, 0xFF, 0xFF}  /* pe3, 9.5 db */
};

static void edp_phy_vm_pe_init(uint32_t link_rate, uint32_t lane_count)
{
	if (link_rate >= 324000) {
		printk(BIOS_DEBUG, "%s: link rate %u\n", __func__, link_rate);
		write32(&edp_phy_lane_tx0->tx_drv_lvl, edp_hbr2_voltage_swing[0][0]);
		write32(&edp_phy_lane_tx0->tx_emp_post1_lvl, edp_hbr2_pre_emphasis[0][0]);
		write32(&edp_phy_lane_tx1->tx_drv_lvl, edp_hbr2_voltage_swing[0][0]);
		write32(&edp_phy_lane_tx1->tx_emp_post1_lvl, edp_hbr2_pre_emphasis[0][0]);
	} else {
		printk(BIOS_DEBUG, "%s: link rate %u\n", __func__, link_rate);
		write32(&edp_phy_lane_tx0->tx_drv_lvl, edp_rbr_voltage_swing[0][0]);
		write32(&edp_phy_lane_tx0->tx_emp_post1_lvl, edp_rbr_pre_emphasis[0][0]);
		write32(&edp_phy_lane_tx1->tx_drv_lvl, edp_rbr_voltage_swing[0][0]);
		write32(&edp_phy_lane_tx1->tx_emp_post1_lvl, edp_rbr_pre_emphasis[0][0]);
	}

	u32 bias_en0, bias_en1, drvr_en0, drvr_en1, phy_cfg_1;

	if (lane_count == 1) {
		printk(BIOS_DEBUG, "%s: lanes %u\n", __func__, lane_count);
		bias_en0 = 0x01;
		bias_en1 = 0x00;
		drvr_en0 = 0x06;
		drvr_en1 = 0x07;
		phy_cfg_1 = 0x01;
	} else if (lane_count == 2) {
		printk(BIOS_DEBUG, "%s: lanes %u\n", __func__, lane_count);
		bias_en0 = 0x03;
		bias_en1 = 0x00;
		drvr_en0 = 0x04;
		drvr_en1 = 0x07;
		phy_cfg_1 = 0x03;
	} else {
		printk(BIOS_DEBUG, "%s: lanes %u\n", __func__, lane_count);
		bias_en0 = 0x03;
		bias_en1 = 0x03;
		drvr_en0 = 0x04;
		drvr_en1 = 0x04;
		phy_cfg_1 = 0x0f;
	}

	write32(&edp_phy_lane_tx0->tx_highz_drvr_en, drvr_en0);
	write32(&edp_phy_lane_tx0->tx_transceiver_bias_en, bias_en0);
	write32(&edp_phy_lane_tx1->tx_highz_drvr_en, drvr_en1);
	write32(&edp_phy_lane_tx1->tx_transceiver_bias_en, bias_en1);
	write32(&edp_phy->cfg1, phy_cfg_1);

	printk(BIOS_DEBUG, "%s: Ends\n", __func__);
}

void edp_phy_config(u8 v_level, u8 p_level, uint32_t link_rate)
{
	if (link_rate >= 324000) {
		printk(BIOS_DEBUG, "%s: link rate %u\n", __func__, link_rate);
		write32(&edp_phy_lane_tx0->tx_drv_lvl, edp_hbr2_voltage_swing[v_level][p_level]);
		write32(&edp_phy_lane_tx0->tx_emp_post1_lvl, edp_hbr2_pre_emphasis[v_level][p_level]);
		write32(&edp_phy_lane_tx1->tx_drv_lvl, edp_hbr2_voltage_swing[v_level][p_level]);
		write32(&edp_phy_lane_tx1->tx_emp_post1_lvl, edp_hbr2_pre_emphasis[v_level][p_level]);
	} else {
		printk(BIOS_DEBUG, "%s: link rate %u\n", __func__, link_rate);
		write32(&edp_phy_lane_tx0->tx_drv_lvl, edp_rbr_voltage_swing[v_level][p_level]);
		write32(&edp_phy_lane_tx0->tx_emp_post1_lvl, edp_rbr_pre_emphasis[v_level][p_level]);
		write32(&edp_phy_lane_tx1->tx_drv_lvl, edp_rbr_voltage_swing[v_level][p_level]);
		write32(&edp_phy_lane_tx1->tx_emp_post1_lvl, edp_rbr_pre_emphasis[v_level][p_level]);
	}
}

static void edp_phy_pll_vco_init(uint32_t link_rate)
{
	edp_phy_ssc_en(false);

	write32(&edp_phy_pll->qserdes_com_svs_mode_clk_sel, 0x01);
	write32(&edp_phy_pll->qserdes_com_sysclk_en_sel, 0x0b);
	write32(&edp_phy_pll->qserdes_com_sys_clk_ctrl, 0x02);
	write32(&edp_phy_pll->qserdes_com_clk_enable1, 0x0c);
	write32(&edp_phy_pll->qserdes_com_sysclk_buf_enable, 0x06);
	write32(&edp_phy_pll->qserdes_com_clk_sel, 0x30);
	write32(&edp_phy_pll->qserdes_com_pll_ivco, 0x07);
	write32(&edp_phy_pll->qserdes_com_lock_cmp_en, 0x08);
	write32(&edp_phy_pll->qserdes_com_pll_cctrl_mode0, 0x36);
	write32(&edp_phy_pll->qserdes_com_pll_rctrl_mode0, 0x16);
	write32(&edp_phy_pll->qserdes_com_cp_ctrl_mode0, 0x06);
	write32(&edp_phy_pll->qserdes_com_div_frac_start1_mode0, 0x00);
	write32(&edp_phy_pll->qserdes_com_cmn_config, 0x12);
	write32(&edp_phy_pll->qserdes_com_integloop_gain0_mode0, 0x3f);
	write32(&edp_phy_pll->qserdes_com_integloop_gain1_mode0, 0x00);
	write32(&edp_phy_pll->qserdes_com_vco_tune_map, 0x00);
	write32(&edp_phy_pll->qserdes_com_bg_timer, 0x0a);
	write32(&edp_phy_pll->qserdes_com_coreclk_div_mode0, 0x14);
	write32(&edp_phy_pll->qserdes_com_vco_tune_ctrl, 0x00);
	write32(&edp_phy_pll->qserdes_com_bias_en_clkbuflr_en, 0x17);
	write32(&edp_phy_pll->qserdes_com_core_clk_en, 0x0f);

	if (link_rate == 810000) {
		printk(BIOS_DEBUG, "%s: link rate %u\n", __func__, link_rate);
		write32(&edp_phy_pll->qserdes_com_hsclk_sel, 0x00);
		write32(&edp_phy_pll->qserdes_com_dec_start_mode0, 0x34);
		write32(&edp_phy_pll->qserdes_com_div_frac_start1_mode0, 0x00);
		write32(&edp_phy_pll->qserdes_com_div_frac_start2_mode0, 0xC0);
		write32(&edp_phy_pll->qserdes_com_div_frac_start3_mode0, 0x0B);
		write32(&edp_phy_pll->qserdes_com_lock_cmp1_mode0, 0x17);
		write32(&edp_phy_pll->qserdes_com_lock_cmp2_mode0, 0x15);

		write32(&edp_phy_pll->qserdes_com_bin_vcocal_cmp_code1_mode0, 0x71);
		write32(&edp_phy_pll->qserdes_com_bin_vcocal_cmp_code2_mode0, 0x0C);

	} else if (link_rate == 540000) {
		printk(BIOS_DEBUG, "%s: link rate %u\n", __func__, link_rate);
		write32(&edp_phy_pll->qserdes_com_hsclk_sel, 0x01);
		write32(&edp_phy_pll->qserdes_com_dec_start_mode0, 0x46);
		write32(&edp_phy_pll->qserdes_com_div_frac_start1_mode0, 0x00);
		write32(&edp_phy_pll->qserdes_com_div_frac_start2_mode0, 0x00);
		write32(&edp_phy_pll->qserdes_com_div_frac_start3_mode0, 0x05);
		write32(&edp_phy_pll->qserdes_com_lock_cmp1_mode0, 0x0F);
		write32(&edp_phy_pll->qserdes_com_lock_cmp2_mode0, 0x0E);

		write32(&edp_phy_pll->qserdes_com_bin_vcocal_cmp_code1_mode0, 0x97);
		write32(&edp_phy_pll->qserdes_com_bin_vcocal_cmp_code2_mode0, 0x10);
	} else if (link_rate == 270000) {
		printk(BIOS_DEBUG, "%s: link rate %u\n", __func__, link_rate);
		write32(&edp_phy_pll->qserdes_com_hsclk_sel, 0x03);
		write32(&edp_phy_pll->qserdes_com_dec_start_mode0, 0x34);
		write32(&edp_phy_pll->qserdes_com_div_frac_start1_mode0, 0x00);
		write32(&edp_phy_pll->qserdes_com_div_frac_start2_mode0, 0xC0);
		write32(&edp_phy_pll->qserdes_com_div_frac_start3_mode0, 0x0B);
		write32(&edp_phy_pll->qserdes_com_lock_cmp1_mode0, 0x07);
		write32(&edp_phy_pll->qserdes_com_lock_cmp2_mode0, 0x07);

		write32(&edp_phy_pll->qserdes_com_bin_vcocal_cmp_code1_mode0, 0x71);
		write32(&edp_phy_pll->qserdes_com_bin_vcocal_cmp_code2_mode0, 0x0C);
	} else if (link_rate == 162000) {
		printk(BIOS_DEBUG, "%s: link rate %u\n", __func__, link_rate);
		write32(&edp_phy_pll->qserdes_com_hsclk_sel, 0x05);
		write32(&edp_phy_pll->qserdes_com_dec_start_mode0, 0x34);
		write32(&edp_phy_pll->qserdes_com_div_frac_start1_mode0, 0x00);
		write32(&edp_phy_pll->qserdes_com_div_frac_start2_mode0, 0xC0);
		write32(&edp_phy_pll->qserdes_com_div_frac_start3_mode0, 0x0B);
		write32(&edp_phy_pll->qserdes_com_lock_cmp1_mode0, 0x37);
		write32(&edp_phy_pll->qserdes_com_lock_cmp2_mode0, 0x04);

		write32(&edp_phy_pll->qserdes_com_bin_vcocal_cmp_code1_mode0, 0x71);
		write32(&edp_phy_pll->qserdes_com_bin_vcocal_cmp_code2_mode0, 0x0C);
	}
}

static void edp_phy_lanes_init(void)
{
	write32(&edp_phy_lane_tx0->tx_clk_buf_enable, 0x0f);
	write32(&edp_phy_lane_tx0->tx_reset_tsync_en, 0x03);
	write32(&edp_phy_lane_tx0->tx_tran_drvr_emp_en, 0x01);
	write32(&edp_phy_lane_tx0->tx_tx_band, 0x4);

	write32(&edp_phy_lane_tx1->tx_clk_buf_enable, 0x0f);
	write32(&edp_phy_lane_tx1->tx_reset_tsync_en, 0x03);
	write32(&edp_phy_lane_tx1->tx_tran_drvr_emp_en, 0x01);
	write32(&edp_phy_lane_tx1->tx_tx_band, 0x4);
}

static void edp_lanes_configure(void)
{
	write32(&edp_phy_lane_tx0->tx_tx_pol_inv, 0x00);
	write32(&edp_phy_lane_tx1->tx_tx_pol_inv, 0x00);

	write32(&edp_phy_lane_tx0->tx_drv_lvl_offset, 0x10);
	write32(&edp_phy_lane_tx1->tx_drv_lvl_offset, 0x10);

	write32(&edp_phy_lane_tx0->tx_rescode_lane_offset_tx0, 0x11);
	write32(&edp_phy_lane_tx0->tx_rescode_lane_offset_tx1, 0x11);

	write32(&edp_phy_lane_tx1->tx_rescode_lane_offset_tx0, 0x11);
	write32(&edp_phy_lane_tx1->tx_rescode_lane_offset_tx1, 0x11);
}

static int edp_phy_pll_vco_configure(uint32_t link_rate, uint32_t lane_count)
{
	u32 phy_vco_div = 0;

	switch (link_rate) {
	case 162000:
		phy_vco_div = 1;
		break;
	case 216000:
	case 243000:
	case 270000:
		phy_vco_div = 1;
		break;
	case 324000:
	case 432000:
	case 540000:
		phy_vco_div = 2;
		break;
	case 594000:
		phy_vco_div = 2;
		break;
	case 810000:
		phy_vco_div = 0;
		break;
	default:
		printk(BIOS_ERR, "%s: Invalid link rate. rate = %u\n", __func__, link_rate);
		break;
	}

	write32(&edp_phy->vco_div, phy_vco_div);
	write32(&edp_phy->cfg, 0x01);
	write32(&edp_phy->cfg, 0x05);
	write32(&edp_phy->cfg, 0x01);
	write32(&edp_phy->cfg, 0x09);
	write32(&edp_phy_pll->qserdes_com_resetsm_cntrl, 0x20);
	if (!wait_ms(500, read32(&edp_phy_pll->qserdes_com_c_ready_status) & BIT(0))) {
		printk(BIOS_ERR, "%s: PLL not locked. Status .., c_ready_status=0x%08x\n",
		       __func__, read32(&edp_phy_pll->qserdes_com_c_ready_status));
		return -1;
	}

	write32(&edp_phy->cfg, 0x19);
	edp_lanes_configure();
	edp_phy_vm_pe_init(link_rate, lane_count);
	if (!wait_us(10000, read32(&edp_phy->status) & BIT(1))) {
		printk(BIOS_ERR, "%s: PHY not ready. Status\n", __func__);
		return -1;
	}

	write32(&edp_phy->cfg, 0x19);
	write32(&edp_phy->cfg, 0x18);

	write32(&edp_phy->cfg, 0x18);
	write32(&edp_phy->cfg, 0x19);
	if (!wait_us(50000, read32(&edp_phy_pll->qserdes_com_c_ready_status) & BIT(0))) {
		printk(BIOS_ERR, "%s: PLL not locked. Status B\n", __func__);
		return -1;
	}

	return 0;
}

int edp_phy_power_on(uint32_t link_rate, uint32_t lane_count)
{
	int ret = 0;
	edp_phy_pll_vco_init(link_rate);

	write32(&edp_phy->tx0_tx1_lane_ctl, 0x5);
	write32(&edp_phy->tx2_tx3_lane_ctl, 0x5);
	edp_phy_lanes_init();
	ret = edp_phy_pll_vco_configure(link_rate, lane_count);

	return ret;
}
