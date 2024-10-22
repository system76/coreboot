/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* Realtek, ALC256 */
	0x10ec0256, /* Vendor ID */
	0x18490256, /* Subsystem ID */
	12, /* Number of entries */
	AZALIA_SUBVENDOR(0, 0x18490256),
	AZALIA_RESET(1),
	AZALIA_PIN_CFG(0, 0x12, 0x40000000),
	AZALIA_PIN_CFG(0, 0x13, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x14, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x18, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x19, 0x02a11020),
	AZALIA_PIN_CFG(0, 0x1a, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1b, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x40400001),
	AZALIA_PIN_CFG(0, 0x1e, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x21, 0x02211010),
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
