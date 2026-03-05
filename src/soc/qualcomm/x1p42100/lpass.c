/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/adsp.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/cmd_db.h>
#include <soc/lpass.h>
#include <soc/rpmh_bcm.h>
#include <soc/rpmh_config.h>
#include <soc/rpmh_regulator.h>
#include <timer.h>

/* Debug configuration value (0 = no debug, non-zero = 0x20000000 = enable debug) */
u32 lpass_q6_debug_val = 0;

static struct clock_freq_config lpass_ddrss_shub_cfg[] = {
	{
		.hz = 400 * MHz,
		.src = SRC_GPLL0_MAIN_600MHZ,
		.div = QCOM_CLOCK_DIV(1.5),
	},
};

/* Configure LPASS DDRSS SHUB RCG (RCGR + M/N) to 400 MHz */
static void lpass_configure_ddrss_shub_rcg(void)
{
	if (clock_configure(&gcc->lpass_ddrss_shub_rcg,
			    lpass_ddrss_shub_cfg,
			    400 * MHz, ARRAY_SIZE(lpass_ddrss_shub_cfg)) != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to configure DDRSS SHUB RCG\n");
		return;
	}
}

static enum cb_err lpass_aon_cc_pll_enable(void)
{
	struct alpha_pll_reg_val_config pll_cfg = {0};

	pll_cfg.reg_mode = &lpass_aon_cc_pll->pll_mode;
	pll_cfg.reg_l = &lpass_aon_cc_pll->pll_l_val;
	pll_cfg.l_val = HAL_CLK_LPASS_AON_CC_PLL_L_VALUE |
		HAL_CLK_LPASS_AON_CC_PLL_PROC_CAL_L_VALUE |
			HAL_CLK_LPASS_AON_CC_PLL_RING_CAL_L_VALUE;

	pll_cfg.reg_alpha = &lpass_aon_cc_pll->pll_alpha_val;
	pll_cfg.alpha_val = HAL_CLK_LPASS_AON_CC_PLL_ALPHA_VALUE;

	pll_cfg.reg_cal_l = &lpass_aon_cc_pll->pll_l_val;

	pll_cfg.reg_config_ctl = &lpass_aon_cc_pll->pll_config_ctl;
	pll_cfg.config_ctl_val = HAL_CLK_LPASS_AON_CC_PLL_CONFIG_CTL;
	pll_cfg.reg_config_ctl_hi = &lpass_aon_cc_pll->pll_config_ctl_u;
	pll_cfg.config_ctl_hi_val = HAL_CLK_LPASS_AON_CC_PLL_CONFIG_CTL_U;
	pll_cfg.reg_config_ctl_hi1 = &lpass_aon_cc_pll->pll_config_ctl_u1;
	pll_cfg.config_ctl_hi1_val = HAL_CLK_LPASS_AON_CC_PLL_CONFIG_CTL_U1;

	pll_cfg.reg_user_ctl = &lpass_aon_cc_pll->pll_user_ctl;
	pll_cfg.user_ctl_val = HAL_CLK_LPASS_AON_CC_PLL_USER_CTL;
	pll_cfg.reg_user_ctl_hi = &lpass_aon_cc_pll->pll_user_ctl_u;
	pll_cfg.user_ctl_hi_val = HAL_CLK_LPASS_AON_CC_PLL_USER_CTL_U;

	pll_cfg.reg_opmode = &lpass_aon_cc_pll->pll_opmode;

	if (clock_configure_enable_gpll(&pll_cfg, false, 0) != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: AON CC PLL configuration failed\n");
		return CB_ERR;
	}

	write32(&lpass_aon_cc_pll->pll_test_ctl, HAL_CLK_LPASS_AON_CC_PLL_TEST_CTL);
	write32(&lpass_aon_cc_pll->pll_test_ctl_u, HAL_CLK_LPASS_AON_CC_PLL_TEST_CTL_U);
	write32(&lpass_aon_cc_pll->pll_test_ctl_u1, HAL_CLK_LPASS_AON_CC_PLL_TEST_CTL_U1);
	write32(&lpass_aon_cc_pll->pll_test_ctl_u2, HAL_CLK_LPASS_AON_CC_PLL_TEST_CTL_U2);

	setbits32(&lpass_aon_cc_pll->pll_user_ctl, BIT(AON_CC_PLL_ENABLE_VOTE_RUN));
	setbits32(&lpass_aon_cc_pll->pll_mode, BIT(PLL_RESET_SHFT));

