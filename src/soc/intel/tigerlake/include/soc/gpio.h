/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_TIGERLAKE_GPIO_H_
#define _SOC_TIGERLAKE_GPIO_H_

#if CONFIG(SOC_INTEL_TIGERLAKE_PCH_H)
#include <soc/gpio_defs_pch_h.h>
#else
#include <soc/gpio_defs.h>
#define CROS_GPIO_DEVICE_NAME	"INT34C5:00"
#endif
#include <intelblocks/gpio.h>

#endif
