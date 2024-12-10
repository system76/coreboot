/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
DefinitionBlock (
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x00010001	/* OEM Revision */
	)
{
	#include <acpi/dsdt_top.asl>

	#include <soc.asl>

	Scope (\_SB.PCI0.LPCB)
	{
		#include <drivers/pc80/pc/ps2_controller.asl>
	}

	#define EC_GPE_SCI 0x6E
	#define EC_GPE_SWI 0x6B
	#include <ec/system76/ec/acpi/ec.asl>
}