	setbits32(&lpass_aon_cc_pll->pll_user_ctl, (BIT(AON_CC_PLL_PLLOUT_EVEN_SHFT_X1P42100) |
				BIT(AON_CC_PLL_PLLOUT_ODD_SHFT_X1P42100)));

	return CB_SUCCESS;
}

static enum cb_err lpass_qdsp6ss_pll_enable(void)
{
	struct alpha_pll_reg_val_config pll_cfg = {0};

	pll_cfg.reg_mode = &lpass_qdsp6ss_pll->pll_mode;
	pll_cfg.reg_l = &lpass_qdsp6ss_pll->pll_l_val;
	pll_cfg.l_val = HAL_CLK_LPASS_QDSP6SS_PLL_L_VALUE_SVS_L1;

	pll_cfg.reg_alpha = &lpass_qdsp6ss_pll->pll_alpha_val;
	pll_cfg.alpha_val = HAL_CLK_LPASS_QDSP6SS_PLL_ALPHA_VALUE_SVS_L1;

	pll_cfg.reg_cal_l = &lpass_qdsp6ss_pll->pll_l_val;
	pll_cfg.cal_l_val = HAL_CLK_LPASS_QDSP6SS_PLL_CAL_L_VALUE;

	pll_cfg.reg_config_ctl = &lpass_qdsp6ss_pll->pll_config_ctl;
	pll_cfg.config_ctl_val = HAL_CLK_LPASS_QDSP6SS_PLL_CONFIG_CTL;
	pll_cfg.reg_config_ctl_hi = &lpass_qdsp6ss_pll->pll_config_ctl_u;
	pll_cfg.config_ctl_hi_val = HAL_CLK_LPASS_QDSP6SS_PLL_CONFIG_CTL_U;
	pll_cfg.reg_config_ctl_hi1 = &lpass_qdsp6ss_pll->pll_config_ctl_u1;
	pll_cfg.config_ctl_hi1_val = HAL_CLK_LPASS_QDSP6SS_PLL_CONFIG_CTL_U1;

	pll_cfg.reg_user_ctl = &lpass_qdsp6ss_pll->pll_user_ctl;
	pll_cfg.user_ctl_val = HAL_CLK_LPASS_QDSP6SS_PLL_USER_CTL;
	pll_cfg.reg_user_ctl_hi = &lpass_qdsp6ss_pll->pll_user_ctl_u;
	pll_cfg.user_ctl_hi_val = HAL_CLK_LPASS_QDSP6SS_PLL_USER_CTL_U;

	pll_cfg.reg_opmode = &lpass_qdsp6ss_pll->pll_opmode;

	if (clock_configure_enable_gpll(&pll_cfg, false, 0) != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: QDSP6SS PLL configuration failed\n");
		return CB_ERR;
	}

	write32(&lpass_qdsp6ss_pll->pll_test_ctl, HAL_CLK_LPASS_QDSP6SS_PLL_TEST_CTL);
	write32(&lpass_qdsp6ss_pll->pll_test_ctl_u, HAL_CLK_LPASS_QDSP6SS_PLL_TEST_CTL_U);
	write32(&lpass_qdsp6ss_pll->pll_test_ctl_u1, HAL_CLK_LPASS_QDSP6SS_PLL_TEST_CTL_U1);
	write32(&lpass_qdsp6ss_pll->pll_test_ctl_u2, HAL_CLK_LPASS_QDSP6SS_PLL_TEST_CTL_U2);

	if (lucidole_pll_enable(&pll_cfg) != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: QDSP6SS PLL enable failed\n");
		return CB_ERR;
	}

	/* Enable PLLOUT_MAIN for QDSP6SS PLL */
	setbits32(&lpass_qdsp6ss_pll->pll_user_ctl, BIT(QDSP6SS_PLL_PLLOUT_MAIN_SHFT));

	/* Configure Q6 Core RCG */
	write32(&lpass_qdsp6ss_core_cc->core_cfg_rcgr, Q6_CORE_RCG_CFG);
	setbits32(&lpass_qdsp6ss_core_cc->core_cmd_rcgr, BIT(CLK_CTL_CMD_UPDATE_SHFT));

