/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DRIVERS_GFX_NVIDIA_GPU_H_
#define _DRIVERS_GFX_NVIDIA_GPU_H_

#include <stdbool.h>

struct nvidia_gpu_config {
	/* GPIO for GPU_PWR_EN */
	unsigned int power_gpio;
	/* GPIO for GPU_RST# */
	unsigned int reset_gpio;
	/* Enable or disable GPU power */
	bool enable;
};

void nvidia_set_power(const struct nvidia_gpu_config *config);

#endif /* _DRIVERS_NVIDIA_GPU_H_ */
