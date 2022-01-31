/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8186 Functional Specification
 * Chapter number: 5.13
 */

#ifndef SOC_MEDIATEK_MT8186_RTC_H
#define SOC_MEDIATEK_MT8186_RTC_H

#include <soc/pmic_wrap_common.h>
#include <soc/rtc_common.h>
#include <stdbool.h>

/* RTC registers */
enum {
	RTC_BBPU	= 0x0588,
	RTC_IRQ_STA	= 0x058A,
	RTC_IRQ_EN	= 0x058C,
	RTC_CII_EN	= 0x058E,
};

enum {
	RTC_TC_SEC	= 0x0592,
	RTC_TC_MIN	= 0x0594,
	RTC_TC_HOU	= 0x0596,
	RTC_TC_DOM	= 0x0598,
	RTC_TC_DOW	= 0x059A,
	RTC_TC_MTH	= 0x059C,
	RTC_TC_YEA	= 0x059E,
};

enum {
	RTC_AL_SEC	= 0x05A0,
	RTC_AL_MIN	= 0x05A2,
	RTC_AL_HOU	= 0x05A4,
	RTC_AL_DOM	= 0x05A6,
	RTC_AL_DOW	= 0x05A8,
	RTC_AL_MTH	= 0x05AA,
	RTC_AL_YEA	= 0x05AC,
	RTC_AL_MASK	= 0x0590,
};

enum {
	RTC_OSC32CON	= 0x05AE,
	RTC_CON		= 0x05C4,
	RTC_WRTGR	= 0x05C2,
};

enum {
	RTC_POWERKEY1	= 0x05B0,
	RTC_POWERKEY2	= 0x05B2,
};

enum {
	RTC_PDN1	= 0x05B4,
	RTC_PDN2	= 0x05B6,
	RTC_SPAR0	= 0x05B8,
	RTC_SPAR1	= 0x05BA,
	RTC_PROT	= 0x05BC,
	RTC_DIFF	= 0x05BE,
	RTC_CALI	= 0x05C0,
};

enum {
	RTC_BBPU_PWREN		= 1U << 0,
	RTC_BBPU_CLR		= 1U << 1,
	RTC_BBPU_INIT		= 1U << 2,
	RTC_BBPU_AUTO		= 1U << 3,
	RTC_BBPU_CLRPKY		= 1U << 4,
	RTC_BBPU_RELOAD		= 1U << 5,
	RTC_BBPU_CBUSY		= 1U << 6,

	RTC_CBUSY_TIMEOUT_US	= 8000,
};

enum {
	RTC_CON_VBAT_LPSTA_RAW	= 1U << 0,
	RTC_CON_EOSC32_LPEN	= 1U << 1,
	RTC_CON_XOSC32_LPEN	= 1U << 2,
	RTC_CON_LPRST		= 1U << 3,
	RTC_CON_CDBO		= 1U << 4,
	RTC_CON_F32KOB		= 1U << 5,
	RTC_CON_GPO		= 1U << 6,
	RTC_CON_GOE		= 1U << 7,
	RTC_CON_GSR		= 1U << 8,
	RTC_CON_GSMT		= 1U << 9,
	RTC_CON_GPEN		= 1U << 10,
	RTC_CON_GPU		= 1U << 11,
	RTC_CON_GE4		= 1U << 12,
	RTC_CON_GE8		= 1U << 13,
	RTC_CON_GPI		= 1U << 14,
	RTC_CON_LPSTA_RAW	= 1U << 15,
};

enum {
	RTC_XOSCCALI_MASK	= 0x1F << 0,
	RTC_XOSC32_ENB		= 1U << 5,
	RTC_EMB_HW_MODE		= 0U << 6,
	RTC_EMB_K_EOSC32_MODE	= 1U << 6,
	RTC_EMB_SW_DCXO_MODE	= 2U << 6,
	RTC_EMB_SW_EOSC32_MODE	= 3U << 6,
	RTC_EMBCK_SEL_MODE_MASK	= 3U << 6,
	RTC_EMBCK_SRC_SEL	= 1U << 8,
	RTC_EMBCK_SEL_OPTION	= 1U << 9,
	RTC_GPS_CKOUT_EN	= 1U << 10,
	RTC_REG_XOSC32_ENB	= 1U << 15,
};

enum {
	RTC_LPD_OPT_XOSC_AND_EOSC_LPD	= 0U << 13,
	RTC_LPD_OPT_EOSC_LPD		= 1U << 13,
	RTC_LPD_OPT_XOSC_LPD		= 2U << 13,
	RTC_LPD_OPT_F32K_CK_ALIVE	= 3U << 13,
	RTC_LPD_OPT_MASK		= 3U << 13,
};

/* PMIC TOP Register Definition */
enum {
	PMIC_RG_SCK_TOP_CON0	= 0x050C,
};