	/* Wait for RCG update */
	if (!wait_us(LPASS_RCG_UPDATE_TIMEOUT_US,
		     !(read32(&lpass_qdsp6ss_core_cc->core_cmd_rcgr) &
		       BIT(CLK_CTL_CMD_UPDATE_SHFT)))) {
		printk(BIOS_ERR, "LPASS: RCG update timeout\n");
		return CB_ERR;
	}

	/* Enable QDSP6 core clock */
	setbits32(&lpass_qdsp6ss_core_cc->core_cbcr, BIT(CLK_CTL_EN_SHFT));

	return CB_SUCCESS;
}

static enum cb_err lpass_setup_core_infrastructure(void)
{

	if (clock_enable(&gcc->gcc_lpass_cfg_noc_sway_cbcr) != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to enable CFG NOC SWAY clock\n");
		return CB_ERR;
	}

	clock_configure_hw_ctl(&lpass_core_gdsc->lpass_top_cc_lpass_core_sway_ahb_ls_cbcr, true);

	write32(&lpass_aon_cc->lpass_hm_collapse_vote_for_q6, LPASS_CORE_HM_VOTE_POWER_ON);

	if (!wait_us(150000, !(read32(&lpass_core_gdsc->core_hm_gdscr) & GDSC_PWR_ON))) {
		printk(BIOS_ERR, "LPASS: Core HM GDSC PWR_ON timeout after vote\n");
		return CB_ERR;
	}

	if (!wait_us(1000000, read32(&lpass_core_gdsc->lpass_core_gds_hm_ready) & LPASS_CORE_HM_READY)) {
		printk(BIOS_ERR, "LPASS: Core HM ready timeout\n");
		return CB_ERR;
	}

	/* Enable retention */
	setbits32(&lpass_core_gdsc->core_hm_gdscr, GDSC_RETAIN_FF_ENABLE);

	/* Enable SYSNOC interface */
	if (clock_enable(&lpass_core_cc->lpass_lpass_core_cc_sysnoc_mport_core_cbcr) != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to enable SYSNOC interface\n");
		return CB_ERR;
	}

	return CB_SUCCESS;
}

static const struct lpass_cbcr_list lpass_rpmh_ignore_clks[] = {
	{ &gcc->gcc_ddrss_lpass_shub_cbcr },
	{ &gcc->gcc_tcu_lpass_audio_qtb_cbcr },
};

static const struct lpass_cbcr_list lpass_pmu_ignore_clks[] = {
	{ &gcc->gcc_lpass_hw_af_noc_ddrss_shub_cbcr },
	{ &gcc->gcc_lpass_aggre_noc_ddrss_shub_cbcr },
	{ &gcc->gcc_lpass_aggre_noc_mpu_client_ddrss_shub_cbcr },
};

/* Boot clocks (GCC CBCRs). */
static u32 *lpass_boot_cbcr[LPASS_BOOT_CLK_COUNT] = {
	[DDRSS_LPASS_SHUB_CBCR]                     = &gcc->gcc_ddrss_lpass_shub_cbcr,
	[LPASS_AGGRE_NOC_DDRSS_SHUB_CBCR]           = &gcc->gcc_lpass_aggre_noc_ddrss_shub_cbcr,
	[LPASS_AGGRE_NOC_MPU_CLIENT_DDRSS_SHUB_CBCR] = &gcc->gcc_lpass_aggre_noc_mpu_client_ddrss_shub_cbcr,
	[LPASS_AON_NOC_DDRSS_SHUB_CBCR]             = &gcc->gcc_lpass_aon_noc_ddrss_shub_cbcr,
	[LPASS_AUDIO_QTB_CBCR]                      = &gcc->gcc_lpass_audio_qtb_cbcr,
	[LPASS_HW_AF_NOC_DDRSS_SHUB_CBCR]           = &gcc->gcc_lpass_hw_af_noc_ddrss_shub_cbcr,
	[LPASS_PWRCTL_CBCR]                         = &gcc->gcc_lpass_pwrctl_cbcr,
	[LPASS_QDSS_TSCTR_CBCR]                     = &gcc->gcc_lpass_qdss_tsctr_cbcr,
	[LPASS_QOSGEN_EXTREF_CBCR]                  = &gcc->gcc_lpass_qosgen_extref_cbcr,
	[LPASS_QTB_AHB_CBCR]                        = &gcc->gcc_lpass_qtb_ahb_cbcr,
	[LPASS_QTB_AT_CBCR]                         = &gcc->gcc_lpass_qtb_at_cbcr,
	[LPASS_XO_CBCR]                             = &gcc->gcc_lpass_xo_cbcr,
	[NOC_LPASS_DCD_XO_CBCR]                     = &gcc->gcc_noc_lpass_dcd_xo_cbcr,
	[QMIP_LPASS_QTB_AHB_CBCR]                   = &gcc->gcc_qmip_lpass_qtb_ahb_cbcr,
	[TCU_LPASS_AUDIO_QTB_CBCR]                  = &gcc->gcc_tcu_lpass_audio_qtb_cbcr,
};

