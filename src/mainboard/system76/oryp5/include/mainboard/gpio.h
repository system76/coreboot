/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_GPIO_H
#define MAINBOARD_GPIO_H

#include <soc/gpio.h>

#define DGPU_RST_N	GPP_F22
#define DGPU_PWR_EN	GPP_F23
#define DGPU_GC6	GPP_C12
#define DGPU_SSID	0x95e61558

#ifndef __ACPI__
void mainboard_configure_early_gpios(void);
void mainboard_configure_gpios(void);
#endif

#endif
