/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MATISSE_IOMAP_H__
#define __SOC_MATISSE_IOMAP_H__

/* MMIO Ranges */
/* IO_APIC_ADDR defined in arch/x86	0xfec00000 */
#define SPI_BASE_ADDRESS		0xfec10000

#if CONFIG(HPET_ADDRESS_OVERRIDE)
#error HPET address override is not allowed and must be fixed at 0xfed00000
#endif
#define HPET_BASE_ADDRESS		0xfed00000

#define I2C_MASTER_DEV_COUNT		6
#define I2C_MASTER_START_INDEX		0
#define I2C_SLAVE_DEV_COUNT		0

#define APU_I2C0_BASE			0xfedc2000
#define APU_I2C1_BASE			0xfedc3000
#define APU_I2C2_BASE			0xfedc4000
#define APU_I2C3_BASE			0xfedc5000
#define APU_I2C4_BASE			0xfedc6000
#define APU_I2C5_BASE			0xfedcb000

/* I2C parameters for lpc_read_resources */
#define I2C_BASE_ADDRESS		APU_I2C2_BASE
#define I2C_DEVICE_SIZE			0x00001000
#define I2C_DEVICE_COUNT		(I2C_MASTER_DEV_COUNT \
					 - I2C_MASTER_START_INDEX)

#define APU_UART0_BASE			0xfedc9000
#define APU_UART1_BASE			0xfedce000

#define APU_EMMC_BASE			0xfedd5000
#define APU_EMMC_CONFIG_BASE		0xfedd5800

#define FLASH_BASE_ADDR			((0xffffffff - CONFIG_ROM_SIZE) + 1)

/* I/O Ranges */
#define ACPI_SMI_CTL_PORT		0xb2
#define MATISSE_ACPI_IO_BASE	CONFIG_MATISSE_ACPI_IO_BASE
#define  ACPI_PM_EVT_BLK	(MATISSE_ACPI_IO_BASE + 0x00)     /* 4 bytes */
#define  ACPI_PM1_STS		(ACPI_PM_EVT_BLK + 0x00)	  /* 2 bytes */
#define  ACPI_PM1_EN		(ACPI_PM_EVT_BLK + 0x02)	  /* 2 bytes */
#define  ACPI_PM1_CNT_BLK	(MATISSE_ACPI_IO_BASE + 0x04)     /* 2 bytes */
#define  ACPI_PM_TMR_BLK	(MATISSE_ACPI_IO_BASE + 0x08)     /* 4 bytes */
#define  ACPI_CPU_CONTROL	(MATISSE_ACPI_IO_BASE + 0x0c)     /* 6 bytes */
/* doc says 0x14 for GPE0_BLK but FT5 only works with 0x20 */
#define  ACPI_GPE0_BLK		(MATISSE_ACPI_IO_BASE + 0x20)     /* 8 bytes */
#define  ACPI_GPE0_STS		(ACPI_GPE0_BLK + 0x00)		  /* 4 bytes */
#define  ACPI_GPE0_EN		(ACPI_GPE0_BLK + 0x04)		  /* 4 bytes */
#define NCP_ERR				0xf0
#define SMB_BASE_ADDR			0xb00
#define PM2_INDEX			0xcd0
#define PM2_DATA			0xcd1
#define BIOSRAM_INDEX			0xcd4
#define BIOSRAM_DATA			0xcd5
#define AB_INDX				0xcd8
#define AB_DATA				(AB_INDX+4)
#define SYS_RESET			0xcf9

/* BiosRam Ranges at 0xfed80500 or I/O 0xcd4/0xcd5 */
#define BIOSRAM_CBMEM_TOP		0xf0 /* 4 bytes */
#define BIOSRAM_UMA_SIZE		0xf4 /* 4 bytes */
#define BIOSRAM_UMA_BASE		0xf8 /* 8 bytes */

#endif /* __SOC_MATISSE_IOMAP_H__ */
