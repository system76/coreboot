/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 realtek_alc255_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x1558f551),
	AZALIA_RESET(1),
	AZALIA_PIN_CFG(0, 0x12, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x14, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x17, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x18, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1d, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x21, AZALIA_PIN_CFG_NC(0)),
};

const u32 pc_beep_verbs[] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name = "Realtek ALC255",
		.vendor_id = 0x10ec0255,
		.subsystem_id = 0x1558f551,
		.address = 0,
		.verbs = realtek_alc255_verbs,
		.verb_count = ARRAY_SIZE(realtek_alc255_verbs),
	},
	{ /* terminator */ }
};

AZALIA_ARRAY_SIZES;
