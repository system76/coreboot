/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_codec/realtek.h>
#include <device/azalia_device.h>
#include <option.h>

/* Leave the speaker path muted so the OS can sequence EAPD and GPIO2 later. */
static const uint32_t speaker_idle_verb[] = {
	AZALIA_VERB_12B(0, 0x01, 0x716, 0x04),
	AZALIA_VERB_12B(0, 0x01, 0x717, 0x04),
	AZALIA_VERB_12B(0, 0x01, 0x715, 0x00),
	AZALIA_VERB_12B(0, ALC269_LINE2, 0x70c, 0x00),
};

static const uint32_t microphone_disable_verb[] = {
	AZALIA_PIN_CFG(0, ALC269_DMIC12, AZALIA_PIN_CFG_NC(0)),
};

void mainboard_azalia_program_runtime_verbs(uint8_t *base, uint32_t viddid)
{
	(void)viddid;

	if (get_uint_option("microphone", 1) == 0)
		azalia_program_verb_table(base, microphone_disable_verb,
					  ARRAY_SIZE(microphone_disable_verb));

	if (get_uint_option("legacy_speaker_control", 1) == 0)
		azalia_program_verb_table(base, speaker_idle_verb,
					  ARRAY_SIZE(speaker_idle_verb));
}
