/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/romstage.h>
#include <cbmem.h>
#include <console/console.h>
#include <fsp/util.h>
#include <intelblocks/cfg.h>
#include <intelblocks/cse.h>
#include <intelblocks/pmclib.h>
#include <memory_info.h>
#include <soc/intel/common/smbios.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include <string.h>

#include "../chip.h"

#define FSP_SMBIOS_MEMORY_INFO_GUID	\
{	\
	0xd4, 0x71, 0x20, 0x9b, 0x54, 0xb0, 0x0c, 0x4e,	\
	0x8d, 0x09, 0x11, 0xcf, 0x8b, 0x9f, 0x03, 0x23	\
}

void __weak mainboard_get_dram_part_num(const char **part_num, size_t *len)
{
	/* Default weak implementation, no need to override part number. */
}

/* Save the DIMM information for SMBIOS table 17 */
static void save_dimm_info(void)
{
	int channel, dimm, dimm_max, index;
	size_t hob_size;
	const CONTROLLER_INFO *ctrlr_info;
	const CHANNEL_INFO *channel_info;
	const DIMM_INFO *src_dimm;
	struct dimm_info *dest_dimm;
	struct memory_info *mem_info;
	const MEMORY_INFO_DATA_HOB *memory_info_hob;
	const uint8_t smbios_memory_info_guid[16] =
			FSP_SMBIOS_MEMORY_INFO_GUID;
	const char *dram_part_num;
	size_t dram_part_num_len;

	/* Locate the memory info HOB, presence validated by raminit */
	memory_info_hob = fsp_find_extension_hob_by_guid(
						smbios_memory_info_guid,
						&hob_size);
	if (memory_info_hob == NULL || hob_size == 0) {
		printk(BIOS_ERR, "SMBIOS MEMORY_INFO_DATA_HOB not found\n");
		return;
	}

	/*
	 * Allocate CBMEM area for DIMM information used to populate SMBIOS
	 * table 17
	 */
	mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(*mem_info));
	if (mem_info == NULL) {
		printk(BIOS_ERR, "CBMEM entry for DIMM info missing\n");
		return;
	}
	memset(mem_info, 0, sizeof(*mem_info));

	/* Describe the first N DIMMs in the system */
	index = 0;
	dimm_max = ARRAY_SIZE(mem_info->dimm);
	ctrlr_info = &memory_info_hob->Controller[0];
	for (channel = 0; channel < MAX_CH && index < dimm_max; channel++) {
		channel_info = &ctrlr_info->ChannelInfo[channel];
		if (channel_info->Status != CHANNEL_PRESENT)
			continue;
		for (dimm = 0; dimm < MAX_DIMM && index < dimm_max; dimm++) {
			src_dimm = &channel_info->DimmInfo[dimm];
			dest_dimm = &mem_info->dimm[index];

			if (src_dimm->Status != DIMM_PRESENT)
				continue;

			dram_part_num_len = sizeof(src_dimm->ModulePartNum);
			dram_part_num = (const char *)
						&src_dimm->ModulePartNum[0];

			/* Allow mainboard to override DRAM part number. */
			mainboard_get_dram_part_num(&dram_part_num,
							&dram_part_num_len);

			u8 memProfNum = memory_info_hob->MemoryProfile;

			/* Populate the DIMM information */
			dimm_info_fill(dest_dimm,
				src_dimm->DimmCapacity,
				memory_info_hob->MemoryType,
				memory_info_hob->ConfiguredMemoryClockSpeed,
				src_dimm->RankInDimm,
				channel_info->ChannelId,
				src_dimm->DimmId,
				dram_part_num,
				dram_part_num_len,
				src_dimm->SpdSave + SPD_SAVE_OFFSET_SERIAL,
				memory_info_hob->DataWidth,
				memory_info_hob->VddVoltage[memProfNum],
				memory_info_hob->EccSupport,
				src_dimm->MfgId,
				src_dimm->SpdModuleType);
			index++;
		}
	}
	mem_info->dimm_cnt = index;
	printk(BIOS_DEBUG, "%d DIMMs found\n", mem_info->dimm_cnt);
}

