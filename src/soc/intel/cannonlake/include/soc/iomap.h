/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2017 Intel Corporation.
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

#ifndef _SOC_CANNONLAKE_IOMAP_H_
#define _SOC_CANNONLAKE_IOMAP_H_

/*
 * Memory-mapped I/O registers.
 */
#define MCFG_BASE_ADDRESS	CONFIG_MMCONF_BASE_ADDRESS
#define MCFG_BASE_SIZE		0x4000000

#define PCH_PRESERVED_BASE_ADDRESS	0xfc800000
#define PCH_PRESERVED_BASE_SIZE	0x02000000

#define PCH_TRACE_HUB_BASE_ADDRESS	0xfc800000
#define PCH_TRACE_HUB_BASE_SIZE	0x00800000

#define UART_BASE_SIZE		0x1000

#define UART_BASE_0_ADDRESS	0xfe032000
/* Both UART BAR 0 and 1 are 4KB in size */
#define UART_BASE_0_ADDR(x)	(UART_BASE_0_ADDRESS + (2 * \
					UART_BASE_SIZE * (x)))
#define UART_BASE(x)		UART_BASE_0_ADDR(x)

#define EARLY_I2C_BASE_ADDRESS	0xfe040000
#define EARLY_I2C_BASE(x)	(EARLY_I2C_BASE_ADDRESS + (0x1000 * (x)))

#define MCH_BASE_ADDRESS	0xfed10000
#define MCH_BASE_SIZE		0x8000

#define DMI_BASE_ADDRESS	0xfeda0000
#define DMI_BASE_SIZE		0x1000

#define EP_BASE_ADDRESS		0xfeda1000
#define EP_BASE_SIZE		0x1000

#define EDRAM_BASE_ADDRESS	0xfed80000
#define EDRAM_BASE_SIZE		0x4000

#define REG_BASE_ADDRESS	0xfc000000
#define REG_BASE_SIZE		0x1000

#define HPET_BASE_ADDRESS	0xfed00000

#define PCH_PWRM_BASE_ADDRESS	0xfe000000
#define PCH_PWRM_BASE_SIZE	0x10000

#define SPI_BASE_ADDRESS	0xfe010000
#define EARLY_GSPI_BASE_ADDRESS 0xfe011000

#define GPIO_BASE_SIZE		0x10000

#define HECI1_BASE_ADDRESS	0xfeda2000

/* PTT registers */
#define PTT_TXT_BASE_ADDRESS	0xfed30800
#define PTT_PRESENT		0x00070000

#define VTD_BASE_ADDRESS	0xFED90000
#define VTD_BASE_SIZE		0x00004000
/*
 * I/O port address space
 */
#define SMBUS_BASE_ADDRESS	0x0efa0
#define SMBUS_BASE_SIZE		0x20

#define ACPI_BASE_ADDRESS	0x1800
#define ACPI_BASE_SIZE		0x100

#define TCO_BASE_ADDRESS	0x400
#define TCO_BASE_SIZE		0x20

#define P2SB_BAR		CONFIG_PCR_BASE_ADDRESS
#define P2SB_SIZE		(16 * MiB)

#endif
