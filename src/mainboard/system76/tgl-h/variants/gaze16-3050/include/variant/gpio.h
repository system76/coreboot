/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VARIANT_GPIO_H
#define VARIANT_GPIO_H

#include <soc/gpio.h>

#define DGPU_RST_N	GPP_F8
#define DGPU_PWR_EN	GPP_F9
#define DGPU_GC6	GPP_K11
#define DGPU_SSID	0x50151558

#ifndef __ACPI__
void variant_configure_early_gpios(void);
void variant_configure_gpios(void);
#endif

#endif