void mainboard_romstage_entry(void)
{
	bool s3wake;
	struct chipset_power_state *ps = pmc_get_power_state();

	/* Program MCHBAR, DMIBAR, GDXBAR and EDRAMBAR */
	systemagent_early_init();
	/* initialize Heci interface */
	heci_init(HECI1_BASE_ADDRESS);

	s3wake = pmc_fill_power_state(ps) == ACPI_S3;
	fsp_memory_init(s3wake);
	pmc_set_disb();
	if (!s3wake)
		save_dimm_info();
}

void soc_display_fspm_upd_params(const FSPM_UPD *fspm_old_upd,
	const FSPM_UPD *fspm_new_upd)
{
	const FSP_M_CONFIG *new;
	const FSP_M_CONFIG *old;

	old = &fspm_old_upd->FspmConfig;
	new = &fspm_new_upd->FspmConfig;

	/* Display the parameters for MemoryInit */
	printk(BIOS_SPEW, "UPD values for MemoryInit:\n");

    //TODO: Add more Comet Lake values
	fsp_display_upd_value("PlatformMemorySize", 8,
		old->PlatformMemorySize, new->PlatformMemorySize);
	fsp_display_upd_value("MemorySpdPtr00", 4, old->MemorySpdPtr00,
		new->MemorySpdPtr00);
	fsp_display_upd_value("MemorySpdPtr01", 4, old->MemorySpdPtr01,
		new->MemorySpdPtr01);
	fsp_display_upd_value("MemorySpdPtr10", 4, old->MemorySpdPtr10,
		new->MemorySpdPtr10);
	fsp_display_upd_value("MemorySpdPtr11", 4, old->MemorySpdPtr11,
		new->MemorySpdPtr11);
	fsp_display_upd_value("MemorySpdDataLen", 2, old->MemorySpdDataLen,
		new->MemorySpdDataLen);
	fsp_display_upd_value("DqByteMapCh0[0]", 1, old->DqByteMapCh0[0],
		new->DqByteMapCh0[0]);
	fsp_display_upd_value("DqByteMapCh0[1]", 1, old->DqByteMapCh0[1],
		new->DqByteMapCh0[1]);
	fsp_display_upd_value("DqByteMapCh0[2]", 1, old->DqByteMapCh0[2],
		new->DqByteMapCh0[2]);
	fsp_display_upd_value("DqByteMapCh0[3]", 1, old->DqByteMapCh0[3],
		new->DqByteMapCh0[3]);
	fsp_display_upd_value("DqByteMapCh0[4]", 1, old->DqByteMapCh0[4],
		new->DqByteMapCh0[4]);
	fsp_display_upd_value("DqByteMapCh0[5]", 1, old->DqByteMapCh0[5],
		new->DqByteMapCh0[5]);
	fsp_display_upd_value("DqByteMapCh0[6]", 1, old->DqByteMapCh0[6],
		new->DqByteMapCh0[6]);
	fsp_display_upd_value("DqByteMapCh0[7]", 1, old->DqByteMapCh0[7],
		new->DqByteMapCh0[7]);
	fsp_display_upd_value("DqByteMapCh0[8]", 1, old->DqByteMapCh0[8],
		new->DqByteMapCh0[8]);
	fsp_display_upd_value("DqByteMapCh0[9]", 1, old->DqByteMapCh0[9],
		new->DqByteMapCh0[9]);
	fsp_display_upd_value("DqByteMapCh0[10]", 1, old->DqByteMapCh0[10],
		new->DqByteMapCh0[10]);
	fsp_display_upd_value("DqByteMapCh0[11]", 1, old->DqByteMapCh0[11],
		new->DqByteMapCh0[11]);
	fsp_display_upd_value("DqByteMapCh1[0]", 1, old->DqByteMapCh1[0],
		new->DqByteMapCh1[0]);
	fsp_display_upd_value("DqByteMapCh1[1]", 1, old->DqByteMapCh1[1],
		new->DqByteMapCh1[1]);
	fsp_display_upd_value("DqByteMapCh1[2]", 1, old->DqByteMapCh1[2],
		new->DqByteMapCh1[2]);
	fsp_display_upd_value("DqByteMapCh1[3]", 1, old->DqByteMapCh1[3],
		new->DqByteMapCh1[3]);
	fsp_display_upd_value("DqByteMapCh1[4]", 1, old->DqByteMapCh1[4],
		new->DqByteMapCh1[4]);
	fsp_display_upd_value("DqByteMapCh1[5]", 1, old->DqByteMapCh1[5],
		new->DqByteMapCh1[5]);
	fsp_display_upd_value("DqByteMapCh1[6]", 1, old->DqByteMapCh1[6],
		new->DqByteMapCh1[6]);
	fsp_display_upd_value("DqByteMapCh1[7]", 1, old->DqByteMapCh1[7],
		new->DqByteMapCh1[7]);
	fsp_display_upd_value("DqByteMapCh1[8]", 1, old->DqByteMapCh1[8],
		new->DqByteMapCh1[8]);
	fsp_display_upd_value("DqByteMapCh1[9]", 1, old->DqByteMapCh1[9],
		new->DqByteMapCh1[9]);
	fsp_display_upd_value("DqByteMapCh1[10]", 1, old->DqByteMapCh1[10],
		new->DqByteMapCh1[10]);
	fsp_display_upd_value("DqByteMapCh1[11]", 1, old->DqByteMapCh1[11],
		new->DqByteMapCh1[11]);
	fsp_display_upd_value("DqsMapCpu2DramCh0[0]", 1,
		old->DqsMapCpu2DramCh0[0], new->DqsMapCpu2DramCh0[0]);
	fsp_display_upd_value("DqsMapCpu2DramCh0[1]", 1,
		old->DqsMapCpu2DramCh0[1], new->DqsMapCpu2DramCh0[1]);
	fsp_display_upd_value("DqsMapCpu2DramCh0[2]", 1,
		old->DqsMapCpu2DramCh0[2], new->DqsMapCpu2DramCh0[2]);
	fsp_display_upd_value("DqsMapCpu2DramCh0[3]", 1,
		old->DqsMapCpu2DramCh0[3], new->DqsMapCpu2DramCh0[3]);
	fsp_display_upd_value("DqsMapCpu2DramCh0[4]", 1,
		old->DqsMapCpu2DramCh0[4], new->DqsMapCpu2DramCh0[4]);
	fsp_display_upd_value("DqsMapCpu2DramCh0[5]", 1,
		old->DqsMapCpu2DramCh0[5], new->DqsMapCpu2DramCh0[5]);
	fsp_display_upd_value("DqsMapCpu2DramCh0[6]", 1,
		old->DqsMapCpu2DramCh0[6], new->DqsMapCpu2DramCh0[6]);
	fsp_display_upd_value("DqsMapCpu2DramCh0[7]", 1,
		old->DqsMapCpu2DramCh0[7], new->DqsMapCpu2DramCh0[7]);
	fsp_display_upd_value("DqsMapCpu2DramCh1[0]", 1,
		old->DqsMapCpu2DramCh1[0], new->DqsMapCpu2DramCh1[0]);
	fsp_display_upd_value("DqsMapCpu2DramCh1[1]", 1,
		old->DqsMapCpu2DramCh1[1], new->DqsMapCpu2DramCh1[1]);
	fsp_display_upd_value("DqsMapCpu2DramCh1[2]", 1,
		old->DqsMapCpu2DramCh1[2], new->DqsMapCpu2DramCh1[2]);
	fsp_display_upd_value("DqsMapCpu2DramCh1[3]", 1,
		old->DqsMapCpu2DramCh1[3], new->DqsMapCpu2DramCh1[3]);
	fsp_display_upd_value("DqsMapCpu2DramCh1[4]", 1,
		old->DqsMapCpu2DramCh1[4], new->DqsMapCpu2DramCh1[4]);
	fsp_display_upd_value("DqsMapCpu2DramCh1[5]", 1,
		old->DqsMapCpu2DramCh1[5], new->DqsMapCpu2DramCh1[5]);
	fsp_display_upd_value("DqsMapCpu2DramCh1[6]", 1,
		old->DqsMapCpu2DramCh1[6], new->DqsMapCpu2DramCh1[6]);
	fsp_display_upd_value("DqsMapCpu2DramCh1[7]", 1,
		old->DqsMapCpu2DramCh1[7], new->DqsMapCpu2DramCh1[7]);
	fsp_display_upd_value("RcompResistor[0]", 2, old->RcompResistor[0],
		new->RcompResistor[0]);
	fsp_display_upd_value("RcompResistor[1]", 2, old->RcompResistor[1],
		new->RcompResistor[1]);
	fsp_display_upd_value("RcompResistor[2]", 2, old->RcompResistor[2],
		new->RcompResistor[2]);
	fsp_display_upd_value("RcompTarget[0]", 2, old->RcompTarget[0],
		new->RcompTarget[0]);
	fsp_display_upd_value("RcompTarget[1]", 2, old->RcompTarget[1],
		new->RcompTarget[1]);
	fsp_display_upd_value("RcompTarget[2]", 2, old->RcompTarget[2],
		new->RcompTarget[2]);
	fsp_display_upd_value("RcompTarget[3]", 2, old->RcompTarget[3],
		new->RcompTarget[3]);
	fsp_display_upd_value("RcompTarget[4]", 2, old->RcompTarget[4],
		new->RcompTarget[4]);
	fsp_display_upd_value("DqPinsInterleaved", 1, old->DqPinsInterleaved,
		new->DqPinsInterleaved);
	fsp_display_upd_value("CaVrefConfig", 1, old->CaVrefConfig,
		new->CaVrefConfig);
	fsp_display_upd_value("SmramMask", 1, old->SmramMask, new->SmramMask);
#if CONFIG(SOC_INTEL_COMETLAKE)
    // Value only exists on Comet Lake
    fsp_display_upd_value("MrcTimeMeasure", 1, old->MrcTimeMeasure,
        new->MrcTimeMeasure);
#endif
	fsp_display_upd_value("MrcFastBoot", 1, old->MrcFastBoot,
		new->MrcFastBoot);
	fsp_display_upd_value("RmtPerTask", 1, old->RmtPerTask,
		new->RmtPerTask);
	fsp_display_upd_value("TrainTrace", 1, old->TrainTrace,
		new->TrainTrace);
	fsp_display_upd_value("IedSize", 4, old->IedSize, new->IedSize);
	fsp_display_upd_value("TsegSize", 4, old->TsegSize, new->TsegSize);
	fsp_display_upd_value("MmioSize", 2, old->MmioSize, new->MmioSize);
	fsp_display_upd_value("ProbelessTrace", 1, old->ProbelessTrace,
		new->ProbelessTrace);
	fsp_display_upd_value("GdxcIotSize", 1, old->GdxcIotSize,
		new->GdxcIotSize);
	fsp_display_upd_value("GdxcMotSize", 1, old->GdxcMotSize,
		new->GdxcMotSize);
	fsp_display_upd_value("SmbusEnable", 1, old->SmbusEnable,
		new->SmbusEnable);
	fsp_display_upd_value("SpdAddressTable[0]", 1, old->SpdAddressTable[0],
		new->SpdAddressTable[0]);
	fsp_display_upd_value("SpdAddressTable[1]", 1, old->SpdAddressTable[1],
		new->SpdAddressTable[1]);
	fsp_display_upd_value("SpdAddressTable[2]", 1, old->SpdAddressTable[2],
		new->SpdAddressTable[2]);
	fsp_display_upd_value("SpdAddressTable[3]", 1, old->SpdAddressTable[3],
		new->SpdAddressTable[3]);
	fsp_display_upd_value("PlatformDebugConsent", 1,
        old->PlatformDebugConsent, new->PlatformDebugConsent);
	fsp_display_upd_value("DciUsb3TypecUfpDbg", 1, old->DciUsb3TypecUfpDbg,
		new->DciUsb3TypecUfpDbg);
	fsp_display_upd_value("PchTraceHubMode", 1, old->PchTraceHubMode,
		new->PchTraceHubMode);
	fsp_display_upd_value("PchTraceHubMemReg0Size", 1,
        old->PchTraceHubMemReg0Size, new->PchTraceHubMemReg0Size);
	fsp_display_upd_value("PchTraceHubMemReg1Size", 1,
        old->PchTraceHubMemReg1Size, new->PchTraceHubMemReg1Size);
    // Skip PchPreMemRsvd[9]
	fsp_display_upd_value("IgdDvmt50PreAlloc", 1, old->IgdDvmt50PreAlloc,
		new->IgdDvmt50PreAlloc);
	fsp_display_upd_value("InternalGfx", 1, old->InternalGfx,
		new->InternalGfx);
	fsp_display_upd_value("ApertureSize", 1, old->ApertureSize,
		new->ApertureSize);
	fsp_display_upd_value("UserBd", 1, old->UserBd,
		new->UserBd);
	fsp_display_upd_value("SaGv", 1, old->SaGv,
		new->SaGv);
    // Skip UnusedUpdSpace0
    fsp_display_upd_value("DdrFreqLimit", 2, old->DdrFreqLimit,
        new->DdrFreqLimit);
    fsp_display_upd_value("FreqSaGvLow", 2, old->FreqSaGvLow,
        new->FreqSaGvLow);
#if ! CONFIG(SOC_INTEL_COMETLAKE)
    // Value does not exist on Comet Lake
    fsp_display_upd_value("FreqSaGvMid", 2, old->FreqSaGvMid,
        new->FreqSaGvMid);
#endif
    fsp_display_upd_value("RMT", 1, old->RMT,
        new->RMT);
    fsp_display_upd_value("DisableDimmChannel0", 1,
        old->DisableDimmChannel0, new->DisableDimmChannel0);
    fsp_display_upd_value("DisableDimmChannel1", 1,
        old->DisableDimmChannel1, new->DisableDimmChannel1);
    fsp_display_upd_value("ScramblerSupport", 1, old->ScramblerSupport,
        new->ScramblerSupport);
    fsp_display_upd_value("SkipMpInit", 1, old->SkipMpInit,
        new->SkipMpInit);
    // Skip UnusedUpdSpace1[15]
    fsp_display_upd_value("SpdProfileSelected", 1,
        old->SpdProfileSelected, new->SpdProfileSelected);
    fsp_display_upd_value("RefClk", 1, old->RefClk,
        new->RefClk);
    fsp_display_upd_value("VddVoltage", 2, old->VddVoltage,
        new->VddVoltage);
    fsp_display_upd_value("Ratio", 1, old->Ratio,
        new->Ratio);
    fsp_display_upd_value("OddRatioMode", 1, old->OddRatioMode,
        new->OddRatioMode);
    fsp_display_upd_value("tCL", 1, old->tCL,
        new->tCL);
    fsp_display_upd_value("tCWL", 1, old->tCWL,
        new->tCWL);
    fsp_display_upd_value("tRCDtRP", 1, old->tRCDtRP,
        new->tRCDtRP);
    fsp_display_upd_value("tRRD", 1, old->tRRD,
        new->tRRD);
    fsp_display_upd_value("tFAW", 2, old->tFAW,
        new->tFAW);
    fsp_display_upd_value("tRAS", 2, old->tRAS,
        new->tRAS);
    fsp_display_upd_value("tREFI", 2, old->tREFI,
        new->tREFI);
    fsp_display_upd_value("tRFC", 2, old->tRFC,
        new->tRFC);
    fsp_display_upd_value("tRTP", 1, old->tRTP,
        new->tRTP);
    fsp_display_upd_value("tWR", 1, old->tWR,
        new->tWR);
    fsp_display_upd_value("NModeSupport", 1, old->NModeSupport,
        new->NModeSupport);
    fsp_display_upd_value("DllBwEn0", 1, old->DllBwEn0,
        new->DllBwEn0);
    fsp_display_upd_value("DllBwEn1", 1, old->DllBwEn1,
        new->DllBwEn1);
    fsp_display_upd_value("DllBwEn2", 1, old->DllBwEn2,
        new->DllBwEn2);
    fsp_display_upd_value("DllBwEn3", 1, old->DllBwEn3,
        new->DllBwEn3);
    fsp_display_upd_value("IsvtIoPort", 1, old->IsvtIoPort,
        new->IsvtIoPort);
    fsp_display_upd_value("CpuTraceHubMode", 1, old->CpuTraceHubMode,
        new->CpuTraceHubMode);
    fsp_display_upd_value("CpuTraceHubMemReg0Size", 1,
        old->CpuTraceHubMemReg0Size, new->CpuTraceHubMemReg0Size);
    fsp_display_upd_value("CpuTraceHubMemReg1Size", 1,
        old->CpuTraceHubMemReg1Size, new->CpuTraceHubMemReg1Size);
    fsp_display_upd_value("PeciC10Reset", 1, old->PeciC10Reset,
        new->PeciC10Reset);
    fsp_display_upd_value("PeciSxReset", 1, old->PeciSxReset,
        new->PeciSxReset);
    // Skip UnusedUpdSpace2[4]
    fsp_display_upd_value("PchHdaEnable", 1, old->PchHdaEnable,
        new->PchHdaEnable);
    fsp_display_upd_value("PchIshEnable", 1, old->PchIshEnable,
        new->PchIshEnable);
    fsp_display_upd_value("HeciTimeouts", 1, old->HeciTimeouts,
        new->HeciTimeouts);
    // Skip UnusedUpdSpace3
    fsp_display_upd_value("Heci1BarAddress", 4, old->Heci1BarAddress,
        new->Heci1BarAddress);
    fsp_display_upd_value("Heci2BarAddress", 4, old->Heci2BarAddress,
        new->Heci2BarAddress);
    fsp_display_upd_value("Heci3BarAddress", 4, old->Heci3BarAddress,
        new->Heci3BarAddress);
    fsp_display_upd_value("SgDelayAfterPwrEn", 2, old->SgDelayAfterPwrEn,
        new->SgDelayAfterPwrEn);
    fsp_display_upd_value("SgDelayAfterHoldReset", 2, old->SgDelayAfterHoldReset,
        new->SgDelayAfterHoldReset);
    fsp_display_upd_value("MmioSizeAdjustment", 2, old->MmioSizeAdjustment,
        new->MmioSizeAdjustment);
    fsp_display_upd_value("DmiGen3ProgramStaticEq", 1,
        old->DmiGen3ProgramStaticEq, new->DmiGen3ProgramStaticEq);
    fsp_display_upd_value("Peg0Enable", 1, old->Peg0Enable,
        new->Peg0Enable);
    fsp_display_upd_value("Peg1Enable", 1, old->Peg1Enable,
        new->Peg1Enable);
    fsp_display_upd_value("Peg2Enable", 1, old->Peg2Enable,
        new->Peg2Enable);
    fsp_display_upd_value("Peg3Enable", 1, old->Peg3Enable,
        new->Peg3Enable);
    fsp_display_upd_value("Peg0MaxLinkSpeed", 1, old->Peg0MaxLinkSpeed,
        new->Peg0MaxLinkSpeed);
    fsp_display_upd_value("Peg1MaxLinkSpeed", 1, old->Peg1MaxLinkSpeed,
        new->Peg1MaxLinkSpeed);
    fsp_display_upd_value("Peg2MaxLinkSpeed", 1, old->Peg2MaxLinkSpeed,
        new->Peg2MaxLinkSpeed);
    fsp_display_upd_value("Peg3MaxLinkSpeed", 1, old->Peg3MaxLinkSpeed,
        new->Peg3MaxLinkSpeed);
    fsp_display_upd_value("Peg0MaxLinkWidth", 1, old->Peg0MaxLinkWidth,
        new->Peg0MaxLinkWidth);
    fsp_display_upd_value("Peg1MaxLinkWidth", 1, old->Peg1MaxLinkWidth,
        new->Peg1MaxLinkWidth);
    fsp_display_upd_value("Peg2MaxLinkWidth", 1, old->Peg2MaxLinkWidth,
        new->Peg2MaxLinkWidth);
    fsp_display_upd_value("Peg3MaxLinkWidth", 1, old->Peg3MaxLinkWidth,
        new->Peg3MaxLinkWidth);
    fsp_display_upd_value("Peg0PowerDownUnusedLanes", 1,
        old->Peg0PowerDownUnusedLanes, new->Peg0PowerDownUnusedLanes);
    fsp_display_upd_value("Peg1PowerDownUnusedLanes", 1,
        old->Peg1PowerDownUnusedLanes, new->Peg1PowerDownUnusedLanes);
    fsp_display_upd_value("Peg2PowerDownUnusedLanes", 1,
        old->Peg2PowerDownUnusedLanes, new->Peg2PowerDownUnusedLanes);
    fsp_display_upd_value("Peg3PowerDownUnusedLanes", 1,
        old->Peg3PowerDownUnusedLanes, new->Peg3PowerDownUnusedLanes);
    fsp_display_upd_value("InitPcieAspmAfterOprom", 1,
        old->InitPcieAspmAfterOprom, new->InitPcieAspmAfterOprom);
    fsp_display_upd_value("PegDisableSpreadSpectrumClocking", 1,
        old->PegDisableSpreadSpectrumClocking,
        new->PegDisableSpreadSpectrumClocking);
    // Skip UnusedUpdSpace4[3]
    //TODO: Offset 0x128 and up
}
