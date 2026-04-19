/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>

const struct usb2_port_setting mainboard_usb2_ports[MAX_USB2_PORTS] = {
	/* P0: VP8 */
	{ 0x0064, 1, USB_OC_PIN_SKIP, USB_PORT_MINI_PCIE },
	/* P1: Port 3, USB3 */
	{ 0x0040, 1, 0, USB_PORT_INTERNAL },
	/* P2: Port 4, USB4 */
	{ 0x0040, 1, 1, USB_PORT_INTERNAL },
	/* P3: Mini Card */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_MINI_PCIE },
	/* P4: Port 1, USB1 */
	{ 0x0040, 1, 2, USB_PORT_INTERNAL },
	/* P5: Port 2, USB2 */
	{ 0x0040, 1, 2, USB_PORT_INTERNAL },
	/* P6: Card Reader */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_INTERNAL },
	/* P7: Pin Header */
	{ 0x0040, 1, 3, USB_PORT_INTERNAL },
};

const struct usb3_port_setting mainboard_usb3_ports[MAX_USB3_PORTS] = {
	/* P1: USB1 */
	{ 1, 2, 0 },
	/* P2: USB2 */
	{ 1, 2, 0 },
	/* P3: USB3 */
	{ 1, 0, 0 },
	/* P4: USB4 */
	{ 1, 1, 0 },
};
