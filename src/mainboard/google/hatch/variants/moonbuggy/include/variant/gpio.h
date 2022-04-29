/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VARIANT_GPIO_H
#define VARIANT_GPIO_H

#include <baseboard/gpio.h>

#undef EC_SYNC_IRQ
#define EC_SYNC_IRQ	GPP_D14_IRQ

#endif
