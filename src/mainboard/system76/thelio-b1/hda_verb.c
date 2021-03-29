/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef HDA_VERB_H
#define HDA_VERB_H

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* Realtek, ALC1220 */
	0x10ec1220, /* Vendor ID */
	0x1458a0c3, /* Subsystem ID */
	12, /* Number of entries */
	AZALIA_SUBVENDOR(0, 0x1458a0c3),
	AZALIA_PIN_CFG(0, 0x12, 0x403fc000),
	AZALIA_PIN_CFG(0, 0x14, 0x01014010),
	AZALIA_PIN_CFG(0, 0x15, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x16, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x17, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x18, 0x01a19040),
	AZALIA_PIN_CFG(0, 0x19, 0x02a19050),
	AZALIA_PIN_CFG(0, 0x1a, 0x0181304f),
	AZALIA_PIN_CFG(0, 0x1b, 0x02214020),
	AZALIA_PIN_CFG(0, 0x1d, 0x4047c669),
	AZALIA_PIN_CFG(0, 0x1e, 0x99430130),
	/* Intel, KabylakeHDMI */
	0x8086280b, /* Vendor ID */
	0x80860101, /* Subsystem ID */
	4, /* Number of entries */
	AZALIA_SUBVENDOR(2, 0x80860101),
	AZALIA_PIN_CFG(2, 0x05, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560010),
	AZALIA_PIN_CFG(2, 0x07, 0x18560010),
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;

#endif
