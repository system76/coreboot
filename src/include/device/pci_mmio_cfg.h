/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _PCI_MMIO_CFG_H
#define _PCI_MMIO_CFG_H

#include <stdint.h>
#include <device/mmio.h>
#include <device/pci_type.h>

/* Using a unique datatype for MMIO writes makes the pointers to _not_
 * qualify for pointer aliasing with any other objects in memory.
 *
 * MMIO offset is a value originally derived from 'struct device *'
 * in ramstage. For the compiler to not discard this MMIO offset value
 * from CPU registers after any MMIO writes, -fstrict-aliasing has to
 * be also set for the build.
 *
 * Bottom 12 bits (4 KiB) are reserved to address the registers of a
 * single PCI function. Declare the bank as a union to avoid some casting
 * in the functions below.
 */
union pci_bank {
	uint8_t reg8[4096];
	uint16_t reg16[4096 / sizeof(uint16_t)];
	uint32_t reg32[4096 / sizeof(uint32_t)];
};

#if CONFIG(ECAM_MMCONF_SUPPORT)

#if CONFIG_ECAM_MMCONF_BASE_ADDRESS == 0
#error "CONFIG_ECAM_MMCONF_BASE_ADDRESS undefined!"
#endif

#if CONFIG_ECAM_MMCONF_BUS_NUMBER * MiB != CONFIG_ECAM_MMCONF_LENGTH
#error "CONFIG_ECAM_MMCONF_LENGTH does not correspond with CONFIG_ECAM_MMCONF_BUS_NUMBER!"
#endif

/* By not assigning this to CONFIG_ECAM_MMCONF_BASE_ADDRESS here we
   prevent some sub-optimal constant folding. */
extern u8 *const pci_mmconf;

static __always_inline
volatile union pci_bank *pci_map_bus(pci_devfn_t dev)
{
	return (void *)&pci_mmconf[PCI_DEVFN_OFFSET(dev)];
}

#else

/* For platforms not supporting ECAM, they need to define pci_map_bus function
 * in their platform-specific code */
volatile union pci_bank *pci_map_bus(pci_devfn_t dev);

#endif

/*
 * Avoid name collisions as different stages have different signature
 * for these functions. The _s_ stands for simple, fundamental IO or
 * MMIO variant.
 */

static __always_inline
uint8_t pci_s_read_config8(pci_devfn_t dev, uint16_t reg)
{
	return pci_map_bus(dev)->reg8[reg];
}

static __always_inline
uint16_t pci_s_read_config16(pci_devfn_t dev, uint16_t reg)
{
	return pci_map_bus(dev)->reg16[reg / sizeof(uint16_t)];
}

static __always_inline
uint32_t pci_s_read_config32(pci_devfn_t dev, uint16_t reg)
{
	return pci_map_bus(dev)->reg32[reg / sizeof(uint32_t)];
}

static __always_inline
void pci_s_write_config8(pci_devfn_t dev, uint16_t reg, uint8_t value)
{
	pci_map_bus(dev)->reg8[reg] = value;
}

static __always_inline
void pci_s_write_config16(pci_devfn_t dev, uint16_t reg, uint16_t value)
{
	pci_map_bus(dev)->reg16[reg / sizeof(uint16_t)] = value;
}

static __always_inline
void pci_s_write_config32(pci_devfn_t dev, uint16_t reg, uint32_t value)
{
	pci_map_bus(dev)->reg32[reg / sizeof(uint32_t)] = value;
}

/*
 * The functions pci_mmio_config*_addr provide a way to determine the MMIO address of a PCI
 * config register. The address returned is dependent of both the MMCONF base address and the
 * assigned PCI bus number of the requested device, which both can change during the boot
 * process. Thus, the pointer returned here must not be cached!
 */
static __always_inline
uint8_t *pci_mmio_config8_addr(pci_devfn_t dev, uint16_t reg)
{
	return (uint8_t *)&pci_map_bus(dev)->reg8[reg];
}

static __always_inline
uint16_t *pci_mmio_config16_addr(pci_devfn_t dev, uint16_t reg)
{
	return (uint16_t *)&pci_map_bus(dev)->reg16[reg / sizeof(uint16_t)];
}

static __always_inline
uint32_t *pci_mmio_config32_addr(pci_devfn_t dev, uint16_t reg)
{
	return (uint32_t *)&pci_map_bus(dev)->reg32[reg / sizeof(uint32_t)];
}

#endif /* _PCI_MMIO_CFG_H */