/* Array mapping enum clk_lpass to actual CBCR registers */
static u32 *lpass_cbcr[LPASS_CLK_COUNT] = {
	[LPASS_CODEC_MEM_CBCR] = &lpass_audio_cc->codec_mem_cbcr,
	[LPASS_CODEC_MEM0_CBCR] = &lpass_audio_cc->codec_mem0_cbcr,
	[LPASS_CODEC_MEM1_CBCR] = &lpass_audio_cc->codec_mem1_cbcr,
	[LPASS_CODEC_MEM2_CBCR] = &lpass_audio_cc->codec_mem2_cbcr,
	[LPASS_CODEC_MEM3_CBCR] = &lpass_audio_cc->codec_mem3_cbcr,
	[LPASS_EXT_MCLK0_CBCR] = &lpass_audio_cc->codec_ext_mclk0_cbcr,
	[LPASS_EXT_MCLK1_CBCR] = &lpass_audio_cc->codec_ext_mclk1_cbcr,
	[LPASS_TX_MCLK_CBCR] = &lpass_aon_cc->tx_mclk_cbcr,
	[LPASS_TX_MCLK_2X_WSA_CBCR] = &lpass_audio_cc->tx_mclk_2x_wsa_cbcr,
	[LPASS_TX_MCLK_WSA_CBCR] = &lpass_audio_cc->tx_mclk_wsa_cbcr,
	[LPASS_WSA_MCLK_2X_CBCR] = &lpass_audio_cc->wsa_mclk_2x_cbcr,
	[LPASS_WSA_MCLK_CBCR] = &lpass_audio_cc->wsa_mclk_cbcr,
	[LPASS_TX_MCLK_2X_WSA2_CBCR] = &lpass_audio_cc->tx_mclk_2x_wsa2_cbcr,
	[LPASS_TX_MCLK_WSA2_CBCR] = &lpass_audio_cc->tx_mclk_wsa2_cbcr,
	[LPASS_WSA2_MCLK_2X_CBCR] = &lpass_audio_cc->wsa2_mclk_2x_cbcr,
	[LPASS_WSA2_MCLK_CBCR] = &lpass_audio_cc->wsa2_mclk_cbcr,
	[LPASS_VA_MEM0_CBCR] = &lpass_aon_cc->va_mem0_cbcr,
	[LPASS_VA_CBCR] = &lpass_aon_cc->va_cbcr,
	[LPASS_VA_2X_CBCR] = &lpass_aon_cc->va_2x_cbcr,
};

static enum cb_err lpass_audio_clocks_enable(void)
{
	for (size_t i = 0; i < LPASS_CLK_COUNT; i++) {
		if (clock_enable(lpass_cbcr[i]) != CB_SUCCESS) {
			printk(BIOS_ERR, "LPASS: Failed to enable audio clock\n");
			return CB_ERR;
		}
	}

	return CB_SUCCESS;
}

static enum cb_err lpass_rpmh_bcm_vote(void)
{
	struct rpmh_vreg arc_reg;
	enum cb_err ret;
	int rc;
	volatile u32 *boot_cookie = (volatile u32 *)AOP_BOOT_COOKIE_ADDR;

	if (!wait_us(AOP_BOOT_TIMEOUT_US, *boot_cookie == AOP_BOOT_COOKIE)) {
		printk(BIOS_ERR,
		       "AOP not booted after %dus (cookie: 0x%x, expected: 0x%x)\n",
		       AOP_BOOT_TIMEOUT_US, *boot_cookie, AOP_BOOT_COOKIE);
		return CB_ERR;
	}

	ret = cmd_db_ready();
	if (ret != CB_SUCCESS) {
		ret = cmd_db_init(CMD_DB_BASE_ADDR, CMD_DB_SIZE);
		if (ret != CB_SUCCESS) {
			printk(BIOS_ERR, "CMD_DB: init failed\n");
			return CB_ERR;
		}
	}

