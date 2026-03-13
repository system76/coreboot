/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/uart.h>
#include <mainboard/addressmap.h>

uintptr_t io_port_mmio_base = SBSA_PCIE_PIO_BASE;

uintptr_t uart_platform_base(unsigned int idx)
{
	return SBSA_UART_BASE;
}
