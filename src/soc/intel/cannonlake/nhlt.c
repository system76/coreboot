/* This file is part of the coreboot project. */
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <nhlt.h>
#include <soc/nhlt.h>

static const struct nhlt_format_config dmic_1ch_formats[] = {
	/* 48 KHz 16-bits per sample. */
	{
		.num_channels = 1,
		.sample_freq_khz = 48,
		.container_bits_per_sample = 16,
		.valid_bits_per_sample = 16,
		.settings_file = "dmic-1ch-48khz-16b.bin",
	},
};

static const struct nhlt_dmic_array_config dmic_1ch_mic_config = {
	.tdm_config = {
		.config_type = NHLT_TDM_MIC_ARRAY,
	},
	.array_type = NHLT_MIC_ARRAY_VENDOR_DEFINED,
};

static const struct nhlt_endp_descriptor dmic_1ch_descriptors[] = {
	{
		.link = NHLT_LINK_PDM,
		.device = NHLT_PDM_DEV,
		.direction = NHLT_DIR_CAPTURE,
		.vid = NHLT_VID,
		.did = NHLT_DID_DMIC,
		.cfg = &dmic_1ch_mic_config,
		.cfg_size = sizeof(dmic_1ch_mic_config),
		.formats = dmic_1ch_formats,
		.num_formats = ARRAY_SIZE(dmic_1ch_formats),
	},
};

static const struct nhlt_format_config dmic_2ch_formats[] = {
	/* 48 KHz 16-bits per sample. */
	{
		.num_channels = 2,
		.sample_freq_khz = 48,
		.container_bits_per_sample = 16,
		.valid_bits_per_sample = 16,
		.settings_file = "dmic-2ch-48khz-16b.bin",
	},
};

static const struct nhlt_dmic_array_config dmic_2ch_mic_config = {
	.tdm_config = {
		.config_type = NHLT_TDM_MIC_ARRAY,
	},
	.array_type = NHLT_MIC_ARRAY_2CH_SMALL,
};

static const struct nhlt_endp_descriptor dmic_2ch_descriptors[] = {
	{
		.link = NHLT_LINK_PDM,
		.device = NHLT_PDM_DEV,
		.direction = NHLT_DIR_CAPTURE,
		.vid = NHLT_VID,
		.did = NHLT_DID_DMIC,
		.cfg = &dmic_2ch_mic_config,
		.cfg_size = sizeof(dmic_2ch_mic_config),
		.formats = dmic_2ch_formats,
		.num_formats = ARRAY_SIZE(dmic_2ch_formats),
	},
};

static const struct nhlt_format_config dmic_4ch_formats[] = {
	/* 48 KHz 16-bits per sample. */
	{
		.num_channels = 4,
		.sample_freq_khz = 48,
		.container_bits_per_sample = 16,
		.valid_bits_per_sample = 16,
		.settings_file = "dmic-4ch-48khz-16b.bin",
	},
};

static const struct nhlt_dmic_array_config dmic_4ch_mic_config = {
	.tdm_config = {
		.config_type = NHLT_TDM_MIC_ARRAY,
	},
	.array_type = NHLT_MIC_ARRAY_4CH_L_SHAPED,
};

static const struct nhlt_endp_descriptor dmic_4ch_descriptors[] = {
	{
		.link = NHLT_LINK_PDM,
		.device = NHLT_PDM_DEV,
		.direction = NHLT_DIR_CAPTURE,
		.vid = NHLT_VID,
		.did = NHLT_DID_DMIC,
		.cfg = &dmic_4ch_mic_config,
		.cfg_size = sizeof(dmic_4ch_mic_config),
		.formats = dmic_4ch_formats,
		.num_formats = ARRAY_SIZE(dmic_4ch_formats),
	},
};

static const struct nhlt_format_config da7219_formats[] = {
	/* 48 KHz 24-bits per sample. */
	{
		.num_channels = 2,
		.sample_freq_khz = 48,
		.container_bits_per_sample = 32,
		.valid_bits_per_sample = 24,
		.settings_file = "dialog-2ch-48khz-24b.bin",
	},
};

static const struct nhlt_tdm_config tdm_config = {
	.virtual_slot = 0,
	.config_type = NHLT_TDM_BASIC,
};

static const struct nhlt_endp_descriptor da7219_descriptors[] = {
	/* Render Endpoint */
	{
		.link = NHLT_LINK_SSP,
		.device = NHLT_SSP_DEV_I2S,
		.direction = NHLT_DIR_RENDER,
		.vid = NHLT_VID,
		.did = NHLT_DID_SSP,
		.cfg = &tdm_config,
		.cfg_size = sizeof(tdm_config),
		.formats = da7219_formats,
		.num_formats = ARRAY_SIZE(da7219_formats),
	},
	/* Capture Endpoint */
	{
		.link = NHLT_LINK_SSP,
		.device = NHLT_SSP_DEV_I2S,
		.direction = NHLT_DIR_CAPTURE,
		.vid = NHLT_VID,
		.did = NHLT_DID_SSP,
		.cfg = &tdm_config,
		.cfg_size = sizeof(tdm_config),
		.formats = da7219_formats,
		.num_formats = ARRAY_SIZE(da7219_formats),
	},
};