	rc = rpmh_rsc_init();
	if (rc) {
		printk(BIOS_ERR, "RPMH_RSC: init failed\n");
		return CB_ERR;
	}

	/* Initialize LCX ARC regulator for LPASS power rail */
	rc = rpmh_regulator_init(&arc_reg, "lcx.lvl", RPMH_REGULATOR_TYPE_ARC);
	if (rc) {
		printk(BIOS_ERR, "Failed to initialize LPASS power rail lcx ARC regulator\n");
		return CB_ERR;
	}

	/* Set LCX level to NOM_L1 */
	rc = rpmh_regulator_arc_set_level(&arc_reg, RPMH_REGULATOR_LEVEL_NOM_L1, true, false);
	if (rc) {
		printk(BIOS_ERR, "Failed to set LCX ARC level\n");
		return CB_ERR;
	}

	rc = rpmh_bcm_vote("LP0", BCM_LP0_VOTE_VALUE);
	if (rc) {
		printk(BIOS_ERR, "LPASS: Failed to send BCM vote for LPASS bus clock manager LP0\n");
		return CB_ERR;
	}

	return CB_SUCCESS;
}

static void lpass_apply_clock_workarounds(void)
{
	size_t i;

	/* CR QCTDD08205470: ignore RPMh clock disable for TCU-QTB link clocks. */
	for (i = 0; i < ARRAY_SIZE(lpass_rpmh_ignore_clks); i++)
		clock_configure_ignore_rpmh_clk_dis(lpass_rpmh_ignore_clks[i].cbcr, true);

	/* CR-3409636: keep AON NOC DDRSS SHUB clock enabled during sleep fencing. */
	clock_configure_ignore_rpmh_clk_dis(&gcc->gcc_lpass_aon_noc_ddrss_shub_cbcr, true);

	/* PMU-GCC spec: ignore PMU clock disable for these clocks. */
	for (i = 0; i < ARRAY_SIZE(lpass_pmu_ignore_clks); i++)
		clock_configure_ignore_pmu_clk_dis(lpass_pmu_ignore_clks[i].cbcr, true);

	/* Prevent automatic GDSC collapse (HW defaults enable it). */
	write32(&gcc->gcc_lpass_dsp_gdsc_sleep_ena_vote, 0x0);
}

static enum cb_err lpass_enable_boot_clocks(void)
{
	size_t i;
	enum cb_err ret;

	/* Configure LPASS DDRSS SHUB RCG */
	lpass_configure_ddrss_shub_rcg();

	/* Enable QTB power domain */
	if (enable_and_poll_gdsc_status(&gcc->gcc_lpass_qtb_gdscr) != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to enable QTB GDSC\n");
		return CB_ERR;
	}

	/* Ensure IGNORE_RPMH_CLK_DIS is set before enabling TCU-QTB link clock. */
	clock_configure_ignore_rpmh_clk_dis(&gcc->gcc_tcu_lpass_audio_qtb_cbcr, true);

	for (i = 0; i < LPASS_BOOT_CLK_COUNT; i++) {
		ret = clock_enable(lpass_boot_cbcr[i]);
		if (ret != CB_SUCCESS)
			printk(BIOS_WARNING, "LPASS: Failed to enable boot clock %zu: %p\n", i, lpass_boot_cbcr[i]);
	}

	return CB_SUCCESS;
}

static enum cb_err lpass_enable_ssc_smem_clock(void)
{
	if (clock_enable(&lpass_ssc_scc->ssc_scc_smem_cbcr) != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to enable SSC_SCC_SMEM clock\n");
		return CB_ERR;
	}
	return CB_SUCCESS;
}

static enum cb_err lpass_debug_bridge_setup(void)
{
	if (clock_enable(&lpass_aon_cc->at_cbcr) != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to enable AT clock\n");
		return CB_ERR;
	}

	if (clock_enable(&lpass_aon_cc->pclkdbg_cbcr) != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to enable PCLKDBG clock\n");
		return CB_ERR;
	}

	/* Allow debug bridge clocks/paths to settle before requesting handshake. */
	udelay(LPASS_NTS_HANDSHAKE_DELAY_US);

