/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/system76_ec.h>
#include <timer.h>

#define SYSTEM76_EC_BASE 0x0E00

static inline uint8_t system76_ec_read(uint8_t addr)
{
	return inb(SYSTEM76_EC_BASE + (uint16_t)addr);
}

static inline void system76_ec_write(uint8_t addr, uint8_t data)
{
	outb(data, SYSTEM76_EC_BASE + (uint16_t)addr);
}

void system76_ec_init(void)
{
	// Clear entire command region
	for (int i = 0; i < 256; i++)
		system76_ec_write((uint8_t)i, 0);
}

void system76_ec_flush(void)
{
	// Send command
	system76_ec_write(0, 4);

	// Wait for command completion, for up to 10 milliseconds
	wait_us(10000, system76_ec_read(0) == 0);

	// Clear length
	system76_ec_write(3, 0);
}

void system76_ec_print(uint8_t byte)
{
	// Read length
	uint8_t len = system76_ec_read(3);
	// Write data at offset
	system76_ec_write(len + 4, byte);
	// Update length
	system76_ec_write(3, len + 1);

	// If we hit the end of the buffer, or were given a newline, flush
	if (byte == '\n' || len >= 128)
		system76_ec_flush();
}
