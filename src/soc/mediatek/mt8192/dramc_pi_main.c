/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/dramc_pi_api.h>
#include <soc/dramc_register.h>
#include <soc/pll.h>
#include <soc/pll_common.h>
#include <soc/regulator.h>

static void set_vcore_voltage(const struct ddr_cali *cali)
{
	u32 vcore = get_vcore_value(cali);

	dramc_info("Set DRAM vcore voltage to %u\n", vcore);
	mainboard_set_regulator_vol(MTK_REGULATOR_VCORE, vcore);
}

static void dramc_calibration_all_channels(struct ddr_cali *cali)
{
}

static void mem_pll_init(void)
{
	SET32_BITFIELDS(&mtk_apmixed->mpll_con3, PLL_POWER_ISO_ENABLE, 3);

	udelay(30);
	SET32_BITFIELDS(&mtk_apmixed->mpll_con3, PLL_ISO_ENABLE, 0);

	udelay(1);
	SET32_BITFIELDS(&mtk_apmixed->mpll_con1, PLL_CON1, MPLL_CON1_FREQ);
	SET32_BITFIELDS(&mtk_apmixed->mpll_con0, PLL_ENABLE, 1);

	udelay(20);
	SET32_BITFIELDS(&mtk_apmixed->pllon_con0, MPLL_IOS_SEL, 0);
	SET32_BITFIELDS(&mtk_apmixed->pllon_con0, MPLL_EN_SEL, 0);
	SET32_BITFIELDS(&mtk_apmixed->pllon_con1, MPLL_PWR_SEL, 0);
	SET32_BITFIELDS(&mtk_apmixed->pllon_con2, MPLL_BY_ISO_DLY, 0);
	SET32_BITFIELDS(&mtk_apmixed->pllon_con3, MPLL_BY_PWR_DLY, 0);
}

void init_dram(const struct dramc_data *dparam)
{
	u32 bc_bak;
	u8 k_shuffle, k_shuffle_end;
	u8 pll_mode = 0;
	bool first_freq_k = true;

	struct ddr_cali cali = {0};
	struct mr_values mr_value;
	const struct ddr_base_info *ddr_info = &dparam->ddr_info;

	cali.pll_mode = &pll_mode;
	cali.mr_value = &mr_value;
	cali.support_ranks = ddr_info->support_ranks;
	cali.cbt_mode[RANK_0] = ddr_info->cbt_mode[RANK_0];
	cali.cbt_mode[RANK_1] = ddr_info->cbt_mode[RANK_1];
	cali.emi_config = &ddr_info->emi_config;

	dramc_set_broadcast(DRAMC_BROADCAST_ON);
	mem_pll_init();

	global_option_init(&cali);
	bc_bak = dramc_get_broadcast();
	dramc_set_broadcast(DRAMC_BROADCAST_OFF);
	emi_mdl_init(cali.emi_config);
	dramc_set_broadcast(bc_bak);

	dramc_sw_impedance_cal(ODT_OFF, &cali.impedance);
	dramc_sw_impedance_cal(ODT_ON, &cali.impedance);

	if (ddr_info->config_dvfs == DRAMC_ENABLE_DVFS)
		k_shuffle_end = CALI_SEQ_MAX;
	else
		k_shuffle_end = CALI_SEQ1;

	for (k_shuffle = CALI_SEQ0; k_shuffle < k_shuffle_end; k_shuffle++) {
		set_cali_datas(&cali, dparam, k_shuffle);
		set_vcore_voltage(&cali);
		dfs_init_for_calibration(&cali);

		if (first_freq_k)
			emi_init2();

		dramc_calibration_all_channels(&cali);

		first_freq_k = false;
	}
}
