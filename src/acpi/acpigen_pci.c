/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpigen_pci.h>
#include <assert.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_type.h>
#include <types.h>

void acpigen_write_ADR_pci_devfn(pci_devfn_t devfn)
{
	/*
	 * _ADR for PCI Bus is encoded as follows:
	 * [63:32] - unused
	 * [31:16] - device #
	 * [15:0]  - function #
	 */
	acpigen_write_ADR(PCI_SLOT(devfn) << 16 | PCI_FUNC(devfn));
}

void acpigen_write_ADR_pci_device(const struct device *dev)
{
	assert(dev->path.type == DEVICE_PATH_PCI);
	acpigen_write_ADR_pci_devfn(dev->path.pci.devfn);
}

/*
 * Emit one PCI _PRT entry for direct GSI (IO-APIC) routing.
 *
 *     Package (0x04) {
 *         (pci_dev << 16) | 0xFFFF,   // Address: device (high word), all functions (0xFFFF)
 *         acpi_pin,                   // Pin: INTA..INTD (0..3)
 *         0,                          // Source: 0 => GSI
 *         gsi                         // Source Index: GSI number
 *     }
 */
void acpigen_write_PRT_GSI_entry(unsigned int pci_dev, unsigned int acpi_pin, unsigned int gsi)
{
	acpigen_write_package(4);
	acpigen_write_dword((pci_dev << 16) | 0xffff);
	acpigen_write_byte(acpi_pin);

	/* Source */
	acpigen_write_byte(0);

	/* Source Index */
	acpigen_write_dword(gsi);

	acpigen_pop_len(); /* Package */
}

/*
 * Emit one PCI _PRT entry for link-device (PIC-style) routing.
 *
 *     Package (0x04) {
 *         (pci_dev << 16) | 0xFFFF,   // Address: device (high word), all functions (0xFFFF)
 *         acpi_pin,                   // Pin: INTA..INTD (0..3)
 *         source_path,                // Source: NameString (e.g. "\\_SB.INTA")
 *         index                       // Source Index
 *     }
 */
void acpigen_write_PRT_source_entry(unsigned int pci_dev, unsigned int acpi_pin,
				    const char *source_path, unsigned int index)
{
	acpigen_write_package(4);
	acpigen_write_dword((pci_dev << 16) | 0xffff);
	acpigen_write_byte(acpi_pin);

	/* Source */
	acpigen_emit_namestring(source_path);

	/* Source Index */
	acpigen_write_dword(index);

	acpigen_pop_len(); /* Package */
}

/*
 * Emit one PCI routing table entry for IO-APIC (GSI) routing:
 *
 *     Package (0x04)
 *     {
 *         0xDDDDFFFF,                 // PCI address: slot (high word), function (low word)
 *         0xPP,                       // PCI interrupt pin (INTA-INTx)
 *         0x00,                       // Source (0 = GSI)
 *         0x000000GG                  // Global system interrupt
 *     }
 */
void acpigen_write_PRT_GSI_entry_devfn(unsigned int devfn, unsigned int acpi_pin,
	unsigned int gsi)
{
	acpigen_write_package(4);
	acpigen_write_dword((PCI_SLOT(devfn) << 16) | PCI_FUNC(devfn));
	acpigen_write_byte(acpi_pin);

	/* Source */
	acpigen_write_byte(0);

	/* Source Index */
	acpigen_write_dword(gsi);

	acpigen_pop_len(); /* Package */
}

/*
* Emit one PCI routing table entry for static interrupt link (PIC) routing:
*
*     Package (0x04)
*     {
*         0xDDDDFFFF,                 // PCI address: slot, function
*         0xPP,                       // PCI interrupt pin
*         \_SB.INTx,                  // Interrupt source (e.g. \_SB.INTA)
*         0x000000II                  // Source index
*     }
*/
void acpigen_write_PRT_source_entry_devfn(unsigned int devfn, unsigned int acpi_pin,
	   const char *source_path,
	   unsigned int index)
{
	acpigen_write_package(4);
	acpigen_write_dword((PCI_SLOT(devfn) << 16) | PCI_FUNC(devfn));
	acpigen_write_byte(acpi_pin);

	/* Source */
	acpigen_emit_namestring(source_path);

	/* Source Index */
	acpigen_write_dword(index);

	acpigen_pop_len(); /* Package */
}
