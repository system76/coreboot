/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/system76_ec.h>
#include <timer.h>
#include "system76_ec.h"

// This is the command region for System76 EC firmware. It must be
// enabled for LPC in the mainboard.
#define SYSTEM76_EC_BASE 0x0E00
#define SYSTEM76_EC_SIZE 256

#define REG_CMD 0
#define REG_RESULT 1
#define REG_DATA 2

// When command register is 0, command is complete
#define CMD_FINISHED 0

// Print command. Registers are unique for each command
#define CMD_PRINT 4
#define CMD_PRINT_REG_FLAGS REG_DATA
#define CMD_PRINT_REG_LEN (REG_DATA + 1)
#define CMD_PRINT_REG_DATA (REG_DATA + 2)

// Get security state command
#define CMD_SECURITY_GET 20

// OK result, any other values are errors
#define RESULT_OK 0

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
	for (int i = 0; i < SYSTEM76_EC_SIZE; i++)
		system76_ec_write((uint8_t)i, 0);
}

void system76_ec_flush(void)
{
	system76_ec_write(REG_CMD, CMD_PRINT);

	// Wait for command completion, for up to 10 milliseconds, with a
	// test period of 1 microsecond
	wait_us(10000, system76_ec_read(REG_CMD) == CMD_FINISHED);

	system76_ec_write(CMD_PRINT_REG_LEN, 0);
}

void system76_ec_print(uint8_t byte)
{
	uint8_t len = system76_ec_read(CMD_PRINT_REG_LEN);
	system76_ec_write(CMD_PRINT_REG_DATA + len, byte);
	system76_ec_write(CMD_PRINT_REG_LEN, len + 1);

	// If we hit the end of the buffer, or were given a newline, flush
	if (byte == '\n' || len >= (SYSTEM76_EC_SIZE - CMD_PRINT_REG_DATA))
		system76_ec_flush();
}

// Issue a command not checking if the console needs to be flushed
// Do not print from this command to avoid EC protocol issues
static int system76_ec_unsafe(uint8_t cmd, uint8_t * data, int length) {
	// Error if length is too long
	if (length > (SYSTEM76_EC_SIZE - REG_DATA)) {
		return -1;
	}

	// Error if command is in progress
	if (system76_ec_read(REG_CMD) != CMD_FINISHED) {
		return -1;
	}

	// Write command data
	for (int i = 0; i < length; i++) {
		system76_ec_write(REG_DATA + i, data[i]);
	}

	// Start command
	system76_ec_write(REG_CMD, cmd);

	// Wait for command completion, for up to 10 milliseconds, with a
	// test period of 1 microsecond
	wait_us(10000, system76_ec_read(REG_CMD) == CMD_FINISHED);

	// Error if command did not complete
	if (system76_ec_read(REG_CMD) != CMD_FINISHED) {
		return -1;
	}

	// Read command data
	for (int i = 0; i < length; i++) {
		data[i] = system76_ec_read(REG_DATA + i);
	}

	// Check result
	if (system76_ec_read(REG_RESULT) != RESULT_OK) {
		return -1;
	}

	return 0;
}

// Wrapper to allow issuing commands while console is being used
// Do not print from this command to avoid EC protocol issues
static int system76_ec_command(uint8_t cmd, uint8_t * data, int length) {
	// Error if command is in progress
	if (system76_ec_read(REG_CMD) != CMD_FINISHED) {
		return -1;
	}

	// Flush print buffer if it has data
	// Checked for completion by system76_ec_unsafe
	if (system76_ec_read(CMD_PRINT_REG_LEN) > 0) {
		system76_ec_flush();
	}

	// Run command now that print buffer is flushed
	int res = system76_ec_unsafe(cmd, data, length);
	if (res < 0) {
		return res;
	}

	// Clear command data (for future prints)
	// Length is checked by system76_ec_unsafe
	for (int i = 0; i < length; i++) {
		system76_ec_write(REG_DATA + i, 0);
	}

	return 0;
}

// Get security state
int system76_ec_security_get(uint8_t * state) {
	*state = 0;
	return system76_ec_command(CMD_SECURITY_GET, state, sizeof(uint8_t));
}
