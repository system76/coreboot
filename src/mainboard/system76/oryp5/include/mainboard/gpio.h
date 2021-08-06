/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_GPIO_H
#define MAINBOARD_GPIO_H

#define DGPU_RST_N GPP_F22
#define DGPU_PWR_EN GPP_F23
#define DGPU_GC6 GPP_C12

#ifndef __ACPI__

void mainboard_configure_early_gpios(void);
void mainboard_configure_gpios(void);

#endif

#endif
