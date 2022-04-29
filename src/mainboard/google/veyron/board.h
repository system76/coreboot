/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GOOGLE_VEYRON_BOARD_H
#define __MAINBOARD_GOOGLE_VEYRON_BOARD_H

#include <gpio.h>

#define GPIO_BACKLIGHT	GPIO(7, A, 2)
#define GPIO_RESET	GPIO(0, B, 5)

void setup_chromeos_gpios(void);

#endif	/* __MAINBOARD_GOOGLE_VEYRON_BOARD_H */
