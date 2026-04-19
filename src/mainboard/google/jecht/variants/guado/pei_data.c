/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>

const struct usb2_port_setting mainboard_usb2_ports[MAX_USB2_PORTS] = {
	/* P0: VP8 */
	{ 0x0064, 1, 0, USB_PORT_MINI_PCIE },
	/* P1: Port A, CN22 */
	{ 0x0040, 1, 0, USB_PORT_INTERNAL },
	/* P2: Port B, CN23 */
	{ 0x0040, 1, 1, USB_PORT_INTERNAL },
	/* P3: WLAN */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_MINI_PCIE },
	/* P4: Port C, CN25 */
	{ 0x0040, 1, 2, USB_PORT_INTERNAL },
	/* P5: Port D, CN25 */
	{ 0x0040, 1, 2, USB_PORT_INTERNAL },
	/* P6: Card Reader */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_INTERNAL },
	/* P7: EMPTY */
	{ 0x0000, 0, 0, USB_PORT_SKIP },
};

const struct usb3_port_setting mainboard_usb3_ports[MAX_USB3_PORTS] = {
	/* P1: CN22 */
	{ 1, 0, 0 },
	/* P2: CN23 */
	{ 1, 1, 0 },
	/* P3: CN25 */
	{ 1, 2, 0 },
	/* P4: CN25 */
	{ 1, 2, 0 },
};
