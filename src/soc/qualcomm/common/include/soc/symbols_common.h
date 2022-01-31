/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_SYMBOLS_COMMON_H_
#define _SOC_QUALCOMM_SYMBOLS_COMMON_H_

#include <symbols.h>

DECLARE_REGION(ddr_training);
DECLARE_REGION(qclib_serial_log);
DECLARE_REGION(ddr_information);
DECLARE_REGION(ssram)
DECLARE_REGION(bsram)
DECLARE_REGION(dram_aop)
DECLARE_REGION(dram_soc)
DECLARE_REGION(dcb)
DECLARE_REGION(pmic)
DECLARE_REGION(limits_cfg)
DECLARE_REGION(aop)
DECLARE_REGION(modem_id)
DECLARE_REGION(aop_code_ram)
DECLARE_REGION(aop_data_ram)
DECLARE_REGION(dram_modem_wifi_only)
DECLARE_REGION(dram_modem_extra)
DECLARE_REGION(dram_wlan)
DECLARE_REGION(dram_wpss)
DECLARE_REGION(shrm)
DECLARE_REGION(dram_cpucp)
DECLARE_REGION(dram_modem)

#endif // _SOC_QUALCOMM_SYMBOLS_COMMON_H_
