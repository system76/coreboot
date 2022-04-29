/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/infracfg.h>
#include <soc/pll.h>
#include <soc/pll_common.h>
#include <soc/pmif.h>
#include <soc/pmif_clk_common.h>
#include <soc/pmif_sw.h>
#include <soc/pmif_spmi.h>
#include <soc/spm.h>

/* APMIXED, ULPOSC1_CON0 */
DEFINE_BITFIELD(OSC1_CALI, 6, 0)
DEFINE_BITFIELD(OSC1_IBAND, 13, 7)
DEFINE_BITFIELD(OSC1_FBAND, 17, 14)
DEFINE_BITFIELD(OSC1_DIV, 23, 18)
DEFINE_BIT(OSC1_CP_EN, 24)

/* APMIXED, ULPOSC1_CON1 */
DEFINE_BITFIELD(OSC1_32KCALI, 7, 0)
DEFINE_BITFIELD(OSC1_RSV1, 15, 8)
DEFINE_BITFIELD(OSC1_RSV2, 23, 16)
DEFINE_BITFIELD(OSC1_MOD, 25, 24)
DEFINE_BIT(OSC1_DIV2_EN, 26)

/* APMIXED, ULPOSC1_CON2 */
DEFINE_BITFIELD(OSC1_BIAS, 7, 0)

/* SPM, POWERON_CONFIG_EN */
DEFINE_BIT(BCLK_CG_EN, 0)
DEFINE_BITFIELD(PROJECT_CODE, 31, 16)

/* SPM, ULPOSC_CON */
DEFINE_BIT(ULPOSC_EN, 0)
DEFINE_BIT(ULPOSC_CG_EN, 2)

/* INFRA, MODULE_SW_CG */
DEFINE_BIT(PMIC_CG_TMR, 0)
DEFINE_BIT(PMIC_CG_AP, 1)
DEFINE_BIT(PMIC_CG_MD, 2)
DEFINE_BIT(PMIC_CG_CONN, 3)

/* INFRA, INFRA_GLOBALCON_RST2 */
DEFINE_BIT(PMIC_WRAP_SWRST, 0)
DEFINE_BIT(PMICSPMI_SWRST, 14)

/* INFRA, PMICW_CLOCK_CTRL */
DEFINE_BITFIELD(PMIC_SYSCK_26M_SEL, 3, 0)

/* TOPCKGEN, CLK_CFG_8 */
DEFINE_BITFIELD(CLK_PWRAP_ULPOSC_SET, 10, 8)
DEFINE_BIT(CLK_PWRAP_ULPOSC_INV, 12)
DEFINE_BIT(PDN_PWRAP_ULPOSC, 15)

/* TOPCKGEN, CLK_CFG_UPDATE1 */
DEFINE_BIT(CLK_CFG_UPDATE1, 2)

static void pmif_ulposc_config(void)
{
	/* ULPOSC1_CON0 */
	SET32_BITFIELDS(&mtk_apmixed->ulposc1_con0, OSC1_CP_EN, 0, OSC1_DIV, 0xe,
			OSC1_FBAND, 0x2, OSC1_IBAND, 0x52, OSC1_CALI, 0x40);

	/* ULPOSC1_CON1 */
	SET32_BITFIELDS(&mtk_apmixed->ulposc1_con1, OSC1_DIV2_EN, 0, OSC1_MOD, 0,
			OSC1_RSV2, 0, OSC1_RSV1, 0x29, OSC1_32KCALI, 0);

	/* ULPOSC1_CON2 */
	SET32_BITFIELDS(&mtk_apmixed->ulposc1_con2, OSC1_BIAS, 0x40);
}

u32 pmif_get_ulposc_freq_mhz(u32 cali_val)
{
	u32 result = 0;

	/* set calibration value */
	SET32_BITFIELDS(&mtk_apmixed->ulposc1_con0, OSC1_CALI, cali_val);
	udelay(50);
	result = mt_fmeter_get_freq_khz(FMETER_ABIST, FREQ_METER_ABIST_AD_OSC_CK);

	return result / 1000;
}

static int pmif_init_ulposc(void)
{
	/* calibrate ULPOSC1 */
	pmif_ulposc_config();

	/* enable spm swinf */
	if (!READ32_BITFIELD(&mtk_spm->poweron_config_set, BCLK_CG_EN))
		SET32_BITFIELDS(&mtk_spm->poweron_config_set, BCLK_CG_EN, 1,
				PROJECT_CODE, 0xb16);

	/* turn on ulposc */
	SET32_BITFIELDS(&mtk_spm->ulposc_con, ULPOSC_EN, 1);
	udelay(100);
	SET32_BITFIELDS(&mtk_spm->ulposc_con, ULPOSC_CG_EN, 1);

	return pmif_ulposc_cali(FREQ_260MHZ);
}

int pmif_clk_init(void)
{
	if (pmif_init_ulposc())
		return E_NODEV;

	/* turn off pmic_cg_tmr, cg_ap, cg_md, cg_conn clock */
	SET32_BITFIELDS(&mt8192_infracfg->module_sw_cg_0_set, PMIC_CG_TMR, 1, PMIC_CG_AP, 1,
			PMIC_CG_MD, 1, PMIC_CG_CONN, 1);

	SET32_BITFIELDS(&mtk_topckgen->clk_cfg_8, PDN_PWRAP_ULPOSC, 0, CLK_PWRAP_ULPOSC_INV,
			0, CLK_PWRAP_ULPOSC_SET, 0);
	SET32_BITFIELDS(&mtk_topckgen->clk_cfg_update1, CLK_CFG_UPDATE1, 1);

	/* use ULPOSC1 clock */
	SET32_BITFIELDS(&mt8192_infracfg->pmicw_clock_ctrl_clr, PMIC_SYSCK_26M_SEL, 0xf);

	/* toggle SPI/SPMI sw reset */
	SET32_BITFIELDS(&mt8192_infracfg->infra_globalcon_rst2_set, PMICSPMI_SWRST, 1,
			PMIC_WRAP_SWRST, 1);
	SET32_BITFIELDS(&mt8192_infracfg->infra_globalcon_rst2_clr, PMICSPMI_SWRST, 1,
			PMIC_WRAP_SWRST, 1);

	/* turn on pmic_cg_tmr, cg_ap, cg_md, cg_conn clock */
	SET32_BITFIELDS(&mt8192_infracfg->module_sw_cg_0_clr, PMIC_CG_TMR, 1, PMIC_CG_AP, 1,
			PMIC_CG_MD, 1, PMIC_CG_CONN, 1);

	return 0;
}