/* PMIC TOP Register Definition */
enum {
	PMIC_RG_TOP_CKPDN_CON0		= 0x010C,
	PMIC_RG_TOP_CKPDN_CON0_SET	= 0x010E,
	PMIC_RG_TOP_CKPDN_CON0_CLR	= 0x0110,
	PMIC_RG_TOP_CKPDN_CON1		= 0x0112,
	PMIC_RG_TOP_CKPDN_CON1_SET	= 0x0114,
	PMIC_RG_TOP_CKPDN_CON1_CLR	= 0x0116,
	PMIC_RG_TOP_CKSEL_CON0		= 0x0118,
	PMIC_RG_TOP_CKSEL_CON0_SET	= 0x011A,
	PMIC_RG_TOP_CKSEL_CON0_CLR	= 0x011C,
};

enum {
	PMIC_RG_FQMTR_32K_CK_PDN_SHIFT	= 10,
	PMIC_RG_FQMTR_CK_PDN_SHIFT	= 11,
};

/* PMIC DCXO Register Definition */
enum {
	PMIC_RG_DCXO_CW00	= 0x0788,
	PMIC_RG_DCXO_CW00_CLR	= 0x078C,
	PMIC_RG_DCXO_CW02	= 0x0790,
	PMIC_RG_DCXO_CW07	= 0x079A,
	PMIC_RG_DCXO_CW09	= 0x079E,
	PMIC_RG_DCXO_CW11	= 0x07A2,
	PMIC_RG_DCXO_CW13	= 0x07AA,
	PMIC_RG_DCXO_CW15	= 0x07AE,
	PMIC_RG_DCXO_CW16	= 0x07B0,
	PMIC_RG_DCXO_CW21	= 0x07BA,
	PMIC_RG_DCXO_CW23	= 0x07BE,
	PMIC_RG_DCXO_ELR0	= 0x07C4,
};

enum {
	PMIC_RG_TOP_TMA_KEY	= 0x03A8,
};

/* PMIC Frequency Meter Definition */
enum {
	PMIC_RG_FQMTR_CKSEL	= 0x0118,
	PMIC_RG_FQMTR_RST	= 0x013E,
	PMIC_RG_FQMTR_CON0	= 0x0514,
	PMIC_RG_FQMTR_WINSET	= 0x0516,
	PMIC_RG_FQMTR_DATA	= 0x0518,

	FQMTR_TIMEOUT_US	= 8000,
};

enum {
	PMIC_FQMTR_FIX_CLK_26M		= 0U << 0,
	PMIC_FQMTR_FIX_CLK_XOSC_32K_DET	= 1U << 0,
	PMIC_FQMTR_FIX_CLK_EOSC_32K	= 2U << 0,
	PMIC_FQMTR_FIX_CLK_RTC_32K	= 3U << 0,
	PMIC_FQMTR_FIX_CLK_SMPS_CK	= 4U << 0,
	PMIC_FQMTR_FIX_CLK_TCK_SEC	= 5U << 0,
	PMIC_FQMTR_FIX_CLK_PMU_75K	= 6U << 0,
	PMIC_FQMTR_CKSEL_MASK		= 7U << 0,
};

enum {
	PMIC_FQMTR_RST_SHIFT = 8
};

enum {
	PMIC_FQMTR_CON0_XOSC32_CK		= 0U << 0,
	PMIC_FQMTR_CON0_DCXO_F32K_CK		= 1U << 0,
	PMIC_FQMTR_CON0_EOSC32_CK		= 2U << 0,
	PMIC_FQMTR_CON0_XOSC32_CK_DETECTON	= 3U << 0,
	PMIC_FQMTR_CON0_FQM26M_CK		= 4U << 0,
	PMIC_FQMTR_CON0_FQM32k_CK		= 5U << 0,
	PMIC_FQMTR_CON0_TEST_CK			= 6U << 0,
	PMIC_FQMTR_CON0_TCKSEL_MASK		= 7U << 0,
	PMIC_FQMTR_CON0_BUSY			= 1U << 3,
	PMIC_FQMTR_CON0_DCXO26M_EN		= 1U << 4,
	PMIC_FQMTR_CON0_FQMTR_EN		= 1U << 15,
};

enum {
	RTC_FQMTR_LOW_BASE	= 794 - 2,
	RTC_FQMTR_HIGH_BASE	= 794 + 2,
};

enum {
	RTC_XOSCCALI_START	= 0x00,
	RTC_XOSCCALI_END	= 0x1f,
};

/* external API */
void rtc_bbpu_power_on(void);
int rtc_init(int recover);
bool rtc_gpio_init(void);
void rtc_boot(void);
u16 rtc_get_frequency_meter(u16 val, u16 measure_src, u16 window_size);

static inline s32 rtc_read(u16 addr, u16 *rdata)
{
	s32 ret;

	ret = pwrap_read(addr, rdata);
	if (ret)
		rtc_info("pwrap_read failed: ret=%d\n", ret);

	return ret;
}

static inline s32 rtc_write(u16 addr, u16 wdata)
{
	s32 ret;

	ret = pwrap_write(addr, wdata);
	if (ret)
		rtc_info("pwrap_write failed: ret=%d\n", ret);

	return ret;
}

#endif /* SOC_MEDIATEK_MT8186_RTC_H */
