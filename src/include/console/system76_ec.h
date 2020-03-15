#ifndef CONSOLE_SYSTEM76_EC_H
#define CONSOLE_SYSTEM76_EC_H 1

#include <stddef.h>
#include <stdint.h>

int system76_ec_print(uint8_t *buf, size_t len);

#define __CONSOLE_SYSTEM76_EC_ENABLE__	(CONFIG(CONSOLE_SYSTEM76_EC) && \
	(ENV_BOOTBLOCK || ENV_ROMSTAGE || ENV_RAMSTAGE || ENV_VERSTAGE || \
	ENV_POSTCAR || (ENV_SMM && CONFIG(DEBUG_SMI))))

#if __CONSOLE_SYSTEM76_EC_ENABLE__
static inline void __system76_ec_tx_byte(unsigned char byte) {
	system76_ec_print(&byte, 1);
}
#else
static inline void __system76_ec_tx_byte(unsigned char byte) {}
#endif

#endif
