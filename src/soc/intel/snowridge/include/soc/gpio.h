/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_SNOWRIDGE_GPIO_H_
#define _SOC_SNOWRIDGE_GPIO_H_

#include "gpio_defs.h"

#define GPIO_MAX_NUM_PER_GROUP 32

#define GPIO_WEST2_GPI_STATUS_REGS \
	(ALIGN_UP(GPIO_WEST2_PAD_NUM, GPIO_MAX_NUM_PER_GROUP) / GPIO_MAX_NUM_PER_GROUP)

#define GPIO_WEST3_GPI_STATUS_REGS \
	(ALIGN_UP(GPIO_WEST3_PAD_NUM, GPIO_MAX_NUM_PER_GROUP) / GPIO_MAX_NUM_PER_GROUP)

#define GPIO_WEST01_GPI_STATUS_REGS \
	(ALIGN_UP(GPIO_WEST01_PAD_NUM, GPIO_MAX_NUM_PER_GROUP) / GPIO_MAX_NUM_PER_GROUP)

#define GPIO_WEST5_GPI_STATUS_REGS \
	(ALIGN_UP(GPIO_WEST5_PAD_NUM, GPIO_MAX_NUM_PER_GROUP) / GPIO_MAX_NUM_PER_GROUP)

#define GPIO_WESTB_GPI_STATUS_REGS \
	(ALIGN_UP(GPIO_WESTB_PAD_NUM, GPIO_MAX_NUM_PER_GROUP) / GPIO_MAX_NUM_PER_GROUP)

#define GPIO_WESTD_PECI_GPI_STATUS_REGS \
	(ALIGN_UP(GPIO_WESTD_PECI_PAD_NUM, GPIO_MAX_NUM_PER_GROUP) / GPIO_MAX_NUM_PER_GROUP)

#define GPIO_EAST2_GPI_STATUS_REGS \
	(ALIGN_UP(GPIO_EAST2_PAD_NUM, GPIO_MAX_NUM_PER_GROUP) / GPIO_MAX_NUM_PER_GROUP)

#define GPIO_EAST3_GPI_STATUS_REGS \
	(ALIGN_UP(GPIO_EAST3_PAD_NUM, GPIO_MAX_NUM_PER_GROUP) / GPIO_MAX_NUM_PER_GROUP)

#define GPIO_EAST0_GPI_STATUS_REGS \
	(ALIGN_UP(GPIO_EAST0_PAD_NUM, GPIO_MAX_NUM_PER_GROUP) / GPIO_MAX_NUM_PER_GROUP)

#define GPIO_EMMC_GPI_STATUS_REGS \
	(ALIGN_UP(GPIO_EMMC_PAD_NUM, GPIO_MAX_NUM_PER_GROUP) / GPIO_MAX_NUM_PER_GROUP)

#define GPIO_MISCCFG          0x10 /**< Miscellaneous Configuration. */
#define GPIO_NUM_PAD_CFG_REGS 4    /**< DW0, DW1 and DW2 but 16-bytes aligned. */

#define NUM_GPI_STATUS_REGS                                         \
	(GPIO_WEST2_GPI_STATUS_REGS + GPIO_WEST3_GPI_STATUS_REGS +  \
	 GPIO_WEST01_GPI_STATUS_REGS + GPIO_WEST5_GPI_STATUS_REGS + \
	 GPIO_WESTB_GPI_STATUS_REGS + GPIO_WESTD_PECI_GPI_STATUS_REGS +  \
	 GPIO_EAST2_GPI_STATUS_REGS + GPIO_EAST3_GPI_STATUS_REGS +  \
	 GPIO_EAST0_GPI_STATUS_REGS + GPIO_EMMC_GPI_STATUS_REGS)

/**
 * @brief SNR doesn't support dynamic GPIO PM hence GPIO community MISCCFG register doesn't
 * have PM bits.
 */
#define MISCCFG_GPIO_PM_CONFIG_BITS 0

/**
 * @note Actually `intelblocks/gpio.h` has already included this file, but to support those
 * file that include this file directly, we need include `intelblocks/gpio.h` recursively.
 */
#include <intelblocks/gpio.h>

#endif // _SOC_SNOWRIDGE_GPIO_H_
