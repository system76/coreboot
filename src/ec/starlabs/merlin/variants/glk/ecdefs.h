/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <stdint.h>

/*
 * EC communication interface for ITE Embedded Controller.
 */

#ifndef _EC_STARLABS_GLK_EC_DEFS_H
#define _EC_STARLABS_GLK_EC_DEFS_H

/* IT8987 chip ID byte values. */
#define ITE_CHIPID_VAL		0x8987

/* EC RAM offsets. */
#define ECRAM_TRACKPAD_STATE	0x14
#define ECRAM_KBL_STATE		0x18
#define ECRAM_KBL_BRIGHTNESS	0x19
#define ECRAM_KBL_TIMEOUT	0x1a
#define ECRAM_FN_LOCK_STATE	0x2c
#define ECRAM_FN_CTRL_REVERSE	0x2d
#define ECRAM_MAX_CHARGE	0x46
#define ECRAM_FAN_MODE		dead_code_t(uint8_t)

#endif
