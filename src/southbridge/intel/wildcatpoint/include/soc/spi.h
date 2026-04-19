/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BROADWELL_SPI_H_
#define _BROADWELL_SPI_H_

/* Registers within SPIBAR */
#define SPIBAR_HSFS                 0x3804   /* SPI hardware sequence status */
#define  SPIBAR_HSFS_FLOCKDN        (1 << 15)/* Flash Configuration Lock-Down */

#define SPIBAR_SSFC		0x3891
#define SPIBAR_FDOC		0x38b0
#define SPIBAR_FDOD		0x38b4

#endif
