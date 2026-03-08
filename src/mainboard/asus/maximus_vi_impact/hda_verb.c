/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 realtek_alc900_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x10438581),
	AZALIA_PIN_CFG(0, 0x11, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x14, 0x01014010),
	AZALIA_PIN_CFG(0, 0x15, 0x4037c040),
	AZALIA_PIN_CFG(0, 0x16, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x17, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x18, 0x01a19040),
	AZALIA_PIN_CFG(0, 0x19, 0x02a19050),
	AZALIA_PIN_CFG(0, 0x1a, 0x0181304f),
	AZALIA_PIN_CFG(0, 0x1b, 0x02214020),
	AZALIA_PIN_CFG(0, 0x1e, 0x01456130),
};

const u32 pc_beep_verbs[0] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC900",
		.vendor_id    = 0x10ec0900,
		.subsystem_id = 0x10438581,
		.address      = 0,
		.verbs        = realtek_alc900_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc900_verbs),
	},
	{ /* terminator */ }
};

AZALIA_ARRAY_SIZES;
