/* SPDX-License-Identifier: GPL-2.0-only */

// Hack for including GMA ACPI code
#ifndef SYSTEM76_ACPI_NO_GFX0
Device (GFX0)
{
	Name (_ADR, 0x00020000)
}
#endif // SYSTEM76_ACPI_NO_GFX0
