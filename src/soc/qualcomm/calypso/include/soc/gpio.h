/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_QUALCOMM_CALYPSO_GPIO_H__
#define __SOC_QUALCOMM_CALYPSO_GPIO_H__

#include <types.h>
#include <soc/addressmap.h>
#include <soc/gpio_common.h>

#define PIN(index, func1, func2, func3, func4, func5, func6) \
	GPIO##index##_ADDR = (TLMM_TILE_BASE + ((index) * TLMM_GPIO_OFF_DELTA)), \
	GPIO##index##_FUNC_##func1 = (1), \
	GPIO##index##_FUNC_##func2 = (2), \
	GPIO##index##_FUNC_##func3 = (3), \
	GPIO##index##_FUNC_##func4 = (4), \
	GPIO##index##_FUNC_##func5 = (5), \
	GPIO##index##_FUNC_##func6 = (6)

/* TODO: update as per datasheet */
enum {
	PIN(0, QUP0_SE0_L0, RES_2, RES_3, RES_4, RES_5, RES_6),
};

#endif /* __SOC_QUALCOMM_CALYPSO_GPIO_H__ */