	/*
	 * In order to attach debugger T32 APB, DAP and NTS bridges need to be enabled
	 */
	setbits32(&lpass_mcc_lpass_reg->atb_low_power_handshake, LPASS_LP_HANDSHAKE_REQUEST);
	setbits32(&lpass_mcc_lpass_reg->apb_low_power_handshake, LPASS_LP_HANDSHAKE_REQUEST);
	setbits32(&lpass_mcc_lpass_reg->nts_low_power_handshake, LPASS_LP_HANDSHAKE_REQUEST);
	dsb();

	/*
	 * Wait for NTS ACK
	 */
	if (!wait_us(LPASS_NTS_HANDSHAKE_TIMEOUT_US,
		     read32(&lpass_mcc_lpass_reg->nts_low_power_handshake) & LPASS_LP_HANDSHAKE_ACK)) {
		printk(BIOS_WARNING, "LPASS: NTS handshake timeout (NTS=0x%x)\n",
		       read32(&lpass_mcc_lpass_reg->nts_low_power_handshake));
		/* Non-fatal: continue boot even if debug bridge handshake doesn't complete */
	}

	return CB_SUCCESS;
}

/* Enable TCM blocks for LPASS processor memory access. */
static void lpass_tcm_init(void)
{
	uint32_t block;

	if (clock_enable(&lpass_lpi_tcm->axis_hs_cbcr) != CB_SUCCESS) {
		printk(BIOS_WARNING, "LPASS: Failed to enable TCM AXIS HS clock\n");
		return;
	}

	/* Initialize all TCM 256KB blocks (n=0..9) */
	for (block = 0; block <= LPASS_TCM_MAX_BLOCKS; block++) {
		clock_configure_force_mem_core_on(&lpass_lpi_tcm->tcm_256kb_block_cbcr[block], true);

		if (clock_enable(&lpass_lpi_tcm->tcm_256kb_block_cbcr[block]) != CB_SUCCESS) {
			printk(BIOS_WARNING, "LPASS: Failed to enable TCM block %d clock\n", block);
			return;
		}
	}
}

static enum cb_err setup_lpass_processor(void)
{
	/* HW_CTL settings */
	clock_configure_hw_ctl(&gcc->gcc_ddrss_lpass_shub_cbcr, false);

	/* Enable boot clocks */
	if (lpass_enable_boot_clocks() != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to enable boot clocks\n");
		return CB_ERR;
	}

	/* Apply clock workarounds */
	lpass_apply_clock_workarounds();

	/* HPG Step 4: Enable LPASS access */
	if (clock_enable(&gcc->gcc_cfg_noc_lpass_cbcr) != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to enable CFG NOC access\n");
		return CB_ERR;
	}

	/* HPG Step 11: Program LPASS_QDSP6SS_RET_CFG = 0x0 (CR QCTDD10415825) */
	write32(&lpass_qdsp6ss->ret_cfg, 0);

	/* HPG Step 12: Check NanoRCG and enable LPI_NOC_HS conditionally */
	if ((read32(&lpass_lpicx_noc->sidebandmanager_rcg_sbm_flagoutstatus0_low) & LPASS_NANO_RCG_PORT2) == 0) {
		if (clock_enable(&lpass_aon_cc->lpi_noc_hs_cbcr) != CB_SUCCESS) {
			printk(BIOS_ERR, "LPASS: Failed to enable LPI NOC HS clock\n");
			return CB_ERR;
		}
	} else {
		if (clock_disable(&lpass_aon_cc->lpi_noc_hs_cbcr) != CB_SUCCESS) {
			printk(BIOS_ERR, "LPASS: Failed to disable LPI NOC HS clock\n");
			return CB_ERR;
		}
	}
	/* HPG Step 13: Enable Q6 AXIM clock after NanoRCG-based LPI_NOC_HS handling */
	if (clock_enable(&lpass_aon_cc->q6_axim_cbcr) != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to enable Q6 AXIM clock\n");
		return CB_ERR;
	}

	/* Configure debug register */
	write32(&lpass_qdsp6ss->dbg_cfg, lpass_q6_debug_val);

	write32(&lpass_mcc_lpass_reg->rsc_wait_event_ovrd_mask, LPASS_RSC_WAIT_EVENT_MASK);

	/* Initialize TCM */
	lpass_tcm_init();

	/* Enable Q6_AHBM clock for PRAM access */
	if (clock_enable(&lpass_aon_cc->q6_ahbm_cbcr) != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to enable Q6_AHBM clock\n");
		return CB_ERR;
	}

