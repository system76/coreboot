/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DRIVERS_SYSTEM76_DGPU_H_
#define _DRIVERS_SYSTEM76_DGPU_H_

#include <stdbool.h>

struct system76_dgpu_config {
	/* GPIO for GPU_PWR_EN */
	unsigned int enable_gpio;
	/* GPIO for GPU_RST# */
	unsigned int reset_gpio;
	/* Enable or disable GPU power */
	bool enable;
};

void dgpu_power_enable(const struct system76_dgpu_config *config);

#endif /* _DRIVERS_SYSTEM76_DGPU_H_ */
