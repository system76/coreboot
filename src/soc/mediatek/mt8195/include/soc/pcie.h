/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_MT8195_PCIE_H
#define SOC_MEDIATEK_MT8195_PCIE_H

#include <soc/pcie_common.h>

void mtk_pcie_reset(uintptr_t reg, bool enable);
void mtk_pcie_pre_init(void);

#endif
