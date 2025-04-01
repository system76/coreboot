/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VARIANT_GPIO_H
#define VARIANT_GPIO_H

#include <soc/gpio.h>

#define DGPU_RST_N	GPP_F22
#define DGPU_PWR_EN	GPP_F23
#define DGPU_GC6	GPP_C12
#define DGPU_SSID	0x65e11558

#ifndef __ACPI__
void variant_configure_early_gpios(void);
void variant_configure_gpios(void);
#endif

#endif
