/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_GPIO_H
#define MAINBOARD_GPIO_H

#include <soc/gpio.h>

#define DGPU_RST_N	GPP_F8
#define DGPU_PWR_EN	GPP_F9
#define DGPU_GC6	GPP_K11

#ifndef __ACPI__
void mainboard_configure_early_gpios(void);
void mainboard_configure_gpios(void);
#endif

#endif
