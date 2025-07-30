/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/lpc.h>
#include <amdblocks/spi.h>
#include <device/mmio.h>
#include <assert.h>
#include <stdint.h>

/* Global SPI controller mutex */
struct thread_mutex spi_hw_mutex;

static uintptr_t spi_base;

void spi_set_base(void *base)
{
	spi_base = (uintptr_t)base;
}

uintptr_t spi_get_bar(void)
{
	if (ENV_X86 && !spi_base)
		spi_set_base((void *)lpc_get_spibase());
	ASSERT(spi_base);

	return spi_base;
}

uint8_t spi_read8(uint8_t reg)
{
	return read8p(spi_get_bar() + reg);
}

uint16_t spi_read16(uint8_t reg)
{
	return read16p(spi_get_bar() + reg);
}

uint32_t spi_read32(uint8_t reg)
{
	return read32p(spi_get_bar() + reg);
}

/* Reads in a block from SPI MMIO. Uses DWORD when possible. */
void spi_read_block(uint8_t reg, uint8_t *dest, size_t len)
{
	uintptr_t addr = spi_get_bar() + reg;

	if (CONFIG(SOC_AMD_COMMON_BLOCK_SPI_DWORD_ACCESS)) {
		/* DWORD align */
		for (; len && !IS_ALIGNED(addr, 4); len--, dest++, addr++)
			*dest = read8p(addr);

		/* Read in DWORDs */
		for (; len >= 4; len -= 4, dest += 4, addr += 4)
			*((uint32_t *)dest) = read32p(addr);
	}

	/* Read remainder */
	for (; len; len--, dest++, addr++)
		*dest = read8p(addr);
}

void spi_write8(uint8_t reg, uint8_t val)
{
	write8p(spi_get_bar() + reg, val);
}

void spi_write16(uint8_t reg, uint16_t val)
{
	write16p(spi_get_bar() + reg, val);
}

void spi_write32(uint8_t reg, uint32_t val)
{
	write32p(spi_get_bar() + reg, val);
}

/* Writes out a block to SPI MMIO. Uses DWORD when possible. */
void spi_write_block(uint8_t reg, const uint8_t *src, size_t len)
{
	uintptr_t addr = spi_get_bar() + reg;

	if (CONFIG(SOC_AMD_COMMON_BLOCK_SPI_DWORD_ACCESS)) {
		/* DWORD align */
		for (; len && !IS_ALIGNED(addr, 4); len--, src++, addr++)
			write8p(addr, *src);

		/* Write out DWORDs */
		for (; len >= 4; len -= 4, src += 4, addr += 4)
			write32p(addr, *((uint32_t *)src));
	}

	/* Write remainder */
	for (; len; len--, src++, addr++)
		write8p(addr, *src);
}
