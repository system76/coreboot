/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MATISSE_UART_H__
#define __MATISSE_UART_H__

#include <types.h>

void set_uart_config(int idx); /* configure hardware of FCH UART selected by idx */
void clear_uart_legacy_config(void); /* disable legacy I/O decode for FCH UART */

uintptr_t get_uart_base(int idx); /* get MMIO base address of FCH UART */

#endif /* __MATISSE_UART_H__ */
