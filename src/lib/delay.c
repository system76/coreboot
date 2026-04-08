/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <delay.h>
#include <limits.h>
#include <timer.h>

void mdelay(unsigned int msecs)
{
	assert(msecs <= UINT_MAX / USECS_PER_MSEC);
	return udelay(msecs * USECS_PER_MSEC);
}

void delay(unsigned int secs)
{
	assert(secs <= UINT_MAX / MSECS_PER_SEC);
	return mdelay(secs * MSECS_PER_SEC);
}
