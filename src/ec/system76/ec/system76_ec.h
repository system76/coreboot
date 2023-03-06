#ifndef EC_SYSTEM76_EC_H
#define EC_SYSTEM76_EC_H

// Default value, flashing is prevented, cannot be set with CMD_SECURITY_SET
#define SYSTEM76_EC_SECURITY_STATE_LOCK 0
// Flashing is allowed, cannot be set with CMD_SECURITY_SET
#define SYSTEM76_EC_SECURITY_STATE_UNLOCK 1
// Flashing will be prevented on the next reboot
#define SYSTEM76_EC_SECURITY_STATE_PREPARE_LOCK 2
// Flashing will be allowed on the next reboot
#define SYSTEM76_EC_SECURITY_STATE_PREPARE_UNLOCK 3

// Get security state
int system76_ec_security_get(uint8_t * state);

#endif /* EC_SYSTEM76_EC_H */
