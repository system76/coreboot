/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* Realtek, ALC255 */
	0x10ec0255, /* Vendor ID */
	0x15582560, /* Subsystem ID */
	12, /* Number of entries */
	AZALIA_SUBVENDOR(0, 0x15582560),
	AZALIA_RESET(1),
	AZALIA_PIN_CFG(0, 0x12, 0x90a60130),
	AZALIA_PIN_CFG(0, 0x14, 0x90170110),
	AZALIA_PIN_CFG(0, 0x17, 0x40000000),
	AZALIA_PIN_CFG(0, 0x18, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x19, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1a, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1b, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x41e79b45),
	AZALIA_PIN_CFG(0, 0x1e, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x21, 0x04211020),
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
