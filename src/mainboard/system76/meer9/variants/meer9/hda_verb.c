/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* Realtek, ALC256 */
	0x10ec0256, /* Vendor ID */
	0x18490256, /* Subsystem ID */
	13, /* Number of entries */
	AZALIA_SUBVENDOR(0, 0x18490256),
	AZALIA_RESET(1),
	AZALIA_PIN_CFG(0, 0x12, 0x40000000),		// DMIC
	AZALIA_PIN_CFG(0, 0x13, AZALIA_PIN_CFG_NC(0)),	// DMIC
	AZALIA_PIN_CFG(0, 0x14, AZALIA_PIN_CFG_NC(0)),	// Front (Port-D)
	AZALIA_PIN_CFG(0, 0x18, AZALIA_PIN_CFG_NC(0)),	// NPC
	AZALIA_PIN_CFG(0, 0x19, 0x02a11020),		// MIC2 (Port-F)
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_CFG_NC(0)),	// LINE1 (Port-C)
	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_CFG_NC(0)),	// LINE2 (Port-E)
	AZALIA_PIN_CFG(0, 0x1d, 0x40400001),		// BEEP-IN
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),	// S/PDIF-OUT
	AZALIA_PIN_CFG(0, 0x21, 0x02211010),		// HP1-OUT (Port-I)

	// Enable HP-JD
	0x0205001B, 0x02040A4B, 0x0205001B, 0x02040A4B,
};

const u32 pc_beep_verbs[] = {
	// Dos beep path - 1
	0x02170C00, 0x02050036, 0x02041151, 0x021707C0,
	// Dos beep path - 2
	0x0213B000, 0x02170C02, 0x02170C02, 0x02170C02,
};

AZALIA_ARRAY_SIZES;
