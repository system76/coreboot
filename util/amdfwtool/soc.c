/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "amdfwtool.h"

/* This file contains the SoC quirks. TODO: Use lookup tables instead. */

enum platform platform_identify(char *soc_name)
{
	if (!strcasecmp(soc_name, "Stoneyridge"))
		return PLATFORM_STONEYRIDGE;
	else if (!strcasecmp(soc_name, "Carrizo"))
		return PLATFORM_CARRIZO;
	else if (!strcasecmp(soc_name, "Mullins"))
		return PLATFORM_MULLINS;
	else if (!strcasecmp(soc_name, "Raven"))
		return PLATFORM_RAVEN;
	else if (!strcasecmp(soc_name, "Picasso"))
		return PLATFORM_PICASSO;
	else if (!strcasecmp(soc_name, "Cezanne"))
		return PLATFORM_CEZANNE;
	else if (!strcasecmp(soc_name, "Mendocino"))
		return PLATFORM_MENDOCINO;
	else if (!strcasecmp(soc_name, "Renoir"))
		return PLATFORM_RENOIR;
	else if (!strcasecmp(soc_name, "Lucienne"))
		return PLATFORM_LUCIENNE;
	else if (!strcasecmp(soc_name, "Phoenix"))
		return PLATFORM_PHOENIX;
	else if (!strcasecmp(soc_name, "Strix"))
		return PLATFORM_STRIX;
	else if (!strcasecmp(soc_name, "Genoa"))
		return PLATFORM_GENOA;
	else if (!strcasecmp(soc_name, "Krackan2e"))
		return PLATFORM_KRACKAN2E;
	else if (!strcasecmp(soc_name, "Strixhalo"))
		return PLATFORM_STRIXHALO;
	else
		return PLATFORM_UNKNOWN;
}

bool platform_needs_ish(enum platform platform_type)
{
	if (platform_type == PLATFORM_MENDOCINO || platform_type == PLATFORM_PHOENIX ||
		platform_type == PLATFORM_STRIX || platform_type == PLATFORM_KRACKAN2E ||
		platform_type == PLATFORM_STRIXHALO)
		return true;
	else
		return false;
}

/* Returns true when the PSP requires 'PSP L1' and 'PSP L2' directory tables. */
bool platform_is_multi_level(enum platform platform_type)
{
	// FIXME: Add turin here as well.

	/*
	 * SoCs older than Family 17h don't support multi level PSP directories.
	 *
	 * Raven and Picasso only support multi-level through PSP Combo directories.
	 * Combo directory generation isn't supported by this tool and different from
	 * the later EFS -> PSP L1 -> PSP L2 multilevel layout.
	 *
	 * Starting from Cezanne 'One Level PSP Directory Layout' is deprecated.
	 */
	switch (platform_type) {
	case PLATFORM_MENDOCINO:
	case PLATFORM_PHOENIX:
	case PLATFORM_STRIX:
	case PLATFORM_KRACKAN2E:
	case PLATFORM_STRIXHALO:
	case PLATFORM_GENOA:
	case PLATFORM_LUCIENNE:
	case PLATFORM_CEZANNE:
	case PLATFORM_RENOIR:
		return true;
	default:
		return false;
	}
}

/*
 * For some SOC generations the APOB_NV binary seems to be treated special regarding the
 * interpretaion of the source address. No matter the address_mode specified for the address
 * the memory ABL always seems to the interpret the source address as MMIO address even if
 * AMD_ADDR_REL_BIOS is specified. So for them we need to always use an MMIO address.
 * This seems to be a bug which affects all SOCs before phoenix generation.
 */
bool platform_has_apob_nv_quirk(enum platform platform_type)
{
	switch (platform_type) {
	case PLATFORM_MULLINS:
	case PLATFORM_CARRIZO:
	case PLATFORM_STONEYRIDGE:
	case PLATFORM_RAVEN:
	case PLATFORM_PICASSO:
	case PLATFORM_RENOIR:
	case PLATFORM_CEZANNE:
	case PLATFORM_MENDOCINO:
	case PLATFORM_LUCIENNE:
		return true;
	default:
		return false;
	}
}

bool platform_is_initial_alignment_required(enum platform platform_type)
{
	switch (platform_type) {
	case PLATFORM_MENDOCINO:
	case PLATFORM_PHOENIX:
	case PLATFORM_STRIX:
	case PLATFORM_KRACKAN2E:
	case PLATFORM_STRIXHALO:
		return false;
	default:
		return true;
	}
}

bool platform_is_second_gen(enum platform platform_type)
{
	switch (platform_type) {
	case PLATFORM_MULLINS:
	case PLATFORM_CARRIZO:
	case PLATFORM_STONEYRIDGE:
	case PLATFORM_RAVEN:
	case PLATFORM_PICASSO:
		return false;
	case PLATFORM_RENOIR:
	case PLATFORM_LUCIENNE:
	case PLATFORM_CEZANNE:
	case PLATFORM_MENDOCINO:
	case PLATFORM_PHOENIX:
	case PLATFORM_STRIX:
	case PLATFORM_GENOA:
	case PLATFORM_KRACKAN2E:
	case PLATFORM_STRIXHALO:
		return true;
	case PLATFORM_UNKNOWN:
	default:
		fprintf(stderr, "Error: Invalid SOC name.\n\n");
		return false;
	}
}

/*
 * Returns true when the PSP supports the Additional Information Field v1
 * in the directory header. Allows to support directories bigger than
 * 4 MiB in total.
 */
bool platform_has_dir_header_v1(enum platform platform_type)
{
	switch (platform_type) {
	case PLATFORM_STRIX:
	case PLATFORM_KRACKAN2E:
	case PLATFORM_STRIXHALO:
		return true;
	default:
		return false;
	}
}

uint32_t platform_get_psp_id(enum platform platform_type)
{
	uint32_t psp_id;
	switch (platform_type) {
	case PLATFORM_RAVEN:
	case PLATFORM_PICASSO:
		psp_id = 0xBC0A0000;
		break;
	case PLATFORM_RENOIR:
	case PLATFORM_LUCIENNE:
		psp_id = 0xBC0C0000;
		break;
	case PLATFORM_CEZANNE:
		psp_id = 0xBC0C0140;
		break;
	case PLATFORM_MENDOCINO:
		psp_id = 0xBC0D0900;
		break;
	case PLATFORM_STONEYRIDGE:
		psp_id = 0x10220B00;
		break;
	case PLATFORM_STRIX:
		psp_id = 0xBC0E0200;
		break;
	case PLATFORM_PHOENIX:
		psp_id = 0xBC0D0400;
		break;
	case PLATFORM_GENOA:
		psp_id = 0xBC0C0111;
		break;
	case PLATFORM_KRACKAN2E:
		psp_id = 0xbc0e1000;
		break;
	case PLATFORM_STRIXHALO:
		psp_id = 0xbc0e0900;
		break;
	case PLATFORM_CARRIZO:
	case PLATFORM_MULLINS:
	default:
		psp_id = 0;
		break;
	}
	return psp_id;
}
