/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 realtek_alc1150_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x10438602),
	AZALIA_PIN_CFG(0, 0x11, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x14, 0x01014010),
	AZALIA_PIN_CFG(0, 0x15, 0x01011012),
	AZALIA_PIN_CFG(0, 0x16, 0x01016011),
	AZALIA_PIN_CFG(0, 0x17, 0x01012014),
	AZALIA_PIN_CFG(0, 0x18, 0x01a19050),
	AZALIA_PIN_CFG(0, 0x19, 0x02a19060),
	AZALIA_PIN_CFG(0, 0x1a, 0x0181305f),
	AZALIA_PIN_CFG(0, 0x1b, 0x0221401f),
	AZALIA_PIN_CFG(0, 0x1e, 0x01456140),
};

const u32 pc_beep_verbs[0] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC1150",
		.vendor_id    = 0x10ec0900,
		.subsystem_id = 0x10438602,
		.address      = 0,
		.verbs        = realtek_alc1150_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc1150_verbs),
	},
	{ /* terminator */ }
};

AZALIA_ARRAY_SIZES;