	/* Enable SSC_SCC_SMEM clock */
	if (lpass_enable_ssc_smem_clock() != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to enable SSC_SCC_SMEM clock\n");
		return CB_ERR;
	}

	/* Setup debug bridges if debug is enabled */
	if (lpass_q6_debug_val != 0) {
		if (lpass_debug_bridge_setup() != CB_SUCCESS)
			printk(BIOS_WARNING, "LPASS: Debug bridge setup failed\n");
	}
	/* Enable QDSP6SS PLL */
	if (lpass_qdsp6ss_pll_enable() != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to enable QDSP6SS PLL\n");
		return CB_ERR;
	}

	/* CR QCTDD10442361: mask RSCp trigger and disable CHIP_CX QCHANNEL timeout SBM. */
	/* HPG Step 24.e: Set CHIP_CX_RSCC_BYPASS_EN = 1 */
	setbits32(&lpass_aon_cc->lpass_noc_island_isolation, CHIP_CX_RSCC_BYPASS_EN);

	/* HPG Step 24.f: Set AON_NOC_STALL_MASK = 0 */
	clrbits32(&lpass_aon_cc->lpass_noc_island_isolation, AON_NOC_STALL_MASK);

	/* HPG Step 24.g: Set Port0 = 0 */
	clrbits32(&lpass_lpicx_noc->sidebandmanager_chipcx_sbm_faultinen0_low,
		  CHIPCX_SBM_FAULTINEN0_PORT0);

	clock_configure_hw_ctl(&gcc->gcc_lpass_aon_noc_ddrss_shub_cbcr, true);

	printk(BIOS_INFO, "LPASS: Processor setup complete\n");
	return CB_SUCCESS;
}

static enum cb_err enable_lpass_processor(void)
{
	/* HPG Step 14: Q6 boot sequence */
	/*
	 * De-assert QDSP6 stop core. QDSP6 will execute instructions after out of reset.
	 */
	setbits32(&lpass_qdsp6ss->boot_core_start, BOOT_CORE_START);

	/*
	 * Trigger boot FSM to start QDSP6 out of reset sequence.
	 */
	write32(&lpass_qdsp6ss->boot_cmd, BOOT_FSM_START);
	dsb();

	/* Wait for boot FSM completion */
	if (!wait_us(LPASS_BOOT_FSM_TIMEOUT_US, read32(&lpass_qdsp6ss->boot_status) != 0)) {
		printk(BIOS_ERR, "LPASS: Boot FSM timeout (boot_status=0x%x)\n",
		       read32(&lpass_qdsp6ss->boot_status));
		return CB_ERR;
	}

	/* Wait for core to come out of reset */
	udelay(5);

	/* HPG Step 19: Disable Q6_AXIM clock after Q6 boots */
	if (clock_disable(&lpass_aon_cc->q6_axim_cbcr) != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to disable Q6_AXIM clock\n");
		return CB_ERR;
	}

	printk(BIOS_INFO, "LPASS: Q6 processor enabled\n");
	return CB_SUCCESS;
}

enum cb_err lpass_bring_up(void)
{
	if (lpass_init() != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: lpass_init() failed\n");
		return CB_ERR;
	}

	if (setup_lpass_processor() != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to setup LPASS processor\n");
		return CB_ERR;
	}

	/* Disable BOOT IMEM */
	write32((void *)TCSR_BOOT_IMEM_DISABLE, 0x1);

	/* Boot the LPASS Q6 processor */
	if (enable_lpass_processor() != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to enable LPASS processor\n");
		return CB_ERR;
	}

	return CB_SUCCESS;
}

enum cb_err lpass_init(void)
{
	if (lpass_rpmh_bcm_vote() != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to initialize RPMH BCM vote\n");
		return CB_ERR;
	}

	if (enable_and_poll_gdsc_status(&lpass_aon_cc->lpass_audio_hm_gdscr) != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to enable Core HM GDSC\n");
		return CB_ERR;
	}

	if (lpass_setup_core_infrastructure() != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to setup core infrastructure\n");
		return CB_ERR;
	}

	if (lpass_aon_cc_pll_enable() != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to enable AON CC PLL\n");
		return CB_ERR;
	}

	if (lpass_audio_clocks_enable() != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to enable audio clocks\n");
		return CB_ERR;
	}

	printk(BIOS_INFO, "LPASS: BCM vote for LP0 and LPASS Init completed successfully\n");
	return CB_SUCCESS;
}
