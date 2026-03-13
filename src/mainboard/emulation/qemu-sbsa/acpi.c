/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_iort.h>
#include <mainboard/addressmap.h>


void acpi_fill_fadt(acpi_fadt_t *fadt)
{
	fadt->ARM_boot_arch |= ACPI_FADT_ARM_PSCI_COMPLIANT;
}

unsigned long acpi_fill_madt(unsigned long current)
{
	return current;
}

uintptr_t platform_get_gicd_base(void)
{
	return SBSA_GIC_DIST;
}

uintptr_t platform_get_gicr_base(void)
{
	return SBSA_GIC_REDIST;
}

static uintptr_t gic_its[] = {
	SBSA_GIC_ITS,
};

int platform_get_gic_its(uintptr_t **base)
{
	*base = gic_its;
	return ARRAY_SIZE(gic_its);
}

unsigned long acpi_soc_fill_iort(acpi_iort_t *iort, unsigned long current)
{
	acpi_iort_node_t *its, *rc;
	u32 its_reference;
	u32 identifiers[] = {0};	/* GIC ITS ID 0 */

	/* ITS Group node */
	current = acpi_iort_its_entry(current, iort, &its, 1, identifiers);
	its_reference = (unsigned long)its - (unsigned long)iort;

	/* Root Complex node for PCIe host bridge (segment 0) */
	current = acpi_iort_rc_entry(current, iort, &rc,
				     0,		/* memory_properties */
				     0,		/* ats_attribute */
				     0,		/* pci_segment_number */
				     0x30,	/* memory_address_limit (48-bit) */
				     0);	/* pasid_capabilities */

	/* Map all PCI RIDs (0-0xFFFF) 1:1 to ITS device IDs */
	current = acpi_iort_id_map_entry(current, rc,
					 0,		/* input_base */
					 0x10000,	/* id_count (65536 RIDs) */
					 0,		/* output_base */
					 its_reference,	/* output_reference */
					 0);		/* flags */

	return current;
}

#define SEC_EL1_TIMER_GISV 0x1d
#define NONSEC_EL1_TIMER_GSIV 0x1e
#define VIRTUAL_TIMER_GSIV 0x1b
#define NONSEC_EL2_TIMER_GSIV 0x1a

#define SBSA_TIMER_FLAGS (ACPI_GTDT_INTERRUPT_POLARITY | ACPI_GTDT_ALWAYS_ON)

void acpi_soc_fill_gtdt(acpi_gtdt_t *gtdt)
{
	/* This value is optional if the system implements EL3 (Security
	   Extensions). If not provided, this field must be 0xFFFFFFFFFFFFFFFF. */
	gtdt->counter_block_address = UINT64_MAX;
	gtdt->secure_el1_interrupt = SEC_EL1_TIMER_GISV;
	gtdt->secure_el1_flags = SBSA_TIMER_FLAGS;
	gtdt->non_secure_el1_interrupt = NONSEC_EL1_TIMER_GSIV;
	gtdt->non_secure_el1_flags = SBSA_TIMER_FLAGS;
	gtdt->virtual_timer_interrupt = VIRTUAL_TIMER_GSIV;
	gtdt->virtual_timer_flags = SBSA_TIMER_FLAGS;
	gtdt->non_secure_el2_interrupt = NONSEC_EL2_TIMER_GSIV;
	gtdt->non_secure_el2_flags = SBSA_TIMER_FLAGS;
	/* This value is optional if the system implements EL3
	   (Security Extensions). If not provided, this field must be
	   0xFFFFFFFFFFFFFFF. */
	gtdt->counter_read_block_address = UINT64_MAX;
}

#define WD_TIMER_GSIV 0x30

unsigned long acpi_soc_gtdt_add_timers(uint32_t *count, unsigned long current)
{
	(*count)++;
	return acpi_gtdt_add_watchdog(current, SBSA_GWDT_REFRESH, SBSA_GWDT_CONTROL,
				      WD_TIMER_GSIV, 0);
}
