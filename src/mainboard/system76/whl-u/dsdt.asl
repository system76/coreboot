/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20110725	/* OEM revision */
)
{
	#include <soc/intel/common/block/acpi/acpi/platform.asl>
	#include <soc/intel/common/block/acpi/acpi/globalnvs.asl>
	#include <cpu/intel/common/acpi/cpu.asl>

	Device (\_SB.PCI0)
	{
		#include <soc/intel/common/block/acpi/acpi/northbridge.asl>
		#include <soc/intel/cannonlake/acpi/southbridge.asl>
	}

	#include <southbridge/intel/common/acpi/sleepstates.asl>

	Scope (\_SB.PCI0.LPCB)
	{
		#include <drivers/pc80/pc/ps2_controller.asl>
	}

	#include "acpi/mainboard.asl"
}
