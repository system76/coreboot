/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_METEORLAKE_GPIO_H_
#define _SOC_METEORLAKE_GPIO_H_

#if CONFIG(SOC_INTEL_ARROWLAKE_PCH_S)
#include <soc/gpio_defs_pch_s.h>
#define CROS_GPIO_NAME		"INTC1084"
#define CROS_GPIO_DEVICE_NAME	"INTC1084:00"
#else
#include <soc/gpio_defs.h>
#define CROS_GPIO_NAME		"INTC1083"
#define CROS_GPIO_DEVICE_NAME	"INTC1083:00"
#endif

#include <intelblocks/gpio.h>

/* Enable GPIO community power management configuration */
#define MISCCFG_GPIO_PM_CONFIG_BITS (MISCCFG_GPVNNREQEN | \
	MISCCFG_GPPGCBDPCGEN | MISCCFG_GPSIDEDPCGEN | \
	MISCCFG_GPRCOMPCDLCGEN | MISCCFG_GPRTCDLCGEN | MISCCFG_GSXSLCGEN \
	| MISCCFG_GPDPCGEN | MISCCFG_GPDLCGEN)

#endif
