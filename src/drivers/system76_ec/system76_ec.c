#include <arch/io.h>
#include <console/system76_ec.h>
#include <delay.h>

#define SYSTEM76_EC_BASE 0x0E00

static uint8_t system76_ec_read(uint8_t addr) {
    return inb(SYSTEM76_EC_BASE + (uint16_t)addr);
}

static void system76_ec_write(uint8_t addr, uint8_t data) {
    outb(data, SYSTEM76_EC_BASE + (uint16_t)addr);
}

int system76_ec_print(uint8_t *buf, size_t len) {
    size_t i;
    for (i=0; i < len;) {
        if (system76_ec_read(0) == 0) {
            size_t j;
            for (j=0; (i < len) && ((j + 4) < 256); i++, j++) {
                system76_ec_write((uint8_t)(j + 4), buf[i]);
            }
            // Flags
            system76_ec_write(2, 0);
            // Length
            system76_ec_write(3, (uint8_t)j);
            // Command
            system76_ec_write(0, 4);
            // Wait for command completion, for up to 1 second
            int timeout;
            for (timeout = 1000000; timeout > 0; timeout--) {
                if (system76_ec_read(0) == 0) break;
                udelay(1);
            }
            if (timeout == 0) {
                // Error: timeout occured
                return -1;
            }
            if (system76_ec_read(1) != 0) {
                // Error: command failed
                return -1;
            }
        } else {
            // Error: command is already running
            return -1;
        }
    }
    return (int)i;
}
