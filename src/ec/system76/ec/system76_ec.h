/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SYSTEM76_EC_H
#define SYSTEM76_EC_H

#include <stdbool.h>
#include <stdint.h>

// Default value, flashing is prevented, cannot be set with CMD_SECURITY_SET
#define SYSTEM76_EC_SECURITY_STATE_LOCK 0
// Flashing is allowed, cannot be set with CMD_SECURITY_SET
#define SYSTEM76_EC_SECURITY_STATE_UNLOCK 1
// Flashing will be prevented on the next reboot
#define SYSTEM76_EC_SECURITY_STATE_PREPARE_LOCK 2
// Flashing will be allowed on the next reboot
#define SYSTEM76_EC_SECURITY_STATE_PREPARE_UNLOCK 3

/*
 * Send a command to the EC.  request_data/request_size are the request payload,
 * request_data can be NULL if request_size is 0.  reply_data/reply_size are
 * the reply payload, reply_data can be NULL if reply_size is 0.
 */
bool system76_ec_cmd(uint8_t cmd, const uint8_t *request_data,
	uint8_t request_size, uint8_t *reply_data, uint8_t reply_size);

bool system76_ec_security_get(uint8_t *state);

#endif
