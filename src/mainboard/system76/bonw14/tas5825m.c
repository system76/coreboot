/* SPDX-License-Identifier: GPL-2.0-only */

#include <drivers/i2c/tas5825m/tas5825m.h>

#include "tas5825m-normal.c"
#include "tas5825m-sub.c"

int tas5825m_setup(struct device *dev, int id)
{
	if (id == 0)
		return tas5825m_setup_normal(dev);
	if (id == 1)
		return tas5825m_setup_sub(dev);
	return -1;
}