static const struct nhlt_format_config max98357_formats[] = {
	/* 48 KHz 24-bits per sample. */
	{
		.num_channels = 2,
		.sample_freq_khz = 48,
		.container_bits_per_sample = 32,
		.valid_bits_per_sample = 24,
		.settings_file = "max98357-render-2ch-48khz-24b.bin",
	},
};

static const struct nhlt_endp_descriptor max98357_descriptors[] = {
	{
		.link = NHLT_LINK_SSP,
		.device = NHLT_SSP_DEV_I2S,
		.direction = NHLT_DIR_RENDER,
		.vid = NHLT_VID,
		.did = NHLT_DID_SSP,
		.formats = max98357_formats,
		.num_formats = ARRAY_SIZE(max98357_formats),
	},
};

static const struct nhlt_format_config max98373_render_formats[] = {
	/* 48 KHz 24-bits per sample. */
	{
		.num_channels = 2,
		.sample_freq_khz = 48,
		.container_bits_per_sample = 32,
		.valid_bits_per_sample = 24,
		.speaker_mask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT,
		.settings_file = "max98373-render-2ch-48khz-24b.bin",
	},
	/* 48 KHz 16-bits per sample. */
	{
		.num_channels = 2,
		.sample_freq_khz = 48,
		.container_bits_per_sample = 16,
		.valid_bits_per_sample = 16,
		.speaker_mask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT,
		.settings_file = "max98373-render-2ch-48khz-16b.bin",
	}
};

static const struct nhlt_format_config max98373_capture_formats[] = {
	/* 48 KHz 16-bits per sample. */
	{
		.num_channels = 2,
		.sample_freq_khz = 48,
		.container_bits_per_sample = 16,
		.valid_bits_per_sample = 16,
		.speaker_mask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT,
		.settings_file = "max98373-render-2ch-48khz-16b.bin",
	},
};

static const struct nhlt_endp_descriptor max98373_descriptors[] = {
	{
		.link = NHLT_LINK_SSP,
		.device = NHLT_SSP_DEV_I2S,
		.direction = NHLT_DIR_RENDER,
		.vid = NHLT_VID,
		.did = NHLT_DID_SSP,
		.formats = max98373_render_formats,
		.num_formats = ARRAY_SIZE(max98373_render_formats),
	},
	{
		.link = NHLT_LINK_SSP,
		.device = NHLT_SSP_DEV_I2S,
		.direction = NHLT_DIR_CAPTURE,
		.vid = NHLT_VID,
		.did = NHLT_DID_SSP,
		.formats = max98373_capture_formats,
		.num_formats = ARRAY_SIZE(max98373_capture_formats),
	},
};

int nhlt_soc_add_dmic_array(struct nhlt *nhlt, int num_channels)
{
	switch (num_channels) {
	case 1:
		return nhlt_add_endpoints(nhlt, dmic_1ch_descriptors,
			ARRAY_SIZE(dmic_1ch_descriptors));
	case 2:
		return nhlt_add_endpoints(nhlt, dmic_2ch_descriptors,
			ARRAY_SIZE(dmic_2ch_descriptors));
	case 4:
		return nhlt_add_endpoints(nhlt, dmic_4ch_descriptors,
			ARRAY_SIZE(dmic_4ch_descriptors));
	default:
		return -1;
	}
}

int nhlt_soc_add_da7219(struct nhlt *nhlt, int hwlink)
{
	/* Virtual bus id of SSP links are the hardware port ids proper. */
	return nhlt_add_ssp_endpoints(nhlt, hwlink, da7219_descriptors,
					ARRAY_SIZE(da7219_descriptors));
}

int nhlt_soc_add_max98357(struct nhlt *nhlt, int hwlink)
{
	/* Virtual bus id of SSP links are the hardware port ids proper. */
	return nhlt_add_ssp_endpoints(nhlt, hwlink, max98357_descriptors,
					ARRAY_SIZE(max98357_descriptors));
}

int nhlt_soc_add_max98373(struct nhlt *nhlt, int hwlink)
{
	/* Virtual bus id of SSP links are the hardware port ids proper. */
	return nhlt_add_ssp_endpoints(nhlt, hwlink, max98373_descriptors,
					ARRAY_SIZE(max98373_descriptors));
}
