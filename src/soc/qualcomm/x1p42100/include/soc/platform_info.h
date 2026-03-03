/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __SOC_QUALCOMM_X1P42100_PLATFORM_INFO_H__
#define __SOC_QUALCOMM_X1P42100_PLATFORM_INFO_H__

/* ADSP BOOT_PARAMS helpers */

/* BOOT_PARAMS bitfield definitions */
#define CHIP_FAMILY_MASK			0xFFFF
#define CHIP_FAMILY_SHIFT			16
#define CHIP_ID_MASK				0xFFFF
#define CHIP_VERSION_MAJOR_SHIFT		8
#define CHIP_VERSION_MASK			0xFF
#define PLATFORM_FIELD_MASK			0xFF
#define PLATFORM_SUBTYPE_SHIFT			16
#define PLATFORM_TYPE_SHIFT			24
#define PLATFORM_VERSION_MAJOR_SHIFT		8

#define EVB_ENABLE				1

/*
 * ADSP BOOT_PARAMS chipinfo identifiers.
 *
 * Keep values as macros (no literals in packer code) to match SMEM chipinfo:
 * - CHIPINFO_FAMILY_*: eChipInfoFamily
 * - CHIPINFO_ID_SCP_*: eChipInfoId (SCP)
 */
#if CONFIG(SOC_QUALCOMM_HAMOA)
/* Hamoa (SC8380XP) identifiers used by ADSP BOOT_PARAMS on x1p42100 platforms. */
#define CHIPINFO_FAMILY				0x0088
#define CHIPINFO_ID_SCP				0x022B

/* Hamoa chip version used for BOOT_PARAMS[3] packing. */
#define CHIPINFO_CHIP_VERSION			0x00020000 /* nChipVersion (SMEM) */
#define CHIPINFO_CHIP_VERSION_MAJOR		0x02
#define CHIPINFO_CHIP_VERSION_MINOR		0x00
#else
/* Purwa Compute (SC8340XP / X1P4x100) */
#define CHIPINFO_FAMILY				0x009A
#define CHIPINFO_ID_SCP				0x027B

#define CHIPINFO_CHIP_VERSION			0x00020000 /* nChipVersion (SMEM) */
#define CHIPINFO_CHIP_VERSION_MAJOR		0x02
#define CHIPINFO_CHIP_VERSION_MINOR		0x00
#endif

/* x1p42100 platform info used for BOOT_PARAMS[4] packing. */
#define PLATFORMINFO_TYPE			0x28 /* ePlatformType */
#define PLATFORMINFO_SUBTYPE			0x00 /* nPlatformSubtype */
#define PLATFORMINFO_VERSION			0x00010000 /* nPlatformVersion */
#define PLATFORMINFO_VERSION_MAJOR		0x01
#define PLATFORMINFO_VERSION_MINOR		0x00

#endif /* __SOC_QUALCOMM_X1P42100_PLATFORM_INFO_H__ */
