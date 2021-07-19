/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <intelblocks/itss.h>
#include <intelblocks/pcr.h>
#include <soc/itss.h>
#include <soc/pcr_ids.h>

/* PCI IRQ assignment */
#include "pci_irqs.asl"

/* PCR access */
#include <soc/intel/common/acpi/pcr.asl>

/* PCH clock */
#include "camera_clock_ctl.asl"

/* GPIO controller */
#if CONFIG(SOC_INTEL_TIGERLAKE_PCH_H)
#include "gpio_pch_h.asl"
#else
#include "gpio.asl"
#endif

/* GFX 00:02.0 */
#include <drivers/intel/gma/acpi/gfx.asl>

/* ESPI 0:1f.0 */
#include <soc/intel/common/block/acpi/acpi/lpc.asl>

/* PCH HDA */
#include "pch_hda.asl"

/* PCIE Ports */
#include "pcie.asl"

/* Serial IO */
#include "serialio.asl"

/* SMBus 0:1f.4 */
#include <soc/intel/common/block/acpi/acpi/smbus.asl>

/* ISH 0:12.0 */
#include <soc/intel/common/block/acpi/acpi/ish.asl>

/* USB XHCI 0:14.0 */
#include "xhci.asl"

/* PCI _OSC */
#include <soc/intel/common/acpi/pci_osc.asl>

/* Intel Power Engine Plug-in */
#include <soc/intel/common/block/acpi/acpi/pep.asl>

/* GbE 0:1f.6 */
#include <soc/intel/common/block/acpi/acpi/pch_glan.asl>
